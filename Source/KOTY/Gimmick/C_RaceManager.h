// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_RaceManager.generated.h"

UCLASS()
class KOTY_API AC_RaceManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AC_RaceManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RegisterKart(class AC_KartBase* Kart);

	// 등록된 모든 카트의 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AC_KartBase*> RegisteredKarts;

	// 등수를 업데이트할 함수
	void UpdateRankings();

	// 등수 업데이트 주기 관리를 위한 타이머 핸들
	FTimerHandle RankUpdateTimerHandle;

	class AC_PlayerController* ControllerRef;
};
