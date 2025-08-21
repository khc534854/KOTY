// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicItemBase.h"
#include "PhysicTrackingItemBase.generated.h"

UENUM(BlueprintType)
enum class ETrackingMode : uint8
{
	Active,
	Inactive,
};

UCLASS()
class KOTY_API APhysicTrackingItemBase : public APhysicItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APhysicTrackingItemBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USplineComponent> SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETrackingMode TrackingMode;
	
public:
	virtual void Tick(float DeltaTime) override;
};
