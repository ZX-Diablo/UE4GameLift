// Copyright ZX.Diablo 2020

#include "Server.h"

#include "Engine/EngineBaseTypes.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"
#include "HAL/FileManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogServer, Log, All);

UServer::UServer()
{
#if WITH_GAMELIFT
	GameLift = FModuleManager::LoadModulePtr<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
	if (!GameLift)
	{
		UE_LOG(LogServer, Error, TEXT("Failed to load GameLiftServerSDK module"));
	}

	Params.port = FURL::UrlConfig.DefaultPort;
	Params.logParameters = TArray<FString>({ IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FGenericPlatformOutputDevices::GetAbsoluteLogFilename()) });

	/**
	 * Called when GameLift service requests to start a new session
	 * @param Session Session data object. Session ID is the most useful here. Another important one is matchmaker data
	 */
	Params.OnStartGameSession.BindLambda([this](Aws::GameLift::Server::Model::GameSession Session)
	{
		FString sessionId = ANSI_TO_TCHAR(Session.GetGameSessionId());

		auto outcome = GameLift->ActivateGameSession();
		if (outcome.IsSuccess())
		{
			UE_LOG(LogServer, Log, TEXT("Session %s has been activated"), *sessionId);
		}
		else
		{
			auto error = outcome.GetError();
			UE_LOG(LogServer, Error, TEXT("Failed to activate game session %s: [%s] %s"), *sessionId, *error.m_errorName, *error.m_errorMessage);
		}
	});

	/**
	 * Heartbeat callback
	 * @return Server health status flag
	 */
	Params.OnHealthCheck.BindLambda([]() { return true; });

	/**
	 * Called when GameLift service needs to force server termination
	 */
	Params.OnTerminate.BindLambda([this]()
	{
		auto outcomeTime = GameLift->GetTerminationTime();
		FString date = "unknown";

		if (outcomeTime.IsSuccess() && outcomeTime.GetResult() > 0)
		{
			date = FDateTime::FromUnixTimestamp(outcomeTime.GetResult()).ToString();
		}

		UE_LOG(LogServer, Warning, TEXT("Received termination call with time: %s"), *date);

		auto outcomeEnd = GameLift->ProcessEnding();
		if (!outcomeEnd.IsSuccess())
		{
			auto error = outcomeEnd.GetError();
			UE_LOG(LogServer, Warning, TEXT("Failed to end process: [%s] %s"), *error.m_errorName, *error.m_errorMessage);
		}

		FGenericPlatformMisc::RequestExit(false);
	});
#endif // WITH_GAMELIFT
}

bool UServer::Start()
{
#if WITH_GAMELIFT
	UE_LOG(LogServer, Log, TEXT("Starting server..."));

	auto outcomeInit = GameLift->InitSDK();
	if (!outcomeInit.IsSuccess())
	{
		auto error = outcomeInit.GetError();
		UE_LOG(LogServer, Error, TEXT("Failed to initialize SDK: [%s] %s"), *error.m_errorName, *error.m_errorMessage);
		return false;
	}

	if (auto world = GetWorld())
	{
		Params.port = world->URL.Port;
	}

	auto outcomeReady = GameLift->ProcessReady(Params);
	if (!outcomeReady.IsSuccess())
	{
		auto error = outcomeReady.GetError();
		UE_LOG(LogServer, Error, TEXT("Failed to get ready: [%s] %s"), *error.m_errorName, *error.m_errorMessage);
		return false;
	}

	UE_LOG(LogServer, Log, TEXT("Server is up and running on %d"), Params.port);
	return true;
#else
	return false;
#endif // WITH_GAMELIFT
}

bool UServer::AcceptPlayerSession(const FString& PlayerSessionId)
{
#if WITH_GAMELIFT
	auto outcome = GameLift->AcceptPlayerSession(PlayerSessionId);
	if (!outcome.IsSuccess())
	{
		auto error = outcome.GetError();
		UE_LOG(LogServer, Warning, TEXT("Failed to accept player session %s: [%s] %s"), *PlayerSessionId, *error.m_errorName, *error.m_errorMessage);
		return false;
	}

	UE_LOG(LogServer, Log, TEXT("Accepted player session %s"), *PlayerSessionId);
	return true;
#else
	return false;
#endif // WITH_GAMELIFT
}

bool UServer::RemovePlayerSession(const FString& PlayerSessionId)
{
#if WITH_GAMELIFT
	auto outcome = GameLift->RemovePlayerSession(PlayerSessionId);
	if (!outcome.IsSuccess())
	{
		auto error = outcome.GetError();
		UE_LOG(LogServer, Warning, TEXT("Failed to remove player session %s: [%s] %s"), *PlayerSessionId, *error.m_errorName, *error.m_errorMessage);
		return false;
	}

	UE_LOG(LogServer, Log, TEXT("Removed player session %s"), *PlayerSessionId);
	return true;
#else
	return false;
#endif // WITH_GAMELIFT
}

bool UServer::TerminateGameSession()
{
#if WITH_GAMELIFT
	auto outcome = GameLift->TerminateGameSession();
	if (!outcome.IsSuccess())
	{
		auto error = outcome.GetError();
		UE_LOG(LogServer, Warning, TEXT("Failed to terminate game session: [%s] %s"), *error.m_errorName, *error.m_errorMessage);
		return false;
	}

	UE_LOG(LogServer, Log, TEXT("Terminated game session"));
	return true;
#else
	return false;
#endif // WITH_GAMELIFT
}
