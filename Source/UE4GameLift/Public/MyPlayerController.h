// Copyright ZX.Diablo 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UCLASS()
class UE4GAMELIFT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void HostGameSession();

	UFUNCTION(Exec)
	void JoinGameSession(const FString& SessionId);
};
