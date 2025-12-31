#include "BreakerCoreUnreal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/Paths.h"
#include "Components/WidgetComponent.h"
#include "BP_HP.h"
#include "BreakerUtilUnreal.h"

static AStaticMeshActor* GetBreakerActor(ActorFigure attr, UWorld* world, FVector pos)
{
    AStaticMeshActor* actor = world->SpawnActor<AStaticMeshActor>(pos, FRotator::ZeroRotator);
    FString str;
    switch (attr)
    {
    case en_wall:
    case en_block:
        str = TEXT("/Engine/BasicShapes/Cube.Cube");
        actor->GetStaticMeshComponent()->SetWorldScale3D(FVector(_SCALE_BLOCK(_BLOCK_WIDTH), _SCALE_BLOCK(_BLOCK_HEIGHT), _SCALE_BLOCK(_BLOCK_HEIGHT))); // 크기 조정
        break;
    case en_ball:
        str = TEXT("/Engine/BasicShapes/Sphere.Sphere");
        actor->GetStaticMeshComponent()->SetWorldScale3D(FVector(_BALL_RADIOUS));
        break;

    }
    actor->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *str));
    actor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

    switch (attr)
    {
    case en_wall:
        actor->GetStaticMeshComponent()->SetWorldScale3D(FVector(_SCALE_BACK_BLOCK(_BLOCK_WIDTH, _BLOCK_COLS), 0.001f, _SCALE_BACK_BLOCK(_BLOCK_HEIGHT, _BLOCK_ROWS))); // 크기 조정
        actor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
        break;
    case en_block:
        actor->GetStaticMeshComponent()->SetWorldScale3D(FVector(_SCALE_BLOCK(_BLOCK_WIDTH), _SCALE_BLOCK(_BLOCK_HEIGHT), _SCALE_BLOCK(_BLOCK_HEIGHT))); // 크기 조정
        break;
    case en_ball:
        actor->GetStaticMeshComponent()->SetWorldScale3D(FVector(_BALL_RADIOUS));
        break;
    }
    actor->SetActorEnableCollision(false); // (선택) 충돌도 꺼줌
    actor->SetActorTickEnabled(false);                  // Tick 끄기 (내부 Tick 없음)
    actor->SetReplicates(false);                        // 네트워크 리플리케이션 끄기
    actor->PrimaryActorTick.bCanEverTick = false;       // tick 끄기
    actor->DisableComponentsSimulatePhysics();          // 물리 시뮬레이션 꺼짐
    actor->SetCanBeDamaged(false);                      // 데미지 처리 X


    UStaticMeshComponent* Mesh = actor->GetStaticMeshComponent();
    Mesh->SetSimulatePhysics(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetGenerateOverlapEvents(false);
//    Mesh->SetCastShadow(false);                        // 그림자 계산 X
    Mesh->SetVisibility(true);                         // 단순 렌더만
    Mesh->SetComponentTickEnabled(false);              // Tick 제거
    return actor;
}

void FBreakerCoreUnreal::Create(UWorld* world)
{
	World = world;

    Blocks.Create();

    /*
    // 배경은 Plane으로
    auto BG = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    UStaticMeshComponent* Mesh = BG->GetStaticMeshComponent();
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane")));
    Mesh->SetWorldScale3D(FVector(10.0f, 10.0f, 1.0f));
    Mesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 90.0f));
    Mesh->SetMaterial(0, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Background.M_Background")));
    */
    //스테이지 백판.
    AStaticMeshActor* Mesh = GetBreakerActor(en_wall, world, FVector::ZeroVector);

    //블록 생성.
    FVector StartPos(-(_BLOCK_COLS - 1) * _BLOCK_WIDTH * 0.5f, 0, _BLOCK_HEIGHT * (_BLOCK_ROWS - 1) * 0.5f);

    for (int Row = 0; Row < _BLOCK_ROWS; ++Row)    //세로
    {
        for (int Col = 0; Col < _BLOCK_COLS; ++Col) //가로
        {
            FVector Pos = StartPos + FVector(Col * _BLOCK_WIDTH, 0, -Row * _BLOCK_HEIGHT);
            AStaticMeshActor* Block = GetBreakerActor(en_block, world, Pos);
            Blocks.Block[Row][Col].Actor = (void*)Block;
            
			//위젯 컴포넌트 추가
            UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(Block);
            WidgetComp->SetupAttachment(Block->GetRootComponent());
            WidgetComp->RegisterComponent();

            // 위젯 설정
            TSubclassOf<UUserWidget> WidgetBPClass = LoadClass<UUserWidget>(
                nullptr,
                TEXT("/Game/bp_hp.bp_hp_C")
            );
            
            if( WidgetBPClass)
                WidgetComp->SetWidgetClass(WidgetBPClass);
            WidgetComp->SetDrawSize(FVector2D(300.f, 100.f));
//            WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
            WidgetComp->SetWidgetSpace(EWidgetSpace::World);
            WidgetComp->SetRelativeRotation(FRotator(0, 90, 0)); // 뒷면 보정
            WidgetComp->SetCastShadow(false);         // 그림자 투영 끔
            WidgetComp->bReceivesDecals = false;      // 데칼 영향 제거
            WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
            UBP_HP* HPWidget = Cast<UBP_HP>(WidgetComp->GetUserWidgetObject());
            if (HPWidget)
            {
                Blocks.Block[Row][Col].Widget = HPWidget;
                WidgetComp->SetRelativeLocation(FVector(0, 60, -HPWidget->GetHalfTextSizeY()));
                HPWidget->SetHP(Blocks.Block[Row][Col].Hp);
            }
        }
    }
#ifdef    _ONE_BALL
    //볼 생성.
    FVector BallPos(0, 0, 0);
    AStaticMeshActor* actor = GetBreakerActor(en_ball, world, FVector::ZeroVector);
    Ball.Actor = actor;
    Ball.Blocks = &Blocks;

    FVector DebugBallPos0(0, -300.000000, 0);
    actor = GetBreakerActor(en_ball, world, DebugBallPos0);
    DebugActor[0] = actor;

    FVector DebugBallPos1(0, -300.000000, 0);
    actor = GetBreakerActor(en_ball, world, DebugBallPos1);
    DebugActor[1] = actor;
#else
    AStaticMeshActor* actor = GetBreakerActor(en_ball, world, FVector::ZeroVector);
    Ball.Actor = actor;
    //최대 볼 actor  미리 생성.
    FVector BallPos(-1000, 0, 0);
    for (int i = 0; i < _NUM_MAX_BALLS; i++)
    {
        BallActor[i] = GetBreakerActor(en_ball, world, BallPos);
    }
#endif

}

