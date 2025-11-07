// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Combat.h"
#include "Components/TimelineComponent.h"

#include "CombatCamera.generated.h"

class ACameraWayPoint;


USTRUCT(BlueprintType)
struct FEndGameCondition 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Condition", VisibleAnywhere, BlueprintReadWrite)
	bool bPlayerWon;
	UPROPERTY(Category = "Condition", VisibleAnywhere, BlueprintReadWrite)
	bool bGameEnded;
};

UCLASS()
class RPGTURNBASEDCOMBAT_API ACombatCamera : public APawn, public ICameraActions
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACombatCamera();

	UFUNCTION(BlueprintCallable)
	class ABaseRPGCharacter* GetCurrentPlayer() const { return CurrentPlayer; }
	UFUNCTION(BlueprintCallable)
	class ABaseRPGCharacter* GetCurrentEnemy() const { return CurrentEnemy; }

	int GetEnemyActorsNum() const { return EnemyActors.Num(); }
	int GetPlayerActorsNum() const { return PlayerActors.Num(); }
	TArray<class ABaseRPGCharacter*> GetEnemyActors() const { return EnemyActors; }
	TArray<class ABaseRPGCharacter*> GetPlayerActors() const { return PlayerActors; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(Category = "Camera", VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;
	UPROPERTY(Category = "Camera", VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* Camera;

	ACameraWayPoint* GetEnemySelectionWaypoint() const { return EnemySelectionWaypoint; }
	ACameraWayPoint* GetEnemyAttackingWaypoint() const { return EnemyAttackingWaypoint; }
	ACameraWayPoint* GetPlayerAttackingWaypoint() const { return PlayerAttackingWaypoint; }

	void NextEnemy();

	virtual void RotateCameraToCurrentEnemy() override;

	TArray<ABaseRPGCharacter*> ListOfIgnoredActors;
	void UpdatePlayersAndEnemies(bool bShouldRotate = true);

	UPROPERTY(Category = "GAME_END", VisibleAnywhere, BlueprintReadWrite)
	FEndGameCondition EndGame;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameEndCheck();

	void CheckGameEnded();

	void MoveCameraToEnemySpectate();

	virtual void MoveToNextCamera() override;
	virtual void RotateCameraToNextEnemy(bool bIsInverted = false) override;
	virtual void MoveCameraToLocationWithRotation(const FVector& NewLocation, const FVector& NewRotation) override;
	virtual void MoveCameraToWidget() override;
private:


	UFUNCTION()
	void CameraTranslationTimelineValue(float val);
	UFUNCTION()
	void CameraRotationTimelineValue(float val);
	UFUNCTION()
	void OnTranslationTimelineFinished();

	void RotateCamera(const FVector& NewRotation, bool bRotatesToWidget = false);

	UPROPERTY(Category = "Camera", EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* TimelineCurve;

	UPROPERTY(Category = "Actors", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<class ABaseRPGCharacter*> PlayerActors;
	UPROPERTY(Category = "Actors", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<class ABaseRPGCharacter*> EnemyActors;


	class ABaseRPGCharacter* CurrentPlayer{};
	class ABaseRPGCharacter* CurrentEnemy{};

	ACameraWayPoint* EnemySelectionWaypoint{};
	ACameraWayPoint* EnemyAttackingWaypoint{};
	ACameraWayPoint* PlayerAttackingWaypoint{};

	FTimeline CameraTranslationCurveFTimeline;
	FTimeline CameraRotationCurveFTimeline;

	FVector StartCameraLoc{};
	FVector EndCameraLoc{};

	FVector StartCameraRot{};
	FVector EndCameraRot{};

	FVector WidgetRotation{};

	FOnTimelineEvent TimelineFinishedEvent;

	int CurrentCameraPosition{};
	int CurrentCameraRotation{1}; // start with index one so it avoids rotating the camera once moving to select enemy location
	int CurrentEnemyIndex{};
};
