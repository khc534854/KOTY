// Fill out your copyright notice in the Description page of Project Settings.


#include "C_KartBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Utility/C_MathUtility.h"
#include "C_RaceGameMode.h"


// Sets default values
AC_KartBase::AC_KartBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);

	WheelL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelL"));
	WheelL->SetupAttachment(StaticMeshComponent);

	WheelR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelR"));
	WheelR->SetupAttachment(StaticMeshComponent);

	WheelB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelB"));
	WheelB->SetupAttachment(StaticMeshComponent);

	ExhaustPipe.Add(CreateDefaultSubobject<USceneComponent>(TEXT("ExhaustPipeL")));
	ExhaustPipe.Add(CreateDefaultSubobject<USceneComponent>(TEXT("ExhaustPipeR")));
	for (auto i : ExhaustPipe)
	{
		i->SetupAttachment(StaticMeshComponent);
	}

	WheelBackDrift.Add(CreateDefaultSubobject<USceneComponent>(TEXT("WheelBackDriftL")));
	WheelBackDrift.Add(CreateDefaultSubobject<USceneComponent>(TEXT("WheelBackDriftR")));
	for (auto i : WheelBackDrift)
	{
		i->SetupAttachment(StaticMeshComponent);
	}
}

// Called when the game starts or when spawned
void AC_KartBase::BeginPlay()
{
	Super::BeginPlay();
	GamemodeRef = Cast<AC_RaceGameMode>(GetWorld()->GetAuthGameMode());
	
}

