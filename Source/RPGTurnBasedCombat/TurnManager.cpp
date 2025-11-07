// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "BaseRPGCharacter.h"
#include "CombatCamera.h"
#include "Engine/World.h"
#include "Combat.h"
#include "BaseRPGCharacterEnemy.h"
#include "CombatCameraController.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ATurnManager::ATurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();

	// check if camera exist, if not, oh well, big time trouble
	CombatCamera = Cast<ACombatCamera>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!CombatCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find combat camera"));
		return;
	}

	if (bPlayerStarts)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ATurnManager::InitPlayer, .5f, false);
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ATurnManager::InitEnemy, .5f, false);
	}
	

}

// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CombatCamera->CheckGameEnded();
	CombatCamera->OnGameEndCheck();

}

void ATurnManager::InitPlayer()
{
	// Enable UI
	if (!GetCombatCamera()->ListOfIgnoredActors.IsEmpty())
	{
		for (ABaseRPGCharacter* actor : GetCombatCamera()->ListOfIgnoredActors)
		{
			actor->Destroy();
		}
		GetCombatCamera()->ListOfIgnoredActors.Empty();
	}

	CombatCamera->CheckGameEnded();
	CombatCamera->OnGameEndCheck();
	CombatCamera->UpdatePlayersAndEnemies();
	
	CombatCamera->GetCurrentPlayer()->ShowAbilitiesWidget(true);
	TurnsLeft = CombatCamera->GetPlayerActorsNum();

	// Move camera to UI attack type choosing position, for now only magic
	if (ICameraActions* CameraActionInterface = Cast<ICameraActions>(CombatCamera))
	{
		CurrentTurnState = ETurnState::AbilitySelection;
		CameraActionInterface->MoveToNextCamera();
	}
	CombatCamera->RotateCameraToCurrentEnemy();

}

void ATurnManager::InitEnemy()
{
	if (!GetCombatCamera()->ListOfIgnoredActors.IsEmpty())
	{
		for (ABaseRPGCharacter* actor : GetCombatCamera()->ListOfIgnoredActors)
		{
			actor->Destroy();
		}
		GetCombatCamera()->ListOfIgnoredActors.Empty();
	}

	CombatCamera->MoveCameraToEnemySpectate();
	CombatCamera->CheckGameEnded();
	CombatCamera->OnGameEndCheck();

	CombatCamera->UpdatePlayersAndEnemies();
	CombatCamera->NextEnemy();
	TurnsLeft = CombatCamera->GetEnemyActorsNum();

	if (ABaseRPGCharacterEnemy* CurrentEnemy = Cast<ABaseRPGCharacterEnemy>(CombatCamera->GetCurrentEnemy()))
	{
		CurrentEnemy->GetLowestHealthPlayer();
		CurrentEnemy->CheckAgainstCritical();
		CurrentEnemy->AttackLowesHealthPlayer();
	}

}


