// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EndZone.h"

#include "C_RaceGameMode.h"
#include "Components/BoxComponent.h"
#include "PlayerKart/C_PlayerController.h"
#include "PlayerKart/C_PlayerKart.h"
#include "UI/C_RaceWidget.h"


// Sets default values
AC_EndZone::AC_EndZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectZone"));
	SetRootComponent(DetectZone);
}

// Called when the game starts or when spawned
void AC_EndZone::BeginPlay()
{
	Super::BeginPlay();

	DetectZone->OnComponentBeginOverlap.AddDynamic(this, &AC_EndZone::OnBoxCompOverlap);
}

void AC_EndZone::OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* Kart = Cast<AC_PlayerKart>(OtherActor);
	auto* pc = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
	
	if (Kart && Kart->MaxProgressPointIndex > 15 && pc->RaceGameModeRef->CurrentState == RaceLevelState::Play)
	{
		Kart->MaxProgressPointIndex = 0;
		pc->CurLaps += 1;
		if (pc->CurLaps > pc->MaxLaps)
		{
			pc->SetFinished();
			return;
		}
		
		pc->CurrentHUD->ChangeLaps();
	}
}
