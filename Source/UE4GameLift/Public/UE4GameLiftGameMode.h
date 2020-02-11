// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE4GameLiftGameMode.generated.h"

UCLASS(MinimalAPI)
class AUE4GameLiftGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUE4GameLiftGameMode();

	/**
	 * Validate player session
	 * @see UServer::AcceptPlayerSession
	 */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/**
	 * Remove player session and shutdown game session if there're no players left
	 * @see UServer::RemovePlayerSession, UServer::TerminateGameSession
	 */
	virtual void Logout(AController* Exiting) override;

protected:
	/**
	 * Setup player state (set player session ID)
	 */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	/**
	 * Start dedicated server
	 * @see UServer::Start
	 */
	virtual void BeginPlay() override;
};
