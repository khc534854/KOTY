// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerController.h"
#include "Blueprint/UserWidget.h"

void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		CurrentHUD = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (CurrentHUD)
		{
			CurrentHUD->AddToViewport();
		}
	}
}
