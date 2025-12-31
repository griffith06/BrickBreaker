// BrickBreakerGameMode.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BreakerCoreUnreal.h"
#include "HUDWidget.h"
#include "BrickBreakerPlayerController.h"
#include "BrickBreakerGameMode.generated.h"

UCLASS()
class BRICKBREAKER_API ABrickBreakerGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ABrickBreakerGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;   // ← Tick 함수 선언
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    FBreakerCoreUnreal BreakerCoreUnreal;
    ABrickBreakerPlayerController* PlayerController;

protected:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> GameHUDClass;
private:
    int Frame;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class AStaticMeshActor> BlockClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class AStaticMeshActor> BallClass;

    UHUDWidget* HUDWidget;
};


extern ABrickBreakerGameMode* gBrickBreakerGameMode;