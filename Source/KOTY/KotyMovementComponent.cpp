// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyMovementComponent.h"

#include "KismetTraceUtils.h"
#include "Components/SphereComponent.h"

FVector UKotyMovementComponent::FindTrackSurface(const float TraceRadius, const int NumRays) const
{
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	FVector ClosestNormal = FVector::ZeroVector;
	float MinDistance = TNumericLimits<float>::Max();

	//유효성 확인
	const UWorld* World = GetWorld();
	if (!World)
	{
		return ClosestNormal;
	}

	//복수의 라인 트레이스
	for (int i = 0; i < NumRays; ++i)
	{
		//골든 앵글 - 골고루 퍼지도록 구면좌표 샘플링
		const float Theta = FMath::Acos(1 - 2 * (static_cast<float>(i) / NumRays));
		const float Phi = FMath::Fmod(i * PI * (3.0f - FMath::Sqrt(5.0f)), 2 * PI);
		FVector Direction = FVector(
			FMath::Sin(Theta) * FMath::Cos(Phi),
			FMath::Sin(Theta) * FMath::Sin(Phi),
			FMath::Cos(Theta)
		);

		//후방은 확인할 필요 없다
		if (Direction.Dot(MoveVelocity.GetSafeNormal()) < 0.7)
		{
			continue;
		}

		//라인 트레이스
		FVector Start = ActorLocation;
		FVector End = Start + Direction * TraceRadius;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
		
		//트레이스 결과
		if (Hit.bBlockingHit)
		{
			//업데이트
			const float Distance = FVector::Dist(Start, Hit.ImpactPoint);
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestNormal = Hit.ImpactNormal;
			}
		}
		else
		{
			// 미히트 디버그 라인
			DrawDebugLine(World, Start, End, FColor::Blue, false, 0.5f);
		}
	}

	return ClosestNormal;
}

void UKotyMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UE_LOG(LogTemp, Log, TEXT("KotyComponent Initialized!"));
}

void UKotyMovementComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//시뮬레이션이 활성화 확인
	if (!bSimulate)
	{
		return;
	}

	//트랙 기반 중력
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + GravityDir * GravityForce;
	FHitResult GravityHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceSingleByChannel(GravityHit, Start, End, ECC_Visibility, Params);
	if (GravityHit.bBlockingHit)
	{
		GravityDir = -GravityHit.ImpactNormal;
	}
	
	//중력 가속도 적용
	MoveVelocity += GravityDir * GravityForce * DeltaTime;
	
	//Owner 및 Component의 유효성을 검증하고, 유의미한 이동 속도인지 확인
	if (!GetOwner() || !UpdatedComponent || MoveVelocity.IsNearlyZero())
	{
		return;
	}
	
	//이번 틱에 이동할 변화량
	const FVector Delta = MoveVelocity * DeltaTime;
	
	//충돌 감지 이동
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	//충돌 처리
	if (Hit.IsValidBlockingHit())
	{
		//현재 평면 법선 벡터
		const FVector SurfaceNormal = Hit.ImpactNormal;

		//바닥면과의 충돌 처리
		if (FVector::DotProduct(Hit.ImpactNormal, GravityDir) < -0.95)
		{
			//지면과 중력의 일치율 평가
			const float GravityDot = FVector::DotProduct(SurfaceNormal, GravityDir);
			const float GravityTime = FMath::Max(0, -GravityDot);

			//일치할 수록 중력 방향 마찰력
			const float DragAgainstGravity = FMath::Lerp(0, LinearDrag, GravityTime);
			MoveVelocity += -MoveVelocity * DragAgainstGravity * DeltaTime;

			//중력 방향 충격에 대한 탄성
			const float ElasticityAgainstGravity = FMath::Lerp(1.0, SurfaceElasticity, GravityTime);

			//반사 연산
			const float CollisionDot = FVector::DotProduct(-MoveVelocity, SurfaceNormal);
			const FVector Impact = CollisionDot * SurfaceNormal;

			//중력과 일치하는 정도에 따라서 탄성력 적용
			MoveVelocity = MoveVelocity + Impact + Impact * ElasticityAgainstGravity;

			//로그
			UE_LOG(LogTemp, Log, TEXT("Surface Attenuation Reflect!"))
		}
		else
		{
			// //스텝업 연산
			// if (const USphereComponent* SphereComp = Cast<USphereComponent>(GetOwner()->GetRootComponent()))
			// {
			// 	//지면과 맞닿은 바닥 위치
			// 	float Radius = SphereComp->GetScaledSphereRadius();
			// 	FVector Location = SphereComp->GetComponentLocation();
			//
			// 	//라인 트레이스
			// 	const FVector Foot = Location - Radius * SurfaceNormal;
			// 	FVector StepStart = Foot + 2 * SurfaceNormal;
			// 	FVector StepEnd = StepStart + Radius * MoveVelocity.GetSafeNormal();
			// 	FHitResult StepHit;
			// 	FCollisionQueryParams StepParams;
			// 	StepParams.AddIgnoredActor(GetOwner());
			// 	GetWorld()->LineTraceSingleByChannel(StepHit, StepStart, StepEnd, ECC_Visibility, Params);
			//
			// 	//스텝 업 가능
			// 	if (!StepHit.bBlockingHit)
			// 	{
			// 		FVector StepLocation = (2 * MoveVelocity.GetSafeNormal()) + (2 * SurfaceNormal);
			// 		GetOwner()->SetActorLocation(StepLocation);
			// 		return;
			// 	}
			// }
			// else
			// {
			// 	
			// }

			//반사 연산
			const float CollisionDot = FVector::DotProduct(-MoveVelocity, SurfaceNormal);
			const FVector Impact = CollisionDot * SurfaceNormal;
			MoveVelocity = MoveVelocity + 2 * Impact;
		}

		//충돌 최종 처리
		MoveUpdatedComponent(MoveVelocity * DeltaTime, UpdatedComponent->GetComponentQuat(), false);	
	}
}

void UKotyMovementComponent::Init(
		const bool InbSimulate,
		const FVector& InMoveVelocity,
		const FVector& InGravityDir,
		const float InGravityForce,
		const float InLinearDrag,
		const float InSurfaceElasticity)
{
	this->bSimulate = InbSimulate;
	this->MoveVelocity = InMoveVelocity;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;
	this->LinearDrag = InLinearDrag;
	this->SurfaceElasticity = InSurfaceElasticity;
}
