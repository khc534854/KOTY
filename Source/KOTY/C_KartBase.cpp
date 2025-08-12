// Fill out your copyright notice in the Description page of Project Settings.


#include "C_KartBase.h"

#include "Components/BoxComponent.h"


// Sets default values
AC_KartBase::AC_KartBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);
}

// Called when the game starts or when spawned
void AC_KartBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_KartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIsGround();

	SetVelocity();
	//UpdateSuspension(DeltaTime);
	UpdateBodyRotation(DeltaTime);
	
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
}

void AC_KartBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AC_KartBase::Stun()
{
	// 스턴 구현
}

void AC_KartBase::CheckIsGround()
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

void AC_KartBase::UpdateBodyRotation(float DeltaTime)
{
	if (!bIsGround)
	{
		FRotator LevelRotation = GetActorRotation();
		LevelRotation.Pitch = 0.f;
		LevelRotation.Roll = 0.f;
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), LevelRotation, DeltaTime, 2.0f));
		return;
	}

	// 평명 프로젝션
	const FVector ForwardVectorOnGround = FVector::VectorPlaneProject(GetActorForwardVector(), GroundNormal).GetSafeNormal();
    
	// 목표 회전값
	const FRotator TargetRotation = FRotationMatrix::MakeFromX(ForwardVectorOnGround).Rotator();

	//보간
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, BodyRotationInterpolationSpeed);

	SetActorRotation(NewRotation);
}

void AC_KartBase::SetVelocity()
{
	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightVector = GetActorRightVector();
	const float ForwardSpeed = FVector::DotProduct(CurVelocity, ForwardVector); // 정면 속도
	const float SidewaysSpeed = FVector::DotProduct(CurVelocity, RightVector);  // 측면 속도
	
	// 가속 힘 계산
	const FVector ThrustForce = GetActorForwardVector() * AccelerationDir * AccelerationForce;
	
	// 마찰력 계산
	const float CurrentSidewaysFriction = bIsDrift ? DriftSidewaysFriction : SidewaysFriction; // 드리프트 시 측면 마찰력 설정
	const FVector ForwardFrictionForce = -ForwardVector * ForwardSpeed * ForwardFriction; // 정면 마찰력
	const FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * CurrentSidewaysFriction; // 측면 마찰력

	FVector SlopeGravityForce = FVector::ZeroVector; // 중력
	if (bIsGround)
	{
		// 중력 경사면 투영
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
		CurVelocity = FVector::VectorPlaneProject(CurVelocity, GroundNormal);
	}
	
	CurVelocity = CurVelocity.GetClampedToMaxSize(MaxSpeed);

	CurSpeed = CurVelocity.Size();
	
	if (CurSpeed > KINDA_SMALL_NUMBER && !FMath::IsNearlyZero(HandlingDir))
	{
		const float TurnMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, MaxSpeed), // 입력 범위 (현재 속도)
			FVector2D(1.f, 0.4f),     // 출력 범위 (회전 배율)
			CurSpeed
		);
		
		float TurnThisFrame = TurnSpeed * TurnMultiplier * HandlingDir * GetWorld()->GetDeltaSeconds();

		const float ForwardDot = FVector::DotProduct(CurVelocity.GetSafeNormal(), GetActorForwardVector());
		if (ForwardDot < 0.0f)
		{
			// 뒤로 움직이고 있을 때, 회전방향 반대
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

void AC_KartBase::UpdateSuspension(float DeltaTime)
{
	// 서스펜션 구현
}

