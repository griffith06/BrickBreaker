#include "BreakerUtil.h"



#define Gf_FixNormal3f(a)		if( !Gf_IsCorrectNormal3f(a) ) {	a[0]=0; a[1]=0; a[2]=1; }
#define Gf_FixNormal4f(a)		if( !Gf_IsCorrectNormal4f(a) ) {	a[0]=0; a[1]=0; a[2]=1; a[3]=0; }

bool Gf_IsCorrectFloat(float value)
{
	union
	{
		float fvalue;
		uint32 err_test;
	};
	fvalue = value;
	//16진수로 비교하자.
	if (err_test == 0x7fc00000 || err_test == 0xffc00000 || err_test == 0xff800000
		|| err_test == 0x7f800000)	//inf
	{
		return false;
	}
	return true;
}
bool Gf_IsCorrectNormal4f(Vector4f normal)	// 맥스에서 잘못된 폴리곤 처리를 깨끗이 정리 안해준다. 노멀값으로 잘못된 face인지 알아낸다.
{
	if (Gf_IsCorrectFloat(normal[0]) && Gf_IsCorrectFloat(normal[1]) && Gf_IsCorrectFloat(normal[2]) && Gf_IsCorrectFloat(normal[3]))
	{
		return TRUE;
	}
	return FALSE;
}
/**
@brief
@return
@param normal

*/

bool Gf_IsCorrectNormal3f(Vector3f normal)	// 맥스에서 잘못된 폴리곤 처리를 깨끗이 정리 안해준다. 노멀값으로 잘못된 face인지 알아낸다.
{
	//16진수로 비교하자.
	if (Gf_IsCorrectFloat(normal[0]) && Gf_IsCorrectFloat(normal[1]) && Gf_IsCorrectFloat(normal[2]))
	{
		return TRUE;
	}
	return FALSE;
}


bool Gf_GetNormal(Vector4f get, Vector3f vv0, Vector3f vv1, Vector3f vv2, UINT cullmode, float epsilon)
{
	//----------------------- 평면의 방정식 생성
	Vector3f v0, v1, v2;

	v0[0] = 0;	v0[1] = 0;	v0[2] = 0;
	v1[0] = vv1[0] - vv0[0];	v1[1] = vv1[1] - vv0[1];	v1[2] = vv1[2] - vv0[2];
	v2[0] = vv2[0] - vv0[0];	v2[1] = vv2[1] - vv0[1];	v2[2] = vv2[2] - vv0[2];

	get[0] = (v0[1] * (v1[2] - v2[2]) + v1[1] * (v2[2] - v0[2]) + v2[1] * (v0[2] - v1[2]));
	get[1] = (v0[2] * (v1[0] - v2[0]) + v1[2] * (v2[0] - v0[0]) + v2[2] * (v0[0] - v1[0]));
	get[2] = (v0[0] * (v1[1] - v2[1]) + v1[0] * (v2[1] - v0[1]) + v2[0] * (v0[1] - v1[1]));

	if (!_Vector3fNormalize(get, epsilon))	//정규화 
	{
		get[3] = get[0] * vv0[0] + get[1] * vv0[1] + get[2] * vv0[2];
		Gf_FixNormal4f(get);
		return false;
	}

	if (cullmode == Gf_CULL_CW)	//시계방향으로 컬링한다면 역으로 해야 한다.
		_Vector3fMul(get, get, -1);

	get[3] = get[0] * vv0[0] + get[1] * vv0[1] + get[2] * vv0[2];
	Gf_FixNormal4f(get);
	return true;
}

/*
void GetNormals(Vector4f* normals, Vector3f pos, float w, float h)
{
	Vector4f normal[4] = {
	0, 0, -1, h,	//위
	-1, 0, 0, w,	//오른쪽
	1, 0, 0, w,	//왼쪽
	0, 0, 1, h,	//아래
	};
	for (int i = 0; i < 4; i++)
	{
		_Vector4fCopy(normals[i], normal[i]);
	}
	normals[0][3] += pos[2];
	normals[1][3] += pos[0];
	normals[2][3] += pos[0];
	normals[3][3] += pos[2];
}*/
void GetNormals(Vector4f* normals, Vector3f pos, float w, float h, int cull_mode)
{
	Vector3f t[4] = {
		w, -100, h,		//위.
		w, 100, h,		//오른쪽
		-w, -100, h,	//왼쪽
		w, -100, -h,	//아래.
	};
	for (int i = 0; i < 4; i++)
	{
		_Vector3fAdd(t[i], t[i], pos);
	}
	int id[4][3] =
	{
		0,2,1,
		0,1,3,
		0,3,1,
		0,1,2,
	};
	for (int i = 0; i < 4; i++)
	{
		Gf_GetNormal(normals[i], t[id[i][0]], t[id[i][1]], t[id[i][2]], cull_mode);
	}
	normals[0][3] = Vector3fDot(normals[0], t[0]);
	normals[1][3] = Vector3fDot(normals[1], t[0]);
	normals[2][3] = Vector3fDot(normals[2], t[2]);
	normals[3][3] = Vector3fDot(normals[3], t[3]);
}

bool IsCorrectFloat(float fix)
{
	DWORD* err_test = (DWORD*)&fix;
	//16진수로 비교하자.
	if (err_test[0] == 0x7fc00000 
		|| err_test[0] == 0xffc00000
		|| err_test[0] == 0x00007ff4)
	{
		return false;
	}
	return true;
}

bool IsCorrectVector3f(Vector3f fix)
{
	if(IsCorrectFloat(fix[0]) && IsCorrectFloat(fix[1]) && IsCorrectFloat(fix[2]))
		return true;
	return false;
}
