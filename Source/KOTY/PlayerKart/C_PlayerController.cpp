// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerController.h"

#include "C_PlayerKart.h"
#include "C_RaceGameMode.h"
#include "Blueprint/UserWidget.h"
#include "UI/C_RaceWidget.h"
#include "EnhancedInputLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraActor.h"
#include "Gimmick/C_StartLakitu.h"
#include "Kismet/GameplayStatics.h"
#include "UI/C_EndWidget.h"

void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		CurrentHUD = CreateWidget<UC_RaceWidget>(this, HUDWidgetClass);
		CurrentHUD->ChangeLaps();
	}

	if (EndWidgetClass)
	{
		EndHUD = CreateWidget<UC_EndWidget>(this, EndWidgetClass);
	}

	
	PlayerKartRef = Cast<AC_PlayerKart>(GetPawn());
	RaceGameModeRef = Cast<AC_RaceGameMode>(GetWorld()->GetAuthGameMode());
	CurrentReadyTime = -4.0f;
	CurrentRaceTime = 0;
	CountdownState = 0;


	StartLakituRef = Cast<AC_StartLakitu>(UGameplayStatics::GetActorOfClass(GetWorld(), AC_StartLakitu::StaticClass()));
}

void AC_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (RaceGameModeRef->CurrentState == RaceLevelState::Ready)
	{
		CurrentReadyTime += DeltaTime;
		CheckReadyState();
		SetInputMode(FInputModeUIOnly());
		if (CurrentReadyTime > -1.f && !CurrentHUD->IsInViewport())
		{
			UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
			CurrentHUD->AddToViewport();
		}
	}
	
	if (RaceGameModeRef->CurrentState == RaceLevelState::Play)
	{
		SetInputMode(FInputModeGameOnly());
		CurrentRaceTime += DeltaTime;

		CheckBoostState();
		if (CurrentRaceTime > 2)
		{
			CurrentHUD->IMG_Ready->SetVisibility(ESlateVisibility::Hidden);
			StartLakituRef->StartLakitu->SetVisibility(false, true);
		}
	}

	if (RaceGameModeRef->CurrentState == RaceLevelState::End)
	{
		//SetInputMode(FInputModeUIOnly());
	}
}

void AC_PlayerController::SetReady()
{
	CurrentReadyTime = -4.0f;
	CurrentRaceTime = 0;
	CountdownState = 0;
	SetInputMode(FInputModeGameOnly());
}

void AC_PlayerController::SetFinished()
{
	CurrentHUD->IMG_Ready->SetVisibility(ESlateVisibility::Visible);
	CurrentHUD->ChangeImg(CurrentHUD->IMG_Ready, CurrentHUD->IMG_Finish);
	CurrentHUD->PlayAnimation(CurrentHUD->ANIM_Finish);

	UPlayerInput* PlayerInputObject = this->PlayerInput;
	
	UEnhancedInputLibrary::FlushPlayerInput(this);
	RaceGameModeRef->CurrentState = RaceLevelState::End;

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("ChangeCamera");
	LatentInfo.Linkage = 0; 
	LatentInfo.UUID = GetUniqueID(); 
	
	UKismetSystemLibrary::Delay(
		this,       // 월드 컨텍스트 오브젝트
		2.0f,       // 지연시킬 시간 (초)
		LatentInfo  // 지연된 액션 정보
	);
}

void AC_PlayerController::CheckReadyState()
{
	if (CurrentReadyTime < 1 &&  CurrentReadyTime >= 0 && CountdownState == 0)
	{
		CurrentHUD->IMG_Ready->SetVisibility(ESlateVisibility::Visible);
		CurrentHUD->ChangeImg(CurrentHUD->IMG_Ready, CurrentHUD->IMG_Three);
		CurrentHUD->PlayAnimation(CurrentHUD->ANIM_Ready);
		StartLakituRef->StartLakitu->SetMaterial(1, StartLakituRef->StartLakituRed);
		CountdownState = 1;
	}
	else if (CurrentReadyTime < 2 &&  CurrentReadyTime >= 1 && CountdownState == 1)
	{
		CurrentHUD->ChangeImg(CurrentHUD->IMG_Ready, CurrentHUD->IMG_Two);
		CurrentHUD->PlayAnimation(CurrentHUD->ANIM_Ready);
		StartLakituRef->StartLakitu->SetMaterial(4, StartLakituRef->StartLakituRed);
		CountdownState = 2;
	}
	else if (CurrentReadyTime < 3 &&  CurrentReadyTime >= 2 && CountdownState == 2)
	{
		CurrentHUD->ChangeImg(CurrentHUD->IMG_Ready, CurrentHUD->IMG_One);
		CurrentHUD->PlayAnimation(CurrentHUD->ANIM_Ready);
		StartLakituRef->StartLakitu->SetMaterial(5, StartLakituRef->StartLakituRed);
		CountdownState = 3;
	}
	else if (CurrentReadyTime < 4 &&  CurrentReadyTime >= 3 && CountdownState == 3)
	{
		CurrentHUD->ChangeImg(CurrentHUD->IMG_Ready, CurrentHUD->IMG_Go);
		CurrentHUD->PlayAnimation(CurrentHUD->ANIM_Ready);
		StartLakituRef->StartLakitu->SetMaterial(1, StartLakituRef->StartLakituGreen);
		StartLakituRef->StartLakitu->SetMaterial(4, StartLakituRef->StartLakituGreen);
		StartLakituRef->StartLakitu->SetMaterial(5, StartLakituRef->StartLakituGreen);
		RaceGameModeRef->CurrentState = RaceLevelState::Play;
		PlayerKartRef->StartAddSpeed(3000.f);
		CountdownState = 4;
	}
}

void AC_PlayerController::ChangeCamera()
{
	const FVector PrevCameraLocation = PlayerKartRef->Camera->GetComponentLocation();
	const FRotator PrevCameraRotation = PlayerKartRef->Camera->GetComponentRotation();
	
	ACameraActor* TempCamera = GetWorld()->SpawnActor<ACameraActor>(PrevCameraLocation, PrevCameraRotation);
	if (TempCamera)
	{
		SetViewTarget(TempCamera);

		PlayerKartRef->Camera->Deactivate();
		PlayerKartRef->EndCamera->Activate();
		SetViewTargetWithBlend(PlayerKartRef, 1.5f, VTBlend_Cubic);

		FTimerHandle TempCameraDestroyTimer;
		GetWorld()->GetTimerManager().SetTimer(TempCameraDestroyTimer, [TempCamera]()
		{
			if (TempCamera)
			{
				TempCamera->Destroy();
			}
		}, 1.5f, false);
	}

	CurrentHUD->RemoveFromParent();

	EndHUD->AddToViewport();
	EndHUD->PlayAnimation(EndHUD->ANIM_Rank);
}

void AC_PlayerController::CheckBoostState()
{
	if (PlayerKartRef->bIsBoosting)
		CurrentHUD->IMG_SpeedEffect->SetVisibility(ESlateVisibility::Visible);
	else
		CurrentHUD->IMG_SpeedEffect->SetVisibility(ESlateVisibility::Hidden);
		
}