void FBreakerCoreUnreal::Destroy()
{

}

void FBreakerCoreUnreal::InitStage()
{
    GameState = en_stage_stop_ball;
    
    //볼 초기 위치.
    FVector BallPos(0, 0, -_BLOCK_ROWS * _BLOCK_HEIGHT * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f);
    CopyVector3fFromFVector(Ball.Pos, BallPos);
    _Vector3fCopy(BallEmitter.StartPos, Ball.Pos);
    _Vector3fCopy(BallEmitter.Pos, Ball.Pos);
#ifdef    _ONE_BALL
    Ball.Hp = 1;
#else
    BallEmitter.Stage = 0;
    BallEmitter.Ball.Cnt = 0;
//    BallEmitter.NextStage();
#endif

    Blocks.Clear();

    srand(1);
    NextStage();
}
void FBreakerCoreUnreal::Refresh()         //렌더링 refresh
{

    for (int Row = 0; Row < _BLOCK_ROWS; ++Row)    //세로
    {
        for (int Col = 0; Col < _BLOCK_COLS; ++Col) //가로
        {
            AStaticMeshActor* actor = (AStaticMeshActor*)Blocks.Block[Row][Col].Actor;
            if (actor == nullptr)
                continue; // Actor가 nullptr인 경우는 무시
            if (Blocks.Block[Row][Col].State == en_block_hide)
            {
                actor->SetActorHiddenInGame(true);   // 사라지게.
            }
            else
            {
                actor->SetActorHiddenInGame(false);   // 나타나게.
                UBP_HP* HPWidget = (UBP_HP*)Blocks.Block[Row][Col].Widget;
                if (HPWidget)
                    HPWidget->SetHP(Blocks.Block[Row][Col].Hp);
            }
        }
    }
}
void FBreakerCoreUnreal::NextStage()
{
    //stage 생성.
    Blocks.NextStage();

    BallEmitter.NextStage();
    //렌더링 리프레쉬.
    Refresh();
}

TRACE_CPUPROFILER_EVENT_SCOPE(BreakerCoreTick);

#define _MIN_FRAME_TIME	(1.0f/20.0f)    //최소 프레임 시간
#define _MOVING_SCALE	2000.0f         //이동 속도


//#define _ONE_BALL

void FBreakerCoreUnreal::Tick(float delta)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(BreakerCoreTick);
    bool b_start = false;
    if (GameState == en_stage_ready_go)
    {
        GameState = en_stage_move_ball;
        b_start = true;
    }
 #ifdef    _ONE_BALL
    if( GameState == en_stage_move_ball)
    {
        if (delta > _MIN_FRAME_TIME)
            delta = _MIN_FRAME_TIME;
        Vector3f n_pos, cur_pos;
        _Vector3fMul(n_pos, Ball.Dir, delta * _MOVING_SCALE);
        _Vector3fAdd(n_pos, n_pos, Ball.Pos);
        _Vector3fCopy(cur_pos, Ball.Pos);

        WriteFileLogFromAction(&Blocks, cur_pos, n_pos);
        
        FCollInfo c_info;
        if(b_start)
            c_info.Start();
		bool b_stop = Ball.CheckCollision(&c_info, cur_pos, n_pos, 0);
        _Vector3fCopy(Ball.Dir, c_info.FinalDir);
        _Vector3fCopy(Ball.Pos, c_info.FinalPos);
        if (b_stop)
        {
            GameState = en_stage_stop_ball;
            NextStage();
        }
	}
