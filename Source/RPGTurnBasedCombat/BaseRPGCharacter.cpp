// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseRPGCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "AbilitiesWidget.h"
#include "UpdateUI.h"
#include "CombatUI.h"
#include "TurnManager.h"
#include "CombatCamera.h"
#include "CameraWayPoint.h"
#include "../../FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseRPGCharacter::ABaseRPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));

	FirstCombatWidget = CreateDefaultSubobject<UAbilitiesWidget>(TEXT("CombatWidget"));

	FirstCombatWidget->SetupAttachment(RootComponent);

	//FirstCombatWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);

	SelectionPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionPoint"));
	SelectionPoint->SetVisibility(false);
	SelectionPoint->SetupAttachment(RootComponent);

}

void ABaseRPGCharacter::ShowAbilitiesWidget(bool bShow)
{
	if (FirstCombatWidget)
	{
		FirstCombatWidget->SetVisibility(bShow);
	}
}

void ABaseRPGCharacter::Init(const bool bShouldGenerateRandomStat, const bool bIsPlayer)
{
	if (bShouldGenerateRandomStat)
	{
		Statuses.CriticalDamage.Empty();
		Statuses.NullifyDamage.Empty();
		Statuses.ReturnDamage.Empty();
		Statuses.WeakDamage.Empty();

		TArray<EDamageTypes> StackSize = {EDamageTypes::BLOOD,EDamageTypes::WIND,EDamageTypes::FIRE,EDamageTypes::MYSTIC,EDamageTypes::ELECTRICITY};
		int dmgIdx = UKismetMathLibrary::RandomInteger(StackSize.Num());
		EDamageTypes CriticalDamage = StackSize[dmgIdx];
		StackSize.RemoveAt(dmgIdx);

		dmgIdx = UKismetMathLibrary::RandomInteger(StackSize.Num());
		EDamageTypes NullifyDamage = StackSize[dmgIdx];
		StackSize.RemoveAt(dmgIdx);

		dmgIdx = UKismetMathLibrary::RandomInteger(StackSize.Num());
		EDamageTypes ReturnDamage = StackSize[dmgIdx];
		StackSize.RemoveAt(dmgIdx);

		dmgIdx = UKismetMathLibrary::RandomInteger(StackSize.Num());
		EDamageTypes WeakDamage = StackSize[dmgIdx];
		StackSize.RemoveAt(dmgIdx);

		Statuses.CriticalDamage.Add(CriticalDamage);
		Statuses.NullifyDamage.Add(NullifyDamage);
		Statuses.ReturnDamage.Add(ReturnDamage);
		Statuses.WeakDamage.Add(WeakDamage);
	}

	bIsPlayerCharacter = bIsPlayer;
	if (!bIsPlayer)
	{
		ShowAbilitiesWidget(false);
	}

}

// Called when the game starts or when spawned
void ABaseRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCombatUI* Widget = Cast<UCombatUI>(FirstCombatWidget->GetWidget());

	if (IUpdateUI* IUpdateUIInterface = Cast<IUpdateUI>(Widget))
	{
		IUpdateUIInterface->Execute_RefreshObjects(Widget, Abilities);
	}
	
}



// Called every frame
void ABaseRPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseRPGCharacter::AfterDealDamageDelay(class ATurnManager* TurnManager)
{
	--TurnManager->TurnsLeft;
	ICameraActions* CameraActionInterface = Cast<ICameraActions>(TurnManager->GetCombatCamera());

	if (TurnManager->TurnsLeft > 0)
	{
		TurnManager->CurrentTurnState = ETurnState::AbilitySelection;
		CameraActionInterface->MoveToNextCamera();
	}
	else
	{
		TurnManager->CurrentTurnState = ETurnState::EnemyAttackSelection;
		APawn* pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		ACombatCamera* CombatCamera = Cast<ACombatCamera>(pawn);
		CameraActionInterface->MoveCameraToLocationWithRotation(CombatCamera->GetEnemyAttackingWaypoint()->GetWorldCombatCameraPosition(), CombatCamera->GetEnemyAttackingWaypoint()->CameraLookAt);
		// start doing AI magic stuff
		TurnManager->CurrentTurn = ECurrentTurn::Enemy;
		TurnManager->InitEnemy();


	}


}

void ABaseRPGCharacter::DealDamage(FDealingDamage ReceivedDamage, class ABaseRPGCharacter* Damager, bool bIsReturnedOnce /*= false*/)
{
	if (!ensure(Damager) || !ensure(Damager->CurrentAbility))
	{
		return;
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Damager->CurrentAbility->MagicAbility.VFX, GetActorLocation());


	bool bHasNullify = Statuses.CheckNullify(ReceivedDamage.DamageType);
	if (bHasNullify) return;

	bool bHasCritical = Statuses.CheckCritical(ReceivedDamage.DamageType);
	bool bHasWeak = Statuses.CheckWeak(ReceivedDamage.DamageType);
	bool bHasReturn = Statuses.CheckReturn(ReceivedDamage.DamageType);


	if (bHasCritical) ReceivedDamage.DamageAmmount *= 2;
	if (bHasWeak) ReceivedDamage.DamageAmmount *= 0.5;
	if (bHasReturn && !bIsReturnedOnce)
	{
		if (Damager)
		{
			Damager->DealDamage(ReceivedDamage, Damager, true);
		}
		return;

	}
	Statuses.Health -= ReceivedDamage.DamageAmmount;
	ACombatCamera* Pawn = Cast<ACombatCamera>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Statuses.Health > 0)
	{
		if (DamagedAnim)
		{ 
			PlayAnimMontage(DamagedAnim);
		}
	}
	else
	{
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		GetMesh()->SetSimulatePhysics(true);
		FTimerHandle TimerHandle;


		Pawn->ListOfIgnoredActors.Add(this);

		if (!Pawn->ListOfIgnoredActors.IsEmpty())
		{
			for (ABaseRPGCharacter* actor : Pawn->ListOfIgnoredActors)
			{
				actor->Destroy();
			}
			Pawn->ListOfIgnoredActors.Empty();
		}
	}

	// detect at the end so it always updates the player and enemy pointers 
	if (Pawn)
	{
		Pawn->UpdatePlayersAndEnemies(false);
		Pawn->CheckGameEnded();
		Pawn->OnGameEndCheck();
	}

	UE_LOG(LogTemp, Warning, TEXT("Damage ammount: %f of damage type: %s"), ReceivedDamage.DamageAmmount, *UEnum::GetDisplayValueAsText(ReceivedDamage.DamageType).ToString());
	UE_LOG(LogTemp, Warning, TEXT("Critical: %d \nWeak: %d \nNullify:%d \nReturn: %d\n "), bHasCritical, bHasWeak, bHasNullify, bHasReturn);


}

void ABaseRPGCharacter::ExecuteAttack(class ABaseRPGCharacter* Damaged, class ATurnManager* TurnManager)
{
	float Anim = PlayAnimMontage(CurrentAbility->MagicAbility.AnimationMontage);
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
	

	UE_LOG(LogTemp, Warning, TEXT("%f"), Anim);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &ABaseRPGCharacter::AfterDealDamageDelay, TurnManager);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, Anim, false);

	Damaged->DealDamage(CurrentDealingDamage, this);
	

	
}

