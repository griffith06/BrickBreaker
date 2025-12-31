#pragma once

#include "CVectorVer2.h"

#define Gf_CULL_NONE		1		///< D3DCULL_NONE
#define Gf_CULL_CW			2		///< D3DCULL_CW하고 맞췄다..	시계반향
#define Gf_CULL_CCW			3		///< D3DCULL_CCW하고 맞췄다..	시계반대방향.


bool Gf_GetNormal(Vector4f get, Vector3f vv0, Vector3f vv1, Vector3f vv2, UINT cullmode, float epsilon = _TOO_SMALL_NUMBER);
void GetNormals(Vector4f* normals, Vector3f pos, float w, float h, int cull_mode);

bool IsCorrectFloat(float fix);
bool IsCorrectVector3f(Vector3f fix);
