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
	FVector MoveVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float Elasticity;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void Init(bool InbSimulate, const FVector& InGravityDir, float InGravityForce, float InLinearDrag);
};
