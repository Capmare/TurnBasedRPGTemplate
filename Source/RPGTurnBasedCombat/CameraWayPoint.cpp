// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraWayPoint.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACameraWayPoint::ACameraWayPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraRotation = UKismetMathLibrary::FindLookAtRotation(GetWorldCombatCameraPosition(), CameraLookAt);

}

// Called when the game starts or when spawned
void ACameraWayPoint::BeginPlay()
{
	Super::BeginPlay();
	
	CameraRotation = UKismetMathLibrary::FindLookAtRotation(GetWorldCombatCameraPosition(), CameraLookAt);
}

// Called every frame
void ACameraWayPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

