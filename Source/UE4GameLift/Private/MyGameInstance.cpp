// Copyright ZX.Diablo 2020

#include "MyGameInstance.h"

#include "Client.h"
#include "Server.h"

UMyGameInstance::UMyGameInstance()
{
	Server = CreateDefaultSubobject<UServer>(TEXT("Server"));
	Client = CreateDefaultSubobject<UClient>(TEXT("Client"));
}
