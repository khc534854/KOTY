// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyItemBase.h"

#include "KotyItemHitComponent.h"

AKotyItemBase::AKotyItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKotyItemBase::ApplyItemEffect(AActor* TargetActor)
{
	//하위 아이템에서 오버라이드하여 구체적인 기능 구현
}

void AKotyItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKotyItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

