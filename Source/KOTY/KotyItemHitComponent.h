// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/GreenTurtleItem.h"
#include "KotyItemHitComponent.generated.h"

//델리게이트 선언
DECLARE_DYNAMIC_DELEGATE_OneParam(FItemEffect, AActor*, TargetActor); 

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTY_API UKotyItemHitComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UKotyItemHitComponent();

protected:
	virtual void BeginPlay() override;		                                         
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void OnRequestApplyEffectFromItem(FItemEffect ItemEffectDelegate, AActor* OtherItem) const;
};
