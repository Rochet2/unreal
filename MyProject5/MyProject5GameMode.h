// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyProject5GameMode.generated.h"

UENUM(BlueprintType)
enum class EBatteryPlayState : uint8
{
    EPlaying,
    EGameOver,
    EWon,
    EUnknown,
};

UCLASS(minimalapi)
class AMyProject5GameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AMyProject5GameMode();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure, Category = "Power")
    float GetPowerToWin() const;

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category = "Power")
    EBatteryPlayState GetCurrentState() const;
    void SetCurrentState(EBatteryPlayState NewState);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
    float DecayRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
    float PowerToWin;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
    TSubclassOf<class UUserWidget> HUDWidgetClass;

    UPROPERTY()
    class UUserWidget* CurrendWidget;

private:
    EBatteryPlayState CurrentState;

    TArray<class ASpawnVolume*> SpawnVolumeActors;

    void HandleNewState(EBatteryPlayState NewState);
};
