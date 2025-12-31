#include "BrickBreakerPlayerController.h"
#include "Engine/Engine.h"
#include "BreakerUtilUnreal.h"
#include "EnhancedInputComponent.h"        // UEnhancedInputComponent
#include "EnhancedInputSubsystems.h"       // UEnhancedInputLocalPlayerSubsystem
#include "InputMappingContext.h"           // UInputMappingContext
#include "InputAction.h"                   // UInputAction
#include "InputTriggers.h"                 // ETriggerEvent
#include "BrickBreakerGameMode.h"

ABrickBreakerPlayerController::ABrickBreakerPlayerController()
{
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(TEXT("/Game/imc_default.imc_default"));
    if (IMC.Succeeded()) imc_default = IMC.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> IA_L(TEXT("/Game/ia_left.ia_left"));
    if (IA_L.Succeeded()) ia_left = IA_L.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> IA_R(TEXT("/Game/ia_right.ia_right"));
    if (IA_R.Succeeded()) ia_right = IA_R.Object;

    if(gBrickBreakerGameMode)
        gBrickBreakerGameMode->PlayerController = this;
}
void ABrickBreakerPlayerController::SetupInputComponent()
{
    gBrickBreakerGameMode->PlayerController = this;
    FileLog(1, "ABrickBreakerPlayerController::SetupInputComponent");
    Super::SetupInputComponent();

    // InputComponent가 nullptr일 수 있어서 확인
    check(InputComponent);

    // PC 마우스 클릭 (Project Settings → Input → Action Mapping 에 "Click" 액션 추가 필요)
//    InputComponent->BindAction("Click", IE_Pressed, this, &ABrickBreakerPlayerController::OnClick);
    InputComponent->BindAction("Click", IE_Pressed, this, &ABrickBreakerPlayerController::OnInputDown);
    InputComponent->BindAction("Click", IE_Released, this, &ABrickBreakerPlayerController::OnInputUp);
    bShowMouseCursor = true;              // 마우스 커서 보이기
    bEnableClickEvents = true;            // 마우스 클릭 이벤트 허용 (선택)
    // 모바일 터치
    InputComponent->BindTouch(IE_Pressed, this, &ABrickBreakerPlayerController::OnTouchPressed);
    FileLog(1, "ABrickBreakerPlayerController::SetupInputComponent End");



    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(imc_default, 0); // IMC_Default = 방금 만든 IMC 애셋
    }
    if (UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // 왼쪽 키 누름
        EIComp->BindAction(ia_left, ETriggerEvent::Started, this, &ABrickBreakerPlayerController::OnLeftPressed);
        // 왼쪽 키 뗌
//        EIComp->BindAction(IA_Left, ETriggerEvent::Completed, this, &ABrickBreakerPlayerController::OnLeftReleased);
        // 오른쪽 키 누름
        EIComp->BindAction(ia_right, ETriggerEvent::Started, this, &ABrickBreakerPlayerController::OnRightPressed);
        // 오른쪽 키 뗌
//        EIComp->BindAction(IA_Right, ETriggerEvent::Completed, this, &ABrickBreakerPlayerController::OnRightReleased);
    }
    bTrackOn = false;
}

void ABrickBreakerPlayerController::OnClick()
{
    UE_LOG(LogTemp, Log, TEXT("PC: 마우스 클릭 이벤트 발생"));
}

void ABrickBreakerPlayerController::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
    UE_LOG(LogTemp, Log, TEXT("모바일: 터치 이벤트 발생 - Finger: %d, 위치: %s"), (int32)FingerIndex, *Location.ToString());
}

void ABrickBreakerPlayerController::OnInputDown()
{
    if( BreakerCoreUnreal->GameState == en_stage_ready_swipe)
        bIsDragging = true;
    if( BreakerCoreUnreal->GameState != en_stage_stop_ball)
		return; // 공이 멈춘 상태에서만 드래그 시작

    bIsDragging = true;
    // 현재 입력 위치 저장
    FVector2D ScreenPos;
    GetMousePosition(ScreenPos.X, ScreenPos.Y);  // PC
    // 모바일은 아래 API
    // GetInputTouchState(0, ScreenPos.X, ScreenPos.Y, bIsCurrentlyPressed);

    DragStart = ScreenPos;
    BreakerCoreUnreal->GameState = en_stage_ready_swipe; // 드래그 시작 상태로 변경
}

