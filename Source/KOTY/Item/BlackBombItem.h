// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicItemBase.h"
#include "BlackBombItem.generated.h"

UCLASS()
class KOTY_API ABlackBombItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABlackBombItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
