#include "HUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "BrickBreakerGameMode.h"
#include "BreakerCoreUnreal.h"

void UHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UHUDWidget::OnRestartClicked);
    }
    if (TrackButton)
    {
        TrackButton->OnClicked.AddDynamic(this, &UHUDWidget::OnTrackClicked);
    }

}

void UHUDWidget::OnRestartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Restart Button Clicked!"));
    if (BrickBreakerGM)
        BrickBreakerGM->BreakerCoreUnreal.InitStage();

}
void UHUDWidget::OnTrackClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Track Button Clicked!"));
    if (BrickBreakerGM)
    {
        BrickBreakerGM->BreakerCoreUnreal.ReadTrack();
        BrickBreakerGM->PlayerController->bTrackOn = true;
    }
}

void UHUDWidget::UpdateScore(int NewScore)
{
    if (ScoreText)
    {
        ScoreText->SetText(FText::AsNumber(NewScore));
    }
}