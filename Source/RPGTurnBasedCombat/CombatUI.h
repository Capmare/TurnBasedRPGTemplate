// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpdateUI.h"
#include "CombatUI.generated.h"

/**
 * 
 */
UCLASS()
class RPGTURNBASEDCOMBAT_API UCombatUI : public UUserWidget, public IUpdateUI
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void NavigateDown();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void NavigateUp();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void SelectAttack();
};
