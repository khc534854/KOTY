// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AiKartController.generated.h"

UENUM(BlueprintType)
enum class ESide : uint8
{
	Left     UMETA(DisplayName="Left"),
	Right  UMETA(DisplayName="Right")
};

USTRUCT(BlueprintType)
struct FLineTraceFrontObstacleResult
{
	GENERATED_BODY()

	bool bHit;
	FVector HitLocation;
	bool NewObstacle;

};

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
	

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ESide KartSide;  
	//
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USplineComponent*> SplineComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Obstacles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DebugSteering = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DebugBrake = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DefaultSpline;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CurrentNearestSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OffsetTangentLocation;
	
	//차량의 MaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YawDelta;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KartMinSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KartMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayTime1Sec;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AActor*> SplineRoads;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakeInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBrake = false;


	//UpdateObstacleDetection()용 변수
	
	FVector ObstacleHitLocationL;
	FVector ObstacleHitLocationR;
	

	
	UFUNCTION()
	AActor* FindNearestSpline();

	UFUNCTION()
	float CalculateSteeringInput(AActor* Spline);

	UFUNCTION()
	float CalculateBrakeInput(AActor* Spline, FVector OriginLocation);

	// UFUNCTION()
	// FLineTraceFrontObstacleResult LineTraceFrontObstacle(ESide InKartSide);
	//
	// UFUNCTION()
	// void UpdateObstacleDetection();
	//
};
