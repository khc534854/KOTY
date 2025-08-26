// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PhysicItemBase.h"
#include "BananaItem.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API ABananaItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	ABananaItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyItemEffect(AActor* TargetActor) override;
	
};
