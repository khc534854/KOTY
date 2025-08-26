// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Base/KotyItemBase.h"
#include "SquidItem.generated.h"

UCLASS()
class KOTY_API ASquidItem : public AKotyItemBase
{
	GENERATED_BODY()

public:
	ASquidItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyItemEffect(AActor* Player) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComp;
};
