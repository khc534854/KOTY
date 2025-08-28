// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicTrackingItemBase.h"
#include "Components/SphereComponent.h"

APhysicTrackingItemBase::APhysicTrackingItemBase(): DetectLength(2000), TrackingMode(ETrackingMode::Active)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//센서로 사용하는 스피어 컴포넌트 부착
	SensorComp = CreateDefaultSubobject<USphereComponent>(TEXT("SensorComp"));
	SensorComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SensorComp->SetupAttachment(GetRootComponent());
}

void APhysicTrackingItemBase::OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APhysicTrackingItemBase::OnHitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

