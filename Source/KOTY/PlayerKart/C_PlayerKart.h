// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

	void SetCurrentSpeed();
	void SetDirection();
	void DegreaseAccelerationTime();
	void Suspension(USceneComponent* Component);
	void CheckIsGround();
	
	//input
	void StartAccelerator(const FInputActionValue& Value);
	void EndAccelerator(const FInputActionValue& Value);
	void HandlingStart(const FInputActionValue& Value);
	void HandlingEnd(const FInputActionValue& Value);
	void Drift(const FInputActionValue& Value);

	
protected:
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
	UPROPERTY(BlueprintReadOnly)
	bool bIsAcceleration;

	UPROPERTY(BlueprintReadOnly)
	FVector CurDirection;

	UPROPERTY(BlueprintReadOnly)
	FVector DestDirection;
	
	UPROPERTY(BlueprintReadOnly)
	FVector BeforeDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelerationTime = 0;
	UPROPERTY(BlueprintReadOnly)
	float MaxAccelerationTime = 0;
	
	UPROPERTY(BlueprintReadOnly)
	float CurSpeed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakePower = 1.f;

	// Handling
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HandlingValue = 1.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHandling = false;

	// Drift
	UPROPERTY(BlueprintReadOnly)
	bool bIsDrift = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsGround;
};
