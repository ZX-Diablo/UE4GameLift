// Copyright ZX.Diablo 2020

#include "Client.h"

#include "Async/Async.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

#include "GameLiftClientApi.h"
#include "GameLiftClientObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogClient, Log, All);

bool UClient::Init()
{
	UE_LOG(LogClient, Log, TEXT("Initializing client..."));
	GameLift = UGameLiftClientObject::CreateGameLiftObject(Key, Secret, Region);
	return GameLift != nullptr;
}

bool UClient::HostSession()
{
	FGameLiftCreateGameSessionConfig config;

	config.SetMaxPlayerCount(2);
	config.SetAliasId(AliasId);

	GameLiftCreateGameSession = GameLift->CreateGameSession(config);
	if (!GameLiftCreateGameSession)
	{
		UE_LOG(LogClient, Error, TEXT("Failed to create new session"));
		return false;
	}

	GameLiftCreateGameSession->OnCreateGameSessionSuccess.AddDynamic(this, &UClient::OnCreateGameSessionSuccess);
	GameLiftCreateGameSession->OnCreateGameSessionFailed.AddDynamic(this, &UClient::OnCreateGameSessionFailed);
	GameLiftCreateGameSession->Activate();

	return true;
}

bool UClient::TryJoinSession(const FString& SessionId)
{
	GameLiftDescribeGameSessionDetails = GameLift->DescribeGameSessionDetails(SessionId);
	if (!GameLiftDescribeGameSessionDetails)
	{
		UE_LOG(LogClient, Error, TEXT("Failed to describe session"));
		return false;
	}

	GameLiftDescribeGameSessionDetails->OnDescribeGameSessionDetailsSuccess.AddDynamic(this, &UClient::OnDescribeGameSessionSuccess);
	GameLiftDescribeGameSessionDetails->OnDescribeGameSessionDetailsFailed.AddDynamic(this, &UClient::OnDescribeGameSessionFailed);
	GameLiftDescribeGameSessionDetails->Activate();

	return true;
}

bool UClient::JoinSession(const FString& SessionId)
{
	auto player = GetWorld()->GetFirstLocalPlayerFromController();
	if (!player)
	{
		UE_LOG(LogClient, Error, TEXT("Failed to get first player"));
		return false;
	}

	GameLiftCreatePlayerSession = GameLift->CreatePlayerSession(SessionId, player->GetPreferredUniqueNetId().ToString());
	if (!GameLiftCreatePlayerSession)
	{
		UE_LOG(LogClient, Error, TEXT("Failed to create player session"));
		return false;
	}

	GameLiftCreatePlayerSession->OnCreatePlayerSessionSuccess.AddDynamic(this, &UClient::OnCreatePlayerSessionSuccess);
	GameLiftCreatePlayerSession->OnCreatePlayerSessionFailed.AddDynamic(this, &UClient::OnCreatePlayerSessionFailed);
	GameLiftCreatePlayerSession->Activate();

	return true;
}

void UClient::OnCreateGameSessionSuccess(const FGameLiftCreateGameSessionResult& GameSessionResult)
{
	UE_LOG(LogClient, Log, TEXT("Game session %s successfully created"), *GameSessionResult.GameSessionId);
	TryJoinSession(GameSessionResult.GameSessionId);
}

void UClient::OnCreateGameSessionFailed(const FString& ErrorMessage)
{
	UE_LOG(LogClient, Error, TEXT("Failed to create game session: %s"), *ErrorMessage);
}

void UClient::OnDescribeGameSessionSuccess(const FString& SessionId, const FString& SessionState, const int& CurrentPlayerCount, const int& MaxPlayerCount, const FString& PlayerSessionCreationPolicy)
{
	AsyncTask(ENamedThreads::GameThread, [this, SessionId, SessionState]()
	{
		if (SessionState == TEXT("ACTIVE"))
		{
			JoinSession(SessionId);
		}
		else if (SessionState == TEXT("ACTIVATING"))
		{
			FTimerHandle ignore;
			GetWorld()->GetTimerManager().SetTimer(ignore, [this, SessionId]()
			{
				TryJoinSession(SessionId);
			}, 0.05f, false);
		}
		else
		{
			UE_LOG(LogClient, Warning, TEXT("Failed attempt to join dead session %s"), *SessionId);
		}
	});
}

void UClient::OnDescribeGameSessionFailed(const FString& ErrorMessage)
{
	UE_LOG(LogClient, Error, TEXT("Failed to describe game session: %s"), *ErrorMessage);
}

void UClient::OnCreatePlayerSessionSuccess(const FString& Ip, const FString& Port, const FString& PlayerSessionId, const FString& PlayerSessionStatus)
{
	UE_LOG(LogClient, Log, TEXT("Player session %s successfully created on server %s:%s"), *PlayerSessionId, *Ip, *Port);

	FString url = Ip + TEXT(":") + Port + TEXT("?PlayerSessionId=") + PlayerSessionId;
	GetWorld()->GetFirstPlayerController()->ClientTravel(url, ETravelType::TRAVEL_Absolute);
}

void UClient::OnCreatePlayerSessionFailed(const FString& ErrorMessage)
{
	UE_LOG(LogClient, Error, TEXT("Failed to create player session: %s"), *ErrorMessage);
}
