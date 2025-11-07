#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Combat.generated.h"


USTRUCT(BlueprintType)
struct FMagicStatus {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Values", BlueprintReadOnly, VisibleDefaultsOnly)
	float DamagePerTurn{};
	UPROPERTY(Category = "Values", BlueprintReadOnly, VisibleDefaultsOnly)
	bool bDoesKnockDown{false};
	UPROPERTY(Category = "Values", BlueprintReadOnly, VisibleDefaultsOnly)
	bool bSkipsTurn{false};
	// Implement VFX
};

UENUM(BlueprintType)
enum class EDamageTypes : uint8
{
	FIRE, ELECTRICITY, WIND, MYSTIC, BLOOD

};


// Types of damages that the character will be affected with
USTRUCT(BlueprintType)
struct FPlayerStatuses
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Values", VisibleAnywhere,BlueprintReadWrite)
	float Health{100};
	UPROPERTY(Category = "Magic", BlueprintReadOnly, VisibleDefaultsOnly)
	FMagicStatus CurrentMagicStatus{};

	
	UPROPERTY(Category = "Weaknesses",VisibleAnywhere, BlueprintReadWrite)
	TArray<EDamageTypes> CriticalDamage;

	UPROPERTY(Category = "Weaknesses", VisibleAnywhere, BlueprintReadWrite)
	TArray<EDamageTypes> NullifyDamage;

	UPROPERTY(Category = "Weaknesses", VisibleAnywhere, BlueprintReadWrite)
	TArray<EDamageTypes> WeakDamage;
	
	UPROPERTY(Category = "Weaknesses", VisibleAnywhere, BlueprintReadWrite)
	TArray<EDamageTypes> ReturnDamage;


	bool CheckCritical(const EDamageTypes& DamageType)
	{
		for (EDamageTypes Damage : CriticalDamage)
		{
			if (Damage == DamageType) return true;
		}
		return false;
	};
	bool CheckNullify(const EDamageTypes& DamageType)
	{
		for (EDamageTypes Damage : NullifyDamage)
		{
			if (Damage == DamageType) return true;
		}
		return false;
	};
	bool CheckWeak(const EDamageTypes& DamageType)
	{
		for (EDamageTypes Damage : WeakDamage)
		{
			if (Damage == DamageType) return true;
		}
		return false;
	};
	bool CheckReturn(const EDamageTypes& DamageType)
	{
		for (EDamageTypes Damage : ReturnDamage)
		{
			if (Damage == DamageType) return true;
		}
		return false;
	};

};


// Damage that the character will give
USTRUCT(BlueprintType)
struct FDealingDamage {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Values",EditDefaultsOnly, BlueprintReadWrite)
	float DamageAmmount{};

	UPROPERTY(Category = "Values",EditDefaultsOnly, BlueprintReadWrite)
	EDamageTypes DamageType;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UCombat : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGTURNBASEDCOMBAT_API ICombat
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual void DealDamage(FDealingDamage ReceivedDamage, class ABaseRPGCharacter* Damager, bool bIsReturnedOnce = false) = 0;
	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAttack(class ABaseRPGCharacter* Damaged, class ATurnManager* TurnManager) = 0;

};

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UCameraActions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGTURNBASEDCOMBAT_API ICameraActions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual void MoveToNextCamera() = 0;
	UFUNCTION(BlueprintCallable)
	virtual void RotateCameraToNextEnemy(bool bIsInverted = false) = 0;
	UFUNCTION(BlueprintCallable)
	virtual void MoveCameraToWidget() = 0;
	UFUNCTION(BlueprintCallable)
	virtual void MoveCameraToLocationWithRotation(const FVector& NewLocation,const FVector& NewRotation) = 0;
	UFUNCTION(BlueprintCallable)
	virtual void RotateCameraToCurrentEnemy() = 0;
};
