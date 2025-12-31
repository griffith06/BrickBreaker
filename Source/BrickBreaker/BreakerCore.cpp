#include "BreakerCore.h"
#include "Common/BreakerUtil.h"

#define coll_epsilon 0.0001f
#define _MOVING_SCALE	2000.0f

enum WhereColl
{
	en_wc_up,
	en_wc_right,
	en_wc_left,
	en_wc_down,
};

int Gf_GetPlaneBound(Vector3f b, Vector4f f_normal)
{
	float back = f_normal[0] * b[0] + f_normal[1] * b[1] + f_normal[2] * b[2] - f_normal[3];

	if (back < -coll_epsilon)
	{
		return 1;		//평면 뒤
	}
	return 0;
}


int Gf_GetPlaneCrossPoint(Vector3f a, Vector3f b, Vector3f cross, Vector4f f_normal)
{
	float front = f_normal[0] * a[0] + f_normal[1] * a[1] + f_normal[2] * a[2] - f_normal[3];
	float back = f_normal[0] * b[0] + f_normal[1] * b[1] + f_normal[2] * b[2] - f_normal[3];

	if ((front > 0 && back > 0) || (front < 0 && back < 0) || (front == 0 && back == 0))
		return 0;

	float dist = front / (front - back);

	cross[0] = a[0] + dist * (b[0] - a[0]);
	cross[1] = a[1] + dist * (b[1] - a[1]);
	cross[2] = a[2] + dist * (b[2] - a[2]);

	if (front <= 0)	//앞으로 부딪힌경우.
		return 1;
	else
		return -1;		//뒤로 부딪힌경우.
}

#define	_CHECK_OPTION_ONE_SIDE			1
#define	_CHECK_OPTION_SKIP_BOUND		2


int CheckCollision(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos
	, Vector3f o_pos, Vector4f* normals, float b_w, float b_h, bool b_block, FCollInfo* r_info, int option = 0)
{
	/*
	재미나이 왈 0.01f는 물리 충돌에서 꽤 큰 값입니다. (Unreal 단위가 cm라면 0.01cm지만, 내부 로직 스케일이 작거나 속도가 빠르면 무시될 수 있음). 
	만약 남은 이동 거리가 0.009f라면 충돌 검사를 스킵해버리고, 공은 벽돌 바로 앞 혹은 살짝 겹친 상태로 멈춥니다. 
	다음 프레임에서 이 상태가 "내부 시작"으로 이어질 수 있습니다.
	*/
	if (_Vector3fAlmostCmp(cur_pos, n_pos, 0.0001f))	//0.0001f로 수정함.
	{
		_Vector3fCopy(c_info->CollPos, cur_pos);
		return -1;
	}
	_Vector3fSub(c_info->Dir, n_pos, cur_pos);
	_Vector3fNormalize(c_info->Dir);
	_Vector3fCopy(c_info->FinalDir, c_info->Dir);

	Vector3f cross;
	float dist = 100000.0f;
	int short_i = -1;
	//충돌여부.	
	for (int i = 0; i < 4; i++)
	{
		if(!b_block && r_info->Num && r_info->El[0].Where == i)
			continue;	//출발점에서 충돌할수 있어서 무시.

		bool b_cross_plane = false;
		int ret_cross = Gf_GetPlaneCrossPoint(cur_pos, n_pos, cross, normals[i]);
		if (option & _CHECK_OPTION_ONE_SIDE)	//단면체크
		{
			if (ret_cross == -1) //back만 인정
				b_cross_plane = true;
		}
		else if(ret_cross)// 양면체크
		{
			b_cross_plane = true;
		}
		if (b_cross_plane)
		{
			bool b_coll = false;
			if (option & _CHECK_OPTION_SKIP_BOUND)
			{
				b_coll = true;
			}
			else
			{
				if (i == en_wc_up || i == en_wc_down)
				{
					if (cross[0] > o_pos[0] - b_w && cross[0] < o_pos[0] + b_w)
					{
						b_coll = true;
					}
				}
				else
				{
					if (cross[2] > o_pos[2] - b_h && cross[2] < o_pos[2] + b_h)
					{
						b_coll = true;
					}
				}
			}
			if (b_coll == false)
				continue;
			float coll_dist = _Vector3fDistAB(cross, cur_pos);
			if (dist > coll_dist)
			{
				dist = coll_dist;
				short_i = i;
				c_info->El[0].Where = i;
				c_info->Dist = dist;
				// 기존 코드
				//_Vector3fCopy(c_info->CollPos, cross);
				// 재미나이 수정 코드: 충돌 지점을 법선 방향으로 아주 조금 밀어냅니다.
				Vector3f push_vec;
				Vector3f plane_normal = { normals[short_i][0], normals[short_i][1], normals[short_i][2] };
				// 평면 밖으로 0.01f 만큼 밀어냄 (값은 상황에 맞춰 조절)
				_Vector3fMul(push_vec, plane_normal, 0.01f);
				_Vector3fAdd(c_info->CollPos, cross, push_vec);
			}
		}
	}
	return short_i;
}

