// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "KotyMovementComponent.generated.h"

class USphereComponent;

//델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnBounce);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(Display="ActiveConstantHorizonSpeed"))
	bool bConstantHorizonSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(EditCondition="bConstantHorizonSpeed"))
	float ConstantHorizonSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float StepUpLimit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SurfaceElasticity;

	UPROPERTY()
	USphereComponent* SphereComp;
	
	UFUNCTION(BlueprintCallable)
    FHitResult LineTraceGravityDirTrack(FVector Start) const;
	
public:
	virtual void InitializeComponent() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ThrowConstantHorizon(
		const bool InbSimulate,
		const FVector InGravityDir,
		const float InGravityForce,
		const float InConstantHorizonSpeed,
		const float InStepUpLimit,
		const FVector InVelocity,
		const float InSurfaceElasticity);

	UFUNCTION(BlueprintCallable)
	void ThrowLinearDrag(
		const bool InbSimulate,
		const FVector InGravityDir,
		const float InGravityForce,
		const float InLinearDrag,
		const float InStepUpLimit,
		const FVector InVelocity,
		const float InSurfaceElasticity);
	
	UFUNCTION(BlueprintCallable)
	void SLerpVelocity(FVector TargetDir);

	UFUNCTION(BlueprintCallable)
	FVector GetGravityDir() const;
	
	FOnBounce OnBounceEventDispatcher;
};
