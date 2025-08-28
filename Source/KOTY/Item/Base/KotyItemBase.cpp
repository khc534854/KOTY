// Fill out your copyright notice in the Description page of Project Settings.

#include "KotyItemBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AKotyItemBase::AKotyItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	//아이템 사운드 감쇠 로드
	if (const ConstructorHelpers::FObjectFinder<USoundAttenuation> Finder(TEXT("/Game/Item/Sound/SA_Item.SA_Item"));
		Finder.Succeeded())
	{
		SoundAttenuation = Finder.Object;
	}
	
	//오디오 컴포넌트 부착
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetAutoActivate(false);
	AudioComp->SetAttenuationSettings(SoundAttenuation);
	AudioComp->SetupAttachment(RootComponent);
}

void AKotyItemBase::ApplyItemEffect(AActor* TargetActor)
{
	//하위 아이템에서 오버라이드하여 구체적인 기능 구현
	UE_LOG(LogTemp, Warning, TEXT("You are Trying to Call ItemBase Effect! There is No Effect on ItemBase!"));
}

void AKotyItemBase::RequestApplyItemEffectToOtherHitComp(const UKotyItemHitComponent* TargetHitComp)
{
	//델리게이트 전달
	FItemEffect ItemEffectDelegate;
	ItemEffectDelegate.BindDynamic(this, &AKotyItemBase::ApplyItemEffect);
	TargetHitComp->OnRequestApplyEffectFromItem(ItemEffectDelegate, this);
}

void AKotyItemBase::OnUseItem(UKotyItemHoldComponent* HoldComp)
{
	//일반 아이템의 경우 자신에게 바로 효과 적용
	//물리 아이템의 경우 전방 혹은 후방을 향해 사출
}

void AKotyItemBase::OnLoseItem(UKotyItemHoldComponent* HoldComp)
{
	//소유 액터가 없어진다
	ItemOwningActor = nullptr;
	
	//일반 아이템의 경우 바로 잃어버림
	//물리 아이템의 경우 랜덤 방향으로 사출
}

void AKotyItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKotyItemBase::Destroyed()
{
	Super::Destroyed();

	//파괴 사운드 재생
	if (DestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);	
	}
}