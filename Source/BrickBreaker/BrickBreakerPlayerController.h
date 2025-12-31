#pragma once

#include "CoreMinimal.h"
#include "BreakerCoreUnreal.h"
#include "GameFramework/PlayerController.h"
#include "BrickBreakerPlayerController.generated.h"

UCLASS()
class BRICKBREAKER_API ABrickBreakerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ABrickBreakerPlayerController();
    virtual void SetupInputComponent() override;
    FBreakerCoreUnreal* BreakerCoreUnreal;
    bool bTrackOn = false;
private:
    virtual void PlayerTick(float DeltaTime) override;
    bool bIsDragging;
    FVector2D DragStart;
    // PC 클릭
    void OnClick();
    void OnInputDown();
    void OnInputUp();

    // 모바일 터치
    void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
    UFUNCTION()
    void OnLeftPressed(const FInputActionValue& Value);

    UFUNCTION()
    void OnRightPressed(const FInputActionValue& Value);
    // Input Mapping Context, Actions도 보관
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputMappingContext* imc_default;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* ia_left;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* ia_right;
};
