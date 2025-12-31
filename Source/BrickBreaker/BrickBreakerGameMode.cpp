// BrickBreakerGameMode.cpp
#include "BrickBreakerGameMode.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "BrickBreakerPlayerController.h"
#include "BreakerCoreUnreal.h" 
#include "Blueprint/UserWidget.h"
#include "BreakerUtilUnreal.h"


ABrickBreakerGameMode* gBrickBreakerGameMode;
ABrickBreakerGameMode::ABrickBreakerGameMode()
{
    gBrickBreakerGameMode = this;
    // 블록 Mesh 기본값 (큐브)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화
    PlayerControllerClass = ABrickBreakerPlayerController::StaticClass();
    DefaultPawnClass = nullptr;     //뜬금없이 sphere생성된거 안보이게

    static ConstructorHelpers::FClassFinder<UUserWidget> HudWidgetClass(TEXT("/Game/bp_hud.bp_hud_C"));
    if (HudWidgetClass.Succeeded())
    {
        GameHUDClass = HudWidgetClass.Class;
    }

    Frame = 0;
}
static bool b_first = false;
void ABrickBreakerGameMode::BeginPlay()
{
    if (b_first == false)
    {
        b_first = true;
        FileLog(0, "ABrickBreakerGameMode Create");
    }
    FileLog(1, "ABrickBreakerGameMode BeginPlay");
    Super::BeginPlay();

    // 위젯 설정
    GameHUDClass = LoadClass<UUserWidget>(
        nullptr,
        TEXT("/Game/bp_hud.bp_hud_C")
    );
    if (GameHUDClass)
    {
        UUserWidget* GameHUD = CreateWidget<UUserWidget>(GetWorld(), GameHUDClass);
        if (GameHUD)
        {
            GameHUD->AddToViewport();  // 화면에 추가
            HUDWidget = Cast<UHUDWidget>(GameHUD);
            HUDWidget->BrickBreakerGM = this;
        }
    }
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MainCamera"), FoundCameras);

    if (FoundCameras.Num() > 0)
    {
        ACameraActor* CamActor = Cast<ACameraActor>(FoundCameras[0]);
        if (CamActor)
        {
            // 플레이어 컨트롤러 가져와서 뷰 타겟 변경
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            if (PC)
            {
                PC->SetViewTarget(CamActor);
				ABrickBreakerPlayerController* BrickPC = Cast<ABrickBreakerPlayerController>(PC);
				BrickPC->BreakerCoreUnreal = &BreakerCoreUnreal; // BreakerCoreUnreal 포인터 설정
            }
        }
    }
    FileLog(1, "ABrickBreakerGameMode BeginPlay InitStage()");
	BreakerCoreUnreal.Create(GetWorld());
	BreakerCoreUnreal.InitStage();
    FileLog(1, "ABrickBreakerGameMode BeginPlay End");
}


void ABrickBreakerGameMode::Tick(float DeltaSeconds)
{
    if( Frame == 0)
        FileLog(1, "ABrickBreakerGameMode::Tick  first");
    Super::Tick(DeltaSeconds);

    // 매 프레임마다 실행되는 로직
 //   UE_LOG(LogTemp, Log, TEXT("Tick: DeltaSeconds = %f"), DeltaSeconds);

    BreakerCoreUnreal.Tick(DeltaSeconds);

    if (Frame == 0)
        FileLog(1, "ABrickBreakerGameMode::Tick  first end");
    Frame++;
}

void ABrickBreakerGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    FileLog(2, "ABrickBreakerGameMode::EndPlay");
}