#define _CHECK_HALF_LENGTH_W (_BLOCK_WIDTH * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f)
#define _CHECK_HALF_LENGTH_H (_BLOCK_HEIGHT * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f)


bool FBlock::OverlapTest(FCollInfo* c_info, Vector3f cur_pos)
{
	float w = _CHECK_HALF_LENGTH_W;
	float h = _CHECK_HALF_LENGTH_H;
	// [추가된 로직] 공이 이미 블록(확장된 영역) 안에 있는지 검사
		// Pos는 블록의 중심
	if (cur_pos[0] > Pos[0] - w && cur_pos[0] < Pos[0] + w &&
		cur_pos[2] > Pos[2] - h && cur_pos[2] < Pos[2] + h)
	{
		// 이미 뚫고 들어와 있음! 가장 가까운 벽으로 밀어낸다.
		float dist_left = cur_pos[0] - (Pos[0] - w);
		float dist_right = (Pos[0] + w) - cur_pos[0];
		float dist_up = cur_pos[2] - (Pos[2] - h); // Z축 좌표계 주의 (위/아래 확인 필요)
		float dist_down = (Pos[2] + h) - cur_pos[2];

		float min_dist = dist_left;
		int hit_dir = 3; // 왼쪽 (enum 값 확인 필요)

		if (dist_right < min_dist) { min_dist = dist_right; hit_dir = 2; }
		if (dist_up < min_dist) { min_dist = dist_up; hit_dir = 0; }
		if (dist_down < min_dist) { min_dist = dist_down; hit_dir = 1; }

		// 강제 충돌 처리
		Flag = 1;
		c_info->Num = 1; // 강제로 1개 설정
		c_info->El[0].Where = hit_dir;
		c_info->El[0].IsBlock = true;
		c_info->Dist = 0.0f; // 이미 붙어있으므로 거리는 0

		_Vector3fCopy(c_info->CollPos, cur_pos); // 현재 위치를 충돌 위치로

		// 반사 벡터 및 위치 보정
		_Vector3fCopy(c_info->FinalDir, c_info->Dir); // 기본 복사

		// 방향 뒤집기 및 밖으로 밀어내기
		switch (hit_dir)
		{
		case 0: // 위
		case 1: // 아래
			c_info->FinalDir[2] *= -1.0f;
			// 침투한 만큼 밖으로 밀어줌 + 알파
			c_info->CollPos[2] += (hit_dir == 0 ? -min_dist : min_dist) * 1.1f;
			break;
		case 2: // 오른쪽
		case 3: // 왼쪽
			c_info->FinalDir[0] *= -1.0f;
			c_info->CollPos[0] += (hit_dir == 3 ? min_dist : -min_dist) * 1.1f;
			break;
		}

		_Vector3fCopy(c_info->FinalPos, c_info->CollPos);
		return true;
	}
	return false;
}

