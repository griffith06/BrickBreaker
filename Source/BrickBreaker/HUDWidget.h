#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 *
 */
UCLASS()
class BRICKBREAKER_API UHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    class ABrickBreakerGameMode* BrickBreakerGM;

    UPROPERTY(meta = (BindWidget))
    class UButton* RestartButton;
    UPROPERTY(meta = (BindWidget))
    class UButton* TrackButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ScoreText;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnRestartClicked();
    UFUNCTION()
    void OnTrackClicked();
    
    void UpdateScore(int NewScore);

};
