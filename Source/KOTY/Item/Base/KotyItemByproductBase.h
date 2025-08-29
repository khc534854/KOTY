// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KotyItemByproductBase.generated.h"

class UKotyItemHitComponent;

UCLASS()
class KOTY_API AKotyItemByproductBase : public AActor
{
	GENERATED_BODY()

public:
	AKotyItemByproductBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAudioComponent> AudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundAttenuation> SoundAttenuation;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RequestApplyItemEffectToOtherHitComp(const UKotyItemHitComponent* TargetHitComp);

	UFUNCTION()
	virtual void ApplyItemEffect(AActor* TargetActor);
};