// Called every frame
void AC_KartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIsGround();
	
	if (bIsGround)
	{
		SetVelocity();
	}
	else
	{
		SetFlyVelocity();
	}

	if (bIsSuspending)
	{
		UpdateSuspension(DeltaTime);
	}

	FHitResult Hit;
	FVector NewLocation;
	NewLocation = GetActorLocation() + CurVelocity * DeltaTime;
	SetActorLocation(NewLocation, true, &Hit);

	if (Hit.bBlockingHit)
	{
		HandleCollision(Hit);
	}
	
	UpdateBodyRotation(DeltaTime);

	if (bIsStunned)
	{
		UpdateStunEffect(DeltaTime);
		return;
	}
	
	// back wheel move
	WheelB->SetRelativeRotation(WheelB->GetRelativeRotation() +  FRotator(0 , 0, DeltaTime * CurSpeed * 0.001f * FVector::DotProduct(CurVelocity, GetActorForwardVector())));
	WheelL->SetRelativeRotation(WheelL->GetRelativeRotation() +  FRotator(0 , 0, DeltaTime * CurSpeed * 0.001f * FVector::DotProduct(CurVelocity, GetActorForwardVector())));
	WheelR->SetRelativeRotation(WheelR->GetRelativeRotation() +  FRotator(0 , 0, DeltaTime * CurSpeed * 0.001f * FVector::DotProduct(CurVelocity, GetActorForwardVector())));


	// StaticMesh 움직여서 드리프트 효과
	if (!bIsDrift)
	{
		//StaticMeshComponent->SetRelativeLocation(FVector(FMath::Lerp(StaticMeshComponent->GetRelativeLocation().X, 70, DeltaTime), 0, 0));
		ForwardFriction = 0.8f;
	}
	else
	{
		//StaticMeshComponent->SetRelativeLocation(FVector(FMath::Lerp(StaticMeshComponent->GetRelativeLocation().X, -70, DeltaTime), 0, 0));
		ForwardFriction = 1.6f;
	}
	//StaticMeshComponent->SetRelativeLocation(StaticMeshComponent->GetRelativeLocation() + (MeshMoveDirection * DeltaTime));
	StaticMeshComponent->SetRelativeLocation(FVector(
		FMath::Lerp(StaticMeshComponent->GetRelativeLocation().X, MeshMoveDirection.X, DeltaTime),
		0,
		FMath::Lerp(StaticMeshComponent->GetRelativeLocation().Z, MeshMoveDirection.Z, DeltaTime * 10)));
			
	StaticMeshComponent->SetRelativeRotation(FRotator(
		0,
		FMath::Lerp(StaticMeshComponent->GetRelativeRotation().Yaw, MeshRotationDirection.Yaw, DeltaTime * 10),
		0));

	
	//SetActorLocation(NewLocation);

	if (bIsGround)
	{
		FHitResult GroundSnapHit;
		FVector StartPoint = GetActorLocation();
		// 차체 아래로 짧게 라인을 쏴서 정확한 지면 위치를 다시 찾습니다.
		FVector EndPoint = StartPoint - GroundNormal * 50.f; // GroundNormal을 사용해 어떤 각도의 표면에서도 작동
        
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(GroundSnapHit, StartPoint, EndPoint, ECC_Visibility, QueryParams))
		{
			// 라인 트레이스가 감지한 지면의 위치에, 차의 절반 높이를 더해 최종 위치를 설정합니다.
			// 이렇게 하면 차의 바닥이 항상 지면 위에 위치하게 됩니다.
			const float KartHalfHeight = 51.f; // 박스 콜리전의 절반 높이 (조절 필요)
			FVector TargetLocation = GroundSnapHit.ImpactPoint + GroundNormal * KartHalfHeight;

			// 최종적으로 계산된 위치로 액터를 '보정'합니다. Sweep은 false로 하여 순간이동 시킵니다.
			SetActorLocation(TargetLocation, false);
		}
	}

	// 부스트 이펙트
	//PlayBoostEffect();
	if (bIsBoosting)
	{
		BoostTimer -= DeltaTime;
		if (BoostTimer <= 0.f)
		{
			bIsBoosting = false;
			AddSpeed = 0.f;
			
			for (UNiagaraComponent* Effect : ActiveBoostEffects)
			{
				if (IsValid(Effect)) // 컴포넌트가 유효한지 확인
				{
					Effect->DestroyComponent();
				}
			}
			// 배열을 비워 깨끗한 상태로 만듭니다.
			ActiveBoostEffects.Empty();
		}
	}

	if (!bIsDrift)
	{
		for (UNiagaraComponent* Effect : ActiveDriftEffects)
		{
			if (IsValid(Effect)) // 컴포넌트가 유효한지 확인
			{
				Effect->DestroyComponent();
			}
		}
		// 배열을 비워 깨끗한 상태로 만듭니다.
		ActiveDriftEffects.Empty();
	}
	
	// 부스트 감소
	if (AddSpeed > KINDA_SMALL_NUMBER)
	{
		AddSpeed -= DeltaTime * 1000.f;
	}

	// 매쉬 위치 복귀 
	if (MeshMoveDirection.Z > KINDA_SMALL_NUMBER)
	{
		MeshMoveDirection.Z -= DeltaTime * 1500.f;
		if (MeshMoveDirection.Z < KINDA_SMALL_NUMBER)
			MeshMoveDirection.Z = 0;
	}

	// if (SplineComponent)
	// {
	// 	int32 ClosestIndex = FindClosestSplinePointIndex(GetActorLocation());
	// 	if (MaxProgressPointIndex == 0)
	// 	{
	// 		if (ClosestIndex == 20 || ClosestIndex == 19)
	// 			return;
	// 	}
	//
	// 	// ✨ 핵심: 새로 찾은 포인트가 이전에 도달했던 최대 진행도보다 크거나 같을 때만 인정
	// 	if (ClosestIndex >= MaxProgressPointIndex)
	// 	{
	// 		// 정주행으로 인정하고, 최대 진행도를 갱신합니다.
	// 		MaxProgressPointIndex = ClosestIndex;
	// 	}
	// 	else
	// 	{
	// 		// 새로 찾은 포인트가 이전 진행도보다 작다면?
	// 		// 이는 코너를 가로질렀거나, 후진 중이거나, U턴 구간에 있는 경우입니다.
	// 		// 이럴 때는 진행도를 갱신하지 않고 무시합니다.
	// 	}
	//
	// 	// 랩 완료 체크
	// 	if (MaxProgressPointIndex >= SplineComponent->GetNumberOfSplinePoints() - 1)
	// 	{
	// 		//
	// 	}
	// }
}

void AC_KartBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AC_KartBase::Stun()
{
	// 스턴 구현

	if (bIsStunned)
	{
		return;
	}

	// 속도 80% 감소
	CurVelocity *= StunSpeedMultiplier;

	// 스턴 상태를 활성화, 타이머를 초기화
	bIsStunned = true;
	StunRotationTimer = 0.f;

}

void AC_KartBase::CheckIsGround()
{	
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - (GetActorUpVector() * 1000.f);
	
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
	
	// 디버그 라인
	DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		bHit ? FColor::Red : FColor::Green,
		false, // 영구 표시 여부
		-1.0f,
		0,
		1.0f // 선의 두께
	);
	
	// 충돌이 발생했을 때의 로직
	if (bHit)
	{
		if (HitResult.Distance < 65.f)
			bIsGround = true;
		else
			bIsGround = false;
			
		GroundNormal = HitResult.ImpactNormal;
		
		if (!bIsSuspending)
		{
			bIsSuspending = true;
		}
		AirTime = 0;
	}
	else
	{
		bIsGround = false;
		AirTime += GetWorld()->DeltaTimeSeconds;
		GroundNormal = FVector::UpVector;
	}
}

