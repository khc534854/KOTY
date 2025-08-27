// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Item/Base/PhysicItemBase.h"
#include "BlackBombItem.generated.h"

UCLASS()
class KOTY_API ABlackBombItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	ABlackBombItem();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void OnSimulateBegin() override;

	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp) override;

	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> MaterialInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTimelineComponent> TimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCurveFloat> CurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ExplosionActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionDelay = 4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WarningDelay = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle ExplosionTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> BombWarningSound;
	
	UFUNCTION()
	void Explode();
	
	UFUNCTION()
	void SetElapsedTime(float TwistedTime) const;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};