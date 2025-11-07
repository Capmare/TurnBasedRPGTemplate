// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseRPGCharacterEnemy.h"
#include "Combat.h"
#include "Kismet/GameplayStatics.h"
#include "CombatCamera.h"
#include "TurnManager.h"
#include "WeaknessInfoWidget.h"
#include "CombatCamera.h"

ABaseRPGCharacterEnemy::ABaseRPGCharacterEnemy()
{
	//Init(true, false);
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurnManager::StaticClass(), FoundActors);
	if (!FoundActors.IsEmpty())
	{
		TurnManager = Cast<ATurnManager>(FoundActors[0]);
	}


	WeaknessWidget = CreateDefaultSubobject<UWeaknessInfoWidget>(TEXT("WeaknessWidget"));
	WeaknessWidget->SetupAttachment(RootComponent);

}

ABaseRPGCharacterEnemy::~ABaseRPGCharacterEnemy()
{

}
// THIS FUNCTION SHOULD BE ALWAYS CALLED BEFORE DOING ANY OTHER STUFF
void ABaseRPGCharacterEnemy::GetAllAlivePlayers(TArray<ABaseRPGCharacter*> val)
{
	TurnManager->CurrentTurnState = ETurnState::SpectateEnemyAttack;
	AlivePlayers = val;
}

///////////////////			This functions should happen in order			///////////////////
void ABaseRPGCharacterEnemy::GetLowestHealthPlayer()
{
	GetAllAlivePlayers(TurnManager->GetCombatCamera()->GetPlayerActors());
	if (!AlivePlayers.IsEmpty())
	{
		CurrentTarget = AlivePlayers[0];
		for (ABaseRPGCharacter* LowestHealthPlayer : AlivePlayers)
		{
			if (LowestHealthPlayer->GetStatuses().Health < CurrentTarget->GetStatuses().Health)
			{
				CurrentTarget = LowestHealthPlayer;
			}
		}
	}
	
}

void ABaseRPGCharacterEnemy::CheckAgainstCritical()
{
	bool bAbilitySet{false};
	for (TSubclassOf<UCombatListObject> Ab : Abilities)
	{
		if (UCombatListObject* A = Cast<UCombatListObject>(Ab->GetDefaultObject()))
		{
			for (auto DamageType : CurrentTarget->GetStatuses().CriticalDamage)
			{
				if (DamageType == A->MagicAbility.DamageType)
				{
					SetCurrentDealingDamage(FDealingDamage{ A->MagicAbility.AmmountOfDamage,A->MagicAbility.DamageType });
					CurrentAbility = A;
					bAbilitySet = true;

				}
			}
			
			if (CurrentDealingDamage.DamageAmmount < 1.f)
			{
				CurrentDealingDamage = FDealingDamage{ A->MagicAbility.AmmountOfDamage,A->MagicAbility.DamageType };
				bAbilitySet = false;
			}

		}
	}
	if (!bAbilitySet)
	{
		UCombatListObject* A = Cast<UCombatListObject>(Abilities[UKismetMathLibrary::RandomIntegerInRange(0, Abilities.Num() - 1)]->GetDefaultObject());
		CurrentAbility = A;
	}
	
}

void ABaseRPGCharacterEnemy::AttackLowesHealthPlayer()
{
	float Anim{};
	if (CurrentAbility)
	{
		if (!MagicSounds.IsEmpty())
		{
			int soundNum = UKismetMathLibrary::RandomIntegerInRange(0, MagicSounds.Num()-1);
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				MagicSounds[soundNum],
				UKismetMathLibrary::RandomFloatInRange(.3f, .8f),
				UKismetMathLibrary::RandomFloatInRange(.3f, 1.f),
				0.f
			);
		}
		Anim = PlayAnimMontage(CurrentAbility->MagicAbility.AnimationMontage);
		UE_LOG(LogTemp, Warning, TEXT("%f"), Anim);
	}

	CurrentTarget->DealDamage(CurrentDealingDamage, this);
	TurnManager->GetCombatCamera()->NextEnemy();

	UE_LOG(LogTemp, Warning, TEXT("Delay starting to switch to next"));
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ABaseRPGCharacterEnemy::NextTurn, Anim, false);
	

}

void ABaseRPGCharacterEnemy::NextTurn()
{
	if (!TurnManager->GetCombatCamera()->ListOfIgnoredActors.IsEmpty())
	{
		for (ABaseRPGCharacter* actor : TurnManager->GetCombatCamera()->ListOfIgnoredActors)
		{
			actor->Destroy();
		}
		TurnManager->GetCombatCamera()->ListOfIgnoredActors.Empty();
	}
	
	TurnManager->GetCombatCamera()->UpdatePlayersAndEnemies(false);
	//TurnManager->GetCombatCamera()->RotateCameraToCurrentEnemy();
	// move to next enemy
	--TurnManager->TurnsLeft;
	if (TurnManager->TurnsLeft > 0)
	{
		if (ABaseRPGCharacterEnemy* CurrentEnemy = Cast<ABaseRPGCharacterEnemy>(TurnManager->GetCombatCamera()->GetCurrentEnemy()))
		{
			CurrentEnemy->GetLowestHealthPlayer();
			CurrentEnemy->CheckAgainstCritical();
			CurrentEnemy->AttackLowesHealthPlayer();

		}
	}
	else
	{

		TurnManager->InitPlayer();
		TurnManager->CurrentTurnState = ETurnState::AbilitySelection;
	}
	UE_LOG(LogTemp, Warning, TEXT("Delay ended to switch to next"));

}
///////////////////////////////////////////////////////////////////////////////////////////////

void ABaseRPGCharacterEnemy::CheckOwnCritical()
{
	// if there is a critical increase chance to run away by .1 each turn
	
}

void ABaseRPGCharacterEnemy::CalculateRunAwayChance()
{

}

void ABaseRPGCharacterEnemy::ShowInfoWidget(bool bShow)
{
	if (WeaknessWidget)
	{
		WeaknessWidget->SetVisibility(bShow);
	}
}



void ABaseRPGCharacterEnemy::CheckArrayOfDamages(const TArray<EDamageTypes>& DamagesArray, EDamageTypes& DamageMatch)
{
	// get all types of damage that matches and return
	
}


