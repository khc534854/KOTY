// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KotyItemBase.h"
#include "PhysicItemBase.generated.h"

UCLASS()
class KOTY_API APhysicItemBase : public AKotyItemBase
{
	GENERATED_BODY()

public:
	APhysicItemBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> SphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> HitComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UKotyMovementComponent> MoveComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> GroundHitSoundCue;

	UFUNCTION(BlueprintCallable)
	virtual void OnSimulateBegin();
};
