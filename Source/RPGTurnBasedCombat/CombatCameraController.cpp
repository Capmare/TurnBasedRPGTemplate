// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatCameraController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Combat.h"
#include "CombatUI.h"
#include "AbilitiesWidget.h"
#include "BaseRPGCharacter.h"
#include "CombatUI.h"
#include "CombatCamera.h"
#include "CameraWayPoint.h"
#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"
#include "BaseRPGCharacterEnemy.h"

void ACombatCameraController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	Pawn = GetPawn();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurnManager::StaticClass(), FoundActors);

	if (!FoundActors.IsEmpty())
	{
		TurnManager = Cast<ATurnManager>(FoundActors[0]);
	}
}

void ACombatCameraController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) {
		
		
		EnhancedInputComponent->BindAction(NextCameraAction, ETriggerEvent::Started, this, &ACombatCameraController::NextCharacter);
		EnhancedInputComponent->BindAction(PreviousCameraAction, ETriggerEvent::Started, this, &ACombatCameraController::PreviousCharacter);
		EnhancedInputComponent->BindAction(MoveListUpAction, ETriggerEvent::Started, this, &ACombatCameraController::MoveListSelectionUp);
		EnhancedInputComponent->BindAction(MoveListDownAction, ETriggerEvent::Started, this, &ACombatCameraController::MoveListSelectionDown);
		EnhancedInputComponent->BindAction(SelectAttackAction, ETriggerEvent::Started, this, &ACombatCameraController::SelectAttack);
		EnhancedInputComponent->BindAction(ConfirmEnemyAction, ETriggerEvent::Started, this, &ACombatCameraController::ConfirmEnemy);

		EnhancedInputComponent->BindAction(OpenWeaknessInfo, ETriggerEvent::Started, this, &ACombatCameraController::OpenWeaknessInfoTab);
		EnhancedInputComponent->BindAction(OpenWeaknessInfo, ETriggerEvent::Completed, this, &ACombatCameraController::CloseWeaknessInfoTab);

		EnhancedInputComponent->BindAction(BackToSelection, ETriggerEvent::Started, this, &ACombatCameraController::GoBackAbilityToSelection);

	}

}




void ACombatCameraController::NextCharacter()
{
	if (!TurnManager->GetCombatCamera()->ListOfIgnoredActors.IsEmpty())
	{
		for (ABaseRPGCharacter* actor : TurnManager->GetCombatCamera()->ListOfIgnoredActors)
		{
			actor->Destroy();
		}
		TurnManager->GetCombatCamera()->ListOfIgnoredActors.Empty();
		TurnManager->GetCombatCamera()->UpdatePlayersAndEnemies(false);

	}

	if (TurnManager->CurrentTurnState == ETurnState::TargetSelection)
	{
		if (auto cameraInterface = Cast<ICameraActions>(Pawn))
		{
			cameraInterface->RotateCameraToNextEnemy(false);
		}
	}

	
}

void ACombatCameraController::PreviousCharacter()
{
	if (!TurnManager->GetCombatCamera()->ListOfIgnoredActors.IsEmpty())
	{
		for (ABaseRPGCharacter* actor : TurnManager->GetCombatCamera()->ListOfIgnoredActors)
		{
			actor->Destroy();
		}
		TurnManager->GetCombatCamera()->ListOfIgnoredActors.Empty();
		TurnManager->GetCombatCamera()->UpdatePlayersAndEnemies(false);

	}

	if (TurnManager->CurrentTurnState == ETurnState::TargetSelection)
	{
		if (auto cameraInterface = Cast<ICameraActions>(Pawn))
		{
			cameraInterface->RotateCameraToNextEnemy(true);
		}
	}
	
}

void ACombatCameraController::MoveListSelectionDown()
{
	if (TurnManager->CurrentTurnState == ETurnState::AbilitySelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			if (ABaseRPGCharacter* RPGCharacter = Cast<ABaseRPGCharacter>(CombatCamera->GetCurrentPlayer()))
			{
				if (UCombatUI* CombatWidget = Cast<UCombatUI>(RPGCharacter->FirstCombatWidget->GetWidget()))
				{
					CombatWidget->NavigateDown();
				}
			}
		}
	}

	
}

