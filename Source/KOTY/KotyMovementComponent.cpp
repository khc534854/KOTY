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

	//중력 검출에 성공하면 이전 GravityDir은 소실 
	FVector LastGravityDir = GravityDir;
	
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

		//UE_LOG(LogTemp, Log, TEXT("%f %f %f"), NewGravityDir.X, NewGravityDir.Y, NewGravityDir.Z);

		//구면 보간을 통해 적절하게 중력 재설정
		GravityDir = FVector::SlerpNormals(GravityDir, NewGravityDir, 0.5);
	}
	
#pragma endregion

#pragma region 지면 회전

	//중력 검출의 부산물을 이용, 역시 하나라도 실패했다면 회전 불가
	if (GravityTraceHits.Num() == NumOfTraceTry)
	{
		//보존해놓았던 이전 중력 방향을 이용하여 회전량 산출
		float DotProduct = FMath::Clamp(FVector::DotProduct(LastGravityDir, GravityDir), -1.0, 1.0);
		float RotationDegree = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
				
		//회전축
		const FVector RotationAxis = FVector::CrossProduct(LastGravityDir, -GravityDir);

		//UE_LOG(LogTemp, Log, TEXT("%f : %f %f %f"), RotationDegree, RotationAxis.X, RotationAxis.Y, RotationAxis.Z);
				
		//안전하게 이동 방향을 회전
		if (RotationAxis.IsNearlyZero() == false)
		{
			Velocity = Velocity.RotateAngleAxis(RotationDegree, RotationAxis);	
		}
	}

#pragma endregion 

#pragma region 속도 연산
	
	//옵션이 활성화되어 있다면 충돌 등으로 인해 손실된 수평 속도를 복귀시킨다
	if (bConstantHorizonSpeed)
	{
		const FVector VerticalValue = FVector::DotProduct(Velocity, GravityDir) * GravityDir;
		const FVector HorizonValue = Velocity - VerticalValue;
		Velocity = HorizonValue.GetSafeNormal() * ConstantHorizonSpeed + VerticalValue;
	}
	
	//중력 가속도 적용
	Velocity += GravityDir * GravityForce * DeltaTime;

#pragma endregion
	
	//유의미한 이동 속도인지 확인
	if (Velocity.IsNearlyZero())
	{
		return;
	}	

#pragma region 이동 적용
	
	//이번 틱에 이동할 변화량
	const FVector Delta = Velocity * DeltaTime;

	//UE_LOG(LogTemp, Log, TEXT("%f %f %f"), Delta.X, Delta.Y, Delta.Z);
	
	//충돌 감지 이동
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

#pragma endregion

#pragma region 충돌 처리

	//전방 충돌 처리
	if (Hit.IsValidBlockingHit())
	{
		//바닥면과의 충돌 처리
		if (FVector::DotProduct(Hit.ImpactNormal, GravityDir) < -0.95)
		{
			//충돌한 지면의 노말 벡터
			const FVector SurfaceNormal = Hit.ImpactNormal;
			
			//지면과 중력의 일치율 평가
			const float GravityDot = FVector::DotProduct(SurfaceNormal, GravityDir);
			const float GravityTime = FMath::Max(0, -GravityDot);

			//수평 속도 손실 활성화?
			if (!bConstantHorizonSpeed)
			{
				//일치할 수록 중력 방향 마찰력
				const float DragAgainstGravity = FMath::Lerp(0, LinearDrag, GravityTime);
				Velocity += -Velocity * DragAgainstGravity * DeltaTime;	
			}

			//중력 방향 충격에 대한 탄성
			const float ElasticityAgainstGravity = FMath::Lerp(1.0, SurfaceElasticity, GravityTime);

			//중력과 일치하는 정도에 따라서 탄성력 적용
			const float CollisionDot = FVector::DotProduct(-Velocity, SurfaceNormal);

			//반사 충격 벡터
			const FVector Impact = CollisionDot * SurfaceNormal;

			//충분한 충격량을 가질 때에만 반사 연산
			if (CollisionDot > 50)
			{
				//지면에 대한 반사 처리
				Velocity = Velocity + Impact + Impact * ElasticityAgainstGravity;
			}
			else
			{
				//지면에 대한 정사영 처리
				Velocity = Velocity + Impact;
			}

			//충분한 충격량을 가질 때에만 호출
			if (CollisionDot > 500)
			{
				//지면 충돌 델리게이트 브로드캐스트
				OnBounceEventDispatcher.Broadcast();
			}

			//UE_LOG(LogTemp, Log, TEXT("Surface"));
		}
		else if(FVector::DotProduct(Hit.ImpactNormal, Velocity.GetSafeNormal()) < -0.1)
		{
			FVector WallNormal = Hit.ImpactNormal;
			
			//스텝 확인을 위해 충돌체 접근
			if (SphereComp)
			{
				//구 충돌체 반지름
				const float SphereCollisionRadius = SphereComp->GetScaledSphereRadius();	

				//내적을 통해 부딪힌 지점의 높이를 구할 수 있다
				const FVector Location = GetOwner()->GetActorLocation();
				const FVector HitVector = Hit.ImpactPoint - Location;

				//스텝 가능한 높이
				if (const float HitHeight = SphereCollisionRadius - FVector::DotProduct(HitVector, GravityDir);
					HitHeight < StepUpLimit)
				{
					//스텝 위치로 이동
					FVector Step = - GravityDir * (HitHeight + 5);
					MoveUpdatedComponent(Step, UpdatedComponent->GetComponentQuat(), true);
					UE_LOG(LogTemp, Log, TEXT("StepUp has been executed!"));
					return;
				}
			}
			
			//반사 연산
			const float CollisionDot = FVector::DotProduct(-Velocity, WallNormal);
			const FVector Impact = CollisionDot * WallNormal;
			Velocity = Velocity + 2 * Impact;

			//UE_LOG(LogTemp, Log, TEXT("Wall"));
		}

		//최종 처리 속도를 근거로 이동
		MoveUpdatedComponent(Velocity * DeltaTime, UpdatedComponent->GetComponentQuat(), true);	
	}
	
#pragma endregion
	
}

void UKotyMovementComponent::ThrowConstantHorizon(
	const bool InbSimulate,
	const FVector InGravityDir,
	const float InGravityForce,
	const float InConstantHorizonSpeed,
	const float InStepUpLimit,
	const FVector InVelocity,
	const float InSurfaceElasticity)
{
	this->bSimulate = InbSimulate;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;
	this->LinearDrag = 0;
	this->bConstantHorizonSpeed = true;
	this->ConstantHorizonSpeed = InConstantHorizonSpeed;
	this->StepUpLimit = InStepUpLimit;
	this->Velocity = InVelocity;
	this->SurfaceElasticity = InSurfaceElasticity;
}

void UKotyMovementComponent::ThrowLinearDrag(
	const bool InbSimulate,
	const FVector InGravityDir,
	const float InGravityForce,
	const float InLinearDrag,
	const float InStepUpLimit,
	const FVector InVelocity,
	const float InSurfaceElasticity)
{
	this->bSimulate = InbSimulate;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;
	this->LinearDrag = InLinearDrag;
	this->bConstantHorizonSpeed = false;
	this->ConstantHorizonSpeed = 0;
	this->StepUpLimit = InStepUpLimit;
	this->Velocity = InVelocity;
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

void UKotyMovementComponent::SLerpVelocity(const FVector TargetDir)
{
	Velocity = FVector::SlerpVectorToDirection(Velocity, TargetDir, 0.25);
}

FVector UKotyMovementComponent::GetGravityDir() const
{
	return GravityDir;
}
