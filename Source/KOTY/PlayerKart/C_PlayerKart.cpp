#include "PlayerKart/C_PlayerKart.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AC_PlayerKart::AC_PlayerKart()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);

	TArray<FVector> WheelLocations;
	WheelLocations.Add(FVector( 120,  70, 0));
	WheelLocations.Add(FVector( 120, -70, 0));
	WheelLocations.Add(FVector(-120,  70, 0));
	WheelLocations.Add(FVector(-120, -70, 0));
	
	for (int a = 0; a < 4; a++)
	{
		FString WheelComponentName = FString::Printf(TEXT("WheelComponent_%d"), a);
		WheelsComponents.Push(CreateDefaultSubobject<UStaticMeshComponent>(FName(*WheelComponentName)));
		WheelsComponents[a]->SetupAttachment(BoxComponent);
		WheelsComponents[a]->SetRelativeLocation(WheelLocations[a]);
	}

	MaxAccelerationTime = Acceleration * 2;

	// Camera
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
			// 입력 매핑 컨텍스트를 추가합니다.
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
	CheckIsGround();

	SetVelocity();
	//UpdateSuspension(DeltaTime);
	UpdateBodyRotation(DeltaTime);
	
	CameraMove();
	if (!bIsDrift)
	{
		StaticMeshComponent->SetRelativeLocation(FVector(FMath::Lerp(StaticMeshComponent->GetRelativeLocation().X, 70, DeltaTime), 0, 0));
		ForwardFriction = 0.8f;
	}
	else
	{
		StaticMeshComponent->SetRelativeLocation(FVector(FMath::Lerp(StaticMeshComponent->GetRelativeLocation().X, -70, DeltaTime), 0, 0));
		ForwardFriction = 1.6f;
	}
	
	FVector NewLocation;

	NewLocation = GetActorLocation() + CurVelocity * DeltaTime;
	
	SetActorLocation(NewLocation);
	
	//UE_LOG(LogTemp, Display, TEXT("CurSpeed: %f"), CurSpeed);
	//UE_LOG(LogTemp, Display, TEXT("CurDir: %f, %f, %f"), CurDirection.X, CurDirection.Y, CurDirection.Z);
	//UE_LOG(LogTemp, Display, TEXT("CameraRot: %f, %f, %f"), SpringArm->GetComponentRotation().Pitch, SpringArm->GetComponentRotation().Roll, SpringArm->GetComponentRotation().Yaw);
	//UE_LOG(LogTemp, Display, TEXT("BeforeSpeed: %f"), SpringArm->GetComponentRotation().Pitch, SpringArm->GetComponentRotation().Roll, SpringArm->GetComponentRotation().Yaw);
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
	}
}

void AC_PlayerKart::SetVelocity()
{
	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightVector = GetActorRightVector();
	const float ForwardSpeed = FVector::DotProduct(CurVelocity, ForwardVector);
	const float SidewaysSpeed = FVector::DotProduct(CurVelocity, RightVector);
	
	// 가속 힘 계산
	const FVector ThrustForce = GetActorForwardVector() * AccelerationDir * AccelerationForce;
	// 마찰력 계산
	const float CurrentSidewaysFriction = bIsDrift ? DriftSidewaysFriction : SidewaysFriction;
	const FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * CurrentSidewaysFriction;
	const FVector ForwardFrictionForce = -ForwardVector * ForwardSpeed * ForwardFriction;

	FVector SlopeGravityForce = FVector::ZeroVector;
	if (bIsGround)
	{
		// 일반적인 중력(아래 방향)을 경사면에 투영하여, 미끄러지는 힘만 추출합니다.
		const FVector GravityVector(0.f, 0.f, GetWorld()->GetGravityZ());
		SlopeGravityForce = GravityVector - (FVector::DotProduct(GravityVector, GroundNormal) * GroundNormal);
	}
	
	// 최종 힘 계산
	const FVector TotalForce = ThrustForce + SidewaysFrictionForce + ForwardFrictionForce + SlopeGravityForce;

	// 속도 업데이트
	CurVelocity += TotalForce * GetWorld()->GetDeltaSeconds();

	CurVelocity.Z += GetWorld()->GetGravityZ() * GetWorld()->GetDeltaSeconds() * 2.f;
	
	if (bIsGround)
	{
		// 차가 경사면에서 뜨거나 파고들지 않도록 속도를 지면 방향으로 투영합니다.
		CurVelocity = FVector::VectorPlaneProject(CurVelocity, GroundNormal);
	}
	
	CurVelocity = CurVelocity.GetClampedToMaxSize(MaxSpeed);

	const float CurrentSpeed = CurVelocity.Size();
	
	if (CurrentSpeed > KINDA_SMALL_NUMBER && !FMath::IsNearlyZero(HandlingDir))
	{
		const float TurnMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, MaxSpeed), // 입력 범위 (현재 속도)
			FVector2D(1.f, 0.4f),     // 출력 범위 (회전 배율)
			CurrentSpeed
		);
		
		float TurnThisFrame = TurnSpeed * TurnMultiplier * HandlingDir * GetWorld()->GetDeltaSeconds();

		const float ForwardDot = FVector::DotProduct(CurVelocity.GetSafeNormal(), GetActorForwardVector());
		if (ForwardDot < 0.0f)
		{
			// 뒤로 움직이고 있을 때, 회전 방향을 반대로 뒤집습니다.
			TurnThisFrame *= -1.0f;
		}
		
		const FRotator RotationDelta(0.f, TurnThisFrame, 0.f);
		AddActorLocalRotation(RotationDelta);
	}

	FRotator TargetRotation;
	if (bIsGround)
	{
		// 지면의 기울기(GroundNormal)와 현재 차의 앞 방향을 기준으로 목표 회전값을 계산
		TargetRotation = FRotationMatrix::MakeFromZX(GroundNormal, GetActorForwardVector()).Rotator();
	}
	else
	{
		// 공중에 있을 땐 차체의 기울기를 수평으로 되돌림
		TargetRotation = GetActorRotation();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;
	}
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), BodyRotationInterpolationSpeed);
	SetActorRotation(NewRotation);
	
	SpeedRatio = CurVelocity.Size() / MaxSpeed;
}

