// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicItemBase.h"
#include <Components/SphereComponent.h>
#include "KotyMovementComponent.h"

// Sets default values
APhysicItemBase::APhysicItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//충돌체를 루트 컴포넌트로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(FName("BlockAll"), true);
	SphereComp->SetSphereRadius(70);
	SphereComp->ShapeColor = FColor::Blue;
	SetRootComponent(SphereComp);

	//적중을 감지하는 콜리전 컴포넌트 부착
	HitComp = CreateDefaultSubobject<USphereComponent>(TEXT("HitComp"));
	HitComp->SetCollisionProfileName(FName("OverlapAll"), true);
	HitComp->SetSphereRadius(80);
	SphereComp->ShapeColor = FColor::Red;
	HitComp->SetupAttachment(RootComponent);

	//아이템 무브먼트 컴포넌트 부착
	MoveComp = CreateDefaultSubobject<UKotyMovementComponent>(TEXT("KotyMovement"));
}