void AC_KartBase::UpdateBodyRotation(float DeltaTime)
{
	// Yaw
	if (CurSpeed > 100.f && !FMath::IsNearlyZero(HandlingDir))
	{
		float TurnMultiplier;
		if (bIsDrift)
		{
			TurnMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, MaxSpeed), // 입력 범위 (현재 속도)
			FVector2D(1.f, 0.6f),     // 출력 범위 (회전 배율)
			CurSpeed);
		}
		else
		{
			TurnMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, MaxSpeed), // 입력 범위 (현재 속도)
			FVector2D(1.f, 0.4f),     // 출력 범위 (회전 배율)
			CurSpeed);
		}

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

	// Pitch, Roll
	const FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation;

	float InterpSpeed = BodyRotationInterpolationSpeed;
	
	if (bIsInPostCollisionState)
	{
		TargetRotation = PostCollisionTargetRotation;
		InterpSpeed = CollisionRotationSpeed; // 충돌 시에는 더 빠르게 회전

		// 목표 각도에 거의 도달했다면, 충돌 후 상태를 해제합니다.
		if (CurrentRotation.Equals(TargetRotation, 1.0f))
		{
			bIsInPostCollisionState = false;
		}
	}
	else
	{
		if (bIsGround)
		{
			// 땅에 있을 때: 경사면에 맞춤
			const FVector ForwardOnGround = FVector::VectorPlaneProject(GetActorForwardVector(), GroundNormal).GetSafeNormal();
			const FRotator RotationOnGround = FRotationMatrix::MakeFromZX(GroundNormal, ForwardOnGround).Rotator();
			TargetRotation = FRotator(RotationOnGround.Pitch, CurrentRotation.Yaw, RotationOnGround.Roll);
		}
		else
		{
			// 공중에 있을 때: 동적 중력에 맞춤
			const FVector UpVectorInAir = -GravityDirection;
			const FVector ForwardVectorInAir = GetActorForwardVector();
			TargetRotation = FRotationMatrix::MakeFromZX(UpVectorInAir, ForwardVectorInAir).Rotator();
		}
	}
	// 최종 목표 회전값 보간
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
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
	const FVector ForwardFrictionForce = -ForwardVector * ForwardSpeed * (bIsDrift ? ForwardFriction * 0.4f : ForwardFriction); // 정면 마찰력 
	const FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * CurrentSidewaysFriction; // 측면 마찰력 * FMath::Clamp((bIsDrift? DriftTime : 1), 0, 16.f)

	const float GravityMagnitude = 980.f;
	const FVector GravityForce = -GroundNormal * GravityMagnitude * 2;
	GravityDirection = GravityForce.GetSafeNormal();

	FVector FinalForce = ThrustForce + SidewaysFrictionForce + ForwardFrictionForce + GravityForce;
	CurVelocity += FinalForce * GetWorld()->GetDeltaSeconds();
	
	// 지면 밀착
	if (bIsGround)
	{
		CurVelocity = FVector::VectorPlaneProject(CurVelocity, GroundNormal);
	}
	
	CurVelocity = CurVelocity.GetClampedToMaxSize(MaxSpeed + AddSpeed);
	CurSpeed = CurVelocity.Size();
	SpeedRatio = CurVelocity.Size() / MaxSpeed;
}

void AC_KartBase::SetFlyVelocity()
{
	const FVector RightVector = GetActorRightVector();
	const float SidewaysSpeed = FVector::DotProduct(CurVelocity, RightVector);  // 측면 속도
	
	const FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * 20.f; 
	// 가속 힘 계산
	const FVector ThrustForce = GetActorForwardVector() * AccelerationDir * AccelerationForce;
	
	const float GravityMagnitude = 980.f;
	const FVector GravityForce = -GroundNormal * GravityMagnitude * 2;
	GravityDirection = GravityForce.GetSafeNormal();

	FVector FinalForce = ThrustForce + GravityForce + SidewaysFrictionForce;
	CurVelocity += FinalForce * GetWorld()->GetDeltaSeconds();
	
	CurVelocity = CurVelocity.GetClampedToMaxSize(MaxSpeed + AddSpeed);
	CurSpeed = CurVelocity.Size();
	SpeedRatio = CurVelocity.Size() / MaxSpeed;
}

