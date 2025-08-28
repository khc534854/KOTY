// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/KotyItemBase.h"
#include "ItemBox.generated.h"

UCLASS()
class KOTY_API AItemBox : public AKotyItemBase
{
	GENERATED_BODY()

public:
	AItemBox();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void Tick(float DeltaTime) override;

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
	EItemList ItemCode;
	
};