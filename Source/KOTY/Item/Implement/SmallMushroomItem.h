// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicItemBase.h"
#include "SmallMushroomItem.generated.h"

UCLASS()
class KOTY_API ASmallMushroomItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	ASmallMushroomItem();

protected:
	virtual void BeginPlay() override;

	virtual void OnSimulateBegin() override;
	
	virtual void ApplyItemEffect(AActor* TargetActor) override;
	
};