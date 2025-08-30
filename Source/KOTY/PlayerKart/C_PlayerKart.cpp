#include "PlayerKart/C_PlayerKart.h"

#include "C_PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "Math/UnrealMathUtility.h"
#include "C_RaceGameMode.h"
#include "Components/AudioComponent.h"
#include "Item/Component/KotyItemHoldComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/C_RaceWidget.h"

// Sets default values
AC_PlayerKart::AC_PlayerKart()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Camera Setting
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	EndCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("EndCamera"));
	EndCamera->SetupAttachment(RootComponent);


	
	SpringArm->TargetArmLength = 600.0f; 
	
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true; // 액터의 Yaw 회전

	// 회전 지연 활성화
	SpringArm->bEnableCameraRotationLag = true;
	
	// 지연 속도
	SpringArm->CameraRotationLagSpeed = 7.0f;

	// 충돌 방지
	SpringArm->bDoCollisionTest = true;

	CurrentAccelSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSoundComponent"));
	CurrentAccelSoundComponent->SetupAttachment(RootComponent);
	// ✨ 처음에는 소리가 자동으로 재생되지 않도록 설정합니다.
	CurrentAccelSoundComponent->bAutoActivate = false;

	
}

// Called when the game starts or when spawned
void AC_PlayerKart::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	PCRef = Cast<AC_PlayerController>(GetWorld()->GetFirstPlayerController());

	//SplineComponent = WorldSplineActor->FindComponentByClass<USplineComponent>();
}

// Called every frame
void AC_PlayerKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CameraMove();
	//UpdateEngineSound(DeltaTime);
}

// Called to bind functionality to input
void AC_PlayerKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		if (AccelerationAction)
		{
			EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Triggered, this, &AC_PlayerKart::StartAccelerator);
			EnhancedInputComponent->BindAction(AccelerationAction, ETriggerEvent::Completed, this, &AC_PlayerKart::EndAccelerator);
		}

		if (HandlingAction)
		{
			EnhancedInputComponent->BindAction(HandlingAction, ETriggerEvent::Triggered, this, &AC_PlayerKart::HandlingStart);
			EnhancedInputComponent->BindAction(HandlingAction, ETriggerEvent::Completed, this, &AC_PlayerKart::HandlingEnd);
		}

		if (DriftAction)
		{
			EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Triggered, this, &AC_PlayerKart::DriftStart);
			EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Completed, this, &AC_PlayerKart::DriftEnd);
		}

		if (AddSpeedAction)
		{
			EnhancedInputComponent->BindAction(AddSpeedAction, ETriggerEvent::Started, this, &AC_PlayerKart::Mushroom);
		}

		if (UseItemAction)
		{
			EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &AC_PlayerKart::UseItem);
		}
	}
}

void AC_PlayerKart::SetVelocity()
{
	Super::SetVelocity();
}

void AC_PlayerKart::CheckIsGround()
{
	Super::CheckIsGround();
}

void AC_PlayerKart::CameraMove()
{
	float MinSpringArmLength = 300.f; // 정지 시 길이
	float MaxSpringArmLength = 600.f; // 최대 속도 시 길이
	float TargetArmLength = FMath::Lerp(MinSpringArmLength, MaxSpringArmLength, SpeedRatio);

	// 보간
	float ArmLengthLerpSpeed;
	if (bIsDrift)
	{
		ArmLengthLerpSpeed = 0.5f;
	}
	else
	{
		ArmLengthLerpSpeed = 5.f;
	}

	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, TargetArmLength, GetWorld()->DeltaTimeSeconds *ArmLengthLerpSpeed);
	
}

void AC_PlayerKart::UpdateSuspension(float DeltaTime)
{
	
}

void AC_PlayerKart::UpdateBodyRotation(float DeltaTime)
{
	Super::UpdateBodyRotation(DeltaTime);
	
}

void AC_PlayerKart::Stun()
{
	Super::Stun();
	//FVector SaveVelocity = CurVelocity;
}

