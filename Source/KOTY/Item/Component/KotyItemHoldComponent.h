// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "KotyItemHoldComponent.generated.h"

//아이템 리스트 열거형
UENUM()
enum class EItemList : uint8
{
	Start,
	Banana,
	BlackBomb,
	Coin,
	GreenTurtle,
	RedTurtle,
	MushroomBig,
	MushroomSmall,
	Squid,
	Star,
	End,
	None,
};

//아이템 획득 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGetItem, EItemList)

//전방 선언
class UArrowComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KOTY_API UKotyItemHoldComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UKotyItemHoldComponent();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> ShootArrowComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotateGap = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotateDegreeSpeed = 180;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EItemList> CodeOfHoldingItems { EItemList::None, EItemList::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class AKotyItemBase> CurrentHoldingItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHoldingItemDegree = 0;

	UFUNCTION()
	void UpdateHoldingItemLocation(float DeltaTime);

	UFUNCTION()
	AKotyItemBase* SpawnItemByCode(EItemList ItemCode) const;

	UFUNCTION()
	void StartRotatingAround(AKotyItemBase* SpawnItem);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AKotyItemBase>> ItemClassArray;

	UFUNCTION(BlueprintCallable)
	FVector GetShootLocation() const;

	FOnGetItem OnGetItemEventDispatcher;
	
	UFUNCTION(BlueprintCallable)
	void GetRandomItem();

	UFUNCTION(BlueprintCallable)
	void GetSpecifiedItem(EItemList ItemCode);

	UFUNCTION(BlueprintCallable)
	void UseCurrentItem();

	UFUNCTION(BlueprintCallable)
	void LoseHoldingItems();

	bool isHasItem() { return CodeOfHoldingItems[0] != EItemList::None; };
};