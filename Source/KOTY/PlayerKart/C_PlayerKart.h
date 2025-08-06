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
	void DegreaseAccelerationTime();
	void Suspension(USceneComponent* Component);
	
	
	
	void StartAccelerator(const FInputActionValue& Value);
	void EndAccelerator(const FInputActionValue& Value);
	

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Status")
	TArray<USceneComponent*> WheelsComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* AccelerationAction;

	bool bIsAcceleration;
	float AccelerationTime = 0;
	float MaxAccelerationTime = 0;
	float CurSpeed = 0.f;
	float MaxSpeed = 3000.f;
	float Acceleration = 1.f;
	float BrakePower = 2.f;
};
