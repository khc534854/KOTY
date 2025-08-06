// Fill out your copyright notice in the Description page of Project Settings.


#include "TurtleItem.h"

#include "KismetTraceUtils.h"

// Sets default values
ATurtleItem::ATurtleItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bActive = false;
	GravityDir = FVector::ZeroVector;
	GravityForce = 980;
	Velocity = FVector::ZeroVector;
	MoveHorizonSpeed = 2000;

	//콜리전 배열 초기화
	CollisionArray.Init(0, CollisionPrecision);
}

void ATurtleItem::Init(
	const FVector InShootDir,
	const float InSpeed,
	const FVector InGravityDir,
	const float InGravityForce,
	const float InLifeSpan)
{
	this->MoveHorizonSpeed = InSpeed;
	this->GravityDir = InGravityDir;
	this->GravityForce = InGravityForce;

	//처음에는 수직 속도가 살아있다
	Velocity = InShootDir * InSpeed;

	//생명 주기
	SetLifeSpan(InLifeSpan);
}


// Called when the game starts or when spawned
void ATurtleItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATurtleItem::AdjustVelocityToHorizon()
{
	//이동 벡터를 지면에 정사영하여 평행 이동할 수 있게 조정
	Velocity -= FVector::DotProduct(Velocity, GravityDir) * GravityDir;
}

void ATurtleItem::ApplyReflection()
{
	//이번 충돌 정보를 저장하기 위한 리플렉션 벡터 배열
	TArray<FHitResult> HitResults;

	//360도 라인 트레이스 정밀도
	const float Section = 360 / CollisionPrecision;
	const FVector Start = GetActorLocation();

	//360도 라인 트레이싱
	for (int i = 0; i < CollisionPrecision; i++)
	{
		//섹션 별 트레이스 위치
		constexpr float Range = 75;
		const float EndX = FMath::Cos(i * Section) * Range;
		const float EndY = FMath::Sin(i * Section) * Range;
		FVector End = FVector(Start.X + EndX, Start.Y + EndY, Start.Z);

		//트레이스 실행
		FHitResult OutHit;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility))
		{
			if (CollisionArray[i] != 0)
				continue;
			
			//다음 5틱 동안 해당 센서에 의한 반사를 차단
			CollisionArray[i] = 5;
		
			//배열 삽입
			HitResults.Emplace(OutHit);
		}
		else
		{
			//카운터를 하나 깎는다
			CollisionArray[i] = FMath::Max(0, CollisionArray[i] - 1);
		}
	}
	
	//충돌이 없다면 반사를 할 필요 없다
	if (HitResults.IsEmpty())
		return;
	
	//입사 벡터
	const FVector Projection = Velocity;

	//반사 벡터 합산용
	FVector SumReflection = FVector::ZeroVector;

	//위치 합산용
	FVector SumLocation = FVector::ZeroVector;

	//모든 반사 벡터 합산
	for (const FHitResult& InHitResult : HitResults)
	{
		//위치 벡터 합산
		SumLocation += InHitResult.Location;

		//반사벡터 합산
		FVector ImpactNormal = InHitResult.ImpactNormal;
		const float Dot = FVector::DotProduct(ImpactNormal, -Projection);
		const FVector Reflection = Projection + 2 * Dot * ImpactNormal;
		SumReflection += Reflection;
	}

	//속도 업데이트
	Velocity = SumReflection.GetSafeNormal() * MoveHorizonSpeed;
	
	//미리 조금 움직인다
	UpdateLocation();
}

void ATurtleItem::ApplyBounce()
{
	//중력 방향으로 5m 짜리 라인 트레이스
	const FVector Start = GetActorLocation();
	const FVector End = GetActorLocation() + GravityDir * 90;
	
	//지면과의 충돌 발생
	if (FHitResult OutHit; GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility))
	{
		//지면 정보
		const FVector Surface = OutHit.ImpactPoint;
		const FVector ImpactNormal = OutHit.Normal;
		
		//충격이 일정 값 이상이면 반사
		const float Dot = FVector::DotProduct(-Velocity, ImpactNormal);
		if (FMath::Abs(Dot) >= 1500)
		{
			//지면의 탄성 계수
			constexpr float Elasticity = 0.8f;

			//정해진 탄성만큼 튕겨나는 속도 연산
			Velocity += (2 * Dot * ImpactNormal) * Elasticity;
		}
		else
		{
			//지면에 딱 닿도록 위치 조정
			SetActorLocation(Surface + ImpactNormal * 80);

			//지면 수평 이동 속도 재연산
			AdjustVelocityToHorizon();
		}
	}
	else
	{
		//지면 방향으로 중력 가속
		Velocity += GravityDir * GravityForce;
	}
}

void ATurtleItem::UpdateLocation()
{
	//현재 속도와 델타 타임을 곱해 다음 위치 연산
	const FVector NextLocation = GetActorLocation() + Velocity * GetWorld()->GetDeltaSeconds();

	//선형 보간
	const FVector Interpolation = FMath::Lerp(GetActorLocation(), NextLocation, 0.9);

	//다음 위치로 액터 이동
	SetActorLocation(Interpolation);
}

void ATurtleItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurtleItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

