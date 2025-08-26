// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_BoostZone.generated.h"

UCLASS()
class KOTY_API AC_BoostZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AC_BoostZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* DetectZone;
};
