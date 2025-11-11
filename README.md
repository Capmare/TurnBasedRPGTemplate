# Unreal Engine 5 Turn-Based RPG Combat System  
**by CAPMARE DAVID NICUSOR**

---

## Overview

This system implements a fully turn-based combat loop in Unreal Engine 5, where players and enemies alternate turns, executing magical or physical attacks defined through Data Assets.

Each attack is tied to an ability definition that determines the damage type, visual effects, animation montage, and power level.  
The system is built around modular interfaces, data-driven assets, and camera actions to provide cinematic combat transitions.

---

## Features

- Player and Enemy Turn Management  
- Damage types & weakness system  
- Magic abilities (Data Asset-based)  
- AI Target Selection (lowest HP, critical type countering)  
- Combat status effects (critical, weak, nullify, return)  
- Dynamic camera movement  
- VFX and SFX integration via Niagara & Sound2D  
- Expandable DataAsset architecture  

---

## How to Use

### 1. Setup Turn Manager

Place the `TurnManager` actor in your combat level.  
This manager orchestrates turn order, initializes the first player/enemy turn, and transitions between states.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
bool bPlayerStarts = true;
```

If `bPlayerStarts` is true, the player will begin the first round.  
Otherwise, the AI enemies start.

---

### 2. Setting up the Combat Camera

The Combat Camera Pawn acts as the main cinematic camera system.  
It controls movement between player selection, attacks, and enemy spectate phases.

The Combat Camera must implement the `ICameraActions` interface, which defines the following functions:

```cpp
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UCameraActions : public UInterface
{
	GENERATED_BODY()
};

class RPGTURNBASEDCOMBAT_API ICameraActions
{
	GENERATED_BODY()

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
```

The Combat Camera tracks current player and enemy actors, manages visibility, and handles transitions such as moving between ability selection and attack views.

---

### 3. Creating Magic Abilities

Each spell or ability is defined inside a `MagicBuilder` Data Asset.  
This makes the ability system fully data-driven and easily extendable.

#### MagicBuilder.h

```cpp
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
```

Abilities can be stored inside `UCombatListObject` or directly in `UMagicBuilder` assets.

```cpp
UCLASS(Blueprintable)
class RPGTURNBASEDCOMBAT_API UCombatListObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FMagicAbility MagicAbility{};
};
```

You can create them via **Right-Click → Miscellaneous → Data Asset → MagicBuilder**.  
Assign animation montages, VFX, and sound cues in the editor.

---

### 4. Combat Interface System

The combat system is powered by two main interfaces: `ICombat` and `ICameraActions`.

`ICombat` defines universal functions for damage dealing and executing attacks.

```cpp
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UCombat : public UInterface
{
	GENERATED_BODY()
};

