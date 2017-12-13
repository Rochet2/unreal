// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Engine/World.h"
#include "Pickup.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
    RootComponent = WhereToSpawn;

    SpawnDelayRangeLow = 1.0f;
    SpawnDelayRangeHigh = 4.5f;
}

FVector ASpawnVolume::GetRandomPointInVolume()
{
    FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
    FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;

    return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SetSpawningActive(bool bShouldSpawn)
{
    if (bShouldSpawn)
    {
        SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
        GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
    }
    else
    {
        GetWorldTimerManager().ClearTimer(SpawnTimer);
    }
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnVolume::SpawnPickup()
{
    if (!WhatToSpawn)
        return;
    if (UWorld* const World = GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;

        FVector SpawnLocation = GetRandomPointInVolume();

        FRotator SpawnRotation;
        SpawnRotation.Yaw = FMath::FRand() * 360.0f;
        SpawnRotation.Pitch = FMath::FRand() * 360.0f;
        SpawnRotation.Roll = FMath::FRand() * 360.0f;

        APickup* const SpawnedPickup = World->SpawnActor<APickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

        SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
        GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
    }
}

