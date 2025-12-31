// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_HP.h"
#include "Components/TextBlock.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"

void UBP_HP::SetHP(int NewHP)
{
    if (HPText)
    {
        HPText->SetText(FText::AsNumber(NewHP));
    }
}
float UBP_HP::GetHalfTextSizeY()
{
    if (!HPText)
        return 0;
    // 폰트 정보 가져오기
    FSlateFontInfo FontInfo = HPText->GetFont();
    FString TextString = HPText->GetText().ToString();

    // 전역 FontMeasure 서비스 얻기
    TSharedPtr<FSlateFontMeasure> FontMeasureService =
        FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    // 텍스트 크기 측정
    FVector2D TextSize = FontMeasureService->Measure(TextString, FontInfo);
    float OffsetY = TextSize.Y * 0.6f;
    FVector2D Size = HPText->GetDesiredSize();
    return OffsetY;
}