class RPGTURNBASEDCOMBAT_API ICombat
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void DealDamage(FDealingDamage ReceivedDamage, class ABaseRPGCharacter* Damager, bool bIsReturnedOnce = false) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAttack(class ABaseRPGCharacter* Damaged, class ATurnManager* TurnManager) = 0;
};
```

---

### 5. Damage Types and Statuses

#### EDamageTypes

Defines the elemental or mystical damage classification.

```cpp
UENUM(BlueprintType)
enum class EDamageTypes : uint8
{
	FIRE, ELECTRICITY, WIND, MYSTIC, BLOOD
};
```

#### FMagicStatus

Defines ongoing magic effects that persist over turns.

```cpp
USTRUCT(BlueprintType)
struct FMagicStatus {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Values", BlueprintReadOnly, VisibleDefaultsOnly)
	float DamagePerTurn{};
	UPROPERTY(Category = "Values", BlueprintReadOnly, VisibleDefaultsOnly)
	bool bDoesKnockDown{false};
	UPROPERTY(Category = "Values", BlueprintReadOnly, VisibleDefaultsOnly)
	bool bSkipsTurn{false};
};
```

#### FPlayerStatuses

Defines player resistances, weaknesses, and health.

```cpp
USTRUCT(BlueprintType)
struct FPlayerStatuses
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Values", VisibleAnywhere, BlueprintReadWrite)
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

	bool CheckCritical(const EDamageTypes& DamageType);
	bool CheckNullify(const EDamageTypes& DamageType);
	bool CheckWeak(const EDamageTypes& DamageType);
	bool CheckReturn(const EDamageTypes& DamageType);
};
```

---

### 6. ABaseRPGCharacter

`ABaseRPGCharacter` represents any combat participant.  
It manages health, abilities, UI, animations, and the logic for taking and dealing damage.

#### Initialization

```cpp
void ABaseRPGCharacter::Init(const bool bShouldGenerateRandomStat, const bool bIsPlayer);
```
- Randomly generates weaknesses/resistances if requested.  
- Hides ability UI for AI-controlled enemies.

#### Dealing Damage

```cpp
void ABaseRPGCharacter::DealDamage(FDealingDamage ReceivedDamage, ABaseRPGCharacter* Damager, bool bIsReturnedOnce);
```

Handles:

- Spawning Niagara VFX at impact point.  
- Checking resistances (nullify, weak, critical, return).  
- Modifying or reflecting damage as needed.  
- Playing hit or death animations.  
- Updating game state through CombatCamera.

**Example log:**  
```
Damage amount: 30.0 of damage type: FIRE
Critical: 1
Weak: 0
Nullify: 0
Return: 0
```

#### Executing Attacks

```cpp
void ABaseRPGCharacter::ExecuteAttack(class ABaseRPGCharacter* Damaged, class ATurnManager* TurnManager);
```

- Plays ability montage and random SFX.  
- Applies damage.  
- Calls `AfterDealDamageDelay` to manage phase transition.

---

### 7. ABaseRPGCharacterEnemy

This subclass automates the AI behavior.

**AI Flow**

1. `GetLowestHealthPlayer()` – Selects the player with the lowest HP.  
2. `CheckAgainstCritical()` – Chooses an ability type that matches the target's weakness if available.  
3. `AttackLowesHealthPlayer()` – Plays animation, applies damage, and advances the turn after a delay.  
4. `NextTurn()` – Decrements remaining turns and calls TurnManager to either move to the next enemy or back to player phase.

---

### 8. TurnManager

The Turn Manager controls the combat loop and manages state transitions.

#### State Enums

```cpp
enum class ETurnState { AbilitySelection, EnemyAttackSelection, SpectateEnemyAttack };
enum class ECurrentTurn { Player, Enemy };
```

#### InitPlayer

```cpp
void ATurnManager::InitPlayer()
{
	CombatCamera->GetCurrentPlayer()->ShowAbilitiesWidget(true);
	TurnsLeft = CombatCamera->GetPlayerActorsNum();
	CurrentTurnState = ETurnState::AbilitySelection;
}
```

#### InitEnemy

```cpp
void ATurnManager::InitEnemy()
{
	CombatCamera->MoveCameraToEnemySpectate();
	TurnsLeft = CombatCamera->GetEnemyActorsNum();
	CurrentEnemy->AttackLowesHealthPlayer();
}
```

#### Turn Flow Summary

| Step | Controller | Function | Description |
|------|-------------|-----------|-------------|
| 1 | TurnManager | InitPlayer() | Starts player phase |
| 2 | Player | ExecuteAttack() | Performs attack |
| 3 | TurnManager | AfterDealDamageDelay() | Handles next phase |
| 4 | TurnManager | InitEnemy() | Starts enemy phase |
| 5 | Enemy | AttackLowesHealthPlayer() | AI picks and attacks |
| 6 | TurnManager | NextTurn() | Rotates to next AI or back to player |

---

### 9. Blueprint Integration

#### Player Ability Selection UI

You can create a `UCombatUI` Blueprint widget containing buttons for each ability.  
Each button calls `ExecuteAttack()` when clicked.

**Example Blueprint logic:**  
```
Event OnClick (FireballButton)
    → Set CurrentAbility
    → ExecuteAttack(TargetEnemy, TurnManager)
```

#### Updating the Ability Widget

Each player has a `UAbilitiesWidget` component that populates via the `IUpdateUI` interface.

```cpp
if (IUpdateUI* IUpdateUIInterface = Cast<IUpdateUI>(Widget))
{
	IUpdateUIInterface->Execute_RefreshObjects(Widget, Abilities);
}
```

This updates the UMG widget with the current ability list.

#### Visual Feedback and Camera Movement

Blueprint nodes in your CombatCamera class can call:

- MoveToNextCamera  
- RotateCameraToNextEnemy  
- MoveCameraToEnemySpectate  

**Blueprint example:**  
```
→ Player Executes Attack
→ Wait for Animation
→ Call TurnManager.AfterDealDamageDelay
→ Move Camera to Enemy
→ Enemy Phase Begins
```

#### End Game UI

When `CombatCamera->CheckGameEnded()` is triggered, show a Victory or Defeat widget.

**Blueprint example:**  
```
Event OnGameEndCheck
    → If PlayerTeamAlive == false → Show Defeat Screen
    → If EnemyTeamAlive == false → Show Victory Screen
```

---

### 10. Example Combat Flow

```
[Player Turn]
    |
    v
[Select Ability]
    |
    v
[Execute Attack]
    |
    v
[Damage Applied] --> [Enemy Dies?] -> [Game End]
    |
    v
[Turn Manager → Enemy Turn]
    |
    v
[Enemy Chooses Target]
    |
    v
[Enemy Executes Attack]
    |
    v
[Turn Manager → Player Turn]
```

---

### Debugging and Logs

- Damage calculations are printed via `UE_LOG(LogTemp, Warning, ...)`  
- Enemy decisions and critical hits are logged for AI debugging  
- Turn transitions are logged inside TurnManager

---

### Extending the System

**Adding New Damage Types**  
Edit `EDamageTypes` and expand the player weaknesses array.

**Adding New Camera Actions**  
Add new `BlueprintCallable` functions to `ICameraActions`.

**Adding Status Effects**  
Expand `FMagicStatus` with new gameplay properties, such as paralysis or poison.

---

## Conclusion

The Turn-Based RPG Combat System by **CAPMARE DAVID NICUSOR** is a modular and extensible Unreal Engine 5 framework for building JRPG-style combat systems.  
It utilizes Data Assets, AI logic, interfaces, and camera transitions to create cinematic gameplay loops that are easy to extend with new mechanics, visual effects, and AI behaviors.
