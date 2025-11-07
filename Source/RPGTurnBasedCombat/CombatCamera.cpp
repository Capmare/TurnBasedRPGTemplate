// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatCamera.h"
#include "Kismet/GameplayStatics.h"
#include "BaseRPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitiesWidget.h"
#include "Camera/CameraComponent.h"
#include "CameraWayPoint.h"
#include "BaseRPGCharacterEnemy.h"


// Sets default values
ACombatCamera::ACombatCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")); 
	SpringArm->TargetArmLength = 0;
	Camera->SetupAttachment(SpringArm);

	TimelineFinishedEvent.BindUFunction(this,"OnTranslationTimelineFinished");
}

// Called when the game starts or when spawned
void ACombatCamera::BeginPlay()
{
	Super::BeginPlay();
	UpdatePlayersAndEnemies();
	
	TArray<AActor*> FoundActors;
	// Get camera positions
	FoundActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraWayPoint::StaticClass(), FoundActors);
	for (int Iterator{}; Iterator < FoundActors.Num(); ++Iterator)
	{
		auto CameraWaypoint = Cast<ACameraWayPoint>(FoundActors[Iterator]);
		switch (CameraWaypoint->CameraLocation)
		{
		case ECameraLocation::EnemyAttacking:
			EnemyAttackingWaypoint = CameraWaypoint;
			break;
		case ECameraLocation::EnemySelection:
			EnemySelectionWaypoint = CameraWaypoint;
			break;
		case ECameraLocation::PlayerAttacking:
			PlayerAttackingWaypoint = CameraWaypoint;
			break;
		}
	}
}

// Called every frame
void ACombatCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraTranslationCurveFTimeline.TickTimeline(DeltaTime);
	CameraRotationCurveFTimeline.TickTimeline(DeltaTime);

	

	//FHitResult out;
	//if (ensure(CurrentEnemy))
	//{
	//	UKismetSystemLibrary::LineTraceSingle(GetWorld(), CurrentEnemy->GetActorLocation(), CurrentEnemy->GetActorLocation() + CurrentEnemy->GetActorUpVector() * 200, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, out, false);
	//}

}

// Called to bind functionality to input
void ACombatCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ACombatCamera::MoveToNextCamera()
{
	CheckGameEnded();
	OnGameEndCheck();

	if (!PlayerActors.IsEmpty()) {
		UpdatePlayersAndEnemies(false);

		if (TimelineCurve)
		{
			CurrentCameraPosition = (CurrentCameraPosition + 1) % PlayerActors.Num();

			CameraTranslationCurveFTimeline.Stop();

			FOnTimelineFloat TimelineProgress{};
			TimelineProgress.BindUFunction(this, FName("CameraTranslationTimelineValue"));
			CameraTranslationCurveFTimeline.AddInterpFloat(TimelineCurve, TimelineProgress);
			CameraTranslationCurveFTimeline.SetLooping(false);

			StartCameraLoc = SpringArm->K2_GetComponentLocation();
			EndCameraLoc = PlayerActors[CurrentCameraPosition]->GetWorldCombatCameraPosition();
			if (CurrentPlayer)
			{
				CurrentPlayer->ShowAbilitiesWidget(false);

			}
			CurrentPlayer = PlayerActors[CurrentCameraPosition];
			CurrentPlayer->ShowAbilitiesWidget(true);
			WidgetRotation = CurrentPlayer->FirstCombatWidget->GetComponentLocation();
			MoveCameraToWidget();
			CameraTranslationCurveFTimeline.PlayFromStart();

		}

	}
}

void ACombatCamera::RotateCameraToNextEnemy(bool bIsInverted /*= false*/)
{
	if (!EnemyActors.IsEmpty())
	{
		if (TimelineCurve)
		{
			CameraRotationCurveFTimeline.Stop();

			CurrentCameraRotation = bIsInverted
				? (CurrentCameraRotation - 1 + EnemyActors.Num()) % EnemyActors.Num()
				: (CurrentCameraRotation + 1) % EnemyActors.Num();
			CurrentEnemy = EnemyActors[CurrentCameraRotation];
			RotateCameraToCurrentEnemy();
			
			UE_LOG(LogTemp, Log, TEXT("Current cam rot: %d"), CurrentCameraRotation);
		}
	}

}

void ACombatCamera::MoveCameraToLocationWithRotation(const FVector& NewLocation, const FVector& NewRotation)
{
	if (TimelineCurve)
	{
		CameraTranslationCurveFTimeline.Stop();


		FOnTimelineFloat TimelineProgress{};
		TimelineProgress.BindUFunction(this, FName("CameraTranslationTimelineValue"));
		CameraTranslationCurveFTimeline.SetTimelineFinishedFunc(TimelineFinishedEvent);

		CameraTranslationCurveFTimeline.AddInterpFloat(TimelineCurve, TimelineProgress);
		CameraTranslationCurveFTimeline.SetLooping(false);

		StartCameraLoc = Camera->K2_GetComponentLocation();
		EndCameraLoc = NewLocation;
		WidgetRotation = NewRotation;

		CameraTranslationCurveFTimeline.PlayFromStart();
		

	}

}

void ACombatCamera::MoveCameraToWidget()
{
	FVector EndLocation = GetCurrentPlayer()->FirstCombatWidget->GetComponentLocation() + (GetCurrentPlayer()->FirstCombatWidget->GetForwardVector() * 200.f);
	FHitResult hit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetCurrentPlayer()->FirstCombatWidget->GetComponentLocation(), EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, hit, false);
	MoveCameraToLocationWithRotation(EndLocation, GetCurrentPlayer()->FirstCombatWidget->GetComponentLocation());
}


