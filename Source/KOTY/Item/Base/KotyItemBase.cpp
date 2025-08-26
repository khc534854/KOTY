// Fill out your copyright notice in the Description page of Project Settings.


#include "KotyItemBase.h"
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
}

void AKotyItemBase::ApplyItemEffect(AActor* TargetActor)
{
	//하위 아이템에서 오버라이드하여 구체적인 기능 구현
	UE_LOG(LogTemp, Warning, TEXT("You are Trying to Call ItemBase Effect! There is No Effect on ItemBase!"));
}

void AKotyItemBase::BeginPlay()
{
	Super::BeginPlay();

	//사용 사운드 재생
	if (UseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), UseSound, GetActorLocation(), GetActorRotation(), 1, 1, 0, SoundAttenuation);
	}
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