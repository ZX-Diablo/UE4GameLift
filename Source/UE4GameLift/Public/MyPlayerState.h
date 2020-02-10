// Copyright ZX.Diablo 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

UCLASS()
class UE4GAMELIFT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString PlayerSessionId;
};
