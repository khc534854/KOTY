// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicTrackingItemBase.h"
#include "RedTurtleItem.generated.h"

UCLASS()
class KOTY_API ARedTurtleItem : public APhysicTrackingItemBase
{
	GENERATED_BODY()

public:
	ARedTurtleItem();

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnHitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	FTimerHandle FindTargetHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RotationDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> MovingSound;

	virtual void OnSimulateBegin() override;

public:
	virtual void ApplyItemEffect(AActor* OtherActor) override;

	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp) override;

	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp) override;
	
};