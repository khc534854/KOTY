// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_MenuController.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API AC_MenuController : public APlayerController
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* StartMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* ModeMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* PlayerSelectMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* KartSelectMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* StageSelectMenuWidget;
	
};
