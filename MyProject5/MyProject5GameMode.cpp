// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyProject5GameMode.h"
#include "MyProject5Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GamePlayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolume.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"

AMyProject5GameMode::AMyProject5GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

    DecayRate = 0.01f;
}

void AMyProject5GameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AMyProject5Character* MyCharacter = Cast<AMyProject5Character>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (MyCharacter)
    {
        if (MyCharacter->GetCurrentPower() >= PowerToWin)
            SetCurrentState(EBatteryPlayState::EWon);
        else if (MyCharacter->GetCurrentPower() > 0)
        {
            MyCharacter->UpdatePower(-DeltaTime*DecayRate*MyCharacter->GetInitialPower());
        }
        else
            SetCurrentState(EBatteryPlayState::EGameOver);
    }
}

float AMyProject5GameMode::GetPowerToWin() const
{
    return PowerToWin;
}

void AMyProject5GameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
        if (SpawnVolumeActor)
        {
            SpawnVolumeActors.AddUnique(SpawnVolumeActor);
        }
    }

    SetCurrentState(EBatteryPlayState::EPlaying);

    AMyProject5Character* MyCharacter = Cast<AMyProject5Character>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (MyCharacter)
    {
        PowerToWin = MyCharacter->GetInitialPower()*1.25f;
    }

    if (HUDWidgetClass)
    {
        CurrendWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (CurrendWidget)
        {
            CurrendWidget->AddToViewport();
        }
    }
}

EBatteryPlayState AMyProject5GameMode::GetCurrentState() const
{
    return CurrentState;
}

void AMyProject5GameMode::SetCurrentState(EBatteryPlayState NewState)
{
    CurrentState = NewState;
    HandleNewState(CurrentState);
}

void AMyProject5GameMode::HandleNewState(EBatteryPlayState NewState)
{
    switch (NewState)
    {
        case EBatteryPlayState::EPlaying:
            for (auto * Volume : SpawnVolumeActors)
                Volume->SetSpawningActive(true);
            break;
        case EBatteryPlayState::EWon:
            for (auto * Volume : SpawnVolumeActors)
                Volume->SetSpawningActive(false);
            break;
        case EBatteryPlayState::EGameOver:
            for (auto * Volume : SpawnVolumeActors)
                Volume->SetSpawningActive(false);
            if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
                PlayerController->SetCinematicMode(true, false, false, true, true);
            if (ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0))
            {
                MyCharacter->GetMesh()->SetSimulatePhysics(true);
                MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
            }
            break;
        case EBatteryPlayState::EUnknown:
            break;
    }
}
