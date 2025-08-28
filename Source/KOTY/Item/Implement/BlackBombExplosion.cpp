// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackBombExplosion.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "Item/Component/KotyItemHitComponent.h"
#include "Kismet/GameplayStatics.h"

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

	//타임라인 컴포넌트
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));
	
	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/BlackBomb/MT_Explosion.MT_Explosion"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}

	//커브 플로트 로드
	if (const ConstructorHelpers::FObjectFinder<UCurveFloat> Finder(TEXT("/Game/Item/BlackBomb/ExplosionAlphaCurve.ExplosionAlphaCurve"));
		Finder.Succeeded())
	{
		CurveFloat = Finder.Object;
	}

	//폭발 사운드 베이스 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Explosion.SW_Explosion"));
		Finder.Succeeded())
	{
		ExplosionSound = Finder.Object;
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
	TimelineComp->SetPlayRate(1/ExplosionDuration);
	TimelineComp->SetLooping(false);
	TimelineComp->PlayFromStart();

	//타이머
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABlackBombExplosion::EndExplosion, ExplosionDuration, false);

	//폭발 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
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
		RequestApplyItemEffectToOtherHitComp(OtherHitComp);
	}
}

void ABlackBombExplosion::ApplyItemEffect(AActor* TargetActor)
{
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