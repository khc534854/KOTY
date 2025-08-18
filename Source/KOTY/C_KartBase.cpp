// Fill out your copyright notice in the Description page of Project Settings.


#include "C_KartBase.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
AC_KartBase::AC_KartBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
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
	//UpdateSuspension(DeltaTime);
	UpdateBodyRotation(DeltaTime);

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
	FVector NewLocation;

	NewLocation = GetActorLocation() + CurVelocity * DeltaTime;
	
	SetActorLocation(NewLocation);
	//SetActorLocation(NewLocation, true);

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
	FVector EndLocation = StartLocation - (GetActorUpVector() * 65.f);
	
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
		bIsGround = true;
		GroundNormal = HitResult.ImpactNormal;
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
	// 현재 조향(Yaw)이 적용된 회전 값을 가져옵니다.
	const FRotator CurrentRotation = GetActorRotation();

	// 목표가 될 회전 값을 선언합니다.
	FRotator TargetRotation;

	if (bIsGround)
	{
		const FVector ForwardOnGround = FVector::VectorPlaneProject(GetActorForwardVector(), GroundNormal).GetSafeNormal();
		const FRotator RotationOnGround = FRotationMatrix::MakeFromZX(GroundNormal, ForwardOnGround).Rotator();
		TargetRotation = FRotator(RotationOnGround.Pitch, CurrentRotation.Yaw, RotationOnGround.Roll);
		const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, BodyRotationInterpolationSpeed);
		SetActorRotation(NewRotation);
	}
	else
	{
		const FVector UpVectorInAir = -FVector(0, 0, -1);
		const FVector ForwardVectorInAir = GetActorForwardVector();
		const FRotator TargetRotationInAir = FRotationMatrix::MakeFromZX(UpVectorInAir, ForwardVectorInAir).Rotator();
		const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotationInAir, DeltaTime, 2);
		SetActorRotation(NewRotation);
	}
    
	// 최종적으로 계산된 목표 회전값을 향해 부드럽게 보간합니다.
	
	// if (!bIsGround)
	// {
	// 	FRotator LevelRotation = GetActorRotation();
	// 	LevelRotation.Pitch = 0.f;
	// 	LevelRotation.Roll = 0.f;
	// 	SetActorRotation(FMath::RInterpTo(GetActorRotation(), LevelRotation, DeltaTime, 2.0f));
	// 	return;
	// }
	//
	// // 평면 프로젝션
	// const FVector ForwardVectorOnGround = FVector::VectorPlaneProject(GetActorForwardVector(), GroundNormal).GetSafeNormal();
 //    
	// // 목표 회전값
	// const FRotator TargetRotation = FRotationMatrix::MakeFromZX(GroundNormal, ForwardVectorOnGround).Rotator();
	//
	// //보간
	// const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, BodyRotationInterpolationSpeed);
	//
	// SetActorRotation(NewRotation);
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
	
	// if (bIsGround)
	// {
	// 	// 땅에 있을 땐 미끄러지는 힘만 적용
	// 	FinalForce = ThrustForce + SidewaysFrictionForce + ForwardFrictionForce + (GravityForce - (FVector::DotProduct(GravityVector, GroundNormal) * GroundNormal));
	// 	 
	// }
	// else // 공중에 있을 때
	// {
	// 	// 공중에 있을 땐 중력 전체를 적용
	// 	FinalForce = ThrustForce + SidewaysFrictionForce + ForwardFrictionForce + GravityForce * 2.f;
	// }
    
	// 2. 속도 업데이트
    
	// 3. 지면 밀착 로직 (기존과 동일하지만, 이제 모든 표면에서 작동)
	if (bIsGround)
	{
		CurVelocity = FVector::VectorPlaneProject(CurVelocity, GroundNormal);
	}
	
	// FVector SlopeGravityForce = FVector::ZeroVector; // 중력
	// if (bIsGround)
	// {
	// 	// 중력 경사면 투영
	// 	const FVector GravityVector(0.f, 0.f, GetWorld()->GetGravityZ());
	// 	SlopeGravityForce = GravityVector - (FVector::DotProduct(GravityVector, GroundNormal) * GroundNormal);
	// }
	
	// 최종 힘 계산
	// const FVector TotalForce = ThrustForce + SidewaysFrictionForce + ForwardFrictionForce + SlopeGravityForce;
	//
	// // 속도 업데이트
	// CurVelocity += TotalForce * GetWorld()->GetDeltaSeconds();
	//
	// CurVelocity.Z += GetWorld()->GetGravityZ() * GetWorld()->GetDeltaSeconds() * 2.f;
	//
	// if (bIsGround)
	// {
	// 	CurVelocity = FVector::VectorPlaneProject(CurVelocity, GroundNormal);
	// }
	
	CurVelocity = CurVelocity.GetClampedToMaxSize(MaxSpeed + AddSpeed);

	CurSpeed = CurVelocity.Size();
	
	if (CurSpeed > 100.f && !FMath::IsNearlyZero(HandlingDir))
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
	// 지면의 기울기(GroundNormal)와 현재 차의 앞 방향을 기준으로 목표 회전값을 계산
	TargetRotation = FRotationMatrix::MakeFromZX(GroundNormal, GetActorForwardVector()).Rotator();
	

	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), BodyRotationInterpolationSpeed);
	SetActorRotation(NewRotation);
	
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

	if (CurSpeed > 100.f && !FMath::IsNearlyZero(HandlingDir))
	{
		const float TurnMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, MaxSpeed), // 입력 범위 (현재 속도)
			FVector2D(0.3f, 0.1f),     // 출력 범위 (회전 배율)
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
	// 공중에 있을 땐 차체의 기울기를 수평으로 되돌림
	TargetRotation = GetActorRotation();
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;
	
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 0.5f);
	SetActorRotation(NewRotation);


	
	SpeedRatio = CurVelocity.Size() / MaxSpeed;
}

void AC_KartBase::UpdateSuspension(float DeltaTime)
{
	// 서스펜션 구현
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
					// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어할 수 있도록 합니다.
					ActiveBoostEffects.Add(EffectComponent);
				}
			}
		}
	}
}

void AC_KartBase::PlayDriftEffect(int EffectType, float DriftStartDir)
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
						FRotator(0, DriftStartDir * -45, 0),   // 상대 회전
						EAttachLocation::KeepRelativeOffset,
						false);
					
						if (EffectComponent)
						{
							EffectComponent->SetRelativeScale3D(FVector(0.8f));
							// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어할 수 있도록 합니다.
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
						FRotator(0, DriftStartDir * -45, 0),   // 상대 회전
						EAttachLocation::KeepRelativeOffset,
						false);
					
						if (EffectComponent)
						{						
							EffectComponent->SetRelativeScale3D(FVector(0.8f));
							// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어할 수 있도록 합니다.
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
						FRotator(0, DriftStartDir * -45, 0),   // 상대 회전
						EAttachLocation::KeepRelativeOffset,
						false);
					
						if (EffectComponent)
						{
							EffectComponent->SetRelativeScale3D(FVector(0.8f));
							// 생성된 이펙트 컴포넌트를 배열에 추가하여 나중에 제어할 수 있도록 합니다.
							ActiveDriftEffects.Add(EffectComponent);
						}
					}

				}
			}
		}
	}

	
}

