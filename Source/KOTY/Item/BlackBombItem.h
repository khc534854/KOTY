// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicItemBase.h"
#include "Components/TimelineComponent.h"
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

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void ApplyItemEffect(AActor* TargetActor) override;

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
	
	UFUNCTION()
	void Explode();
	
	UFUNCTION()
	void SetElapsedTime(float TwistedTime) const;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
