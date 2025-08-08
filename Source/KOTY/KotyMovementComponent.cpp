// Fill out your copyright notice in the Description page of Project Settings.

#include "KotyMovementComponent.h"
#include "Components/SphereComponent.h"

void UKotyMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//충돌체 접근
	SphereComp = Cast<USphereComponent>(GetOwner()->GetRootComponent());
	if (SphereComp)
	{
		UE_LOG(LogTemp, Log, TEXT("KotyComponent Successfully Initialized!"));	
	}
}

void UKotyMovementComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//액터의 유효성 검증
	if (!GetOwner() || !UpdatedComponent)
	{
		return;
	}

	//시뮬레이션 활성화 확인
	if (!bSimulate)
	{
		return;
	}

#pragma region 중력 검출
	
	//현재 중력 방향으로 복수의 라인 트레이스
	//결과값을 총합하여 새로운 중력 방향으로
	constexpr int32 NumOfTraceTry = 36;
	TArray<FHitResult> GravityTraceHits;
	for (int32 i = 0; i < NumOfTraceTry; i++)
	{
		//원형을 그리며 라인 트레이스
		float Sin = 0;
		float Cos = 0;
		FMath::SinCos(&Sin, &Cos, i * NumOfTraceTry);
		const float Radius = SphereComp->GetScaledSphereRadius() / 2;
		const float X = Sin * (Radius / 2);
		const float Y = Cos * (Radius / 2);
		const FVector Start = GetOwner()->GetActorLocation() + FVector(X, Y, 0);
		const FHitResult GravityHit = LineTraceGravityDirTrack(Start);

		//하나라도 실패하면 업데이트 불가 판정
		if (GravityHit.bBlockingHit == false)
		{
			break;
		}

		//배열에 추가
		GravityTraceHits.Emplace(GravityHit);
	}

	//하나라도 실패하면 업데이트 불가 판정
	if (GravityTraceHits.Num() == NumOfTraceTry)
	{
		//모든 지면 데이터 총합
		FVector SumOfNormals = FVector::ZeroVector;
		for (int32 i = 0; i < GravityTraceHits.Num(); i++)
		{
			SumOfNormals += GravityTraceHits[i].ImpactNormal;
		}
		FVector NewGravityDir = -SumOfNormals.GetSafeNormal();
		UE_LOG(LogTemp, Log, TEXT("%f %f %f"), NewGravityDir.X, NewGravityDir.Y, NewGravityDir.Z);
		GravityDir = FVector::SlerpNormals(GravityDir, NewGravityDir, 0.5);
	}
	
#pragma endregion 
	
	//옵션이 활성화되어 있다면 충돌 등으로 인해 손실된 수평 속도를 복귀시킨다
	if (bConstantHorizonSpeed)
	{
		const FVector VerticalValue = FVector::DotProduct(MoveVelocity, GravityDir) * GravityDir;
		const FVector HorizonValue = MoveVelocity - VerticalValue;
		MoveVelocity = HorizonValue.GetSafeNormal() * ConstantHorizonSpeed + VerticalValue;
	}
	
	//중력 가속도 적용
	MoveVelocity += GravityDir * GravityForce * DeltaTime;
	
	//유의미한 이동 속도인지 확인
	if (MoveVelocity.IsNearlyZero())
	{
		return;
	}
	
	//이번 틱에 이동할 변화량
	const FVector Delta = MoveVelocity * DeltaTime;
	
	//충돌 감지 이동
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

#pragma region 충돌 처리

	//전방 충돌 처리
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

			//수평 속도 손실 활성화?
			if (!bConstantHorizonSpeed)
			{
				//일치할 수록 중력 방향 마찰력
				const float DragAgainstGravity = FMath::Lerp(0, LinearDrag, GravityTime);
				MoveVelocity += -MoveVelocity * DragAgainstGravity * DeltaTime;	
			}

			//중력 방향 충격에 대한 탄성
			const float ElasticityAgainstGravity = FMath::Lerp(1.0, SurfaceElasticity, GravityTime);

			//중력과 일치하는 정도에 따라서 탄성력 적용
			const float CollisionDot = FVector::DotProduct(-MoveVelocity, SurfaceNormal);
			const FVector Impact = CollisionDot * SurfaceNormal;
			MoveVelocity = MoveVelocity + Impact + Impact * ElasticityAgainstGravity;

			UE_LOG(LogTemp, Log, TEXT("Surface"));

			//충돌 최종 처리
			MoveUpdatedComponent(MoveVelocity * DeltaTime, UpdatedComponent->GetComponentQuat(), false);	
		}
		else if(FVector::DotProduct(Hit.ImpactNormal, MoveVelocity.GetSafeNormal()) < -0.1)
		{
			//스텝 확인을 위해 충돌체 접근
			if (SphereComp)
			{
				//구 충돌체 반지름
				const float SphereCollisionRadius = SphereComp->GetScaledSphereRadius();	

				//내적을 통해 부딪힌 지점의 높이를 구할 수 있다
				const FVector Location = GetOwner()->GetActorLocation();
				const FVector HitVector = Hit.ImpactPoint - Location;
				const float HitHeight = SphereCollisionRadius - FVector::DotProduct(HitVector, GravityDir);

				//스텝 가능한 높이
				if (HitHeight < StepUpLimit)
				{
					//스텝 위치로 이동
					GetOwner()->SetActorLocation(Hit.ImpactPoint + FVector(0, 0, HitHeight + 0.5));
					return;
				}
			}
			
			//반사 연산
			const float CollisionDot = FVector::DotProduct(-MoveVelocity, SurfaceNormal);
			const FVector Impact = CollisionDot * SurfaceNormal;
			MoveVelocity = MoveVelocity + 2 * Impact;

			UE_LOG(LogTemp, Log, TEXT("Wall"));

			//충돌 최종 처리
			MoveUpdatedComponent(MoveVelocity * DeltaTime, UpdatedComponent->GetComponentQuat(), false);	
		}
	}
	
#pragma endregion
	
}

void UKotyMovementComponent::ThrowConstantHorizon(
	const bool InbSimulate,
	const FVector InGravityDir,
	const float InGravityForce,
	const float InConstantHorizonSpeed,
	const float InStepUpLimit,
	const FVector InMoveVelocity,
	const float InSurfaceElasticity)
{
	this->bSimulate = InbSimulate;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;
	this->LinearDrag = 0;
	this->bConstantHorizonSpeed = true;
	this->ConstantHorizonSpeed = InConstantHorizonSpeed;
	this->StepUpLimit = InStepUpLimit;
	this->MoveVelocity = InMoveVelocity;
	this->SurfaceElasticity = InSurfaceElasticity;
}

void UKotyMovementComponent::ThrowLinearDrag(
	const bool InbSimulate,
	const FVector InGravityDir,
	const float InGravityForce,
	const float InLinearDrag,
	const float InStepUpLimit,
	const FVector InMoveVelocity,
	const float InSurfaceElasticity)
{
	this->bSimulate = InbSimulate;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;
	this->LinearDrag = InLinearDrag;
	this->bConstantHorizonSpeed = false;
	this->ConstantHorizonSpeed = 0;
	this->StepUpLimit = InStepUpLimit;
	this->MoveVelocity = InMoveVelocity;
	this->SurfaceElasticity = InSurfaceElasticity;
}

FHitResult UKotyMovementComponent::LineTraceGravityDirTrack(const FVector Start) const
{
	//현재 중력 방향으로 
	const FVector End = Start + GravityDir * 300;
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	//충돌 결과 구조체 반환
	return Hit;
}