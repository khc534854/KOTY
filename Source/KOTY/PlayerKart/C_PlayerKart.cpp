#include "PlayerKart/C_PlayerKart.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AC_PlayerKart::AC_PlayerKart()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Camera Setting
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
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
}

// Called every frame
void AC_PlayerKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CameraMove();
	
}

// Called to bind functionality to input
void AC_PlayerKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input Component로 형변환합니다.
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// 입력 액션을 함수에 바인딩합니다.
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
	float MaxSpringArmLength = 500.f; // 최대 속도 시 길이
	float TargetArmLength = FMath::Lerp(MinSpringArmLength, MaxSpringArmLength, SpeedRatio);

		// 보간
	float ArmLengthLerpSpeed = 5.0f;
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, TargetArmLength, GetWorld()->DeltaTimeSeconds *ArmLengthLerpSpeed);
}

void AC_PlayerKart::UpdateSuspension(float DeltaTime)
{
	// float TotalSuspensionForce = 0.f;
	// int GroundedWheels = 0;
	//
	// // 4개의 바퀴에 대해 각각 라인 트레이스를 실행합니다.
	// for (USceneComponent* Wheel : WheelsComponents)
	// {
	// 	FVector WheelLocation = Wheel->GetComponentLocation();
	// 	FVector EndLocation = WheelLocation - (GetActorUpVector() * SuspensionLength);
	// 	FHitResult HitResult;
	// 	FCollisionQueryParams QueryParams;
	// 	QueryParams.AddIgnoredActor(this);
	//
	// 	if (GetWorld()->LineTraceSingleByChannel(HitResult, WheelLocation, EndLocation, ECC_Visibility, QueryParams))
	// 	{
	// 		// 바퀴가 땅에 닿았을 때
	// 		GroundedWheels++;
	//
	// 		// 1) 스프링 힘 계산
	// 		float CompressionRatio = 1.f - (HitResult.Distance / SuspensionLength);
	// 		float SpringForce = CompressionRatio * SuspensionStiffness;
	//
	// 		// 2) 댐핑 힘 계산
	// 		float DampingForce = VerticalVelocity * SuspensionDamping;
	//
	// 		TotalSuspensionForce += SpringForce - DampingForce;
	// 	}
	// }
 //    
	// float AverageSuspensionForce = 0.f;
	// if (GroundedWheels > 0)
	// {
	// 	AverageSuspensionForce = TotalSuspensionForce / GroundedWheels;
	// }
	//
	// // --- 중력 및 수직 속도 업데이트 ---
	// float GravityForce = GetWorld()->GetGravityZ();
	// VerticalVelocity += (GravityForce + AverageSuspensionForce) * DeltaTime;
	//
	// if (GroundedWheels > 0 && VerticalVelocity < 0)
	// {
	// 	VerticalVelocity = 0; 
	// }
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
	bIsHandling = true;
	HandlingDir = Value.Get<float>();
	WheelL->SetRelativeRotation(FRotator(180, 180 + 30 * HandlingDir, 0));
	WheelR->SetRelativeRotation(FRotator(0, 30 * HandlingDir, 0));
}

void AC_PlayerKart::HandlingEnd(const FInputActionValue& Value)
{
	HandlingDir = 0;
	bIsHandling = false;

	WheelL->SetRelativeRotation(FRotator(180, 180, 0));
	WheelR->SetRelativeRotation(FRotator(0, 0, 0));
}
void AC_PlayerKart::DriftStart(const FInputActionValue& Value)
{
	if (!bIsGround)
	{
		bIsGround = false;
		return;
	}
	
	if (!bIsDrift)
	{
		//StaticMeshComponent->SetRelativeLocation(StaticMeshComponent->GetRelativeLocation() + FVector(0, 0, 50));
		
		bIsDrift = true;
		DriftUpAction();
		MeshMoveDirection.X = -70.f;
		CurVelocity += GroundNormal * DriftHopImpulse;
	}
	else
	{
		//StaticMeshComponent->SetRelativeLocation(FVector(0, 0, FMath::Lerp(StaticMeshComponent->GetRelativeLocation().Z, -50, GetWorld()->DeltaTimeSeconds)));
	}
}

void AC_PlayerKart::DriftEnd(const FInputActionValue& Value)
{
	if (bIsDrift)
	{
		MeshMoveDirection.X = 70.f;
		bIsDrift = false;
		StartAddSpeed(1500.f);
	}
}

void AC_PlayerKart::Mushroom(const FInputActionValue& Value)
{
	StartAddSpeed(3000.f);
}


