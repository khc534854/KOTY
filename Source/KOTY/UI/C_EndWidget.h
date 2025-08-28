// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "C_EndWidget.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API UC_EndWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	void SetCurrentTime(float CurTime);
	void SetBestTime(float NewBestTime);

	
public:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VB_RankBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentTime;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BestTime;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ANIM_Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MenuNum;
};