void AC_PlayerKart::StartAccelerator(const FInputActionValue& Value)
{
	if (PCRef->bIsTestMap)
	{
		if (bIsStunned)
		{
			AccelerationDir = 0;
			return;
		}

	}
	else 
	{
		if ((bIsStunned || GamemodeRef->CurrentState == RaceLevelState::End))
		{
			AccelerationDir = 0;
			return;
		}
	}
	
	//CurrentKartSoundComponent = UGameplayStatics::CreateSound2D(this, *KartSoundData.Find(FName("AccelStart")));;

	bIsAcceleration = true;
	AccelerationDir = Value.Get<float>();

}

void AC_PlayerKart::EndAccelerator(const FInputActionValue& Value)
{
	AccelerationDir = 0.f;
	bIsAcceleration = false;
}

void AC_PlayerKart::HandlingStart(const FInputActionValue& Value)
{
	if (PCRef->bIsTestMap)
	{
		if (bIsStunned)
		{
			return;
		}

	}
	else 
	{
		if ((bIsStunned || GamemodeRef->CurrentState == RaceLevelState::End))
		{
			return;
		}
	}
	
	bIsHandling = true;
	HandlingDir = Value.Get<float>();
	WheelL->SetRelativeRotation(FRotator(180, 180 + 30 * HandlingDir, 0));
	WheelR->SetRelativeRotation(FRotator(0, 30 * HandlingDir, 0));
}

void AC_PlayerKart::HandlingEnd(const FInputActionValue& Value)
{
	
	//if (bIsStunned || bIsDrift)
	if (bIsStunned)
		return;
	
	HandlingDir = 0;
	bIsHandling = false;

	WheelL->SetRelativeRotation(FRotator(180, 180, 0));
	WheelR->SetRelativeRotation(FRotator(0, 0, 0));
}
void AC_PlayerKart::DriftStart(const FInputActionValue& Value)
{
	if (PCRef->bIsTestMap)
	{
		if (bIsStunned)
		{
			return;
		}

	}
	else 
	{
		if ((bIsStunned || GamemodeRef->CurrentState == RaceLevelState::End))
		{
			return;
		}
	}

	if (!bIsGround)
	{
		bIsGround = false;
		return;
	}

	if (HandlingDir == 0)
	{
		//DriftUpAction();
		return;
	}

	const float ForwardDot = FVector::DotProduct(CurVelocity.GetSafeNormal(), GetActorForwardVector());
	if (ForwardDot < 0.0f)
	{
		return;
	}
	
	DriftTime += GetWorld()->GetDeltaSeconds();
	
	if (!bIsDrift)
	{
		DriftStartDir = HandlingDir;
		//StaticMeshComponent->SetRelativeLocation(StaticMeshComponent->GetRelativeLocation() + FVector(0, 0, 50));
		bIsDrift = true;
		DriftUpAction();
		//PlayDriftEffect();
		MeshMoveDirection.X = -70.f;
		MeshRotationDirection.Yaw = -90.f + HandlingDir * 30.f;
		CurVelocity += GroundNormal * DriftHopImpulse;
	}
	
	if (DriftTime > 1.f && DriftTime < 2.0f)
	{
		PlayDriftEffect(1, DriftStartDir);
	}
	else if (DriftTime > 2.0f && DriftTime < 3.5f)
	{
		PlayDriftEffect(2, DriftStartDir);
	}
	else if (DriftTime > 3.5f)
	{
		PlayDriftEffect(3, DriftStartDir);
	}
	
}

void AC_PlayerKart::DriftEnd(const FInputActionValue& Value)
{
	if (bIsDrift)
	{
		MeshMoveDirection.X = 70.f;
		bIsDrift = false;
		MeshRotationDirection.Yaw = -90.f;
		if (DriftTime > 1.f && DriftTime < 2.f)
		{
			StartAddSpeed(1500.f);
		}
		else if (DriftTime > 2.f && DriftTime < 3.5f)
		{
			StartAddSpeed(2000.f);
		}
		else if (DriftTime > 3.5f)
		{
			StartAddSpeed(2500.f);
		}
		
		DriftTime = 0;
		CurrentDriftType = 0;
		if(CurrentKartSoundComponent->IsPlaying())
			CurrentKartSoundComponent->Stop();
	}
}

