// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "C_KartBase.generated.h"

UCLASS()
class KOTY_API AC_KartBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AC_KartBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void Stun();
	virtual void CheckIsGround();
	virtual void UpdateBodyRotation(float DeltaTime);
	virtual void SetVelocity();
	virtual void SetFlyVelocity();
	virtual void UpdateSuspension(float DeltaTime);

	void StartAddSpeed(float Add);
	void DriftUpAction();
	void PlayBoostEffect();
	void PlayDriftEffect(int EffectType, float DriftStartDir);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WheelL")
	UStaticMeshComponent* WheelL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WheelR")
	UStaticMeshComponent* WheelR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WheelB")
	UStaticMeshComponent* WheelB;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boost")
	TArray<USceneComponent*> ExhaustPipe;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drift")
	TArray<USceneComponent*> WheelBackDrift;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* BoostEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DriftEffect1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DriftEffect2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DriftEffect3;

	uint32 CurrentDriftType = 0;
	
	TArray<UNiagaraComponent*> ActiveBoostEffects;
	TArray<UNiagaraComponent*> ActiveDriftEffects;

	bool bIsBoosting = false;
	float BoostTimer = 0;
	float BoostDuration = 1.5f;
	
	//Acceleration
public:
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

	UPROPERTY(BlueprintReadWrite, Category = "Kart Settings")
	float AccelerationDir = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Kart Settings")
	float CurSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float MaxSpeed = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float AddSpeed = 0.f;

	float SpeedRatio = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kart Settings")
	float BrakePower = 1.f;

	FVector MeshMoveDirection;
	FRotator MeshRotationDirection = FRotator(0, -90.f, 0);
	
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

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Drift")
	float DriftHopImpulse = 200.f;

	FVector GroundNormal = FVector::UpVector;

	float AirTime = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GravityDirection = FVector(0.f, 0.f, -1.f);
	
protected:
	// Suspension
	float VerticalVelocity;

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Suspension")
	float SuspensionLength = 80.f; // 서스펜션의 최대 길이 (라인 트레이스 길이)

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Suspension")
	float SuspensionStiffness = 50.f; // 서스펜션 스프링의 단단함 (값이 클수록 더 강하게 밀어냄)

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Suspension")
	float SuspensionDamping = 5.f; // 서스펜션의 댐핑 (출렁거림을 줄여줌)

	float DriftTime = 0;
};
