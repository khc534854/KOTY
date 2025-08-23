// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyItemHitComponent.h"

#include "KotyItemBase.h"
#include "Components/BoxComponent.h"

UKotyItemHitComponent::UKotyItemHitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//오너의 루트 컴포넌트에 부착
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void UKotyItemHitComponent::BeginPlay()
{
	Super::BeginPlay();

	//액터에 태그를 추가
	GetOwner()->Tags.Add(FName("HasHitComp"));
}

void UKotyItemHitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UKotyItemHitComponent::OnRequestApplyEffectFromItem(const FItemEffect ItemEffectDelegate, AActor* OtherItem) const
{
	//조건을 만족하는 경우 전달받은 아이템 효과 델리게이트 실행
	ItemEffectDelegate.Execute(GetOwner());
}