void AC_KartBase::UpdateSuspension(float DeltaTime)
{
	suspensioningTime += DeltaTime;

	float ratio = suspensioningTime / SuspensionTime;

	if (suspensioningTime >= SuspensionTime)
	{
		bIsSuspending = false;
		suspensioningTime = 0.f;
		return;
	}

	float targetZLocation = (1 - UC_MathUtility::EaseOutBounce(ratio)) * 10.f;

	StaticMeshComponent->SetRelativeLocation(FVector(0, 0, targetZLocation));
}
//
// void AC_KartBase::UpdateRotation()
// {
// 	if (CurSpeed > 100.f && !FMath::IsNearlyZero(HandlingDir))
// 	{
// 		const float TurnMultiplier = FMath::GetMappedRangeValueClamped(
// 			FVector2D(0.f, MaxSpeed), // 입력 범위 (현재 속도)
// 			FVector2D(1, 0.4f),     // 출력 범위 (회전 배율)
// 			CurSpeed
// 		);
// 		
// 		float TurnThisFrame = TurnSpeed * TurnMultiplier * HandlingDir * GetWorld()->GetDeltaSeconds();
//
// 		const float ForwardDot = FVector::DotProduct(CurVelocity.GetSafeNormal(), GetActorForwardVector());
// 		if (ForwardDot < 0.0f)
// 		{
// 			// 뒤로 움직이고 있을 때, 회전방향 반대
// 			TurnThisFrame *= -1.0f;
// 		}
// 		
// 		const FRotator RotationDelta(0.f, TurnThisFrame, 0.f);
// 		AddActorLocalRotation(RotationDelta);
// 	}
// 	
// 	FRotator TargetRotation;
// 	// 공중에 있을 땐 차체의 기울기를 수평으로 되돌림
// 	TargetRotation = GetActorRotation();
// 	TargetRotation.Pitch = 0.f;
// 	TargetRotation.Roll = 0.f;
// 	
// 	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 0.5f);
// 	SetActorRotation(NewRotation);
// }

void AC_KartBase::HandleCollision(const FHitResult& HitResult)
{
	const FVector ReflectedVelocity = FMath::GetReflectionVector(CurVelocity, HitResult.ImpactNormal);
	CurVelocity = ReflectedVelocity * CollisionDampingFactor;

	const float ImpactDot = FVector::DotProduct(GetActorForwardVector(), HitResult.ImpactNormal);

	const float FortyFiveDegreesInDot = FMath::Cos(FMath::DegreesToRadians(45.f));

	FVector TargetForwardDirection;

	// 입사각이 45도 이상일 경우 (정면 충돌에 가까울 때)
	if (FMath::Abs(ImpactDot) > FortyFiveDegreesInDot)
	{
		// 규칙 2: 노멀값에 평행한 방향 (벽에서 튕겨 나오는 방향)으로 차체를 정렬
		TargetForwardDirection = -HitResult.ImpactNormal;
	}
	else // 입사각이 45도 이하일 경우 (비스듬히 긁혔을 때)
	{
		// 규칙 1: 노멀값에 수직인 방향 (벽면과 평행하게 미끄러질 방향)으로 차체를 정렬
		TargetForwardDirection = FVector::VectorPlaneProject(GetActorForwardVector(), HitResult.ImpactNormal);
	}
    
	// 3. 계산된 목표 회전값을 변수에 저장하고, 충돌 후 상태를 활성화
	PostCollisionTargetRotation = FRotationMatrix::MakeFromX(TargetForwardDirection).Rotator();
	PostCollisionTargetRotation.Roll = 0.f;
	PostCollisionTargetRotation.Pitch = 0.f;
	bIsInPostCollisionState = true;
}

void AC_KartBase::StartAddSpeed(float Add)
{
	AddSpeed = Add;

	bIsBoosting = true;
	PlayBoostEffect();
	
	CurVelocity += GetActorForwardVector() * AddSpeed;
}

void AC_KartBase::DriftUpAction()
{
	MeshMoveDirection.Z = 150.f;
}

void AC_KartBase::PlayBoostEffect()
{
	//ActiveBoostEffects.Empty();

	BoostTimer = BoostDuration;
	
	if (ActiveBoostEffects.IsEmpty())
	{
		for (USceneComponent* Pipe : ExhaustPipe)
		{
			if (Pipe && BoostEffect)
			{
				UNiagaraComponent* EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
					BoostEffect,              
					Pipe,
					NAME_None,
					FVector::ZeroVector,      // 상대 위치
					FRotator(0, -90, 0),   // 상대 회전
					EAttachLocation::KeepRelativeOffset,
					false                     
				);

				if (EffectComponent)
				{
					EffectComponent->SetRelativeScale3D(FVector(0.8f));
					// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어
					ActiveBoostEffects.Add(EffectComponent);
				}
			}
		}
	}
}

