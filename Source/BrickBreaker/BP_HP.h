// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BP_HP.generated.h"

/**
 * 
 */
UCLASS()
class BRICKBREAKER_API UBP_HP : public UUserWidget
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void SetHP(int NewHP);
    float GetHalfTextSizeY();

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HPText;   // BP에서 이름이 HPText인 TextBlock 자동 바인딩
};
