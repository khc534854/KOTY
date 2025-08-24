// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBox.generated.h"

UCLASS()
class KOTY_API AItemBox : public AActor
{
	GENERATED_BODY()

public:
	AItemBox();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USphereComponent> SphereComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> BoxMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> FontMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APawn> PlayerPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotateAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> SelectSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> DestroySound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundAttenuation> SoundAttenuation;

public:
	virtual void Tick(float DeltaTime) override;
};
