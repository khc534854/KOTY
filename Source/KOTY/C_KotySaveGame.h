// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "C_KotySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API UC_KotySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	float Save_BestTime;
};
