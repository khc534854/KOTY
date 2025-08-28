// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EndWidget.h"

#include "Components/TextBlock.h"

void UC_EndWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MenuNum = 0;
}

void UC_EndWidget::SetCurrentTime(float CurTime)
{
	int32 Minutes = FMath::FloorToInt(CurTime / 60.f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(CurTime, 60.f));
	int32 Centiseconds = FMath::FloorToInt((CurTime - FMath::FloorToFloat(CurTime)) * 100.f);

	FString TimeString = FString::Printf(TEXT("%02d:%02d.%02d"), Minutes, Seconds, Centiseconds);

	CurrentTime->SetText(FText::FromString(TimeString));
}

void UC_EndWidget::SetBestTime(float NewBestTime)
{
	int32 Minutes = FMath::FloorToInt(NewBestTime / 60.f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(NewBestTime, 60.f));
	int32 Centiseconds = FMath::FloorToInt((NewBestTime - FMath::FloorToFloat(NewBestTime)) * 100.f);

	FString TimeString = FString::Printf(TEXT("%02d:%02d.%02d"), Minutes, Seconds, Centiseconds);

	BestTime->SetText(FText::FromString(TimeString));
}
