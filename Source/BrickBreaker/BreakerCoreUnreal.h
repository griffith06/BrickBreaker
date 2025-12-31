#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BreakerCore.h"
#include "Engine/StaticMeshActor.h"

enum BrickGameState
{
    en_game_title,
    en_stage_ready,
    en_stage_ready_swipe,
    en_stage_ready_go,
    en_stage_move_ball,     //공이 움직이는 상태.
    en_stage_stop_ball,     //공이 멈춘 상태.
};
struct FTrackAction
{
    FBlocks Blocks;
    Vector3f Start, End;
};

struct FBreakerCoreUnreal
{
    UWorld* World;
    FBall Ball;             // 출발 위치 볼.
    FBallEmitter BallEmitter;
    FBlocks Blocks;
    BrickGameState GameState = en_game_title; // 게임 상태
    FCollInfo CDebugInfo;
    AStaticMeshActor* BallActor[_NUM_MAX_BALLS];
    //트랙관련.
    int CurTrack;
    TArray<FTrackAction> Track;
    FVector TrStart,TrEnd;
    void ReadTrack();
    void SetTrack();    //현재 트랙으로 보여준다.

    AStaticMeshActor* DebugActor[2];
 
    void Create(UWorld* world);
	void Destroy();
    void Tick(float delta);

    void Refresh();         //렌더링 refresh
	void InitStage();
    void NextStage();
};
