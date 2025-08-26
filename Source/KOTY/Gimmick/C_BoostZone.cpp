// Fill out your copyright notice in the Description page of Project Settings.


#include "C_BoostZone.h"
#include "Components/BoxComponent.h"
#include "PlayerKart/C_PlayerKart.h"


// Sets default values
AC_BoostZone::AC_BoostZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectZone"));
	SetRootComponent(DetectZone);
}

// Called when the game starts or when spawned
void AC_BoostZone::BeginPlay()
{
	Super::BeginPlay();

	DetectZone->OnComponentBeginOverlap.AddDynamic(this, &AC_BoostZone::OnBoxCompOverlap);
}

void AC_BoostZone::OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* Kart = Cast<AC_PlayerKart>(OtherActor);
	if (Kart)
	{
		Kart->StartAddSpeed(500.f);
	}
}
