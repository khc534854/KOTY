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

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void ApplyItemEffect(AActor* TargetActor) override;

	virtual void OnSimulateBegin() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RotationDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> MovingSound;

public:
	virtual void Tick(float DeltaTime) override;
	
};
