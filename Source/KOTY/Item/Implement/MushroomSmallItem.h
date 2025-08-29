// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicItemBase.h"
#include "MushroomSmallItem.generated.h"

UCLASS()
class KOTY_API AMushroomSmallItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	AMushroomSmallItem();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void OnSimulateBegin() override;
	
	virtual void ApplyItemEffect(AActor* TargetActor) override;

	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp) override;

	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp) override;
	
};