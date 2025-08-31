// Fill out your copyright notice in the Description page of Project Settings.

#include "SquidItem.h"

#include "Engine/PostProcessVolume.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "UObject/ConstructorHelpers.h"

ASquidItem::ASquidItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	const auto SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);
	
	//메시 컴포넌트 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName(FName("NoCollision"), false);
	MeshComp->SetupAttachment(GetRootComponent());

	//타임라인 컴포넌트 부착
	TimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComp"));

	//스태틱 메시 로드
	if (const ConstructorHelpers::FObjectFinder<UStaticMesh> Finder(TEXT("/Game/Item/Squid/Model/SM_Squid.SM_Squid")); Finder.Succeeded())
	{
		MeshComp->SetStaticMesh(Finder.Object);
	}

	//머터리얼 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterial> Finder(TEXT("/Game/Item/Squid/Model/MT_Squid.MT_Squid")); Finder.Succeeded())
	{
		MeshComp->SetMaterial(0, Finder.Object);
	}
	
	//커브 플로트 로드
	if (const ConstructorHelpers::FObjectFinder<UCurveFloat> Finder(TEXT("/Game/Item/Squid/C_SplatterAlpha.C_SplatterAlpha"));
		Finder.Succeeded())
	{
		CurveFloat = Finder.Object;
	}

	//머터리얼 패러미터 콜렉션 로드
	if (const ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> Finder(TEXT("/Game/Item/Squid/MPC_SquidSplatter.MPC_SquidSplatter"));
	Finder.Succeeded())
	{
		MaterialParameterCollection = Finder.Object;
	}

	//적용 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_SquidSplatter.SW_SquidSplatter"));
		Finder.Succeeded())
	{
		UseSound = Finder.Object;
	}
}

void ASquidItem::BeginPlay()
{
	Super::BeginPlay();
	
	CollectionInstance = GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection);
}

void ASquidItem::ApplyItemEffect(AActor* TargetActor)
{
	Super::ApplyItemEffect(TargetActor);

	UE_LOG(LogTemp, Warning, TEXT("Squid Item Used To %s"), *TargetActor->GetName());
	
	//먹물로 화면을 가리는 포스트 프로세스
	CollectionInstance->SetScalarParameterValue(FName("SplatterAlpha"), 1);
	
	//플레이어의 경우
	if (TargetActor == Cast<AActor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		FOnTimelineFloat Callback;
		Callback.BindUFunction(this, FName("UpdateSplatter"));
		TimelineComp->AddInterpFloat(CurveFloat, Callback);
		TimelineComp->SetPlayRate(1.0/SplatterDuration);
		TimelineComp->SetLooping(false);
		TimelineComp->PlayFromStart();
	}

	//비가시
	MeshComp->SetVisibility(false);

	//파괴
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		this->Destroy();
	}, SplatterDuration, false);
}

void ASquidItem::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnUseItem(HoldComp);

	//모든 카트 검색
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("HasHitComp"), OutActors);
	for (const auto Actor : OutActors)
	{
		//자신 이외의 모든 카트에 대해
		if (Actor != ItemOwningActor)
		{

		}

		if (const auto OtherHitComp = Actor->GetComponentByClass<UKotyItemHitComponent>())
		{
			//아이템 효과 적용 요청
			RequestApplyItemEffectToOtherHitComp(OtherHitComp);
		}
	}
}

void ASquidItem::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	Super::OnLoseItem(HoldComp);

	//파괴
	this->Destroy();
}

void ASquidItem::UpdateSplatter(float Value)
{
	//먹물로 화면을 가리는 포스트 프로세스
	CollectionInstance->SetScalarParameterValue(FName("SplatterAlpha"), Value);
}
