// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KotyItemBase.generated.h"

UCLASS()
class KOTY_API AKotyItemBase : public AActor
{
	GENERATED_BODY()

public:
	AKotyItemBase();

	UFUNCTION()
	virtual void ApplyItemEffect(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> UseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> DestroySound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundAttenuation> SoundAttenuation;

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
};
