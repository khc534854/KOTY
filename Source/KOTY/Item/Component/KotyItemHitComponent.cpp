// Fill out your copyright notice in the Description page of Project Settings.

#include "KotyItemHitComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

UKotyItemHitComponent::UKotyItemHitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//오너의 루트 컴포넌트에 부착
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	//BoxComp->SetCollisionProfileName(TEXT("ItemHit"));
	BoxComp->SetupAttachment(this);

	//방어 사운드 로드
	if (const ConstructorHelpers::FObjectFinder<USoundBase> Finder(TEXT("/Game/Item/Sound/SW_StarBarrier.SW_StarBarrier"));
		Finder.Succeeded())
	{
		BarrierSound = Finder.Object;
	}

	//아이템 사운드 감쇠 로드
	if (const ConstructorHelpers::FObjectFinder<USoundAttenuation> Finder(TEXT("/Game/Item/Sound/SA_Item.SA_Item"));
		Finder.Succeeded())
	{
		SoundAttenuation = Finder.Object;
	}
}

void UKotyItemHitComponent::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->SetupAttachment(this);

	//액터에 태그를 추가
	GetOwner()->Tags.Add(FName("HasHitComp"));
}

void UKotyItemHitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UKotyItemHitComponent::OnRequestApplyEffectFromItem(const FItemEffect ItemEffectDelegate, AActor* OtherItem)
{
	//데미지를 받지 않는 시간
	if (NoDamageTimerHandle.IsValid())
	{
		//방어 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BarrierSound, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), 1, 1, 0, SoundAttenuation);
		
		//해당 아이템 파괴
		OtherItem->Destroy();
		
		return;
	}
	
	//조건을 만족하는 경우 전달받은 아이템 효과 델리게이트 실행
	ItemEffectDelegate.Execute(GetOwner());
}

void UKotyItemHitComponent::StartInvincibleTime(float InvincibleTime)
{
	//타이머를 통해 무적 시간을 준다
	GetWorld()->GetTimerManager().SetTimer(NoDamageTimerHandle, [this]()
	{
		//타이머 클리어
		GetWorld()->GetTimerManager().ClearTimer(NoDamageTimerHandle);
	}, 5, false);
}
