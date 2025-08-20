// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/C_RaceWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UC_RaceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CurrentTime += InDeltaTime;
	
	if (CurrentTime >= 2.0f)
	{
		FString r = TEXT("/Script/Engine.Texture2D'/Game/Koty/UI/UIResource/StartGo.StartGo'");
		ChangeImg(IMG_Ready, r);
	}

	
}

void UC_RaceWidget::ChangeImg(UImage* TargetImg, FString Resource)
{
	FSlateBrush brush = TargetImg->GetBrush();

	NewTexture = LoadObject<UTexture2D>(nullptr, *Resource);
	
	brush.SetImageSize(NewTexture->GetImportedSize());
	brush.SetResourceObject(NewTexture);
		
	IMG_Ready->SetBrush(brush);
}
