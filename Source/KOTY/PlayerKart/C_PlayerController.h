// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API AC_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable)
	void SetReady();

	UFUNCTION(BlueprintCallable)
	void SetFinished();

	UFUNCTION(BlueprintCallable)
	void CheckReadyState();

	UFUNCTION(BlueprintCallable)
	void ChangeCamera();

	UFUNCTION(BlueprintCallable)
	void CheckBoostState();

	UFUNCTION()
	void OnItemAcquired(EItemList AcquiredItem);

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UC_RaceWidget* CurrentHUD;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> EndWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UC_EndWidget* EndHUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AC_PlayerKart* PlayerKartRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AC_RaceGameMode* RaceGameModeRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurLaps = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLaps = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentRaceTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentReadyTime = -1.f;
	
	UPROPERTY()
	class AC_StartLakitu* StartLakituRef;

	int32 CountdownState = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, USoundBase*> SoundData;

	UPROPERTY()
	UAudioComponent* CurrentBGMComponent;

	bool bIsTestMap = false;
};
