// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicItemBase.h"
#include "BananaItem.generated.h"

UCLASS()
class KOTY_API ABananaItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	ABananaItem();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
public:
	virtual void ApplyItemEffect(AActor* TargetActor) override;

	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp) override;

	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp) override;
	
};