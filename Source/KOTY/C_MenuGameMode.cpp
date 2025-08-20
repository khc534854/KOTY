// Fill out your copyright notice in the Description page of Project Settings.


#include "C_MenuGameMode.h"

#include "C_MenuController.h"

void AC_MenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentMenu = MenuType::StartMenu;

	DefaultPawnClass = nullptr;
	PlayerControllerClass = AC_MenuController::StaticClass();
}

void AC_MenuGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentMenu)
	{
		case MenuType::StartMenu:
			StartMenuState();
			break;
		case MenuType::ModeMenu:
			ModeMenuState();
			break;
		case MenuType::PlayerSelectMenu:
			PlayerSelectMenuState();
			break;
		case MenuType::KartSelectMenu:
			KartSelectMenuState();
			break;
		case MenuType::StageSelectMenu:
			StageSelectMenuState();
			break;
		default:
			break;
	}
	
}

void AC_MenuGameMode::ChangeMenu(MenuType NextMenu)
{
	BeforeMenu = CurrentMenu;
	CurrentMenu = NextMenu;
}

void AC_MenuGameMode::StartMenuState()
{
	
}

void AC_MenuGameMode::ModeMenuState()
{
}

void AC_MenuGameMode::PlayerSelectMenuState()
{
}

void AC_MenuGameMode::KartSelectMenuState()
{
}

void AC_MenuGameMode::StageSelectMenuState()
{
}
