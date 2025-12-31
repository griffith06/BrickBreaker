#pragma once

#include	"Common/CVectorVer2.h"
#include	"Common/Jmalloc.h"
#define _BLOCK_ROWS		 9
#define _BLOCK_COLS		 6

#define _BLOCK_WIDTH		 150.0f // Unreal 단위 1.5미터
#define _BLOCK_HEIGHT		 100.0f

#define _SCALE_BLOCK(a)         ((a - a*0.08f) * 0.01f)
#define _SCALE_BACK_BLOCK(a,b)    ((a*b) * 0.01f)
#define _BALL_RADIOUS   0.4f//0.2f 

#define _MAX_NUM_COLL		(_BLOCK_ROWS*_BLOCK_COLS + 4 + 16)  //4벽, 16여분.

enum ActorFigure 
{
	en_wall,
	en_block,
	en_ball,
};

enum BlockState
{
	en_block_hide,
	en_block_normal,
	en_block_hit,
	en_block_die,
};

struct FCollInfoEl
{
	bool IsBlock;		//block? false면 벽.
	int Where;			//0윗, 1 아래, 2 오른쪽, 3 왼쪽.
	int i, j;			//블록 위치
};

struct FCollInfo
{
	union 
	{
		Vector3f CollPos;	//충돌위치
		Vector3f CurPos;	//현재위치.
	};
	Vector3f Dir;		//현재-> 다음 방향.
	Vector3f FinalDir;	//충돌후 틀어질 방향 또는 미 충돌시 다음방향.
	Vector3f FinalPos;	//최종 위치, 만약 충돌시 꺽어진 위치
	float Dist;			// 현재 위치에서 충돌 거리.

	int Num;			//충돌 전체 개수 [0]배열에만 있다.
	FCollInfoEl El[3];	//충돌 정보. 최대 3개.
	FCollInfo()
	{
		memset(this, 0, sizeof(FCollInfo));
	}
	void Start()	//바닥에서 출발.
	{
		Num=1;
		El[0].Where = 3;
		El[0].IsBlock = false;
	}
	bool IsSKipBlock(int i, int j)
	{
		int m;
		for (m = 0; m < Num; m++)
		{
			if (El[m].IsBlock && El[m].i == i && El[m].j == j)
				return true;
		}
		return false;
	}
};

struct FObjectCommon
{
	BlockState State; 
	int Hp;
	Vector3f Pos;
	void* Actor;
	void* Widget;
	bool CheckWall(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos, FCollInfo* r_info);
};

struct FBlock : public FObjectCommon
{
	int Flag;		//0 , 1  충돌.
	Vector3f Dir;	//충돌이후 방향.
	void CopyBlock(FBlock *block);
	bool CollBlock(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos);
	bool OverlapTest(FCollInfo* c_info, Vector3f cur_pos);
	void Clear()	//스테이지 시작전 한번씩 클리어.
	{
		Flag = 0;
	}
};

struct FBlocks
{
	int Stage;
	FBlock Block[_BLOCK_ROWS][_BLOCK_COLS];

	void Clear();
	void NextStage();
	void Tick(float delta);
	void ShiftBlocks();
	void Create();
	void CopyFrom(FBlocks* blocks);
};
struct FBall : public FObjectCommon
{
	bool bStart;
	bool bPlaying;
	FBlocks* Blocks;
	Vector3f Dir;    //normalized direction vector

	bool CollBlocks(FCollInfo* c_info, Vector3f n_pos, Vector3f cur_pos, bool b_sub_hp, FCollInfo* r_info);
	bool OutWall(FCollInfo* c_info, Vector3f cur_pos);
	bool CheckCollision(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos, int depth);
};
//최대 볼 개수.
#define _NUM_MAX_BALLS	100

enum BallEmitterState
{
	en_ball_state_stop,
	en_ball_state_emitting,
};
struct FBallEmitter
{
	bool bFirstStop;
	Vector3f Dir, Pos, StartPos;
	int Stage;
	BallEmitterState State;
	int Frame;  //프레임 카운터
	DAlloc Ball;
	void* Widget;
	void NextStage();
	void PreTick();
	bool Tick(float delta);
};
