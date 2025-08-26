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

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void ApplyItemEffect(AActor* OtherActor) override;

	virtual void OnSimulateBegin() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RotationDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> MovingSound;

public:
	virtual void Tick(float DeltaTime) override;
};
