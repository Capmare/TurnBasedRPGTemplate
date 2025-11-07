// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatStartPoint.h"

// Sets default values
ACombatStartPoint::ACombatStartPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACombatStartPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombatStartPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

