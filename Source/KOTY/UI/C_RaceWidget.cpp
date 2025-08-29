// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/C_RaceWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "PlayerKart/C_PlayerController.h"
#include "Item/Component/KotyItemHoldComponent.h"

void UC_RaceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UC_RaceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UC_RaceWidget::ChangeImg(UImage* TargetImg, FString Resource)
{
	FSlateBrush brush = TargetImg->GetBrush();

	NewTexture = LoadObject<UTexture2D>(nullptr, *Resource);
	
	brush.SetImageSize(NewTexture->GetImportedSize());
	brush.SetResourceObject(NewTexture);
		
	TargetImg->SetBrush(brush);
}

void UC_RaceWidget::ChangeItemImg(int32 SlotNum, int32 ItemNum)
{
	if (SlotNum == 0)
	{
		FSlateBrush brush = IMG_FirstItem->GetBrush();
	
		NewTexture = LoadObject<UTexture2D>(nullptr, *ItemResourceList[ItemNum]);
	
		brush.SetImageSize(NewTexture->GetImportedSize());
		brush.SetResourceObject(NewTexture);
		
		IMG_FirstItem->SetBrush(brush);
	}
	else if (SlotNum == 1)
	{
		FSlateBrush brush = IMG_SecondItem->GetBrush();
	
		NewTexture = LoadObject<UTexture2D>(nullptr, *ItemResourceList[ItemNum]);
	
		brush.SetImageSize(NewTexture->GetImportedSize());
		brush.SetResourceObject(NewTexture);
		
		IMG_SecondItem->SetBrush(brush);
	}
}

void UC_RaceWidget::ChangeUsingItemImg()
{
	// 아이템 없을때
	if (IMG_FirstItem->GetBrush().GetResourceName() == FName("M_Empty"))
	{
		return;
	}
	else if (IMG_SecondItem->GetBrush().GetResourceName() == FName("M_Empty"))//아이템 하나있을때
	{
		FSlateBrush brush = IMG_FirstItem->GetBrush();
	
		NewTexture = LoadObject<UTexture2D>(nullptr, *ItemResourceList[11]);
	
		brush.SetImageSize(NewTexture->GetImportedSize());
		brush.SetResourceObject(NewTexture);
		
		IMG_FirstItem->SetBrush(brush);
	}
	else // 아이템 2개 있을때
	{
		// 2번 아이템 텍스처 1번으로
		FSlateBrush brush = IMG_SecondItem->GetBrush();
		IMG_FirstItem->SetBrush(brush);
		
		//2번 아이템 텍스처 none으로
		NewTexture = LoadObject<UTexture2D>(nullptr, *ItemResourceList[11]);
		brush.SetImageSize(NewTexture->GetImportedSize());
		brush.SetResourceObject(NewTexture);
		IMG_SecondItem->SetBrush(brush);
	}
}

void UC_RaceWidget::ChangeLaps()
{
	auto pc = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());

	CurLaps = pc->CurLaps;
	MaxLaps = pc->MaxLaps;
	
	FString t = FString::Printf(TEXT("%d/%d"), CurLaps, MaxLaps);
	TXT_Laps->SetText(FText::FromString(t));
}

void UC_RaceWidget::ChangeRank(int32 CurRank)
{
	FSlateBrush brush = IMG_CurRank->GetBrush();
	
	NewTexture = LoadObject<UTexture2D>(nullptr, *RankResourceList[CurRank]);
	
	brush.SetImageSize(NewTexture->GetImportedSize());
	brush.SetResourceObject(NewTexture);
		
	IMG_CurRank->SetBrush(brush);
}
