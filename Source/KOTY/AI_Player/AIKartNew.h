// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "AIKartNew.generated.h"

UCLASS()
class KOTY_API AAIKartNew : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIKartNew();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	//박스 콜라이더
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;
	
	//메쉬 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> BodyMeshComp;

	//오른쪽 앞 바퀴
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USceneComponent> FRWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> FRWheelMesh;
	
	//왼쪽 앞 바퀴
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USceneComponent> FLWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> FLWheelMesh;
	
	//오른쪽 뒷 바퀴
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USceneComponent> BRWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> BRWheelMesh;
	
	//왼쪽 뒷 바퀴
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USceneComponent> BLWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> BLWheelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DrawDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WheelTouchingGround = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuspensionForceMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WheelMeshLocation;

	
	//자동차가 공중에 떠있을 때 서스펜션이 아무런 저항없이 늘어져있는 기본 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RestDistance = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringStrength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringDamper = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SteeringInput = 0.f;           // 현재 입력값 (-1 ~ 1)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelInput = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSteeringAngle = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CarTopSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Engine")
	class UCurveFloat* PowerCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Engine")
	class UCurveFloat* SteeringCurve;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;          // 스프링암

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;           // 추적 카메라

	// UPROPERTY()
	// TObjectPtr<class UInputMappingContext> AiKartMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInputMappingContext* AiKartMappingContext;
	
	UPROPERTY()
	class UInputAction* IA_Accelerate;

	UPROPERTY()
	class UInputAction* IA_Steer;
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelMeshRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NormalizedCurrentSpeed;
	
	UFUNCTION(BlueprintCallable)
	virtual void OnAccelerate(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	virtual void OnAccelerateReleased(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	virtual void OnSteer(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	virtual void OnSteerReleased(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable)
	virtual void SimulateSuspension(USceneComponent* InSuspensionStartPos, UStaticMeshComponent* WheelMesh);

	UFUNCTION(BlueprintCallable)
	virtual void SimulateSteering(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh);

	// UFUNCTION(BlueprintCallable)
	// virtual void SimulateReverse(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh);
	
	UFUNCTION(BlueprintCallable)
	virtual void SimulateFriction(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh);

	
	UFUNCTION(BlueprintCallable)
	virtual void SimulateAccelerate(USceneComponent* Wheel, UStaticMeshComponent* WheelMesh);
};
