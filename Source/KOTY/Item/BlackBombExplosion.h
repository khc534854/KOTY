// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KotyItemBase.h"
#include "BlackBombExplosion.generated.h"

UCLASS()
class KOTY_API ABlackBombExplosion : public AKotyItemBase
{
	GENERATED_BODY()

public:
	ABlackBombExplosion();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void ApplyItemEffect(AActor* TargetActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> HitComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> MaterialInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTimelineComponent> TimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionDuration = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCurveFloat> CurveFloat;
	
	UFUNCTION()
	void UpdateExplosionAlpha(float Value);

	UFUNCTION()
	void EndExplosion();
};
