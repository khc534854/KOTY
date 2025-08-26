// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicItemBase.h"
#include "CoinItem.generated.h"

UCLASS()
class KOTY_API ACoinItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	ACoinItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyItemEffect(AActor* TargetActor) override;
	
};