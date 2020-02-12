// Copyright ZX.Diablo 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UClient;
class UServer;

UCLASS()
class UE4GAMELIFT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMyGameInstance();

public:
	UPROPERTY(Transient)
	UServer* Server;

	UPROPERTY(Transient)
	UClient* Client;
};
