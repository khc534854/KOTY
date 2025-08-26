// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicItemBase.h"
#include "BigMushroomItem.generated.h"

UCLASS()
class KOTY_API ABigMushroomItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	ABigMushroomItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyItemEffect(AActor* TargetActor) override;
	
};