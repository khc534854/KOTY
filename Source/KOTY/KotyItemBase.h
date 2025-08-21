// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KotyItemBase.generated.h"

UCLASS()
class KOTY_API AKotyItemBase : public AActor
{
	GENERATED_BODY()

public:
	AKotyItemBase();

	virtual void ApplyItemEffect(AActor* TargetActor);
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
