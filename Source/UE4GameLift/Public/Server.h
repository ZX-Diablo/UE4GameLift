// Copyright ZX.Diablo 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameLiftServerSDK.h"

#include "Server.generated.h"

UCLASS()
class UE4GAMELIFT_API UServer : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Load GameLiftServerSDK and make initial params setup
	 */
	UServer();

	/**
	 * Initialization and server start
	 * @note If World is available then will launch on the current port otherwise on the default port
	 * @return Success flag
	 */
	bool Start();

	/**
	 * Validate player session on GameLift side
	 * @param PlayerSessionId Player session ID given by GameLift
	 * @return Success flag
	 */
	bool AcceptPlayerSession(const FString& PlayerSessionId);

	/**
	 * Notify GameLift that player session has ended
	 * @param PlayerSessionId Player session ID given by GameLift
	 * @return Success flag
	 */
	bool RemovePlayerSession(const FString& PlayerSessionId);

	/**
	 * Notify GameLift that game session has ended
	 * @note Server continues to work and can host new game session
	 * @return Success flag
	 */
	bool TerminateGameSession();

private:
	FGameLiftServerSDKModule* GameLift;
	FProcessParameters Params;
};
