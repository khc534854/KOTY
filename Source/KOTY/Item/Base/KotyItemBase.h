// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KotyItemBase.generated.h"

//전방 선언
class UKotyItemHoldComponent;

UCLASS()
class KOTY_API AKotyItemBase : public AActor
{
	GENERATED_BODY()

public:
	AKotyItemBase();

	UFUNCTION()
	virtual void ApplyItemEffect(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> UseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> DestroySound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundAttenuation> SoundAttenuation;

	UFUNCTION(BlueprintCallable)
	virtual void OnUseItem(UKotyItemHoldComponent* HoldComp);

	UFUNCTION(BlueprintCallable)
	virtual void OnLoseItem(UKotyItemHoldComponent* HoldComp);

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
};
