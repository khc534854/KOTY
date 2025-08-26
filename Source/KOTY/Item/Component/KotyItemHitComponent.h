// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KotyItemHitComponent.generated.h"

//델리게이트 선언
DECLARE_DYNAMIC_DELEGATE_OneParam(FItemEffect, AActor*, TargetActor);

//피아식별 열거형 선언
UENUM()
enum class EItemEffectType : uint8
{
	Ally = 0,
	Enemy = 1,
	Neutral = 2,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTY_API UKotyItemHitComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UKotyItemHitComponent();

protected:
	virtual void BeginPlay() override;		                                         
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;
	
public:	
	UFUNCTION(BlueprintCallable)
	void OnRequestApplyEffectFromItem(FItemEffect ItemEffectDelegate, AActor* OtherItem) const;
};