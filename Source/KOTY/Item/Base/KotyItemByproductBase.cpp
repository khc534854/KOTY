// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyItemByproductBase.h"


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

void AKotyItemByproductBase::RequestApplyItemEffectToOtherHitComp(const UKotyItemHitComponent* TargetHitComp)
{
	
}

void AKotyItemByproductBase::ApplyItemEffect(AActor* TargetActor)
{
	
}