bool FBlock::CollBlock(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos)
{
	float w = _CHECK_HALF_LENGTH_W;
	float h = _CHECK_HALF_LENGTH_H;
//	float w = _BLOCK_WIDTH * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f;
//	float h = _BLOCK_HEIGHT * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f;

	//재미나이가 추천한 Depenetration(끼임 탈출)
	if (OverlapTest(c_info, cur_pos))
		return true;

	Vector4f normals[4];
	GetNormals(normals, Pos, w, h, Gf_CULL_CW);

	float b_dist = 0.0f;
	int ret = CheckCollision(c_info, cur_pos, n_pos, Pos, normals, w, h, true, nullptr);
	if (ret != -1)
	{
		Flag = 1;
		switch (ret)
		{
		case en_wc_up:
		case en_wc_down:
			c_info->FinalDir[2] = -c_info->Dir[2];
			break;
		case en_wc_right:
		case en_wc_left:
			c_info->FinalDir[0] = -c_info->Dir[0];
			break;
		}
		_Vector3fMul(c_info->FinalPos, c_info->FinalDir, c_info->Dist);
		_Vector3fAdd(c_info->FinalPos, c_info->FinalPos, c_info->CollPos);
		return true;
	}
	return false;
}


bool FObjectCommon::CheckWall(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos, FCollInfo* r_info)
{
	float w = _BLOCK_WIDTH * _BLOCK_COLS / 2;
	float h = _BLOCK_HEIGHT * _BLOCK_ROWS / 2;

	Vector3f zero = { 0,0,0 };
	Vector4f normals[4];
	GetNormals(normals, zero, w, h, Gf_CULL_CCW);

	for (int i = 0; i < 4; i++)
		normals[i][3] += _BALL_RADIOUS * 100.0f * 0.5f;

	float b_dist = 0.0f;
	//벽은 back면만 충톨체크한다.
	int ret = CheckCollision(c_info, cur_pos, n_pos, zero, normals, w, h,false, r_info, _CHECK_OPTION_ONE_SIDE | _CHECK_OPTION_SKIP_BOUND);
	if (ret != -1)
	{
		switch (ret)
		{
		case en_wc_up:
			c_info->FinalDir[2] = -c_info->Dir[2];
			break;
		case en_wc_right:
		case en_wc_left:
			c_info->FinalDir[0] = -c_info->Dir[0];
			break;
		case en_wc_down:
			_Vector3fCopy(c_info->FinalPos, c_info->CollPos);
			c_info->FinalPos[2] = -_BLOCK_ROWS * _BLOCK_HEIGHT * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f;
			c_info->Num = 1;
			c_info->El[0].Where = ret;
			c_info->El[0].IsBlock = false;
			return true;
		}
		float n_dist = _Vector3fDistAB(cur_pos, n_pos) - c_info->Dist;
		_Vector3fMul(c_info->FinalPos, c_info->FinalDir, n_dist);
		_Vector3fAdd(c_info->FinalPos, c_info->FinalPos, c_info->CollPos);
		c_info->Num = 1;
		c_info->El[0].Where = ret;
		c_info->El[0].IsBlock = false;
		return true;
	}
	else
	{
		c_info->Num = 0;
		_Vector3fCopy(c_info->FinalPos, n_pos);
		_Vector3fCopy(c_info->FinalDir, c_info->Dir);
	}
	return false;
}
bool FBall::OutWall(FCollInfo* c_info, Vector3f cur_pos)
{
	float w = _BLOCK_WIDTH * _BLOCK_COLS / 2;
	float h = _BLOCK_HEIGHT * _BLOCK_ROWS / 2;

	Vector3f zero = { 0,0,0 };
	Vector4f normals[4];
	GetNormals(normals, zero, w, h, Gf_CULL_CCW);
	for (int i = 0; i < 4; i++)
		normals[i][3] += _BALL_RADIOUS * 100.0f * 0.5f;

	for (int i = 0; i < 4; i++)
	{
		int ret = Gf_GetPlaneBound(cur_pos, normals[i]);
		if (ret == 1)
		{
			_Vector3fCopy(c_info->FinalPos, cur_pos);
			c_info->FinalPos[2] = -_BLOCK_ROWS * _BLOCK_HEIGHT * 0.5f + _BALL_RADIOUS * 100.0f * 0.5f;
			return true;
		}
	}
	return false;
}

