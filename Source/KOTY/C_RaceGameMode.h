// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "C_RaceGameMode.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class RaceLevelState : uint8
{
	Cinematic,
	Ready,
	Play,
	End
};

UCLASS()
class KOTY_API AC_RaceGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AC_RaceGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APawn> Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	RaceLevelState CurrentState = RaceLevelState::Cinematic;

	void MySaveGame(); 
	void MyLoadGame(int32 defaultHighScore = 5999.99f);

	FString SaveSlotName = TEXT("BEST_TIME");
	int32   SaveUserindex = 0;
	
	float CurrentTime = 0;
	float BestTime    = 0;
};
