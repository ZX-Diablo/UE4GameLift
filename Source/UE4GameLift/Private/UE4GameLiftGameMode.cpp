// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UE4GameLiftGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

#include "Client.h"
#include "MyGameInstance.h"
#include "MyPlayerState.h"
#include "Server.h"

AUE4GameLiftGameMode::AUE4GameLiftGameMode()
{
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AUE4GameLiftGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

#if WITH_GAMELIFT
	if (!ErrorMessage.IsEmpty())
	{
		return;
	}

	FString playerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	if (playerSessionId.IsEmpty())
	{
		ErrorMessage = TEXT("Player session ID not found");
		return;
	}

	if (auto gameInstance = GetGameInstance<UMyGameInstance>())
	{
		if (!gameInstance->Server->AcceptPlayerSession(playerSessionId))
		{
			ErrorMessage = TEXT("Failed to validate player session");
		}
	}
#endif // WITH_GAMELIFT
}

void AUE4GameLiftGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

#if WITH_GAMELIFT
	if (auto playerState = Exiting->GetPlayerState<AMyPlayerState>())
	{
		if (auto gameInstance = GetGameInstance<UMyGameInstance>())
		{
			gameInstance->Server->RemovePlayerSession(playerState->PlayerSessionId);
		}
	}

	if (GetGameState<AGameStateBase>()->PlayerArray.Num() == 1)
	{
		if (auto gameInstance = GetGameInstance<UMyGameInstance>())
		{
			if (!gameInstance->Server->TerminateGameSession())
			{
				UE_LOG(LogTemp, Fatal, TEXT("Failed to terminate game session"));
			}
		}
	}
#endif // WITH_GAMELIFT
}

FString AUE4GameLiftGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString errorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

#if WITH_GAMELIFT
	if (!errorMessage.IsEmpty())
	{
		return errorMessage;
	}

	FString playerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	if (playerSessionId.IsEmpty())
	{
		return TEXT("Player session ID not found");
	}

	auto playerState = NewPlayerController->GetPlayerState<AMyPlayerState>();
	if (!playerState)
	{
		return TEXT("Failed to get player state");
	}

	playerState->PlayerSessionId = playerSessionId;

	return FString();
#else
	return errorMessage;
#endif // WITH_GAMELIFT
}

void AUE4GameLiftGameMode::BeginPlay()
{
	Super::BeginPlay();

#if !UE_SERVER
	if (auto gameInstance = GetGameInstance<UMyGameInstance>())
	{
		if (!gameInstance->Client->Init())
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to initialize client"));
		}
	}
#endif // !UE_SERVER

#if WITH_GAMELIFT
	if (auto gameInstance = GetGameInstance<UMyGameInstance>())
	{
		if (!gameInstance->Server->Start())
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to start server"));
		}
	}
#endif // WITH_GAMELIFT
}
