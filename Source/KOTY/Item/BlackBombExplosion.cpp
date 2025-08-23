// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackBombExplosion.h"
#include "KotyItemHitComponent.h"
#include "KotyMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"

class UKotyMovementComponent;

ABlackBombExplosion::ABlackBombExplosion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//적중을 감지하는 콜리전 컴포넌트를 루트로 설정
	HitComp = CreateDefaultSubobject<USphereComponent>(TEXT("HitComp"));
	HitComp->SetCollisionProfileName(FName("OverlapAll"), true);
	HitComp->SetSphereRadius(80);
	SetRootComponent(HitComp);

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//메시 컴포넌트의 메시와 머터리얼 설정
	const ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	ConstructorHelpers::FObjectFinder<UMaterial> TempMaterial(
		TEXT("/Game/Item/BlackBomb/MT_Explosion.MT_Explosion"));
	if (TempMesh.Succeeded() && TempMaterial.Succeeded())
	{
		MeshComp->SetStaticMesh(TempMesh.Object);
	 	MeshComp->SetMaterial(0, TempMaterial.Object);
	}

	//타임라인 컴포넌트
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));

	//커브 플로트 로드
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Temp(
		TEXT("/Game/Item/BlackBomb/ExplosionAlphaCurve.ExplosionAlphaCurve"));
	if (Temp.Succeeded())
	{
		CurveFloat = Temp.Object;
	}
}

void ABlackBombExplosion::BeginPlay()
{
	Super::BeginPlay();

	//머터리얼 인스턴스
	MaterialInst = MeshComp->CreateDynamicMaterialInstance(0);

	//타임라인
	FOnTimelineFloat Callback;
	Callback.BindUFunction(this, FName("UpdateExplosionAlpha"));
	TimelineComp->AddInterpFloat(CurveFloat, Callback);
	TimelineComp->SetTimelineLength(1/ExplosionDuration);
	TimelineComp->SetLooping(false);
	TimelineComp->PlayFromStart();

	//타이머
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABlackBombExplosion::EndExplosion, ExplosionDuration, false);
}

void ABlackBombExplosion::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//충돌 상대가 아이템 충돌체였다
	if (const UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//오너를 대상으로 아이템 효과 적용
		UE_LOG(LogTemp, Log, TEXT("Apply Item Effect to OtherKart!"));

		//델리게이트 전달
		FItemEffect ItemEffectDelegate;
		ItemEffectDelegate.BindDynamic(this, &AKotyItemBase::ApplyItemEffect);
		OtherHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
	}
}

void ABlackBombExplosion::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Display, TEXT("BlackBombExplosion Applyed to %s!"), *TargetActor->GetName());
}

void ABlackBombExplosion::UpdateExplosionAlpha(float Value)
{
	//머터리얼의 투명도와 메시 컴포넌트의 스케일을 동시에 조정
	MaterialInst->SetScalarParameterValue(FName("Opacity"), Value * 0.85);
	SetActorScale3D(FVector::Zero() + Value * 14);
}

void ABlackBombExplosion::EndExplosion()
{
	//폭발이 종료되었으므로 파괴
	this->Destroy();
}