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
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ChangeImg(UImage* TargetImg, FString Resource);

	UFUNCTION(BlueprintCallable)
	void ChangeLaps();
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_Ready;
	

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_ItemSlot;	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_FirstItem;	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_SecondItem;
	
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Coin;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TXT_Coin;

	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Laps;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TXT_Laps;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_CurRank;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_Map;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* IMG_SpeedEffect;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ANIM_Ready;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ANIM_Finish;
	
	float CurTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* NewTexture;

	float CurrentTime = -5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_Three = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/Start3.Start3'");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_Two = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/Start2.Start2'");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_One = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/Start1.Start1'");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_Go = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/StartGo.StartGo'");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_Finish = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/Finish.Finish'");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_RaceFirst = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/Racing1.Racing1'");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString IMG_RaceSecond = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/Racing2.Racing2'");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurLaps = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLaps = 2;
};
