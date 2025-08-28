// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "C_MenuGameMode.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class MenuType : uint8
{
	StartMenu,
	ModeMenu,
	PlayerSelectMenu,
	KartSelectMenu,
	StageSelectMenu,
};


UCLASS()
class KOTY_API AC_MenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ChangeMenu(MenuType NextMenu);

	void StartMenuState();
	void ModeMenuState();
	void PlayerSelectMenuState();
	void KartSelectMenuState();
	void StageSelectMenuState();

public:
	UPROPERTY(BlueprintReadOnly)
	MenuType CurrentMenu;
	
	UPROPERTY(BlueprintReadOnly)
	MenuType BeforeMenu;
	
};