#else
    if (GameState == en_stage_move_ball)
    {
        BallEmitter.PreTick();
        //구조가 맘에 안든다.
        for (int i = 0; i < BallEmitter.Ball.Cnt; i++)
        {
            FBall* ball = (FBall*)BallEmitter.Ball.GetPtr(i);
            ball->Actor = BallActor[i];
            ball->Blocks = &Blocks;
        }
        if (delta > _MIN_FRAME_TIME)
            delta = _MIN_FRAME_TIME;
        bool b_ing = BallEmitter.Tick(delta);
        for (int i = 0; i < BallEmitter.Ball.Cnt; i++)
        {
            FBall* ball = (FBall*)BallEmitter.Ball.GetPtr(i);
            AStaticMeshActor* actor = (AStaticMeshActor*)ball->Actor;
            if (ball->State == en_block_normal)
                actor->SetActorHiddenInGame(false);
            else
                actor->SetActorHiddenInGame(true);   // 사라지게.
        }
        if (!b_ing)
        {
            GameState = en_stage_stop_ball;
            NextStage();
        }
    }

#endif
    int i, j;
	//블럭 상태 갱신.
    for (i = 0; i < _BLOCK_ROWS; i++)
    {
        for (j = 0; j < _BLOCK_COLS; j++)
        {
            FBlock* block = &Blocks.Block[i][j];
            if (block->State == en_block_hit && block->Actor)
            {
                if (block->Hp <= 0)
                {
                    block->State = en_block_hide;
                    AStaticMeshActor* actor = (AStaticMeshActor*)block->Actor;
                    actor->SetActorHiddenInGame(true);   // 사라지게.
                }
                else
                {
                    block->State = en_block_normal;
                }
                UBP_HP* HPWidget = (UBP_HP*)block->Widget;
                if (HPWidget)
                    HPWidget->SetHP(block->Hp);
            }
        }
    }
    
#ifdef    _ONE_BALL
    AStaticMeshActor* actor = (AStaticMeshActor*)Ball.Actor;
    if (actor)
    {
        //actor->SetActorLocation(FVector(Ball.Pos[0], Ball.Pos[1], Ball.Pos[2]));
        actor->GetRootComponent()->SetWorldLocation(FVector(Ball.Pos[0], Ball.Pos[1], Ball.Pos[2]), false, nullptr, ETeleportType::TeleportPhysics);
    }
#else
    AStaticMeshActor* actor = (AStaticMeshActor*)Ball.Actor;
    FVector pos = { BallEmitter.StartPos[0],BallEmitter.StartPos[1], BallEmitter.StartPos[2] };
    actor->GetRootComponent()->SetWorldLocation(pos, false, nullptr, ETeleportType::TeleportPhysics);
    for (i = 0; i < BallEmitter.Ball.Cnt; i++)
    {
        FBall* ball = (FBall*)BallEmitter.Ball.GetPtr(i);
        actor = (AStaticMeshActor*)ball->Actor;
        actor->GetRootComponent()->SetWorldLocation(FVector(ball->Pos[0], ball->Pos[1], ball->Pos[2]), false, nullptr, ETeleportType::TeleportPhysics);
    }
#endif
}


void FBreakerCoreUnreal::ReadTrack()
{
    CurTrack = 0;
    Track.Empty();
    FVector pos;
    ReadAllTrackFromAction(Track, pos, TrStart, TrEnd);
}

void FBreakerCoreUnreal::SetTrack()
{
    if (Track.IsEmpty())
        return;
    if(Track.Num())
        CurTrack %= Track.Num();
    if (CurTrack < 0 && CurTrack >= Track.Num())
        return;

    Blocks.CopyFrom(&Track[CurTrack].Blocks);
    FVector t;
    CopyFVectorFromVector3f(t, Track[CurTrack].Start);
    t.Y = 50;
    DebugActor[0]->SetActorLocation(t);
    CopyFVectorFromVector3f(t, Track[CurTrack].End);
    t.Y = 50;
    DebugActor[1]->SetActorLocation(t);

    Refresh();
    if (CurTrack == 0)
        CDebugInfo.Start();
#ifdef    _ONE_BALL
    Vector3f n_pos, cur_pos;
    _Vector3fCopy(cur_pos, Track[CurTrack].Start);
    _Vector3fCopy(n_pos, Track[CurTrack].End);
    Ball.CheckCollision(&CDebugInfo, cur_pos, n_pos, 0);
#endif

}
