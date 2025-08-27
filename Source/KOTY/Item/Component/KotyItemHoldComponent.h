// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "KotyItemHoldComponent.generated.h"

UENUM()
enum class EItemList : uint8
{
	Start,
	Banana,
	BigMushroom,
	BlackBomb,
	Coin,
	GreenTurtle,
	RedTurtle,
	SmallMushroom,
	Squid,
	Star,
	End,
	None,
};

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
	TSubclassOf<class ABananaItem> BananaItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABigMushroomItem> BigMushroomClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABlackBombItem> BlackBombClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACoinItem> CoinItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AGreenTurtleItem> GreenTurtleItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ARedTurtleItem> RedTurtleItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ASmallMushroomItem> SmallMushroomItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ASquidItem> SquidItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AStarItem> StarItemClass;

	UFUNCTION(BlueprintCallable)
	FVector GetShootLocation() const;
	
	UFUNCTION(BlueprintCallable)
	void GetRandomItem();

	UFUNCTION(BlueprintCallable)
	void UseCurrentItem();

	UFUNCTION(BlueprintCallable)
	void LoseHoldingItems();
};
