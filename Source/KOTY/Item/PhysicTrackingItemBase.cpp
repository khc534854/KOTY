// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicTrackingItemBase.h"

APhysicTrackingItemBase::APhysicTrackingItemBase(): DetectLength(2000), TrackingMode(ETrackingMode::Active)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APhysicTrackingItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void APhysicTrackingItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

