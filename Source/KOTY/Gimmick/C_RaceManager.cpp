// Fill out your copyright notice in the Description page of Project Settings.


#include "C_RaceManager.h"
#include "C_KartBase.h"
#include "C_RaceGameMode.h"
#include "PlayerKart/C_PlayerController.h"
#include "PlayerKart/C_PlayerKart.h"
#include "UI/C_RaceWidget.h"


// Sets default values
AC_RaceManager::AC_RaceManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AC_RaceManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
	RankUpdateTimerHandle, this, &AC_RaceManager::UpdateRankings, 0.2f, true, 1.0f);
	ControllerRef = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void AC_RaceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_RaceManager::RegisterKart(AC_KartBase* Kart)
{
	if (Kart)
	{
		RegisteredKarts.Add(Kart);
	}
}

void AC_RaceManager::UpdateRankings()
{// 정렬(Sort) 기능을 사용하여 등수 결정
	if(ControllerRef->RaceGameModeRef->CurrentState == RaceLevelState::End)
		return;
	
	RegisteredKarts.Sort([](const AC_KartBase& A, const AC_KartBase& B)
	{
		// 1. 바퀴 수가 다르면, 바퀴 수가 많은 쪽이 더 높은 등수
		if (A.CurrentLap != B.CurrentLap)
		{
			return A.CurrentLap > B.CurrentLap;
		}

		// 2. 바퀴 수가 같으면, 스플라인 진행도가 높은 쪽이 더 높은 등수
		if (A.MaxProgressPointIndex != B.MaxProgressPointIndex)
		{
			return A.MaxProgressPointIndex > B.MaxProgressPointIndex;
		}

		// 3. 진행도까지 같으면, 해당 지점에 '먼저 도달한' 쪽이 더 높은 등수
		// (업데이트 시간이 더 작은 쪽)
		return A.LastProgressUpdateTime < B.LastProgressUpdateTime;
	});

	// 정렬된 순서대로 등수를 할당 (1등, 2등, ...)
	for (int32 i = 0; i < RegisteredKarts.Num(); i++)
	{
		if (RegisteredKarts[i])
		{
			RegisteredKarts[i]->CurRank = i;

			if (RegisteredKarts[i]->IsA(AC_PlayerKart::StaticClass()))
			{
				ControllerRef->CurrentHUD->ChangeRank(i);
			}
		}
	}
}