void ABrickBreakerPlayerController::OnInputUp()
{
    if (!bIsDragging)
        return;
    bIsDragging = false;

    FVector2D ScreenPos;
    GetMousePosition(ScreenPos.X, ScreenPos.Y);
    if (ScreenPos.Equals(DragStart, 0.01f))
    {
        return;
    }

    FVector2D Dir = ScreenPos - DragStart; // 드래그 벡터

    //  여기서 Dir을 공에 전달해서 발사
    if (!BreakerCoreUnreal)
        return;

    // 월드 좌표 변환
    FVector WorldStart, WorldDir;
    DeprojectScreenPositionToWorld(DragStart.X, DragStart.Y, WorldStart, WorldDir);
    FVector WorldEnd, WorldDir2;
    DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldEnd, WorldDir2);
/*
    //충돌 테스트
    Vector3f n_dir;
    Vector3f start, end;
    CopyVector3fFromFVector(start, WorldStart);
    CopyVector3fFromFVector(end, WorldEnd);
    start[1] = 0; end[1] = 0;
    bool b_c_block = BreakerCoreUnreal->Ball.CollBlocks(end, start, n_dir);
    BreakerCoreUnreal->GameState = en_stage_stop_ball;
    return;
*/

#ifdef    _ONE_BALL
    if (bTrackOn)
    {
        FVector pos;
        //파일에서 읽어서 다시 해본다.
        if (ReadFileLogFromAction(&BreakerCoreUnreal->Blocks, pos, WorldStart, WorldEnd))
        {
            CopyVector3fFromFVector(BreakerCoreUnreal->Ball.Pos, pos);
            //성공시 렌더링 리프레쉬.
            BreakerCoreUnreal->Refresh();
        }
    }
#endif

    FileLog(1, "start %f %f %f  end %f %f %f"
        , WorldStart.X, WorldStart.Y, WorldStart.Z
        , WorldEnd.X, WorldEnd.Y, WorldEnd.Z
    );

    FVector w_dir = WorldEnd - WorldStart;
    w_dir.Normalize();

#ifdef    _ONE_BALL
    CopyVector3fFromFVector(BreakerCoreUnreal->Ball.Dir, w_dir);
#else
    CopyVector3fFromFVector(BreakerCoreUnreal->BallEmitter.Dir, w_dir);
#endif

    BreakerCoreUnreal->GameState = en_stage_ready_go; // 드래그 종료 상태로 변경
}
void ABrickBreakerPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (bIsDragging)
    {
        FVector2D ScreenPos;
        GetMousePosition(ScreenPos.X, ScreenPos.Y);

        // 월드 좌표 변환
        FVector WorldStart, WorldDir;
        DeprojectScreenPositionToWorld(DragStart.X, DragStart.Y, WorldStart, WorldDir);
        FVector WorldEnd, WorldDir2;
        DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldEnd, WorldDir2);

        DrawDebugLine(GetWorld(), WorldStart, WorldEnd, FColor::Green, false, -1, 0, 2.0f);
    }
    FVector test1(-140.293182, 1062.00134606659, 273.660736);
    FVector test2(-177.334732, 1062.00134768558, 257.288696);
    DrawDebugLine(GetWorld(), test1, test2, FColor::Red, false, -1, 0, 2.0f);
}


void ABrickBreakerPlayerController::OnLeftPressed(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("Left Key Down"));
    if (BreakerCoreUnreal->CurTrack > 0)
    {
        BreakerCoreUnreal->CurTrack--;
        BreakerCoreUnreal->SetTrack();
    }
}

void ABrickBreakerPlayerController::OnRightPressed(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("Right Key Down"));
	BreakerCoreUnreal->CurTrack++; // 다음 스테이지로 이동
    BreakerCoreUnreal->SetTrack();
}
