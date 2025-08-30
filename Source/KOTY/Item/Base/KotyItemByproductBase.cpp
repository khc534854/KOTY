// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyItemByproductBase.h"

#include "Item/Component/KotyItemHitComponent.h"


// Sets default values
AKotyItemByproductBase::AKotyItemByproductBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AKotyItemByproductBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKotyItemByproductBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKotyItemByproductBase::RequestApplyItemEffectToOtherHitComp(UKotyItemHitComponent* TargetHitComp)
{
	//델리게이트 전달
	FItemEffect ItemEffectDelegate;
	ItemEffectDelegate.BindDynamic(this, &AKotyItemByproductBase::ApplyItemEffect);
	TargetHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
}

void AKotyItemByproductBase::ApplyItemEffect(AActor* TargetActor)
{
	
}