bool FBall::CheckCollision(FCollInfo* c_info, Vector3f cur_pos, Vector3f n_pos, int depth)
{
	FCollInfo t_info1;
	FCollInfo t_info2;

	bool b_c_block = CollBlocks(&t_info1, cur_pos, n_pos, false, c_info);
	bool b_w_block = CheckWall(&t_info2, cur_pos, n_pos, c_info);

	if( b_c_block && b_w_block)
	{
		//둘중 가까운 충돌부터 재계산 하자.
		if (t_info2.Dist < t_info1.Dist)
		{
			_Vector3fCopy(n_pos, t_info2.FinalPos);
			_Vector3fCopy(cur_pos, t_info2.CurPos);
			*c_info = t_info2;
		}
		else
		{
			FCollInfo t_info;
			CollBlocks(&t_info, cur_pos, n_pos, true, c_info);
			_Vector3fCopy(n_pos, t_info1.FinalPos);
			_Vector3fCopy(cur_pos, t_info1.CurPos);
			*c_info = t_info1;
		}
		return CheckCollision(c_info, cur_pos, n_pos, depth + 1);
	}

	if (b_c_block)
	{
		FCollInfo t_info;
		CollBlocks(&t_info, cur_pos, n_pos, true, c_info);
		_Vector3fCopy(n_pos, t_info1.FinalPos);
		_Vector3fCopy(cur_pos, t_info1.CurPos);
		*c_info = t_info1;
		return CheckCollision(c_info, cur_pos, n_pos, depth + 1);
	}
	if (b_w_block)
	{
		_Vector3fCopy(n_pos, t_info2.FinalPos);
		_Vector3fCopy(cur_pos, t_info2.CurPos);
		*c_info = t_info2;
		if (t_info2.El[0].Where == en_wc_down)
			return true;
		return CheckCollision(c_info, cur_pos, n_pos, depth + 1);
	}

	Vector3f zero = { 0,0,0 };
	_Vector3fCopy(c_info->FinalDir, t_info1.FinalDir);
	_Vector3fCopy(c_info->FinalPos, n_pos);
	if (depth == 0)
		c_info->Num = 0;
	if (OutWall(c_info, n_pos) || _Vector3fAlmostCmp(c_info->FinalDir, zero, 0.001f))
		return true;
	return false;
}

void FBlocks::Create()
{
	int i, j;

	Vector3f start_pos = { -(_BLOCK_COLS - 1) * _BLOCK_WIDTH * 0.5f, 0, _BLOCK_HEIGHT * (_BLOCK_ROWS - 1) * 0.5f };
	for (i = 0; i < _BLOCK_ROWS; i++)
	{
		for (j = 0; j < _BLOCK_COLS; j++)
		{
			Vector3f pos = { j * _BLOCK_WIDTH, 0, -i * _BLOCK_HEIGHT };
			_Vector3fAdd(Block[i][j].Pos, pos, start_pos);
			Block[i][j].State = en_block_hide;
			Block[i][j].Actor = nullptr;
		}
	}
}

void FBlocks::Tick(float delta)
{

}
void FBlock::CopyBlock(FBlock* block)
{
	State = block->State;
	Hp = block->Hp;
}

void FBlocks::ShiftBlocks()
{
	int i, j;
	for (i = _BLOCK_ROWS -1; i > 0; i--)
	{
		for (j = 0; j < _BLOCK_COLS; j++)
		{
			Block[i][j].CopyBlock(&Block[i-1][j]);
		}
	}
}

