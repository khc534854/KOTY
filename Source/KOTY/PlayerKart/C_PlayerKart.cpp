#include "PlayerKart/C_PlayerKart.h"

#include "C_PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "Math/UnrealMathUtility.h"
#include "C_RaceGameMode.h"
#include "Item/Component/KotyItemHoldComponent.h"

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



	if (!(Tags.IsEmpty()))
	{
		this->Tags.Empty();
		Tags.Add(FName("Player"));
	}
	
	CameraMove();


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
	}
}

void AC_PlayerKart::Mushroom(const FInputActionValue& Value)
{
	StartAddSpeed(3000.f);
}

void AC_PlayerKart::UseItem(const FInputActionValue& Value)
{
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
	}
}

void AC_PlayerKart::OnThrowMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		USkeletalMeshComponent* KartMesh = FindComponentByClass<USkeletalMeshComponent>();
		if (IsValid(KartMesh) && DriveMontage)
		{
			// ✨ 이제 다른 애니메이션(예: 기본 주행 애니메이션)으로 전환합니다.
			KartMesh->PlayAnimation(DriveMontage, true);
		}
	}
}


