// Fill out your copyright notice in the Description page of Project Settings.


#include "AiKartController.h"
#include "Algorithm.h"
#include "AlgorithmBPLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"

#include "C_KartBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetNodes/KismetNodeInfoContext.h"


// Sets default values
AAiKartController::AAiKartController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// UWorld* World = GetWorld();
	// if (!World) return;
	//
	// UClass* BP_AiTestKart = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/AI_Player/Kart/Vehicles/BP_AiTestKart1.BP_AiTestKart1_C"));
	//
	// if (BP_AiTestKart)
	// {
	// 	TArray<AActor*> FoundActors;
	// 	UGameplayStatics::GetAllActorsOfClass(World, BP_AiTestKart, FoundActors);
	//
	// 	for (AActor* Actor : FoundActors)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Found specific BP_KartPawn: %s"), *Actor->GetName());
	// 	}
	// 	
	// }
	
}

// Called when the game starts or when spawned
void AAiKartController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
	


	//FoundActors중에서 USplineComponent를 가지고 있는 엑터를 SplineRoads에 담음
	for (auto FoundActor : FoundActors)
	{
		if (FoundActor->GetComponentByClass<USplineComponent>())
		{
			SplineRoads.Add(FoundActor);
			// UE_LOG(LogTemp, Warning, TEXT("SplineRoads Add: %s"), *FoundActor->GetName());

		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("SplineRoads Count: %d"), SplineRoads.Num());

	DefaultSpline = FindNearestSpline();
	CurrentNearestSpline = DefaultSpline;

	//60
	DefaultMaxSpeed = MaxSpeed;

	KartMinSpeed = 5.0;
	KartMaxSpeed = 3000.0;
	
}

// Called every frame
void AAiKartController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DelayTime1Sec = DelayTime1Sec + DeltaTime;
	UE_LOG(LogTemp, Warning, TEXT("DelayTime1Sec: %f"), DelayTime1Sec);
	if (IsValid(CurrentNearestSpline))
	{
		
		Cast<AC_KartBase>(GetPawn())->HandlingDir = CalculateSteeringInput(CurrentNearestSpline);

		TargetSpeed = UKismetMathLibrary::MapRangeClamped(FMath::Abs(YawDelta), 30.f, 5.f, KartMinSpeed, KartMaxSpeed);
		float CurSpeed = Cast<AC_KartBase>(GetPawn())->CurSpeed;
		UE_LOG(LogTemp, Warning, TEXT("TargetSpeed: %f"), TargetSpeed);

		if (CanBrake == true)
		{
			BrakeInput = CalculateBrakeInput(CurrentNearestSpline, OffsetTangentLocation);
		}
		
		if (BrakeInput > 0.9f && CanBrake == true)
		{
			Cast<AC_KartBase>(GetPawn())->AccelerationDir = 0.1f;

		} else
		{
			if ( CurSpeed < TargetSpeed )
			{
				Cast<AC_KartBase>(GetPawn())->AccelerationDir = 1.0f;
			}
			else
			{
				Cast<AC_KartBase>(GetPawn())->AccelerationDir = -1.0f;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("CanBrake: %s"), CanBrake ? TEXT("true") : TEXT("false"));

		if (DelayTime1Sec > 1.f)
		{
			CanBrake = true;

			// Cast<AC_KartBase>(GetPawn())->AccelerationDir = -CalculateBrakeInput(CurrentNearestSpline, OffsetTangentLocation);
		}

	}
}