int GetRand(bool b_s[_BLOCK_COLS], int stage)
{
	int i;
	//2개에서 5개 생성.
	int cnt = 0;
	for (i = 0; i < _BLOCK_COLS; i++)
	{
		int r = rand()%256;
		if (r > 128)
			b_s[i] = false;
		else
		{
			b_s[i] = true;
			cnt++;
		}
	}
	//2보다 작으면 클때까지 돌린다.
	while (1)
	{
		if (cnt >= 2)
			break;
		cnt = GetRand(b_s, stage);
	}
	if (cnt == _BLOCK_COLS)	//6개 다켜지면 하나는 끄자.
	{
		b_s[rand() % _BLOCK_COLS] = false;
	}
	return cnt;
}
void FBlocks::Clear()
{
	int i, j;
	for (i = _BLOCK_ROWS-1; i >= 0; i--)
	{
		for (j = 0; j < _BLOCK_COLS; j++)
		{
			Block[i][j].State = en_block_hide;
			Block[i][j].Hp = 0;
		}
	}
	Stage = 0;
}

void FBlocks::NextStage()
{
	Stage++;
	ShiftBlocks();

	bool b_success[_BLOCK_COLS];
	GetRand(b_success, Stage);
	for (int i = 0; i < _BLOCK_COLS; i++)
	{
		if (b_success[i])
		{
			Block[0][i].State = en_block_normal;
			Block[0][i].Hp = Stage;
		}
		else
		{
			Block[0][i].State = en_block_hide;
			Block[0][i].Hp = 0;
		}
	}
	/*
		//--------------------------------------------테스트
		int i, j;
		for (i = 0; i < _BLOCK_ROWS; i++)
		{
			for (j = 0; j < _BLOCK_COLS; j++)
			{
				Block[j][i].State = en_block_normal;
				Block[j][i].Hp = Stage;
			}
		}
	*/
}

struct c_info
{
	FBlock* block;
	FCollInfo c_info;
	int i ,j;	//가로 세로
	bool no_calc_hp;
	bool no_add_coll;
};

void SetDelete2(c_info* c_block, int a, int b)
{
	if (c_block[a].c_info.El[0].Where == en_wc_up)
	{
		if (c_block[b].c_info.El[0].Where == en_wc_right)
		{
			if (c_block[a].i + 1 == c_block[b].i && c_block[a].j + 1 == c_block[b].j)
			{
			}
			else
			{
				if (c_block[a].c_info.Dist < c_block[b].c_info.Dist)
				{
					c_block[b].no_calc_hp = true;
					c_block[b].no_add_coll = true;
				}
				else
				{
					c_block[a].no_calc_hp = true;
					c_block[a].no_add_coll = true;
				}
			}
		}
		else if (c_block[b].c_info.El[0].Where == en_wc_left)
		{
			if (c_block[a].i == c_block[b].i + 1 && c_block[a].j + 1 == c_block[b].j)
			{
			}
			else
			{
				if (c_block[a].c_info.Dist < c_block[b].c_info.Dist)
				{
					c_block[b].no_calc_hp = true;
					c_block[b].no_add_coll = true;
				}
				else
				{
					c_block[a].no_calc_hp = true;
					c_block[a].no_add_coll = true;
				}
			}
		}
		return;
	}
	if (c_block[b].c_info.El[0].Where == en_wc_down)
	{
		if (c_block[a].c_info.El[0].Where == en_wc_right)
		{
			if (c_block[b].i + 1 == c_block[a].i && c_block[a].j + 1 == c_block[b].j)
			{
			}
			else
			{
				if (c_block[a].c_info.Dist < c_block[b].c_info.Dist)
				{
					c_block[b].no_calc_hp = true;
					c_block[b].no_add_coll = true;
				}
				else
				{
					c_block[a].no_calc_hp = true;
					c_block[a].no_add_coll = true;
				}
			}
		}
		else if (c_block[a].c_info.El[0].Where == en_wc_left)
		{
			if (c_block[b].i == c_block[a].i + 1 && c_block[a].j + 1 == c_block[b].j)
			{
			}
			else
			{
				if (c_block[a].c_info.Dist < c_block[b].c_info.Dist)
				{
					c_block[b].no_calc_hp = true;
					c_block[b].no_add_coll = true;
				}
				else
				{
					c_block[a].no_calc_hp = true;
					c_block[a].no_add_coll = true;
				}
			}
		}
		return;
	}
}
void FixHitCnt(c_info* c_block, int hit_cnt)
{
	check(hit_cnt <= 3);	//3개이상이면 문제가 있다.

	//2개 부터 뚫고 
	if (hit_cnt == 2)
	{
		//평행선상 충돌 발생은 정상.
		if (c_block[0].c_info.El[0].Where == c_block[1].c_info.El[0].Where)
			return;
		SetDelete2(c_block, 0, 1);
		SetDelete2(c_block, 1, 0);
		return;
	}

	//3개 충돌은  2개로 고친다.
	//##
	//#
	if ((c_block[0].i == c_block[1].i + 1 && c_block[0].j == c_block[1].j)
	&& (c_block[1].i == c_block[2].i && c_block[1].j + 1 == c_block[2].j))
		c_block[1].no_calc_hp = true;
	//##
	// #
	if ((c_block[0].i == c_block[2].i + 1 && c_block[0].j == c_block[2].j)
	&& (c_block[1].i == c_block[2].i && c_block[1].j + 1 == c_block[2].j))
		c_block[2].no_calc_hp = true;
	//#
	//##
	if ((c_block[0].i == c_block[1].i && c_block[0].j + 1 == c_block[1].j)
	&& (c_block[0].i == c_block[2].i + 1 && c_block[0].j == c_block[2].j))
		c_block[0].no_calc_hp = true;
	// #
	//##
	if ((c_block[0].i == c_block[1].i && c_block[0].j + 1 == c_block[1].j)
	&& (c_block[1].i == c_block[2].i + 1 && c_block[1].j == c_block[2].j))
		c_block[1].no_calc_hp = true;

}

