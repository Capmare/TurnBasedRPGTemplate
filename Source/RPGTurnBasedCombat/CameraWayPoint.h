// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "CameraWayPoint.generated.h"

UENUM(BlueprintType)
enum class ECameraLocation : uint8
{
	EnemySelection,
	PlayerAttacking,
	EnemyAttacking
};


UCLASS()
class RPGTURNBASEDCOMBAT_API ACameraWayPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraWayPoint();

	UPROPERTY(Category = "Camera", EditAnywhere, Meta = (MakeEditWidget = "true"))
	ECameraLocation CameraLocation;

	UPROPERTY(Category = "Camera", EditAnywhere, Meta = (MakeEditWidget = "true"))
	FVector CombatCameraPosition;

	UPROPERTY(Category = "Camera", EditAnywhere, Meta = (MakeEditWidget = "true"))
	FVector CameraLookAt;

	UPROPERTY(Category = "Camera", BlueprintReadOnly,VisibleAnywhere, Meta = (MakeEditWidget = "true"))
	FRotator CameraRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	FVector GetWorldCombatCameraPosition() const { return UKismetMathLibrary::TransformLocation(GetActorTransform(), CombatCameraPosition); }
};
