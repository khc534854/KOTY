// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicTrackingItemBase.h"
#include "RedTurtleItem.generated.h"

UCLASS()
class KOTY_API ARedTurtleItem : public APhysicTrackingItemBase
{
	GENERATED_BODY()

public:
	ARedTurtleItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