bool FBall::CollBlocks(FCollInfo* f_info, Vector3f cur_pos, Vector3f n_pos, bool b_sub_hp, FCollInfo* r_info)
{
	c_info c_block[_BLOCK_ROWS * _BLOCK_COLS];
	memset(c_block, 0, sizeof(c_info) * _BLOCK_ROWS * _BLOCK_COLS);

	int hit_cnt = 0;
	int i, j;
	for (i = _BLOCK_ROWS - 1; i >= 0; i--)
	{
		//		i = 0;
		for (j = 0; j < _BLOCK_COLS; j++)
		{
			FBlock* block = &Blocks->Block[i][j];
			if (block->State != en_block_normal)
				continue;
			if (r_info->IsSKipBlock(i, j))
				continue;
			FCollInfo c_info;
			if (block->CollBlock(&c_info, cur_pos, n_pos))
			{
				c_block[hit_cnt].block = block;
				c_block[hit_cnt].c_info = c_info;
				c_block[hit_cnt].i = i;
				c_block[hit_cnt].j = j;
				hit_cnt++;
				/*
				block->Hp -= Hp;
				//2개 분기 경우 추가.
				if (block->Hp <= 0)
				{
					block->State = en_block_hit;
				}
				*/
			}
		}
	}
	if (hit_cnt)
	{
		FixHitCnt(c_block, hit_cnt);
		float dist = 1000000;
		int shortest_i = -1;
		Vector3f nu_dir = { 0,0,0 };
		for (i = 0; i < hit_cnt; i++)
		{
			if (f_info->Num >= 3)	//3개이상 나오면 안된다.
			{
				continue;
			}
			if (c_block[i].no_calc_hp == false)	//hp 계산제외.
			{
				if (b_sub_hp)
				{
					c_block[i].block->Hp -= Hp;
					c_block[i].block->State = en_block_hit;
					//2개 분기 경우 추가.
				}
				_Vector3fAdd(nu_dir, nu_dir, c_block[i].c_info.FinalDir);
				if (c_block[i].c_info.Dist < dist)
				{
					dist = c_block[i].c_info.Dist;
					shortest_i = i;
				}
			}
			if (c_block[i].no_add_coll == false)	//coll 추가 제외
			{
				f_info->El[f_info->Num].i = c_block[i].i;
				f_info->El[f_info->Num].j = c_block[i].j;
				f_info->El[f_info->Num].IsBlock = true;
				f_info->El[f_info->Num].Where = c_block[i].c_info.El[0].Where;
				f_info->Num++;
			}
		}
		if (!_Vector3fNormalize(nu_dir, 0.01f))	// 정면으로 계산된 경우. 반사처리.
		{
			_Vector3fSub(nu_dir, cur_pos, n_pos);
			_Vector3fNormalize(nu_dir);
		}
		float n_dist = _Vector3fDistAB(cur_pos, n_pos) - c_block[shortest_i].c_info.Dist;
		f_info->Dist = dist;
		_Vector3fCopy(f_info->FinalDir, nu_dir);
		_Vector3fCopy(f_info->CurPos, c_block[shortest_i].c_info.CollPos);
		_Vector3fMul(f_info->FinalPos, f_info->FinalDir, n_dist);
		_Vector3fAdd(f_info->FinalPos, f_info->FinalPos, c_block[shortest_i].c_info.CollPos);

	}
	else
	{
		f_info->Num = 0;
		_Vector3fCopy(f_info->FinalPos, n_pos);
		_Vector3fCopy(f_info->CurPos, cur_pos);
		_Vector3fSub(f_info->FinalDir, n_pos, cur_pos);
		_Vector3fNormalize(f_info->FinalDir);
		return false;
	}

	return true;
}

