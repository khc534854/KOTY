// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "C_RaceWidget.generated.h"

/**
 * 
 */
UCLASS()
class KOTY_API UC_RaceWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ChangeImg(UImage* TargetImg, FString Resource);
public:
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Ready;
	

	UPROPERTY(meta = (BindWidget))
	UImage* IMG_ItemSlot;	
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_FirstItem;	
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_SecondItem;
	
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Coin;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Coin;

	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Laps;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Laps;
	
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_CurRank;

	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Map;

	float CurTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* NewTexture;

	float CurrentTime = 0;;
};
