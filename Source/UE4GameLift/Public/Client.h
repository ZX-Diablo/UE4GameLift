// Copyright ZX.Diablo 2020

#pragma once

#include "CoreMinimal.h"
#include "GameLiftClientTypes.h"
#include "UObject/NoExportTypes.h"
#include "Client.generated.h"

class UGameLiftClientObject;
class UGameLiftCreateGameSession;
class UGameLiftCreatePlayerSession;
class UGameLiftDescribeGameSessionDetails;

UCLASS(Config = Game)
class UE4GAMELIFT_API UClient : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Initialize client
	 * @return Success flag
	 */
	bool Init();

	/**
	 * Create new game session on GameLift side
	 * @return Request success flag
	 */
	bool HostSession();

	/**
	 * Check session status, join it if active, wait for it if activating
	 * @see JoinSession
	 * @param SessionId Session ID to join
	 * @return Request success flag
	 */
	bool TryJoinSession(const FString& SessionId);

	/**
	 * Join given session (create player session and travel to server)
	 * @param SessionId Session ID to join
	 * @return Request success flag
	 */
	bool JoinSession(const FString& SessionId);

private:
	UFUNCTION()
	void OnCreateGameSessionSuccess(const FGameLiftCreateGameSessionResult& GameSessionResult);

	UFUNCTION()
	void OnCreateGameSessionFailed(const FString& ErrorMessage);

	UFUNCTION()
	void OnDescribeGameSessionSuccess(const FString& SessionId, const FString& SessionState, const int& CurrentPlayerCount, const int& MaxPlayerCount, const FString& PlayerSessionCreationPolicy);

	UFUNCTION()
	void OnDescribeGameSessionFailed(const FString& ErrorMessage);

	UFUNCTION()
	void OnCreatePlayerSessionSuccess(const FString& Ip, const FString& Port, const FString& PlayerSessionId, const FString& PlayerSessionStatus);

	UFUNCTION()
	void OnCreatePlayerSessionFailed(const FString& ErrorMessage);

private:
	UPROPERTY(Transient)
	UGameLiftClientObject* GameLift;

	UPROPERTY(Config)
	FString Key;

	UPROPERTY(Config)
	FString Secret;

	UPROPERTY(Config)
	FString Region;

	UPROPERTY(Config)
	FString AliasId;

	UPROPERTY()
	UGameLiftCreateGameSession* GameLiftCreateGameSession;

	UPROPERTY()
	UGameLiftDescribeGameSessionDetails* GameLiftDescribeGameSessionDetails;

	UPROPERTY()
	UGameLiftCreatePlayerSession* GameLiftCreatePlayerSession;
};
