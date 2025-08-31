// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/PhysicItemBase.h"
#include "StarItem.generated.h"

UCLASS()
class KOTY_API AStarItem : public APhysicItemBase
{
	GENERATED_BODY()

public:
	AStarItem();

protected:
	virtual void BeginPlay() override;

	virtual void OnSimulateBegin() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAudioComponent> SpawnedAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTimelineComponent> TimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> CurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterial> RainbowMaterial;

	UFUNCTION()
	void UpdateStarSoundVolume(float Value) const;
	
public:
	
	virtual void ApplyItemEffect(AActor* TargetActor) override;

	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp) override;

	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp) override;
	
};
