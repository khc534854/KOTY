// Fill out your copyright notice in the Description page of Project Settings.


#include "C_RaceGameMode.h"

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
	
}

void AC_RaceGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentState == RaceLevelState::End)
	{
		//GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
	}
}
