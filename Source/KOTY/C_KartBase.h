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
	UFUNCTION(BlueprintCallable)
	virtual void Stun();

	virtual void CheckIsGround();
	virtual void UpdateBodyRotation(float DeltaTime);
	virtual void SetVelocity();
	virtual void SetFlyVelocity();
	virtual void UpdateSuspension(float DeltaTime);

	//void UpdateRotation();

	void HandleCollision(const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable)
	void StartAddSpeed(float Add);
	void DriftUpAction();
	void PlayBoostEffect();
	void PlayDriftEffect(int EffectType, float DriftStartDirEffect);
	void UpdateStunEffect(float DeltaTime);

	//UFUNCTION(BlueprintCallable)
	//int32 FindClosestSplinePointIndex(const FVector& WorldLocation);

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	class USphereComponent* BoxComponent;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
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
	
	float DriftStartDir;
	
	UPROPERTY(EditAnywhere, Category = "Kart Settings|Physics")
	float CollisionDampingFactor = 0.4f;

	bool bIsInPostCollisionState = false; // 현재 충돌 후 상태인지 여부
	FRotator PostCollisionTargetRotation;   // 충돌 후 따라가야 할 목표 회전값

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Physics")
	float CollisionRotationSpeed = 6.0f;

	//Stun
	bool bIsStunned = false;      // 현재 스턴 상태인지 여부
	float StunRotationTimer = 0.f; // 스턴 애니메이션 경과 시간

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Stun")
	float StunDuration = 0.8f;  // 스턴 메시가 한 바퀴 도는 데 걸리는 시간

	UPROPERTY(EditAnywhere, Category = "Kart Settings|Stun")
	float StunSpeedMultiplier = 0.2f;

	
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

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//AActor* WorldSplineActor;
	//
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class USplineComponent* SplineComponent;
	//
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//int32 MaxProgressPointIndex = 0;
};
