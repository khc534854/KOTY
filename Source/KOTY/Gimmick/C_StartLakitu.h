﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_StartLakitu.generated.h"

UCLASS()
class KOTY_API AC_StartLakitu : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AC_StartLakitu();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StartLakitu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* StartLakituRed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* StartLakituGreen;
};
