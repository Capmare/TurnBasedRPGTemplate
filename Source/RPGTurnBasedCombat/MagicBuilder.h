// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Combat.h"
#include "Animation/AnimMontage.h"
#include "Components/Image.h"
#include "NiagaraComponent.h"
#include "MagicBuilder.generated.h"

USTRUCT(BlueprintType)
struct FMagicAbility {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int AbilityLevel{1};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AttackName;
	UPROPERTY(Category = "Animation", BlueprintReadWrite, EditAnywhere)
	UAnimMontage* AnimationMontage;
	UPROPERTY(Category = "Damage", BlueprintReadWrite, EditAnywhere)
	EDamageTypes DamageType;
	UPROPERTY(Category = "Damage", BlueprintReadWrite, EditAnywhere)
	float AmmountOfDamage{0};
	UPROPERTY(Category = "VFX", BlueprintReadWrite, EditAnywhere)
	UNiagaraSystem* VFX;
	UPROPERTY(Category = "Damage", BlueprintReadWrite, EditAnywhere)
	FLinearColor MagicColor;

};

UCLASS(Blueprintable)
class RPGTURNBASEDCOMBAT_API UCombatListObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FMagicAbility MagicAbility{};
};
/**
 * 
 */
UCLASS()
class RPGTURNBASEDCOMBAT_API UMagicBuilder : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FMagicAbility MagicAbility{};

};
