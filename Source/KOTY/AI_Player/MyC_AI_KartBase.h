// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_KartBase.h"
#include "MyC_AI_KartBase.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API AMyC_AI_KartBase : public AC_KartBase
{
	GENERATED_BODY()
	
	virtual void Stun();
	virtual void CheckIsGround();
	virtual void UpdateBodyRotation(float DeltaTime);
	virtual void SetVelocity();
	virtual void UpdateSuspension(float DeltaTime);
};
