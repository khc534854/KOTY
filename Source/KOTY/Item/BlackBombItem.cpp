// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackBombItem.h"

#include "BlackBombExplosion.h"
#include "KotyItemHitComponent.h"
#include "KotyMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"

ABlackBombItem::ABlackBombItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//타임라인 컴포넌트
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/BlackBomb/Model/SM_BlackBomb.SM_BlackBomb"));
		Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/BlackBomb/Model/MT_BlackBomb.MT_BlackBomb"));
		Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);		
	}

	//사용 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_Swoosh.SW_Swoosh"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}

	//폭탄 경고 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_BombWarning.SW_BombWarning"));
		Finder.Succeeded())
	{
		BombWarningSound = Finder.Object;
	}

	//커브 플로트 로드
	if (const ConstructorHelpers::FObjectFinder<UCurveFloat> Finder(TEXT("/Game/Item/BlackBomb/WarningEmissionCurve.WarningEmissionCurve"));
		Finder.Succeeded())
	{
		CurveFloat = Finder.Object;
	}

	//오디오 컴포넌트 초기화
	AudioComp->SetSound(BombWarningSound);
	AudioComp->SetAttenuationSettings(SoundAttenuation);
}

void ABlackBombItem::BeginPlay()
{
	Super::BeginPlay();

	//머터리얼 인스턴스
	MaterialInst = MeshComp->CreateDynamicMaterialInstance(0);
	MaterialInst->SetScalarParameterValue(FName("WarningDelay"), WarningDelay);

	//타임라인
	FOnTimelineFloat Callback;
	Callback.BindUFunction(this, FName("SetElapsedTime"));
	TimelineComp->AddInterpFloat(CurveFloat, Callback);
	TimelineComp->SetPlayRate(1/ExplosionDelay);
	TimelineComp->SetLooping(false);
	TimelineComp->PlayFromStart();

	//폭발 타이머 설정
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ABlackBombItem::Explode, ExplosionDelay, false);
	
	//폭탄 경고 사운드 재생
	AudioComp->Play();
}

void ABlackBombItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//이미 폭발한 상태로 유효하지 않음
	if (ExplosionTimerHandle.IsValid() == false)
	{
		return;
	}
	
	//충돌 상대가 다른 아이템이었다
	if (AKotyItemBase* OtherItem = Cast<AKotyItemBase>(OtherActor))
	{
		//폭탄병 아이템은 폭발 액터를 소환
		UE_LOG(LogTemp, Log, TEXT("Item Hit with OtherItem!"));
		Explode();
	}

	//충돌 상대가 아이템 충돌체였다
	if (const UKotyItemHitComponent* OtherHitComp = OtherActor->GetComponentByClass<UKotyItemHitComponent>())
	{
		//폭탄병 아이템은 폭발 액터를 소환
		UE_LOG(LogTemp, Log, TEXT("Item Hit with OtherItem!"));
		Explode();
	}
}

void ABlackBombItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);
}

void ABlackBombItem::Explode()
{
	if (ExplosionTimerHandle.IsValid())
	{
		//폭발 액터를 소환하고 자기 자신은 파괴
		GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);
		GetWorld()->SpawnActor<AActor>(ExplosionActor, GetActorTransform());
		this->Destroy();	
	}
}

void ABlackBombItem::SetElapsedTime(const float TwistedTime) const
{
	//업데이트
	MaterialInst->SetScalarParameterValue("ElapsedTime", TwistedTime);
}

void ABlackBombItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MeshComp->AddRelativeRotation(FQuat::FindBetweenVectors(MeshComp->GetUpVector(), -MoveComp->GetGravityDir()) * DeltaTime);
	
}

