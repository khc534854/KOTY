// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyMovementComponent.h"

#include "KismetTraceUtils.h"

void UKotyMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UE_LOG(LogTemp, Log, TEXT("KotyComponent Initialized!"));
}

void UKotyMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//시뮬레이션이 활성화 확인
	if (!bSimulate)
	{
		return;
	}

	const FVector Start = GetOwner()->GetActorLocation();
	const FVector End = GetOwner()->GetActorLocation() + GravityDir * 300;
	FHitResult GravityChecker;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceSingleByChannel(GravityChecker, Start, End, ECC_Visibility, TraceParams);

	//중력 검출기에 반응
	if (GravityChecker.IsValidBlockingHit())
	{
		//새로운 중력 방향
		const FVector NewGravityDir = -GravityChecker.ImpactNormal;
		
		//회전량
		const float AngleDeg = FMath::Acos(FVector::DotProduct(GravityDir, NewGravityDir));
			
		//이전 평면에서 현재 평면으로 회전하기 위한 축
		const FVector RotationAxis = FVector::CrossProduct(GravityDir, NewGravityDir);
			
		if (!FMath::IsNearlyZero(AngleDeg) && !RotationAxis.IsNearlyZero())
		{
			//회전 쿼터니언을 이용하여 회전
			const FQuat RotationQuat(RotationAxis, AngleDeg);
			MoveVelocity = RotationQuat.RotateVector(MoveVelocity);
			UE_LOG(LogTemp, Log, TEXT("RotateAlongSurface! %f"), AngleDeg);
		}

		//중력 업데이트
		GravityDir = NewGravityDir;
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

	//충돌 결과 구조체
	FHitResult Hit;

	//충돌 감지 이동
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	//유효한 충돌
	if (Hit.IsValidBlockingHit())
	{
		//현재 평면 법선 벡터
		const FVector CurrSurfaceNormal = Hit.ImpactNormal;
		
		//충격 방향과 중력 방향의 내적
		const float GravityDot = FVector::DotProduct(CurrSurfaceNormal, GravityDir);

		//0~1 사이의 값으로 정규화
		const float GravityTime = FMath::Max(0, -GravityDot);

		//중력 방향 마찰력
		const float DragAgainstGravity = FMath::Lerp(0, LinearDrag, GravityTime);
		MoveVelocity += -MoveVelocity * DragAgainstGravity * DeltaTime;

		//이동 방향과 평면 노말의 내적
		const float NormalizedCollisionDot = FVector::DotProduct(MoveVelocity.GetSafeNormal(), CurrSurfaceNormal);
		const float CollisionDot = FVector::DotProduct(-MoveVelocity, CurrSurfaceNormal);
		UE_LOG(LogTemp, Log, TEXT("Degree : %f"), FMath::Acos(FMath::Abs(NormalizedCollisionDot)));
		
		//이동방향과 거의 평행한 충돌으로 판단
		if (FMath::Abs(NormalizedCollisionDot) < FMath::Cos(5.0))
		{
			//정사영 연산
			const FVector Impact = CollisionDot * CurrSurfaceNormal;
			MoveVelocity += Impact;
		}
		else
		{
			//중력 방향 충격에 대한 탄성
			const float ElasticityAgainstGravity = FMath::Lerp(1.0, SurfaceElasticity, GravityTime);
			
			//반사 연산
			const FVector Impact = CollisionDot * CurrSurfaceNormal;
			MoveVelocity += Impact + Impact * ElasticityAgainstGravity;

			UE_LOG(LogTemp, Log, TEXT("Surface!!!"))
		}

		//충돌 처리가 완료됐고, 새로운 방향으로 이동
		UpdatedComponent->MoveComponent((MoveVelocity * DeltaTime), UpdatedComponent->GetComponentRotation(), false);
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
