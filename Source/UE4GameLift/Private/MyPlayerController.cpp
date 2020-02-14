// Copyright ZX.Diablo 2020

#include "MyPlayerController.h"

#include "Client.h"
#include "MyGameInstance.h"

void AMyPlayerController::HostGameSession()
{
	if (auto gameInstance = GetGameInstance<UMyGameInstance>())
	{
		gameInstance->Client->HostSession();
	}
}

void AMyPlayerController::JoinGameSession(const FString& SessionId)
{
	if (auto gameInstance = GetGameInstance<UMyGameInstance>())
	{
		gameInstance->Client->TryJoinSession(SessionId);
	}
}