AActor* AAiKartController::FindNearestSpline()
{
	float Distance = 0.0f;
	float NearestDistance = 0.0f;
	AActor* NearestSpline = nullptr;;
	
	for (int i = 0; i < SplineRoads.Num(); i++)
	{
		// auto Temp = SplineRoads[i]->GetComponentByClass<USplineComponent>();

		// auto Temp = Cast<USplineComponent>(SplineRoads[i]->GetComponentByClass(USplineComponent::StaticClass()));
		// USplineComponent* Temp = Cast<USplineComponent>(SplineRoads[i]->GetComponentByClass(USplineComponent::StaticClass()));
		USplineComponent* Temp = SplineRoads[i]->GetComponentByClass<USplineComponent>();
		
		if (IsValid(Temp))
		{
			FVector PointPos = Temp->FindLocationClosestToWorldLocation(GetPawn()->GetActorLocation(), ESplineCoordinateSpace::Type::World);
			FVector KartPos = GetPawn()->GetActorLocation();
			Distance = FVector::Distance(PointPos, KartPos);
			if (i == 0)
			{
				//스플라인의 거리가 가깝든 멀든 일단 첫번째 스플라인을 NearestSpline에 저장한다.
				//그 후 for문을 돌며 Distance를 비교해 더 짧은 Spline이 있으면 NearestSpline에 넣는다.
				if (Temp)
				{
					NearestSpline = SplineRoads[i];
					NearestDistance = Distance;	
				}
			}
			else
			{
				if (Distance < NearestDistance)
				{
					NearestDistance = Distance;
					NearestSpline = SplineRoads[i];
				}
			
			}
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("Spline Roads not found"));
		}
		
	}

	return NearestSpline;
}

float AAiKartController::CalculateSteeringInput(AActor* Spline)
{
	AActor* NearestSpline = Spline;
	FVector RoadDir;
	
	//차량에서 가장 가까운 스플라인 포인트의 접선벡터를 구한다. (포인트가 향하는 방향, 차도 방향)
	RoadDir = NearestSpline->GetComponentByClass<USplineComponent>()->FindTangentClosestToWorldLocation(GetPawn()->GetActorLocation(), ESplineCoordinateSpace::Type::World);
	RoadDir.Normalize(0.0001);

	float CurSpeed = Cast<AC_KartBase>(GetPawn())->CurSpeed;

	//카트의 한틱 후 미래 위치
	FVector FuturePos = GetPawn()->GetActorLocation() + RoadDir * CurSpeed;

	FVector KartPos = GetPawn()->GetActorLocation();
	OffsetTangentLocation = NearestSpline->GetComponentByClass<USplineComponent>()->FindLocationClosestToWorldLocation(FuturePos, ESplineCoordinateSpace::Type::World);

	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(KartPos, OffsetTangentLocation);

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRot, GetPawn()->GetActorRotation());
	UE_LOG(LogTemp, Warning, TEXT("DeltaRot: %s"), *DeltaRot.ToString());
	
	if (DebugSteering)
	{
		DrawDebugSphere(GetWorld(),OffsetTangentLocation, 25, 12, FColor::Red , false, 0, 0, 1);
		DrawDebugDirectionalArrow(GetWorld(),KartPos, OffsetTangentLocation, 1, FColor::Red , false, 0, 0, 10);
	}
	YawDelta = DeltaRot.Yaw;

	float ClampedSteeringInput = UKismetMathLibrary::MapRangeClamped(YawDelta, -15.0, 15.0, -1.0, 1.0);
	
	
	return ClampedSteeringInput;
}

float AAiKartController::CalculateBrakeInput(AActor* Spline, FVector OriginLocation)
{
	USplineComponent* Temp = Spline->GetComponentByClass<USplineComponent>();
	FVector DirToPoint = Temp->FindTangentClosestToWorldLocation(OriginLocation, ESplineCoordinateSpace::Type::World);
	DirToPoint.Normalize(0.0001);
	FVector KartFuturePos = OriginLocation + DirToPoint * 50 * FMath::Clamp(Cast<AC_KartBase>(GetPawn())->CurSpeed, 10, Cast<AC_KartBase>(GetPawn())->CurSpeed);

	FVector TargetPointPos = Temp->FindLocationClosestToWorldLocation(KartFuturePos, ESplineCoordinateSpace::Type::World);
	FRotator  LookAtRot = UKismetMathLibrary::FindLookAtRotation(Cast<AC_KartBase>(GetPawn())->GetActorLocation(), TargetPointPos);
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRot, Cast<AC_KartBase>(GetPawn())->GetActorRotation());
	
	float TempValue = FMath::Abs(DeltaRot.Yaw) + (FMath::Abs(YawDelta) / 8.f);
	UE_LOG(LogTemp, Warning, TEXT("TempValue %f"), TempValue);

	BrakeInput = UKismetMathLibrary::MapRangeClamped(TempValue, 15, 80 , 0, 1);
	if (Cast<AC_KartBase>(GetPawn())->CurSpeed <= KartMinSpeed)
	{
		BrakeInput = 0;
	}

	if (DebugBrake)
	{
		DrawDebugSphere(GetWorld(),TargetPointPos, 25, 12, FColor::Green , false, 0, 0, 1);
		DrawDebugDirectionalArrow( GetWorld(),Cast<AC_KartBase>(GetPawn())->GetActorLocation(),TargetPointPos, 1, FColor::Green, false, 0, 0, 10);
	}
	
	return BrakeInput;
}

