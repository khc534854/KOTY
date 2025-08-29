// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "C_KartBase.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "C_PlayerKart.generated.h"

using namespace std;

UCLASS()
class KOTY_API AC_PlayerKart : public AC_KartBase
{
	GENERATED_BODY()

public:
	AC_PlayerKart();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void SetVelocity() override;
	virtual void CheckIsGround() override;
	virtual void UpdateBodyRotation(float DeltaTime) override;
	virtual void Stun() override;
	
	void CameraMove();
	virtual void UpdateSuspension(float DeltaTime) override;
	
	//input
	void StartAccelerator(const FInputActionValue& Value);
	void EndAccelerator(const FInputActionValue& Value);
	void HandlingStart(const FInputActionValue& Value);
	void HandlingEnd(const FInputActionValue& Value);
	void DriftStart(const FInputActionValue& Value);
	void DriftEnd(const FInputActionValue& Value);
	void Mushroom(const FInputActionValue& Value);
	void UseItem(const FInputActionValue& Value);


	// // spline
	// UFUNCTION(BlueprintCallable)
	// int32 FindClosestSplinePointIndex(const FVector& WorldLocation);
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// AActor* WorldSplineActor;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// class USplineComponent* SplineComponent;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 MaxProgressPointIndex = 0;
	
public:
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* EndCamera;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	// TArray<USceneComponent*> WheelsComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* AccelerationAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* HandlingAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* DriftAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* AddSpeedAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* UseItemAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DriveMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* ThrowMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* WinMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* LoseMontage;

};
