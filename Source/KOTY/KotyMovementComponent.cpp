// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyMovementComponent.h"

void UKotyMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Owner 및 Component의 유효성을 검증하고, 유의미한 이동 속도인지 확인
	if (!GetOwner() || !UpdatedComponent || MoveVelocity.IsNearlyZero())
	{
		return;
	}

	//시뮬레이션이 활성화 확인
	if (!bSimulate)
	{
		return;
	}

	//중력 가속도 적용
	MoveVelocity += GravityDir * GravityForce;
	
	//이번 틱에 이동할 변화량
	const FVector Delta = MoveVelocity * DeltaTime;

	//충돌 결과 구조체
	FHitResult Hit;

	//충돌 감지 이동
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	//유효한 충돌
	if (Hit.IsValidBlockingHit())
	{
		//이동 방향과 평면 노말의 내적
		const float Dot = FVector::DotProduct(-MoveVelocity, Hit.ImpactNormal);
		
		//이동방향과 거의 평행한 충돌으로 판단
		if (FMath::Abs(Dot) < 0.05f)
		{
			//정사영 연산
			const FVector Impact = Dot * Hit.ImpactNormal;
			MoveVelocity += Impact;
		}
		else
		{
			//반사 연산
			const FVector Impact = Dot * Hit.ImpactNormal;
			MoveVelocity += 2 * Impact;
		}
	}
}

void UKotyMovementComponent::Init(const bool InbSimulate, const FVector& InGravityDir, const float InGravityForce, const float InLinearDrag)
{
	this->bSimulate = InbSimulate;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;
	this->LinearDrag = InLinearDrag;
}
