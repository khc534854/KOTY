// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "C_MathUtility.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API UC_MathUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static float EaseOutElastic(float x);

	static float EaseOutBounce(float x);
};
