// Fill out your copyright notice in the Description page of Project Settings.

#include "KotyItemHoldComponent.h"
#include "Components/ArrowComponent.h"
#include "Item/Base/KotyItemBase.h"

UKotyItemHoldComponent::UKotyItemHoldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//부착
	ShootArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ShootArrowComp->SetupAttachment(this);
	
	//아이템 열거형 획득
	if (const UEnum* ItemEnum = StaticEnum<EItemList>())
	{
		constexpr int32 Start = static_cast<int32>(EItemList::Start);
		constexpr int32 End = static_cast<int32>(EItemList::End);
		for (int32 i = Start + 1; i < End; i++)
		{
			//열거형 오브젝트에서 추출
			FString EnumString = ItemEnum->GetNameStringByIndex(i);

			//에셋 경로
			FString AssetPath = FString::Printf(TEXT("/Game/Item/%s/BP_%sItemFinal.BP_%sItemFinal_C"), *EnumString, *EnumString, *EnumString);

			//에셋 로드
			if (ConstructorHelpers::FClassFinder<AKotyItemBase> Finder(*AssetPath);
				Finder.Succeeded())
			{
				//배열에 추가
				ItemClassArray.Add(Finder.Class);
			}		
		}
	}
}

void UKotyItemHoldComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (CodeOfHoldingItems[0] != EItemList::None)
	{
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(CodeOfHoldingItems[0]))
		{
			StartRotatingAround(SpawnItem);	
		}
	}

	//액터에 태그를 추가
	GetOwner()->Tags.Add(FName("HasHoldComp"));
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
	//Fall-Through 스타일
	switch (ItemCode)
	{
	case EItemList::Start:
	case EItemList::End:
	case EItemList::None:
		{
			//예외 상황에 대한 처리
			UE_LOG(LogTemp, Error, TEXT("ItemCode is Invalid!"));
			return nullptr;
		}
	default:
		{
			//유효 상황에 대한 처리
			UE_LOG(LogTemp, Log, TEXT("ItemCode is Valid!"));
			TSubclassOf<AKotyItemBase> Target = ItemClassArray[static_cast<int32>(ItemCode) - 1];
			const FTransform SpawnTransform = this->GetOwner()->GetActorTransform();
			return GetWorld()->SpawnActor<AKotyItemBase>(Target, SpawnTransform);
		}
	}
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
	UE_LOG(LogTemp, Log, TEXT("%s"), *ShootArrowComp->GetComponentLocation().ToString());
	return ShootArrowComp->GetComponentLocation();
}

void UKotyItemHoldComponent::GetRandomItem()
{
	//랜덤으로 뽑은 하나의 아이템
	int Dice = FMath::RandRange(
		static_cast<int32>(EItemList::Start) + 1,
		static_cast<int32>(EItemList::End) - 1);
	const EItemList ItemCode = static_cast<EItemList>(Dice);
	UE_LOG(LogTemp, Log, TEXT("%d"), ItemCode);
	
	if (CodeOfHoldingItems[0] == EItemList::None)
	{
		//아이템 생성 성공
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(ItemCode))
		{
			//회전 시작
			StartRotatingAround(SpawnItem);

			//아이템 코드 저장
			CodeOfHoldingItems[0] = ItemCode;

			//소유자
			SpawnItem->ItemOwningActor = GetOwner();
		}

		//아이템 획득 사실 전파
		OnGetItemEventDispatcher.Broadcast(ItemCode);
	}
	else if (CodeOfHoldingItems[1] == EItemList::None)
	{
		//아이템 코드만 저장
		CodeOfHoldingItems[1] = ItemCode;

		//아이템 획득 사실 전파
		OnGetItemEventDispatcher.Broadcast(ItemCode);
	}
}

void UKotyItemHoldComponent::GetSpecifiedItem(const EItemList ItemCode)
{
	if (CodeOfHoldingItems[0] == EItemList::None)
	{
		//아이템 생성 성공
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(ItemCode))
		{
			//아이템 회전
			StartRotatingAround(SpawnItem);

			//아이템 코드 저장
			CodeOfHoldingItems[0] = ItemCode;

			//소유자
			SpawnItem->ItemOwningActor = GetOwner();
		}	
	}
	else if (CodeOfHoldingItems[1] == EItemList::None)
	{
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

	//유효한 코드를 가졌다면 2번째에 있던 아이템이 있었다는 의미
	if (CodeOfHoldingItems[0] != EItemList::None)
	{
		//아이템 생성 성공
		if (AKotyItemBase* SpawnItem = SpawnItemByCode(CodeOfHoldingItems[0]))
		{
			//아이템 회전
			StartRotatingAround(SpawnItem);

			//소유자
			SpawnItem->ItemOwningActor = GetOwner();
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