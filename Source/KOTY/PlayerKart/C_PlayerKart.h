// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
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
class KOTY_API AC_PlayerKart : public APawn
{
	GENERATED_BODY()

public:
	AC_PlayerKart();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SetVelocity();
	void CheckIsGround();
	void CameraMove();
	void UpdateSuspension(float DeltaTime);
	void UpdateBodyRotation(float DeltaTime);
	
	//input
	void StartAccelerator(const FInputActionValue& Value);
	void EndAccelerator(const FInputActionValue& Value);
	void HandlingStart(const FInputActionValue& Value);
	void HandlingEnd(const FInputActionValue& Value);
	void DriftStart(const FInputActionValue& Value);
	void DriftEnd(const FInputActionValue& Value);

	
protected:
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;
	
	// Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TArray<USceneComponent*> WheelsComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* AccelerationAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* HandlingAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* DriftAction;

	
	//Acceleration

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Physics")
	float AccelerationForce = 3500.f; // 가속 힘

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Physics")
	float FrictionConstant = 1000.f; // 마찰력 상수 (일정한 저항)

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Physics")
	float ForwardFriction = 0.8f;
	
	UPROPERTY(EditAnywhere, Category = "Kart Settings|Physics")
	float SidewaysFriction = 15.0f; 

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Drift")
	float DriftSidewaysFriction = 4.0f; 
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsAcceleration;

	UPROPERTY(BlueprintReadOnly)
	FVector CurVelocity;

	UPROPERTY(BlueprintReadOnly)
	FVector DestDirection;
	
	UPROPERTY(BlueprintReadOnly)
	FVector BeforeDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float AccelerationTime = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Kart Settings")
	float MaxAccelerationTime = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Kart Settings")
	float AccelerationDir = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Kart Settings")
	float CurSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float MaxSpeed = 3000.f;

	float SpeedRatio = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float Acceleration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float BrakePower = 1.f;

	// Handling

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float HandlingDir = 0;

	UPROPERTY(EditAnywhere, Category = "Kart Settings")
	float TurnSpeed = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> VelocityHistory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float HandlingValue = 1.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHandling = false;

	// Drift
	UPROPERTY(BlueprintReadOnly)
	bool bIsDrift = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsGround;

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Visual")
	float BodyRotationInterpolationSpeed = 5.0f;

	FVector GroundNormal = FVector::UpVector;

	// Suspension
	float VerticalVelocity;

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Suspension")
	float SuspensionLength = 80.f; // 서스펜션의 최대 길이 (라인 트레이스 길이)

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Suspension")
	float SuspensionStiffness = 50.f; // 서스펜션 스프링의 단단함 (값이 클수록 더 강하게 밀어냄)

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Suspension")
	float SuspensionDamping = 5.f; // 서스펜션의 댐핑 (출렁거림을 줄여줌)
	
};