void ACombatCameraController::MoveListSelectionUp()
{
	if (TurnManager->CurrentTurnState == ETurnState::AbilitySelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			if (ABaseRPGCharacter* RPGCharacter = Cast<ABaseRPGCharacter>(CombatCamera->GetCurrentPlayer()))
			{
				if (UCombatUI* CombatWidget = Cast<UCombatUI>(RPGCharacter->FirstCombatWidget->GetWidget()))
				{
					CombatWidget->NavigateUp();

				}
			}
		}
	}
	

}

void ACombatCameraController::SelectAttack() 
{
	if (TurnManager->CurrentTurnState == ETurnState::AbilitySelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			if (ABaseRPGCharacter* RPGCharacter = Cast<ABaseRPGCharacter>(CombatCamera->GetCurrentPlayer()))
			{
				if (UCombatUI* CombatWidget = Cast<UCombatUI>(RPGCharacter->FirstCombatWidget->GetWidget()))
				{
					CombatWidget->SelectAttack();

					if (auto cameraInterface = Cast<ICameraActions>(Pawn))
					{
						cameraInterface->MoveCameraToLocationWithRotation(CombatCamera->GetEnemySelectionWaypoint()->GetWorldCombatCameraPosition(), CombatCamera->GetEnemySelectionWaypoint()->CameraLookAt);
						// camera should rotate after this ends
						cameraInterface->RotateCameraToNextEnemy(true);

						TurnManager->CurrentTurnState = ETurnState::TargetSelection;
					}
				}
			}
		}
	}
	
}

void ACombatCameraController::ConfirmEnemy()
{
	if (TurnManager->CurrentTurnState == ETurnState::TargetSelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			if (auto cameraInterface = Cast<ICameraActions>(Pawn))
			{
				cameraInterface->MoveCameraToLocationWithRotation(CombatCamera->GetPlayerAttackingWaypoint()->GetWorldCombatCameraPosition(), CombatCamera->GetPlayerAttackingWaypoint()->CameraLookAt);
				TurnManager->CurrentTurnState = ETurnState::SpectatePlayerAttack;
				// create timer handle and add delay until camera reaches its point and then play montage and vfx, after that remove -1 from player turn and go to the next enemy
				// if the player turn is exhausted change to the enemy and do the same
				FTimerHandle handle;
				FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(CombatCamera->GetCurrentPlayer(), &ABaseRPGCharacter::ExecuteAttack, CombatCamera->GetCurrentEnemy(),TurnManager);
				GetWorldTimerManager().SetTimer(handle, TimerDel, 2.f, false);
						
			}
		}
	}
	
	
	
}

void ACombatCameraController::OpenWeaknessInfoTab()
{
	if (TurnManager->CurrentTurnState == ETurnState::TargetSelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			if (ABaseRPGCharacterEnemy* Enemy =Cast<ABaseRPGCharacterEnemy>(CombatCamera->GetCurrentEnemy()))
			{
				Enemy->UpdateWeaknesses();
				Enemy->ShowInfoWidget(true);
			}
		}
	}
}

void ACombatCameraController::CloseWeaknessInfoTab()
{
	if (TurnManager->CurrentTurnState == ETurnState::TargetSelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			for (auto enemy : CombatCamera->GetEnemyActors())
			{
				if (ABaseRPGCharacterEnemy* Enemy = Cast<ABaseRPGCharacterEnemy>(enemy))
				{
					Enemy->UpdateWeaknesses();
					Enemy->ShowInfoWidget(false);
				}
			}

		}
	}
}

void ACombatCameraController::GoBackAbilityToSelection()
{

	if (TurnManager->CurrentTurnState == ETurnState::TargetSelection)
	{
		if (ACombatCamera* CombatCamera = Cast<ACombatCamera>(Pawn))
		{
			TurnManager->CurrentTurnState = ETurnState::AbilitySelection;

			CombatCamera->GetCurrentPlayer()->ShowAbilitiesWidget(true);
			CombatCamera->MoveCameraToLocationWithRotation(CombatCamera->GetCurrentPlayer()->GetWorldCombatCameraPosition(), CombatCamera->GetCurrentPlayer()->GetWorldCombatCameraPosition());
			CombatCamera->MoveCameraToWidget();
		}
	}


	
}

