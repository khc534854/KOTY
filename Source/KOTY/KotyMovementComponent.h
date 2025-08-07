// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "KotyMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTY_API UKotyMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	bool bSimulate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector GravityDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float GravityForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float LinearDrag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector MoveDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float MoveSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector MoveVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SurfaceElasticity;
	
	UFUNCTION(BlueprintCallable)
    FVector FindTrackSurface(float TraceRadius = 100.f, int NumRays = 32) const;
	
public:
	virtual void InitializeComponent() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Init(
		const bool InbSimulate,
		const FVector& InMoveVelocity,
		const FVector& InGravityDir,
		const float InGravityForce,
		const float InLinearDrag,
		const float InSurfaceElasticity);
};
