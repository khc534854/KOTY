// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/C_RaceWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

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
		
	IMG_Ready->SetBrush(brush);
}