void ACombatCamera::RotateCameraToCurrentEnemy()
{
	if (!EnemyActors.IsEmpty())
	{
		if (TimelineCurve)
		{
			CameraRotationCurveFTimeline.Stop();
			RotateCamera(EnemyActors[CurrentCameraRotation]->GetActorLocation());
		
			UE_LOG(LogTemp, Log, TEXT("Current cam rot: %d"), CurrentCameraRotation);
		}
	}
}

void ACombatCamera::UpdatePlayersAndEnemies(bool bShouldRotate /*= true*/)
{
	PlayerActors.Empty();
	EnemyActors.Empty();

	TArray<AActor*> FoundActors;
	// use sorted map to skip manually sorting
	TSortedMap<uint8, ABaseRPGCharacter*> SortedCharacters;

	// get all characters
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseRPGCharacter::StaticClass(), FoundActors);


	for (int Iterator{}; Iterator < FoundActors.Num(); ++Iterator)
	{

		// remove actors from array if not playable character
		if (auto player = Cast<ABaseRPGCharacterEnemy>(FoundActors[Iterator]))
		{

			EnemyActors.Add(player);
			FoundActors.RemoveAt(Iterator);
			--Iterator;
		}
	}
	for (auto& actors : FoundActors)
	{
		ABaseRPGCharacter* CastedActor = Cast<ABaseRPGCharacter>(actors);
		CastedActor->ShowAbilitiesWidget(false);
		SortedCharacters.Add(CastedActor->PlayerPriority, CastedActor);

	}
	// put sorted characters into TArray
	for (const auto& Pair : SortedCharacters)
	{
		PlayerActors.Add(Pair.Value);
	}
	if (!PlayerActors.IsEmpty() && bShouldRotate)
	{
		SpringArm->SetWorldLocation(PlayerActors[0]->GetWorldCombatCameraPosition());
		RotateCameraToNextEnemy(false);
		CurrentPlayer = PlayerActors[0];
	}
}

void ACombatCamera::CheckGameEnded()
{
	if (PlayerActors.IsEmpty())
	{
		EndGame.bPlayerWon = false;
		EndGame.bGameEnded = true;

	}
	if (EnemyActors.IsEmpty())
	{
		EndGame.bPlayerWon = true;
		EndGame.bGameEnded = true;
	}
}

void ACombatCamera::MoveCameraToEnemySpectate()
{
	MoveCameraToLocationWithRotation(GetEnemyAttackingWaypoint()->GetWorldCombatCameraPosition(), GetEnemyAttackingWaypoint()->CameraLookAt);

}



void ACombatCamera::RotateCamera(const FVector& NewRotation, bool bRotatesToWidget /*= false*/)
{
	if (TimelineCurve)
	{
		CameraRotationCurveFTimeline.Stop();

		FOnTimelineFloat TimelineProgress{};
		TimelineProgress.BindUFunction(this, FName("CameraRotationTimelineValue"));
		CameraRotationCurveFTimeline.AddInterpFloat(TimelineCurve, TimelineProgress);
		CameraRotationCurveFTimeline.SetLooping(false);

		StartCameraRot = Camera->K2_GetComponentLocation();
		if (bRotatesToWidget)
		{
			EndCameraRot = WidgetRotation;

		}
		else
		{
			EndCameraRot = NewRotation;

		}
		CameraRotationCurveFTimeline.PlayFromStart();

	}
}

void ACombatCamera::NextEnemy()
{
	if (!ListOfIgnoredActors.IsEmpty())
	{
		for (ABaseRPGCharacter* actor : ListOfIgnoredActors)
		{
			actor->Destroy();
		}
		ListOfIgnoredActors.Empty();
	}

	CheckGameEnded();
	OnGameEndCheck();

	

	if (!EnemyActors.IsEmpty())
	{
		CurrentEnemyIndex = (CurrentEnemyIndex + 1) % EnemyActors.Num();
		CurrentEnemy = EnemyActors[CurrentEnemyIndex];
		if (ABaseRPGCharacterEnemy* Enemy = Cast<ABaseRPGCharacterEnemy>(CurrentEnemy))
		{
			Enemy->GetAllAlivePlayers(PlayerActors);
		}
	}
	
	
	
}

void ACombatCamera::CameraTranslationTimelineValue(float val)
{
	FVector NewLocation = FMath::Lerp(StartCameraLoc, EndCameraLoc,val);
	SpringArm->SetWorldLocation(NewLocation);
}

void ACombatCamera::CameraRotationTimelineValue(float val)
{
	FRotator EnemyLookAt = UKismetMathLibrary::FindLookAtRotation(StartCameraRot, EndCameraRot);
	EnemyLookAt.Yaw = UKismetMathLibrary::Lerp(Camera->K2_GetComponentRotation().Yaw, EnemyLookAt.Yaw,val);
	EnemyLookAt.Pitch = UKismetMathLibrary::Lerp(Camera->K2_GetComponentRotation().Pitch, EnemyLookAt.Pitch,val);
	Camera->SetWorldRotation(EnemyLookAt);

}

void ACombatCamera::OnTranslationTimelineFinished()
{
	RotateCamera(FVector{0,0,0},true);
}

