// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "CombatCameraController.generated.h"

/**
 * 
 */
UCLASS()
class RPGTURNBASEDCOMBAT_API ACombatCameraController : public APlayerController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
public:
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	bool GetCanConsumeInput() const { return bCanConsumeInput; }
	UFUNCTION(BlueprintCallable)
	void SetCanConsumeInput(bool val) { bCanConsumeInput = val; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* NextCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PreviousCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveListUpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveListDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SelectAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ConfirmEnemyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenWeaknessInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BackToSelection;

private:
	// KEYBOARD
	void NextCharacter();
	void PreviousCharacter();
	void MoveListSelectionDown();
	void MoveListSelectionUp();
	void SelectAttack();
	void ConfirmEnemy();
	void OpenWeaknessInfoTab();
	void CloseWeaknessInfoTab();
	void GoBackAbilityToSelection();

	APawn* Pawn;
	class ATurnManager* TurnManager;

	bool bCanConsumeInput{ true };
	bool bSelectsAbility{ true };

};
