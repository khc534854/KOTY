#include "PlayerKart/C_PlayerKart.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
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

	// SplineComponent = WorldSplineActor->FindComponentByClass<USplineComponent>();
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

	if (SplineComponent)
	{
		int32 ClosestIndex = FindClosestSplinePointIndex(GetActorLocation());
		if (MaxProgressPointIndex == 0)
		{
			if (ClosestIndex == 20 || ClosestIndex == 19)
				return;
		}
	
		// ✨ 핵심: 새로 찾은 포인트가 이전에 도달했던 최대 진행도보다 크거나 같을 때만 인정
		if (ClosestIndex >= MaxProgressPointIndex)
		{
			// 정주행으로 인정하고, 최대 진행도를 갱신합니다.
			MaxProgressPointIndex = ClosestIndex;
		}
		else
		{
			// 새로 찾은 포인트가 이전 진행도보다 작다면?
			// 이는 코너를 가로질렀거나, 후진 중이거나, U턴 구간에 있는 경우입니다.
			// 이럴 때는 진행도를 갱신하지 않고 무시합니다.
		}
	
		// 랩 완료 체크
		if (MaxProgressPointIndex >= SplineComponent->GetNumberOfSplinePoints() - 1)
		{
			//
		}
	}
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
	if (bIsStunned)
		return;
	
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
	// if (bIsDrift && (Value.Get<float> != HandlingDir))
	// {
	// 	
	// }
	//
	if (bIsStunned)
		return;
	
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

int32 AC_PlayerKart::FindClosestSplinePointIndex(const FVector& WorldLocation)
{
	if (!SplineComponent || SplineComponent->GetNumberOfSplinePoints() < 1)
	{
		// 스플라인이 없거나 포인트가 없으면 유효하지 않은 인덱스 반환
		return -1;
	}

	int32 ClosestPointIndex = -1;
	float MinDistanceSquared = -1.f;

	const int32 PointCount = SplineComponent->GetNumberOfSplinePoints();
	// 0번부터 마지막 포인트까지 순회
	for (int32 i = 0; i < PointCount; ++i)
	{
		// i번째 스플라인 포인트의 월드 위치를 가져옴
		const FVector PointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
        
		// 입력된 월드 위치와의 거리 제곱을 계산
		const float DistanceSquared = FVector::DistSquared(WorldLocation, PointLocation);

		// 첫 번째 포인트이거나, 이전에 찾은 최소 거리보다 현재 거리가 더 짧으면 정보 갱신
		if (ClosestPointIndex == -1 || DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			ClosestPointIndex = i;
		}
	}

	return ClosestPointIndex;
}


