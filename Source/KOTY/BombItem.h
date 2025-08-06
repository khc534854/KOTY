// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombItem.generated.h"

UCLASS()
class KOTY_API ABombItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABombItem();

	UFUNCTION(BlueprintCallable)
	void Init(
		const FVector InShootDir,
		const float InSpeed,
		const FVector InGravityDir,
		const float InGravityForce,
		const float InDrag,
		const float InLifeSpan);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bActive;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FVector GravityDir;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float GravityForce;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float MoveHorizonSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float Drag;

	//360도를 얼마나 분할하여 트레이스 하는지 - 정확히 나눠떨어져야 한다
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int CollisionPrecision = 36;
	
	//동일 지점의 연속 충돌을 방지하기 위한 콜리전 카운터 맵
	TArray<int> CollisionArray;
	
	UFUNCTION(BlueprintCallable)
	void AdjustVelocityToHorizon();
	
	UFUNCTION(BlueprintCallable)
	void ApplyReflection();

	UFUNCTION(BlueprintCallable)
	void ApplyBounce();

	UFUNCTION(BlueprintCallable)
	void ApplyDrag();

	UFUNCTION(BlueprintCallable)
	void UpdateLocation();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
