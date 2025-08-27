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
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VB_RankBox;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ANIM_Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MenuNum;
};
