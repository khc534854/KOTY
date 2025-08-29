// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AiKartController.generated.h"

UCLASS()
class KOTY_API AAiKartController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAiKartController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<class USplineComponent*> SplineComponents;

	UPROPERTY(EditAnywhere)
	bool DebugSteering = true;

	UPROPERTY(EditAnywhere)
	bool DebugBrake = true;
	
	UPROPERTY(EditAnywhere)
	AActor* DefaultSpline;
	
	UPROPERTY(EditAnywhere)
	AActor* CurrentNearestSpline;

	UPROPERTY(EditAnywhere)
	float DefaultMaxSpeed;

	UPROPERTY(EditAnywhere)
	FVector OffsetTangentLocation;
	
	//차량의 MaxSpeed;
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 60.f;

	UPROPERTY(EditAnywhere)
	float YawDelta;

	UPROPERTY(EditAnywhere)
	float KartMinSpeed;
	
	UPROPERTY(EditAnywhere)
	float KartMaxSpeed;

	UPROPERTY(EditAnywhere)
	float TargetSpeed;

	UPROPERTY(EditAnywhere)
	float DelayTime1Sec;
	
	UPROPERTY(EditAnywhere)
	TArray<class AActor*> SplineRoads;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakeInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBrake = false;
	
	UFUNCTION()
	AActor* FindNearestSpline();

	UFUNCTION()
	float CalculateSteeringInput(AActor* Spline);

	UFUNCTION()
	float CalculateBrakeInput(AActor* Spline, FVector OriginLocation);

	
};