void AC_PlayerKart::Mushroom(const FInputActionValue& Value)
{
	StartAddSpeed(3000.f);
}

void AC_PlayerKart::UseItem(const FInputActionValue& Value)
{
	if (!HoldComp->isHasItem())
		return;
	
	HoldComp->UseCurrentItem();

	TArray<USkeletalMeshComponent*> SKMesh;
	GetComponents<USkeletalMeshComponent>(SKMesh);

	for (USkeletalMeshComponent* Child : SKMesh)
	{
		if (IsValid(Child))
		{
			UAnimInstance* AnimInstance = Child->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->OnMontageEnded.AddDynamic(this, &AC_PlayerKart::OnThrowMontageEnded);
				AnimInstance->Montage_Play(ThrowMontage);
			}
		}
		PCRef->CurrentHUD->ChangeUsingItemImg();
	}
}

void AC_PlayerKart::OnThrowMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		USkeletalMeshComponent* KartMesh = FindComponentByClass<USkeletalMeshComponent>();
		if (IsValid(KartMesh) && DriveMontage)
		{
			KartMesh->PlayAnimation(DriveMontage, true);
		}
	}
}

void AC_PlayerKart::UpdateEngineSound(float DeltaTime)
{
	
	const bool bIsAccelerating = AccelerationDir > 0;
	// 차가 앞으로 움직이고 있는지 여부 (매우 낮은 속도는 0으로 간주)
	const bool bIsMovingForward = FVector::DotProduct(CurVelocity, GetActorForwardVector()) > 10.f;

	// --- 상태 전환 로직 ---

	// 조건 1: 엑셀을 밟았고, 차가 거의 멈춰있으며, 현재 아무 소리도 나지 않을 때
	if (bIsAccelerating && !bIsMovingForward && CurrentEngineSoundState == EEngineSoundState::None)
	{
		// '가속 시작' 소리를 재생하고 상태를 변경합니다.
		CurrentEngineSoundState = EEngineSoundState::Starting;
		CurrentAccelSoundComponent = UGameplayStatics::CreateSound2D(this, *KartSoundData.Find(FName("AccelStart")), 2.f);
		CurrentAccelSoundComponent->Play();
		return; // 이번 프레임은 할 일을 마쳤으므로 종료
	}

	// 조건 2: '가속 시작' 소리가 재생 중인데, 그 소리가 끝났을 때
	if (CurrentEngineSoundState == EEngineSoundState::Starting && !CurrentAccelSoundComponent->IsPlaying())
	{
		// '가속 루프' 소리로 전환하고 상태를 변경합니다.
		CurrentEngineSoundState = EEngineSoundState::Looping;
		CurrentAccelSoundComponent->FadeOut(0.1f, 0.f);
		CurrentAccelSoundComponent = UGameplayStatics::CreateSound2D(this, *KartSoundData.Find(FName("AccelLoop")), 2.f);
		CurrentAccelSoundComponent->Play();
		return;
	}

	// 조건 3: 엑셀을 밟고 있거나 앞으로 움직이고 있을 때, 현재 소리가 없거나 시작 소리만 재생된 경우
	// (예: 내리막길에서 엑셀을 떼도 소리가 나야 할 때)
	if ((bIsAccelerating || bIsMovingForward) && CurrentEngineSoundState != EEngineSoundState::Looping)
	{
		// 바로 '가속 루프' 소리를 재생합니다.
		CurrentEngineSoundState = EEngineSoundState::Looping;
		CurrentAccelSoundComponent = UGameplayStatics::CreateSound2D(this, *KartSoundData.Find(FName("AccelLoop")), 2.f);
		CurrentAccelSoundComponent->Play();
		return;
	}

	// 조건 4: 엑셀을 떼고 있고, 차도 거의 멈췄을 때
	if (!bIsAccelerating && !bIsMovingForward)
	{
		// 모든 소리를 끄고 상태를 초기화합니다.
		if (CurrentAccelSoundComponent->IsPlaying())
		{
			CurrentAccelSoundComponent->Stop();
		}
		CurrentEngineSoundState = EEngineSoundState::None;
		return;
	}
}


