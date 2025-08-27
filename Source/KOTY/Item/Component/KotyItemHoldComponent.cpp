// Fill out your copyright notice in the Description page of Project Settings.

#include "KotyItemHoldComponent.h"

#include "Components/ArrowComponent.h"
#include "Item/Implement/BananaItem.h"
#include "Item/Implement/BigMushroomItem.h"
#include "Item/Implement/BlackBombItem.h"
#include "Item/Implement/CoinItem.h"
#include "Item/Implement/GreenTurtleItem.h"
#include "Item/Implement/RedTurtleItem.h"
#include "Item/Implement/SmallMushroomItem.h"
#include "Item/Implement/SquidItem.h"
#include "Item/Implement/StarItem.h"

UKotyItemHoldComponent::UKotyItemHoldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//부착
	ShootArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ShootArrowComp->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
}

void UKotyItemHoldComponent::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugPoint(GetWorld(), this->GetComponentLocation(), 50, FColor::Red, true);
	
	if (CodeOfHoldingItems[0] != EItemList::None)
	{
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(CodeOfHoldingItems[0]))
		{
			StartRotatingAround(SpawnItem);	
		}
	}
}

void UKotyItemHoldComponent::UpdateHoldingItemLocation(const float DeltaTime)
{
	//시간에 따른 각도
	CurrentHoldingItemDegree += RotateDegreeSpeed * DeltaTime;

	//라디안으로 변환
	const float RotateRad = FMath::DegreesToRadians(CurrentHoldingItemDegree);
	
	//상대 위치 연산
	const FVector NewLocation =
		RotateGap * FVector(FMath::Sin(RotateRad), FMath::Cos(RotateRad), 0);	

	//현재 홀드 중인 아이템이 있다면
	if (CurrentHoldingItem)
	{
		//원형의 상대 위치 설정
		CurrentHoldingItem->SetActorRelativeLocation(NewLocation);
	}
}

AKotyItemBase* UKotyItemHoldComponent::SpawnItemByCode(const EItemList ItemCode) const
{
	FTransform SpawnTransform = this->GetOwner()->GetActorTransform();
	
	AKotyItemBase* SpawnItem = nullptr;
	
	switch (ItemCode)
	{
	case EItemList::Start:
		{
			UE_LOG(LogTemp, Error, TEXT("ItemCode is Invalid!"));
			break;
		}
	case EItemList::Banana:
		{
			SpawnItem = GetWorld()->SpawnActor<ABananaItem>(BananaItemClass, SpawnTransform);
			break;
		}
	case EItemList::BigMushroom:
		{
			SpawnItem = GetWorld()->SpawnActor<ABigMushroomItem>(BigMushroomClass, SpawnTransform);
			break;
		}
	case EItemList::BlackBomb:
		{
			SpawnItem = GetWorld()->SpawnActor<ABlackBombItem>(BlackBombClass, SpawnTransform);
			break;
		}
	case EItemList::Coin:
		{
			SpawnItem = GetWorld()->SpawnActor<ACoinItem>(CoinItemClass, SpawnTransform);
			break;
		}
	case EItemList::GreenTurtle:
		{
			SpawnItem = GetWorld()->SpawnActor<AGreenTurtleItem>(GreenTurtleItemClass, SpawnTransform);
			break;
		}
	case EItemList::RedTurtle:
		{
			SpawnItem = GetWorld()->SpawnActor<ARedTurtleItem>(RedTurtleItemClass, SpawnTransform);
			break;
		}
	case EItemList::SmallMushroom:
		{
			SpawnItem = GetWorld()->SpawnActor<ASmallMushroomItem>(SmallMushroomItemClass, SpawnTransform);
			break;
		}
	case EItemList::Squid:
		{
			SpawnItem = GetWorld()->SpawnActor<ASquidItem>(SquidItemClass, SpawnTransform);
			break;
		}
	case EItemList::Star:
		{
			SpawnItem = GetWorld()->SpawnActor<AStarItem>(StarItemClass, SpawnTransform);
			break;
		}
	case EItemList::End:
		{
			UE_LOG(LogTemp, Error, TEXT("ItemCode is Invalid!"));
			break;
		}
	case EItemList::None:
		{
			UE_LOG(LogTemp, Error, TEXT("ItemCode is Invalid!"));
			break;
		}
	default: UE_LOG(LogTemp, Error, TEXT("ItemCode is Invalid!"));
	}
	return SpawnItem;
}

void UKotyItemHoldComponent::StartRotatingAround(AKotyItemBase* SpawnItem)
{
	//정보 업데이트
	CurrentHoldingItem = SpawnItem;

	//아이템의 부모를 컴포넌트로 설정
	SpawnItem->AttachToActor(this->GetOwner(), FAttachmentTransformRules::KeepWorldTransform);

	//각도 초기화
	CurrentHoldingItemDegree = 180;
}

void UKotyItemHoldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//상대 위치 업데이트
	UpdateHoldingItemLocation(DeltaTime);
}

FVector UKotyItemHoldComponent::GetShootLocation() const
{
	return ShootArrowComp->GetComponentLocation();
}

void UKotyItemHoldComponent::GetRandomItem()
{
	if (CodeOfHoldingItems[0] == EItemList::None)
	{
		//랜덤으로 뽑은 하나의 아이템
		int Dice = FMath::RandRange(
			static_cast<int32>(EItemList::End) + 1,
			static_cast<int32>(EItemList::End) - 1);
		const EItemList ItemCode = static_cast<EItemList>(Dice);

		//아이템 생성 성공
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(ItemCode))
		{
			StartRotatingAround(SpawnItem);
			CodeOfHoldingItems[0] = ItemCode;
		}	
	}
	else if (CodeOfHoldingItems[1] == EItemList::None)
	{
		//랜덤으로 뽑은 하나의 아이템
		int Dice = FMath::RandRange(
			static_cast<int32>(EItemList::End) + 1,
			static_cast<int32>(EItemList::End) - 1);
		const EItemList ItemCode = static_cast<EItemList>(Dice);

		//아이템 코드만 저장
		CodeOfHoldingItems[1] = ItemCode;
	}
}

void UKotyItemHoldComponent::UseCurrentItem()
{
	//홀드 중인 아이템이 있다면
	if (CurrentHoldingItem)
	{
		//임시 저장
		const auto Temp = CurrentHoldingItem;

		//미리 지운다
		CurrentHoldingItem = nullptr;
		
		//아이템 사용
		Temp->OnUseItem(this);
		
		//정보 업데이트
		CodeOfHoldingItems[0] = CodeOfHoldingItems[1];
		CodeOfHoldingItems[1] = EItemList::None;
	}

	//유효한 코드를 가졌다면
	if (CodeOfHoldingItems[0] != EItemList::None)
	{
		//아이템 생성 성공
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(CodeOfHoldingItems[0]))
		{
			StartRotatingAround(SpawnItem);
		}
	}
}

void UKotyItemHoldComponent::LoseHoldingItems()
{
	if (CurrentHoldingItem)
	{
		//홀딩 중인 아이템 소실
		CurrentHoldingItem->OnLoseItem(this);
	}

	if (CodeOfHoldingItems[1] != EItemList::None)
	{
		//아이템 스폰 후 소실
		SpawnItemByCode(CodeOfHoldingItems[1])->OnLoseItem(this);
	}

	//정보 업데이트
	CurrentHoldingItem = nullptr;
	CodeOfHoldingItems = { EItemList::None, EItemList::None };
}