void AC_KartBase::PlayDriftEffect(int EffectType, float DriftStartDirEffect)
{
	if (EffectType != CurrentDriftType)
	{
		for (UNiagaraComponent* Effect : ActiveDriftEffects)
		{
			if (IsValid(Effect))
			{
				Effect->DestroyComponent();
			}
		}
		ActiveDriftEffects.Empty();
		
		//if (ActiveBoostEffects.IsEmpty())
		{
			for (USceneComponent* Wheel : WheelBackDrift)
			{
				if (Wheel)
				{
					UNiagaraComponent* EffectComponent;
				
					if (EffectType == 1 && DriftEffect1)
					{
						CurrentDriftType = 1;
						EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
						DriftEffect1,
						Wheel,
						NAME_None,
						FVector::ZeroVector,      // 상대 위치
						FRotator(0, DriftStartDirEffect * -45, 0),   // 상대 회전
						EAttachLocation::KeepRelativeOffset,
						false);
					
						if (EffectComponent)
						{
							EffectComponent->SetRelativeScale3D(FVector(0.8f));
							// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어
							ActiveDriftEffects.Add(EffectComponent);
						}
					}
					else if (EffectType == 2 && DriftEffect2)
					{
						CurrentDriftType = 2;
						EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
						DriftEffect2,
						Wheel,
						NAME_None,
						FVector::ZeroVector,      // 상대 위치
						FRotator(0, DriftStartDirEffect * -45, 0),   // 상대 회전
						EAttachLocation::KeepRelativeOffset,
						false);
					
						if (EffectComponent)
						{						
							EffectComponent->SetRelativeScale3D(FVector(0.8f));
							// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어
							ActiveDriftEffects.Add(EffectComponent);
						}
					}
					else if (EffectType == 3 && DriftEffect3)
					{					
						CurrentDriftType = 3;
						EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
						DriftEffect3,
						Wheel,
						NAME_None,
						FVector::ZeroVector,      // 상대 위치
						FRotator(0, DriftStartDirEffect * -45, 0),   // 상대 회전
						EAttachLocation::KeepRelativeOffset,
						false);
					
						if (EffectComponent)
						{
							EffectComponent->SetRelativeScale3D(FVector(0.8f));
							// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어
							ActiveDriftEffects.Add(EffectComponent);
						}
					}

				}
			}
		}
	}

	
}

void AC_KartBase::UpdateStunEffect(float DeltaTime)
{
	StunRotationTimer += DeltaTime;

	float Alpha = FMath::Clamp(StunRotationTimer / StunDuration, 0.f, 1.f);

	float EasedAlpha = UC_MathUtility::EaseOutElastic(Alpha);
	
	float CurrentRoll = FMath::Lerp(0, 360.f, EasedAlpha);

	StaticMeshComponent->SetRelativeRotation(FRotator(0.f, CurrentRoll -90.f,0.f ));

	if (StunRotationTimer >= StunDuration)
	{
		bIsStunned = false;
		StaticMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f )); // 메시 회전을 원래대로 리셋
	}
}

// int32 AC_KartBase::FindClosestSplinePointIndex(const FVector& WorldLocation)
// {
// 	if (!SplineComponent || SplineComponent->GetNumberOfSplinePoints() < 1)
// 	{
// 		// 스플라인이 없거나 포인트가 없으면 유효하지 않은 인덱스 반환
// 		return -1;
// 	}
//
// 	int32 ClosestPointIndex = -1;
// 	float MinDistanceSquared = -1.f;
//
// 	const int32 PointCount = SplineComponent->GetNumberOfSplinePoints();
// 	// 0번부터 마지막 포인트까지 순회
// 	for (int32 i = 0; i < PointCount; ++i)
// 	{
// 		// i번째 스플라인 포인트의 월드 위치를 가져옴
// 		const FVector PointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
//         
// 		// 입력된 월드 위치와의 거리 제곱을 계산
// 		const float DistanceSquared = FVector::DistSquared(WorldLocation, PointLocation);
//
// 		// 첫 번째 포인트이거나, 이전에 찾은 최소 거리보다 현재 거리가 더 짧으면 정보 갱신
// 		if (ClosestPointIndex == -1 || DistanceSquared < MinDistanceSquared)
// 		{
// 			MinDistanceSquared = DistanceSquared;
// 			ClosestPointIndex = i;
// 		}
// 	}
//
// 	return ClosestPointIndex;
// }

