// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatStartPoint.generated.h"


UENUM(BlueprintType)
enum ECombatantType
{
	Player,
	Enemy
};


UCLASS()
class RPGTURNBASEDCOMBAT_API ACombatStartPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatStartPoint();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECombatantType> CombatantType{};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsTaken{false};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