void AC_PlayerKart::CheckIsGround()
{
	FVector StartLocation = GetActorLocation();
	
	// 라인 트레이스 끝 위치 (예: 현재 위치에서 아래로 1000cm)
	FVector EndLocation = StartLocation - (GetActorUpVector() * 100.f);
	
	// 충돌 결과를 담을 구조체
	FHitResult HitResult;
	
	// 충돌 쿼리 매개변수
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 충돌 체크에서 자기 자신을 무시하도록 설정
	QueryParams.bTraceComplex = false; // 단순 충돌(Simple Collision)만 체크
	
	// Line Trace 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility, // 충돌 채널 (예: Visibility)
		QueryParams
	);
	
	// 디버그 라인 그리기 (선택 사항)
	DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		bHit ? FColor::Red : FColor::Green,
		false, // 영구 표시 여부
		-1.0f, // 표시 시간 (-1.0f는 한 프레임만)
		0,
		1.0f // 선의 두께
	);
	
	// 충돌이 발생했을 때의 로직
	if (bHit)
	{
		bIsGround = true;
		GroundNormal = HitResult.ImpactNormal;
	}
	else
	{
		bIsGround = false;
		GroundNormal = FVector::UpVector;
	}
}

void AC_PlayerKart::CameraMove()
{
	float MinSpringArmLength = 300.f; // 정지 시 길이
	float MaxSpringArmLength = 500.f; // 최대 속도 시 길이
	float TargetArmLength = FMath::Lerp(MinSpringArmLength, MaxSpringArmLength, SpeedRatio);

		// 현재 길이를 목표 길이로 부드럽게 보간합니다.
	float ArmLengthLerpSpeed = 5.0f;
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, TargetArmLength, GetWorld()->DeltaTimeSeconds *ArmLengthLerpSpeed);
}

void AC_PlayerKart::UpdateSuspension(float DeltaTime)
{
	float TotalSuspensionForce = 0.f;
	int GroundedWheels = 0;

	// 4개의 바퀴에 대해 각각 라인 트레이스를 실행합니다.
	for (USceneComponent* Wheel : WheelsComponents)
	{
		FVector WheelLocation = Wheel->GetComponentLocation();
		FVector EndLocation = WheelLocation - (GetActorUpVector() * SuspensionLength);
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, WheelLocation, EndLocation, ECC_Visibility, QueryParams))
		{
			// 바퀴가 땅에 닿았을 때
			GroundedWheels++;

			// 1) 스프링 힘 계산
			float CompressionRatio = 1.f - (HitResult.Distance / SuspensionLength);
			float SpringForce = CompressionRatio * SuspensionStiffness;

			// 2) 댐핑 힘 계산
			float DampingForce = VerticalVelocity * SuspensionDamping;

			TotalSuspensionForce += SpringForce - DampingForce;
		}
	}
    
	float AverageSuspensionForce = 0.f;
	if (GroundedWheels > 0)
	{
		AverageSuspensionForce = TotalSuspensionForce / GroundedWheels;
	}

	// --- 중력 및 수직 속도 업데이트 ---
	float GravityForce = GetWorld()->GetGravityZ();
	VerticalVelocity += (GravityForce + AverageSuspensionForce) * DeltaTime;

	if (GroundedWheels > 0 && VerticalVelocity < 0)
	{
		VerticalVelocity = 0; 
	}
}

void AC_PlayerKart::UpdateBodyRotation(float DeltaTime)
{
	if (!bIsGround)
	{
		// (선택사항) 공중에 있을 때는 차체를 수평으로 천천히 되돌릴 수 있습니다.
		FRotator LevelRotation = GetActorRotation();
		LevelRotation.Pitch = 0.f;
		LevelRotation.Roll = 0.f;
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), LevelRotation, DeltaTime, 2.0f));
		return;
	}

	// 1. 목표 회전값 계산
	// 현재 차의 앞 방향을 지면의 경사(GroundNormal)에 투영하여, 경사면 위에서의 '진짜' 앞 방향을 계산합니다.
	// 이렇게 하면 Yaw(좌우 방향)는 유지하면서 Pitch와 Roll만 경사면에 맞게 계산됩니다.
	const FVector ForwardVectorOnGround = FVector::VectorPlaneProject(GetActorForwardVector(), GroundNormal).GetSafeNormal();
    
	// 계산된 앞 방향과, 지면이 정의하는 위쪽 방향(GroundNormal)을 사용하여 목표 회전값을 만듭니다.
	const FRotator TargetRotation = FRotationMatrix::MakeFromX(ForwardVectorOnGround).Rotator();

	// 2. 부드러운 보간(Interpolation)
	// 현재 회전값에서 목표 회전값으로 부드럽게 변경합니다.
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, BodyRotationInterpolationSpeed);

	SetActorRotation(NewRotation);
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
}

void AC_PlayerKart::HandlingEnd(const FInputActionValue& Value)
{
	HandlingDir = 0;
	bIsHandling = false;
}
void AC_PlayerKart::DriftStart(const FInputActionValue& Value)
{
	if (!bIsGround)
	{
		bIsGround = false;
		return;
	}
	bIsDrift = true;
}

void AC_PlayerKart::DriftEnd(const FInputActionValue& Value)
{
	bIsDrift = false;
}


