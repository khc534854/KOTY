// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PhysicItemBase.h"
#include "StarItem.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API AStarItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	AStarItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyItemEffect(AActor* TargetActor) override;
	
};
