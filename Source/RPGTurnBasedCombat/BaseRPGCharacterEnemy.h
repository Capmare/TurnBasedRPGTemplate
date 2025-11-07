// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseRPGCharacter.h"
#include "BaseRPGCharacterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class RPGTURNBASEDCOMBAT_API ABaseRPGCharacterEnemy : public ABaseRPGCharacter
{
	GENERATED_BODY()
	ABaseRPGCharacterEnemy();
	~ABaseRPGCharacterEnemy();

public:
	void GetAllAlivePlayers(TArray<ABaseRPGCharacter*> val);

	void GetLowestHealthPlayer();
	void CheckAgainstCritical();
	void CheckOwnCritical();
	void AttackLowesHealthPlayer();
	// this will get the player character health, possible critical damage received and current enemy health
	void CalculateRunAwayChance(); 

	void ShowInfoWidget(bool bShow);

	UPROPERTY(Category = "UI",EditDefaultsOnly,BlueprintReadWrite)
	class UWeaknessInfoWidget* WeaknessWidget;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaknesses();
private:
	void CheckArrayOfDamages(const TArray<EDamageTypes>& DamagesArray, EDamageTypes& DamageMatch);
	void NextTurn();

	ATurnManager* TurnManager{};
	TArray<ABaseRPGCharacter*> AlivePlayers{};
	ABaseRPGCharacter* CurrentTarget{};


	float RunAwayChance = 0.f;
};
