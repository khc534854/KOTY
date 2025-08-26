// Fill out your copyright notice in the Description page of Project Settings.


#include "RedTurtleItem.h"
#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Item/Component/KotyMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class UKotyItemHitComponent;

ARedTurtleItem::ARedTurtleItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/RedTurtle/Model/SM_RedTurtle.SM_RedTurtle"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/RedTurtle/Model/MT_RedTurtle.MT_RedTurtle"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//사용 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Swoosh.SW_Swoosh"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//이동 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_RedTurtle.SW_RedTurtle"));
		Finder.Succeeded())
	{
		MovingSound = Finder.Object;
	}

	//명중 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Destroyed.SW_Destroyed"));
		Finder.Succeeded())
	{
		DestroySound = Finder.Object;
	}

	//오디오 컴포넌트 초기화
	AudioComp->SetSound(MovingSound);
	AudioComp->SetAttenuationSettings(SoundAttenuation);
}

void ARedTurtleItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//충돌 상대가 다른 아이템이었다
	if (AKotyItemBase* OtherItem = Cast<AKotyItemBase>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Item Hit with OtherItem!"));
		this->Destroy();
	}

	//충돌 상대가 아이템 충돌체였다
	if (const UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//오너를 대상으로 아이템 효과 적용
		UE_LOG(LogTemp, Log, TEXT("Apply Item Effect to OtherKart!"));

		//델리게이트 전달
		FItemEffect ItemEffectDelegate;
		ItemEffectDelegate.BindDynamic(this, &AKotyItemBase::ApplyItemEffect);
		OtherHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);

		//파괴
		this->Destroy();
	}
}

void ARedTurtleItem::ApplyItemEffect(AActor* OtherActor)
{
	UE_LOG(LogTemp, Display, TEXT("RedTurtle Applyed to %s!"), *OtherActor->GetName());
}

void ARedTurtleItem::OnSimulateBegin()
{
	Super::OnSimulateBegin();

	//스플라인 컴포넌트 중 랜덤으로 하나
	TArray<AActor*> GroundTrackingSpline;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GroundTrackingSpline"), GroundTrackingSpline);
	if (GroundTrackingSpline.IsEmpty() == false)
		SplineComp = GroundTrackingSpline[FMath::RandRange(0, GroundTrackingSpline.Num() - 1)]->GetComponentByClass<USplineComponent>();

	//충돌체를 가진 액터 중 랜덤으로 하나
	TArray<AActor*> HasHitComps;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("HasHitComp"), HasHitComps);
	if (HasHitComps.IsEmpty() == false)
		TargetActor = HasHitComps[FMath::RandRange(0, HasHitComps.Num() - 1)];

	//델리게이트 등록
	MoveComp->OnSimulateBeginEventDispatcher.AddUFunction(this, FName("OnSimulateBegin"));

	//회전 방향 결정
	RotationDir = FMath::RandRange(0, 1);

	//오디오 재생
	AudioComp->Play();
}

void ARedTurtleItem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

#pragma region 메시 회전

	//메시 컴포넌트의 머리가 중력의 반대 방향을 향하도록 업데이트
	MeshComp->AddWorldRotation(FQuat::FindBetweenVectors(MeshComp->GetUpVector(), -MoveComp->GetGravityDir()) * DeltaTime);

	//메시 컴포넌트가 머리 방향을 축으로 회전하도록 업데이트
	const FVector Dir = RotationDir ? MeshComp->GetRightVector() : MeshComp->GetRightVector() * -1;
	const FVector SlerpDir = FVector::SlerpVectorToDirection(MeshComp->GetForwardVector(), Dir, 0.1);
	const FQuat RotationQuat = FQuat::FindBetweenVectors(MeshComp->GetForwardVector(), SlerpDir);
	MeshComp->AddWorldRotation(RotationQuat * DeltaTime);

#pragma endregion

#pragma region 스플라인 및 타깃 추적
	
	//타깃 액터 유효
	if (TargetActor)
	{
		//타깃 액터와의 거리
		const FVector Diff = TargetActor->GetActorLocation() - GetActorLocation();
		if (const float Length = Diff.Length(); Length < DetectLength)
		{
			//타깃 액터를 향해 회전
			MoveComp->SLerpVelocity(Diff.GetSafeNormal());
			return;
		}
	}

	//트래킹 모드 확인
	if (TrackingMode == ETrackingMode::Inactive)
		return;

	//스플라인 확인
	if (SplineComp == nullptr)
		return;

	//거리에 따른 속도 보간
	const FVector FaceLocation = GetActorLocation() + MoveComp->Velocity.GetSafeNormal();
	const FVector ClosestLocation = SplineComp->FindLocationClosestToWorldLocation(FaceLocation, ESplineCoordinateSpace::World);
	const FVector Diff = ClosestLocation - GetActorLocation();
	if (const float Length = Diff.Length(); Length < 300)
	{
		//거리가 충분히 가까우므로 스플라인의 방향을 따라 속도 보간
		const FVector ClosestDirection = SplineComp->FindDirectionClosestToWorldLocation(FaceLocation, ESplineCoordinateSpace::World);
		MoveComp->SLerpVelocity(ClosestDirection);
	}
	else
	{
		//거리가 아직 있으므로 스플라인을 향해 속도 보간
		MoveComp->SLerpVelocity(Diff.GetSafeNormal());
	}

#pragma endregion 
	
}