void FBlocks::CopyFrom(FBlocks* blocks)
{
	Stage = blocks->Stage;
	int i, j;
	for (i = 0; i < _BLOCK_ROWS; i++)
	{
		for (j = 0; j < _BLOCK_COLS; j++)
		{
			//_Vector3fCopy(Block[i][j].Pos, );
			Block[i][j].State = blocks->Block[i][j].State;
			Block[i][j].Hp = blocks->Block[i][j].Hp;
		}
	}
}

void FBallEmitter::NextStage()
{ 
	Frame = 0; 
	State = en_ball_state_emitting;
	_Vector3fCopy(Pos, StartPos);
	if (Ball.Buf == nullptr)
	{
		Ball.Create(_NUM_MAX_BALLS+1, _NUM_MAX_BALLS, sizeof(FBall));
	}
	Ball.Cnt = 0;
	Stage++;
	bFirstStop = false;
}

void FBallEmitter::PreTick()
{
	if (State == en_ball_state_emitting)
	{
		//		int num_balls = Gf_min(Stage, _NUM_MAX_BALLS);
		if (Ball.Cnt < Stage)
		{
			if (Frame % 4 == 0)
			{
				FBall ball;
				ball.bStart = true;
				ball.State = en_block_normal;
				ball.Hp = 1;
				_Vector3fCopy(ball.Pos, Pos);
				_Vector3fCopy(ball.Dir, Dir);
				Ball.Add(&ball);
			}
		}
	}

}
bool FBallEmitter::Tick(float delta)
{
	int i;
	int b_stop_cnt = 0;
	for (i = 0; i < Ball.Cnt; i++)
	{
		FBall* ball = (FBall*)Ball.GetPtr(i);
		Vector3f n_pos, cur_pos;
		_Vector3fMul(n_pos, ball->Dir, delta * _MOVING_SCALE);
		_Vector3fAdd(n_pos, n_pos, ball->Pos);
		_Vector3fCopy(cur_pos, ball->Pos);

//		WriteFileLogFromAction(&Blocks, cur_pos, n_pos);
		FCollInfo c_info;
		if (ball->bStart)
			c_info.Start();

		bool b_stop = ball->CheckCollision(&c_info, cur_pos, n_pos, 0);
		_Vector3fCopy(ball->Dir, c_info.FinalDir);
		_Vector3fCopy(ball->Pos, c_info.FinalPos);
		if (b_stop)
		{
			ball->State = en_block_hide;
			if (bFirstStop == false)
			{
				bFirstStop = true;
				_Vector3fCopy(StartPos, c_info.FinalPos);
			}
			b_stop_cnt++;
		}
	}

	Frame++;

	if (b_stop_cnt == Ball.Cnt)
		return false;
	return true;
}
