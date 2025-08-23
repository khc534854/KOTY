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

