// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat.h"
#include "Kismet/KismetMathLibrary.h"
#include "MagicBuilder.h"
#include "BaseRPGCharacter.generated.h"




UCLASS()
class RPGTURNBASEDCOMBAT_API ABaseRPGCharacter : public ACharacter, public ICombat
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseRPGCharacter();

	void ShowAbilitiesWidget(bool bShow);

	UFUNCTION(BlueprintCallable)
	void Init(const bool bShouldGenerateRandomStat, const bool bIsPlayer);

	UFUNCTION(BlueprintCallable)
	FPlayerStatuses GetStatuses() const { return Statuses; }

	UFUNCTION(BlueprintCallable)
	void SetStatuses(FPlayerStatuses val) { Statuses = val; }

	UFUNCTION(BlueprintCallable)
	void AddAbility(TSubclassOf<UCombatListObject> NewAbility) { Abilities.Add(NewAbility); }
	UFUNCTION(BlueprintCallable)
	void RemoveAllAbilities() { Abilities.Empty(); }
	UFUNCTION(BlueprintCallable)
	void RemoveFirstAbility() { Abilities.RemoveAt(0); }
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<UCombatListObject>> GetAbilities() const { return Abilities; }

	FVector GetWorldCombatCameraPosition() const { return UKismetMathLibrary::TransformLocation(GetTransform(),CombatCameraPosition); }
	bool GetIsPlayer() const { return bIsPlayerCharacter; }


	UPROPERTY(Category = "UI",EditDefaultsOnly,BlueprintReadWrite)
	class UAbilitiesWidget* FirstCombatWidget;

	UPROPERTY(Category = "Combat_Priority", VisibleAnywhere, BlueprintReadWrite)
	uint8 PlayerPriority{ 0 };

	UPROPERTY(Category = "Damage", BlueprintReadWrite,EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent* SelectionPoint;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category = "Statuses", VisibleAnywhere, meta = (AllowPrivateAccess))
	FPlayerStatuses Statuses;
	UPROPERTY(Category = "Magic", EditDefaultsOnly, BlueprintReadWrite,meta = (AllowPrivateAccess))
	TArray<TSubclassOf<UCombatListObject>> Abilities;

	FDealingDamage CurrentDealingDamage{};

	UPROPERTY(Category = "Damage", EditDefaultsOnly, meta = (AllowPrivateAccess))
	UAnimMontage* DamagedAnim;



public:	
	UFUNCTION(BlueprintCallable)
	virtual void DealDamage(FDealingDamage ReceivedDamage, class ABaseRPGCharacter* Damager, bool bIsReturnedOnce = false) override;
	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAttack(class ABaseRPGCharacter* Damaged, class ATurnManager* TurnManager) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetCurrentDealingDamage(FDealingDamage val) { CurrentDealingDamage = val; }

	UPROPERTY(Category = "Combat_UI", VisibleAnywhere, BlueprintReadWrite)
	UCombatListObject* CurrentAbility{};

protected:
	UPROPERTY(Category = "Sound", EditDefaultsOnly, meta = (AllowPrivateAccess))
	TArray<class USoundBase*> MagicSounds{};
private:

	void AfterDealDamageDelay(class ATurnManager* TurnManager);

	// weapon, used only for player and not enemies
	UPROPERTY(Category = "Weapon", EditDefaultsOnly, meta = (AllowPrivateAccess))
	UWeapon* Weapon;

	UPROPERTY(Category = "Camera", EditAnywhere, meta = (AllowPrivateAccess), Meta = (MakeEditWidget = "true"))
	FVector CombatCameraPosition;



	bool bIsPlayerCharacter{ false };
};

UCLASS()
class RPGTURNBASEDCOMBAT_API UWeapon: public USceneComponent
{
	GENERATED_BODY()

public:
		

protected:	
	UPROPERTY(Category = "Weapon", BlueprintReadWrite, EditDefaultsOnly)
	UStaticMeshComponent* WeaponMesh;

private:

};