// FLineTraceFrontObstacleResult AAiKartController::LineTraceFrontObstacle(ESide InKartSide)
// {
// 	FLineTraceFrontObstacleResult Result;
// 	
// 	FVector KartRightVector = GetPawn()->GetActorRightVector();
// 	FVector KartLeftVector = -1 * KartRightVector;
// 	FVector CurrentKartSideVector;
// 	
// 	switch (InKartSide)
// 	{
// 		case ESide::Left:
// 			CurrentKartSideVector = KartLeftVector;
// 		case ESide::Right:
// 			CurrentKartSideVector = KartRightVector;
// 	}
//
// 	
// 	FVector TempVector = GetPawn()->GetActorLocation() + CurrentKartSideVector;
// 	float VectorX = TempVector.X;
// 	float VectorY = TempVector.Y;
// 	float VectorZ = TempVector.Z + 50.f;
// 	FVector AdjustedVector = FVector(VectorX, VectorY, VectorZ);
//
// 	FRotator TempRotator = GetControlRotation();
// 	float Roll = TempRotator.Roll;
// 	float Pitch = TempRotator.Pitch;
// 	float Yaw = TempRotator.Yaw;
//
// 	FRotator Rot(0, Yaw, 0);
// 	FVector Dir = Rot.Vector();
//
// 	//수정이 필요할 수 있음
// 	float TempFloat = Cast<AC_KartBase>(GetPawn())->CurSpeed * 60.f + 800.f;
// 		
// 	FVector End = Dir * TempFloat + AdjustedVector;
// 	
// 	FHitResult OutHit;
// 	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, AdjustedVector,End, ECC_Visibility, FCollisionQueryParams::DefaultQueryParam, FCollisionResponseParams::DefaultResponseParam);
// 	FColor LineColor = bHit ? FColor::Red : FColor::Green;  // 맞으면 빨강, 아니면 초록
// 	DrawDebugLine(
// 		GetWorld(),
// 		AdjustedVector,
// 		End,
// 		LineColor,
// 		false,   // bPersistentLines (true면 계속 유지됨, false면 Duration 동안만 표시)
// 		0.f,    // Duration (초 단위)
// 		0,       // DepthPriority
// 		2.0f     // Thickness
// 	);
//
// 	Result.HitLocation = OutHit.Location;
// 	if (Obstacles.Find(OutHit.GetActor()) == -1)
// 	{
// 		Result.NewObstacle = true;
// 	}else
// 	{
// 		Result.NewObstacle = false;
// 	}
// 	Result.bHit = bHit;
//
// 	return Result;
// }

// void AAiKartController::UpdateObstacleDetection()
// {
// 	// FLineTraceFrontObstacleResult LResult = LineTraceFrontObstacle(ESide::Left);
// 	// FLineTraceFrontObstacleResult RResult = LineTraceFrontObstacle(ESide::Right);
// 	// ObstacleHitLocationL = LResult.HitLocation;
// 	// ObstacleHitLocationR = RResult.HitLocation;
// 	//
// 	// FAlgorithmModule Algorithm;
// 	// if (LResult.bHit || RResult.bHit)
// 	// {
// 	// 	UAlgorithmBPLibrary::
// 	// }
// 	// else
// 	// {
// 	// 	
// 	// }
//
// 	
// }



