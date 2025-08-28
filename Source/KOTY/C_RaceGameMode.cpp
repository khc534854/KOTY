// Fill out your copyright notice in the Description page of Project Settings.


#include "C_RaceGameMode.h"

#include "C_KotySaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerKart/C_PlayerController.h"
#include "PlayerKart/C_PlayerKart.h"

AC_RaceGameMode::AC_RaceGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Koty/PlayerKart/BPC_PlayerKart_C"));
	if (PlayerPawnBPClass.Succeeded())
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Koty/PlayerKart/BPC_RaceController_C"));

	if (PlayerControllerBPClass.Succeeded())
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

void AC_RaceGameMode::BeginPlay()
{
	Super::BeginPlay();
	MyLoadGame();
}

void AC_RaceGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void AC_RaceGameMode::MySaveGame()
{
	auto* instance = Cast<UC_KotySaveGame>(UGameplayStatics::CreateSaveGameObject(UC_KotySaveGame::StaticClass()));

	if (instance)
	{
		instance->Save_BestTime = BestTime;
		bool bSuccess = UGameplayStatics::SaveGameToSlot(instance, SaveSlotName, SaveUserindex);

	}
}

void AC_RaceGameMode::MyLoadGame(int32 defaultHighScore)
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserindex))
	{
		BestTime = defaultHighScore;
		return;
	}
	auto* instance = Cast<UC_KotySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserindex));

	BestTime = instance->Save_BestTime;
}
