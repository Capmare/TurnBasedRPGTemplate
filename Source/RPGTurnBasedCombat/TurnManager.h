// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

UENUM(BlueprintType)
enum class ECurrentTurn : uint8 {
	Player, Enemy
};

UENUM(BlueprintType)
enum class ETurnState : uint8 {
	AbilitySelection,
	TargetSelection,
	SpectatePlayerAttack,
	EnemyTargetSelection,
	EnemyAttackSelection,
	SpectateEnemyAttack
};

UCLASS()
class RPGTURNBASEDCOMBAT_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	ECurrentTurn CurrentTurn;
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	ETurnState CurrentTurnState;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	int TurnsLeft{3};

	void InitPlayer();
	void InitEnemy();

	

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool bPlayerStarts{false};

	UFUNCTION(BlueprintImplementableEvent)
	void GoToPreviousGameLevel();

	
private:
	class ACombatCamera* CombatCamera;


public:
	class ACombatCamera* GetCombatCamera() const { return CombatCamera; }
};
