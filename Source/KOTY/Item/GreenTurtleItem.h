// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicItemBase.h"
#include "GreenTurtleItem.generated.h"

UCLASS()
class KOTY_API AGreenTurtleItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	AGreenTurtleItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
