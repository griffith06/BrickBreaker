#ifndef __C_VECTOR2_H__
#define __C_VECTOR2_H__

/**
@file     CVectorVer2.h
@brief    CVector 2.0 벡터 디파인들.

벡터클래스를 쓰지 않는이유는 함수오버로딩이 매우느리다.(디버그모드시) 디버그작업시 많이느려 그냥 함수호출로 하기로했다.\n\n
그리고 CVector 1.0에서는 Vector3fCopy(s,d) 처럼 dest가 뒤에 있기때문에(오래된 C방식이다..) 2.0애서는 _Vector3fCopy(d,s)로_를 앞에 붙여서 dest를 앞으로 다 땡겼다.\n\n
호환성때문에 1.0 디파인들이 있는데... 작업시 _Vector... 로 쓰길바란다.\n\n

*/

/*
#ifdef _WIDE_CHAR
#define mchar wchar_t
#else
#define mchar char
#endif // _WIDE_CHAR


WideCharToMultiByte
*/

#include <math.h>
#include <windows.h>
#include "CVectorVer1.h"

#define _Gf_NO_SWAP_3DSMAX_YZ	//3ds max 스왑을 사용하지 않는다
#define _Gf_NO_USE_UV			//uv를 사용하지 않는다

class CVectorVer2{
public:
	union{
	float mX;
	float mV[1];	//mV[0]-mV[2]까지 사용하려고...
	};
	float mY, mZ;
};

//-------벡터 포인터
#define Vector3cp	char *
#define Vector4fp	float *
#define Vector3fp	float *
#define Vector3lp	long *
#define Vector3sp	short int *
#define Vector2fp	Vector3fp
#define Vector1fp	Vector3fp
#define Vector2lp	Vector3lp
#define Vector2sp	Vector3sp

typedef float Vector2f[2];  /* st */
typedef float Vector3f[3];  /* xyz */
typedef float Vector4f[4];  // xyzw 또는 평면 ax+by+cz=d;

typedef long Vector2l[2];  /* uv */
typedef long Vector3l[3];  /* 1,2,3 */
typedef long Vector4l[4];  /* 1,2,3,4 */

typedef int Vector2i[2];  /* uv */
typedef int Vector3i[3];  /* 1,2,3 */
typedef int Vector4i[4];  /* 1,2,3,4 */

typedef char Vector2c[2];  /* uv */
typedef char Vector3c[3];  /* uv */

typedef short int Vector4s[3];		// short형 고정 소수점에 쓸 예정...
typedef short int Vector3s[3];		// short형 고정 소수점에 쓸 예정...
typedef short int Vector2s[2];		// short형 고정 소수점에 쓸 예정...

typedef unsigned short int Vector4us[3];		// short형 고정 소수점에 쓸 예정...
typedef unsigned short int Vector3us[3];		// short형 고정 소수점에 쓸 예정...
typedef unsigned short int Vector2us[2];		// short형 고정 소수점에 쓸 예정...


#define USE_INLINE_VECTOR
#if !defined(USE_INLINE_VECTOR)

//------vector ver 2.0

#define _Vector1fSwap(d,s) {	float temp=s;	s=d; d=temp;	}
#define _Vector2fSwap(d,s) {	_Vector1fSwap(d[0],s[0]);	_Vector1fSwap(d[1],s[1]);	}
#define _Vector3fSwap(d,s) {	_Vector1fSwap(d[0],s[0]);	_Vector1fSwap(d[1],s[1]);	_Vector1fSwap(d[2],s[2]);	}
#define _Vector4fSwap(d,s) {	_Vector1fSwap(d[0],s[0]);	_Vector1fSwap(d[1],s[1]);	_Vector1fSwap(d[2],s[2]);		_Vector1fSwap(d[3],s[3]);	}

#define _Vector4fSet(v,a,b,c,d)	{v[0]=a;v[1]=b;v[2]=c;v[3]=d;}
#define _Vector4fCopy(d,s) {d[0]=s[0];d[1]=s[1];d[2]=s[2];d[3]=s[3];}
#define _Vector4fZero(a)	{a[0]=0; a[1]=0; a[2]=0; a[3]=0;}
#define _Vector4fOne(a)		{a[0]=1; a[1]=1; a[2]=1; a[3]=1;}
#define _Vector4fAdd(c,a,b) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];c[3]=a[3]+b[3];}
#define _Vector4fMul(c,a,b) {c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;c[3]=a[3]*b;}
#define _Vector4fDiv(c,a,b) {c[0]=a[0]/b;c[1]=a[1]/b;c[2]=a[2]/b;c[3]=a[3]/b;}
#define _Vector4fSub(c,a,b) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];c[3]=a[3]-b[3];} 
#define _Vector4fScale(c,a,b) {c[0]=b*a[0];c[1]=b*a[1];c[2]=b*a[2];c[3]=b*a[3];}

#define _Vector3lCopy(d,s) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}

#define _Vector3fSet(v,a,b,c)	{v[0]=a;v[1]=b;v[2]=c;}
#define _Vector3fDistAB(a,b)	(sqrtf((b[0]-a[0])*(b[0]-a[0])+(b[1]-a[1])*(b[1]-a[1])+(b[2]-a[2])*(b[2]-a[2])))
#define _Vector3fDistABExceptY(a,b) (sqrtf((b[0]-a[0])*(b[0]-a[0])+(b[2]-a[2])*(b[2]-a[2])))
#define _Vector3fDist(v) (sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))
#define _Vector3fDist2(v) ((v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))

#define _Vector3fCopy(d,s) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}
#define _Vector3fAdd(c,a,b) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define _Vector3fMul(c,a,b) {c[0]=a[0]*(b);c[1]=a[1]*(b);c[2]=a[2]*(b);}
#define _Vector3fMul3f(c,a,b) {c[0]=a[0]*b[0];c[1]=a[1]*b[1];c[2]=a[2]*b[2];}
#define _Vector3fDiv(c,a,b) {c[0]=a[0]/(b);c[1]=a[1]/(b);c[2]=a[2]/(b);}
#define _Vector3fSub(c,a,b) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];} 
#define _Vector3fScale(c,a,b) {c[0]=(b)*a[0];c[1]=(b)*a[1];c[2]=(b)*a[2];}
#define _Vector3fZero(a)	{a[0]=0; a[1]=0; a[2]=0;}
#define _Vector3fSaturate(a)	{ a[0]=min(a[0],1); a[0]=max(a[0],0);a[1]=min(a[1],1); a[1]=max(a[1],0);a[2]=min(a[2],1); a[2]=max(a[2],0); }

//#define _MAX_FLOAT_LIMIT	6.5536e8   ///< 걍 내가 정한거다. 이정도의 float값은 엔진이던 게임이던 쓰지 않는다.
#define _MAX_FLOAT_LIMIT	3.402823466e+38F   ///< float 최대치
#define _Vector3fSetMin(a) {a[0]=-_MAX_FLOAT_LIMIT; a[1]=-_MAX_FLOAT_LIMIT; a[2]=-_MAX_FLOAT_LIMIT; }
#define _Vector3fSetMax(a) {a[0]=_MAX_FLOAT_LIMIT; a[1]=_MAX_FLOAT_LIMIT; a[2]=_MAX_FLOAT_LIMIT; }
#define _Vector3fMin(c,a,b) {	c[0] = min(a[0],b[0]); c[1] = min(a[1],b[1]); c[2] = min(a[2],b[2]);  }
#define _Vector3fMax(c,a,b) {	c[0] = max(a[0],b[0]); c[1] = max(a[1],b[1]); c[2] = max(a[2],b[2]);  }


#ifdef _USE_INTEL_COMPILER 
	float   _Vector3fDot(float *a,float *b);
#else
	#define _Vector3fDot(a,b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#endif

#define _Vector4fAvg(c,a,b) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;c[2]=(a[2]+b[2])*0.5f;c[3]=(a[3]+b[3])*0.5f;}
#define _Vector4fCmp(a,b) (a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2]&&a[3]==b[3])

#define _Vector3fAvg(c,a,b) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;c[2]=(a[2]+b[2])*0.5f;}
#define _Vector3fCmp(a,b) (a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2])
#define _Vector3fCross(r,a,b) {  r[0] = (a[1] * b[2]) - (a[2] * b[1]); r[1] = (a[2] * b[0]) - (a[0] * b[2]); r[2] = (a[0] * b[1]) - (a[1] * b[0]);}

#define _Vector2fCopy(d,s) {d[0]=s[0];d[1]=s[1];}
#define _Vector2fAvg(c,a,b) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;}
#define _Vector2fCmp(a,b) (a[0]==b[0]&&a[1]==b[1])

#define _Vector4fSetMin(a) {a[0]=-_MAX_FLOAT_LIMIT; a[1]=-_MAX_FLOAT_LIMIT; a[2]=-_MAX_FLOAT_LIMIT; a[3]=-_MAX_FLOAT_LIMIT; }
#define _Vector4fSetMax(a) {a[0]=_MAX_FLOAT_LIMIT; a[1]=_MAX_FLOAT_LIMIT; a[2]=_MAX_FLOAT_LIMIT; a[3]=_MAX_FLOAT_LIMIT; }

#define _Vector1fCopy(d,s) {d=s;}
#define _Vector1fAvg(c,a,b) {c=(a+b)*0.5f;}

#define _Vector2iCopy(d,s) {d[0]=s[0];d[1]=s[1];}
#define _Vector2lCopy(d,s) {d[0]=s[0];d[1]=s[1];}


#ifdef _USE_INTEL_COMPILER 
float	SQRT ( float input);
inline void _Vector3fNormalize(Vector3f v) { 
	//double a= sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	float input = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
	float a = SQRT( input );
	v[0]/=a;	v[1]/=a;	v[2]/=a;	}
#else
inline void _Vector3fNormalize(Vector3f v) { double a= sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0]/=(float)a;	v[1]/=(float)a;	v[2]/=(float)a;	}
#endif

#else // !defined(USE_INLINE_VECTOR)

//
// 2007. 12. 7. 강성환
//
// 매크로 함수 -> inline 함수화
//

#define _MAX_FLOAT_LIMIT	(3.402823466e+38F)   ///< float 최대치
#define _TOO_SMALL_NUMBER	(1.e-8)				 ///< 너무작은수 0에 가까운
//

#define _Vector1fSwap(d,s)	{ float temp=s; s=d; d=temp; }
#define _Vector1fCopy(d,s)	{ d=s; }
#define _Vector1fAvg(c,a,b)	{ c=((a)+(b))*0.5f;}

#define _Vector2iCopy(d,s) {d[0]=s[0];d[1]=s[1];}
#define _Vector2lCopy(d,s) {d[0]=s[0];d[1]=s[1];}

// vector4
inline void _Vector4fSet( Vector4f d, float s )
{
	d[0] = s;
	d[1] = s;
	d[2] = s;
	d[3] = s;
}
inline void _Vector4fSwap(Vector4f d, Vector4f s)
{
	_Vector1fSwap(d[0],s[0]);
	_Vector1fSwap(d[1],s[1]);
	_Vector1fSwap(d[2],s[2]);
	_Vector1fSwap(d[3],s[3]);
}

inline void _Vector4fCopy(Vector4f d, const Vector4f s)
{
	memcpy(d, s, sizeof(Vector4f));
}

inline void _Vector4fZero(Vector4f a)
{
	memset(a, 0, sizeof(Vector4f));
}
inline void _Vector4fOne(Vector4f a)
{
	a[0]=1; a[1]=1; a[2]=1; a[3]=1;
}

inline void _Vector4fAdd(Vector4f c, const Vector4f a, const Vector4f b)
{
	c[0]=a[0]+b[0];
	c[1]=a[1]+b[1];
	c[2]=a[2]+b[2];
	c[3]=a[3]+b[3];
}

inline void _Vector4fMul(Vector4f c, const Vector4f a, const float b)
{
	c[0]=a[0]*b;
	c[1]=a[1]*b;
	c[2]=a[2]*b;
	c[3]=a[3]*b;
}

inline void _Vector4fDiv(Vector4f c, const Vector4f a, const float b)
{
	c[0]=a[0]/b;
	c[1]=a[1]/b;
	c[2]=a[2]/b;
	c[3]=a[3]/b;
}

inline void _Vector4fSub(Vector4f c, const Vector4f a, const Vector4f b)
{
	c[0]=a[0]-b[0];
	c[1]=a[1]-b[1];
	c[2]=a[2]-b[2];
	c[3]=a[3]-b[3];
}

inline void _Vector4fScale(Vector4f c, const Vector4f a, const float b)
{
	c[0]=b*a[0];
	c[1]=b*a[1];
	c[2]=b*a[2];
	c[3]=b*a[3];
}

inline void _Vector4fSetMin(Vector4f a)
{
	a[0]=-_MAX_FLOAT_LIMIT;
	a[1]=-_MAX_FLOAT_LIMIT;
	a[2]=-_MAX_FLOAT_LIMIT;
	a[3]=-_MAX_FLOAT_LIMIT;
}

inline void _Vector4fSetMax(Vector4f a)
{
	a[0]=_MAX_FLOAT_LIMIT;
	a[1]=_MAX_FLOAT_LIMIT;
	a[2]=_MAX_FLOAT_LIMIT;
	a[3]=_MAX_FLOAT_LIMIT;
}

inline void _Vector4fAvg(Vector4f c, const Vector4f a, const Vector4f b)
{
	c[0]=(a[0]+b[0])*0.5f;
	c[1]=(a[1]+b[1])*0.5f;
	c[2]=(a[2]+b[2])*0.5f;
	c[3]=(a[3]+b[3])*0.5f;
}

inline bool _Vector4fCmp(const Vector4f a, const Vector4f b)
{
	return (a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2]&&a[3]==b[3]);
}

// vector3

#define _Vector3lCopy(d,s) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}	// (!) 이건 long형 용?

inline void _Vector3fSet(Vector3f d, float fx, float fy, float fz )
{
	d[0] = fx;	d[1] = fy, d[2] = fz;
}
inline void _Vector3fSet(Vector3f d, float s )
{
	d[0] = s;	d[1] = s, d[2] = s;
}
inline void _Vector3fSwap(Vector3f d, Vector3f s)
{
	_Vector1fSwap(d[0],s[0]);
	_Vector1fSwap(d[1],s[1]);
	_Vector1fSwap(d[2],s[2]);
}
inline float  _Vector3fDistAB(const Vector3f a, const Vector3f b)
{
	return sqrtf((b[0]-a[0])*(b[0]-a[0])+(b[1]-a[1])*(b[1]-a[1])+(b[2]-a[2])*(b[2]-a[2]));
}

inline float  _Vector3fDistAB2(const Vector3f a, const Vector3f b)
{
	return ((b[0]-a[0])*(b[0]-a[0])+(b[1]-a[1])*(b[1]-a[1])+(b[2]-a[2])*(b[2]-a[2]));
}

inline float _Vector3fDistABExceptY(const Vector3f a, const Vector3f b)
{
	float dx = b[0]-a[0];
	float dz = b[2]-a[2];
	return sqrtf(dx*dx + dz*dz);
}

inline float _Vector3fDist(const Vector3f v)
{
	return sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

inline float _Vector3fDist2(const Vector3f v)
{
	return (v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

inline void _Vector3fCopy(Vector3f d, const Vector3f s)
{
	memcpy(d, s, sizeof(Vector3f));
}

inline void _Vector3fZero(Vector3f a)
{
	memset(a, 0, sizeof(Vector3f));
}
inline void _Vector3fSaturate(Vector3f a)	
{
#define t_max(a,b)    (((a) > (b)) ? (a) : (b))
#define t_min(a,b)    (((a) < (b)) ? (a) : (b))
	a[0]=t_min(a[0],1); 
	a[0]=t_max(a[0],0);
	a[1]=t_min(a[1],1); 
	a[1]=t_max(a[1],0);
	a[2]=t_min(a[2],1); 
	a[2]=t_max(a[2],0); 
}

inline void _Vector3fOne(Vector3f a)
{
	a[0]=1; a[1]=1; a[2]=1;
}

inline BOOL _IsVector3fZero(Vector3f a)
{
	if( a[0] == 0 && a[1] == 0 && a[2] == 0 )
		return TRUE;
	return FALSE;
}

inline void _Vector3fAdd(Vector3f c, const Vector3f a, const Vector3f b)
{
	c[0]=a[0]+b[0];
	c[1]=a[1]+b[1];
	c[2]=a[2]+b[2];
}

inline void _Vector3fMul(Vector3f c, const Vector3f a, const float b)
{
	c[0]=a[0]*b;
	c[1]=a[1]*b;
	c[2]=a[2]*b;
}
#define _Vector3fMul3f(c,a,b) {c[0]=a[0]*b[0];c[1]=a[1]*b[1];c[2]=a[2]*b[2];}

inline void _Vector3fDiv(Vector3f c, const Vector3f a, const float b)
{
	c[0]=a[0]/b;
	c[1]=a[1]/b;
	c[2]=a[2]/b;
}

inline void _Vector3fSub(Vector3f c, const Vector3f a, const Vector3f b)
{
	c[0]=a[0]-b[0];
	c[1]=a[1]-b[1];
	c[2]=a[2]-b[2];
}

inline void _Vector3fScale(Vector3f c, const Vector3f a, const float b)
{
	c[0]=b*a[0];
	c[1]=b*a[1];
	c[2]=b*a[2];
}

inline void _Vector3fSetMin(Vector3f a)
{
	a[0]=-_MAX_FLOAT_LIMIT;
	a[1]=-_MAX_FLOAT_LIMIT;
	a[2]=-_MAX_FLOAT_LIMIT;
}

inline void _Vector3fSetMax(Vector3f a)
{
	a[0]=_MAX_FLOAT_LIMIT;
	a[1]=_MAX_FLOAT_LIMIT;
	a[2]=_MAX_FLOAT_LIMIT;
}
#define Gf_max(a,b)            (((a) > (b)) ? (a) : (b))
#define Gf_min(a,b)            (((a) < (b)) ? (a) : (b))

inline void _Vector3fMin(Vector3f c, const Vector3f a, const Vector3f b)
{
	c[0] = Gf_min(a[0],b[0]);
	c[1] = Gf_min(a[1],b[1]);
	c[2] = Gf_min(a[2],b[2]);
}

inline void _Vector3fMax(Vector3f c, const Vector3f a, const Vector3f b)
{
	c[0] = Gf_max(a[0],b[0]);
	c[1] = Gf_max(a[1],b[1]);
	c[2] = Gf_max(a[2],b[2]);
}

inline float _Vector3fDot(const Vector3f a, const Vector3f b)
{
	return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
}

inline void _Vector3fCross(Vector3f r, const Vector3f a, const Vector3f b)
{
	r[0] = (a[1] * b[2]) - (a[2] * b[1]);
	r[1] = (a[2] * b[0]) - (a[0] * b[2]);
	r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

inline bool _Vector3fNormalize(Vector3f v, float epsilon = _TOO_SMALL_NUMBER)
{
	float PreSquare = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

	if (PreSquare < epsilon)
		return false;

	float a = sqrtf(PreSquare);
	v[0] /= a;
	v[1] /= a;
	v[2] /= a;
	return true;
}
inline void _Vector3fAvg(Vector3f c, const Vector3f a, const Vector3f b)
{
	c[0]=(a[0]+b[0])*0.5f;
	c[1]=(a[1]+b[1])*0.5f;
	c[2]=(a[2]+b[2])*0.5f;
}

inline bool _Vector3fCmp(const Vector3f a, const Vector3f b)
{
	return (a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2]);
}

//

inline void _Vector2fSwap(Vector2f d, Vector2f s)
{
	_Vector1fSwap(d[0],s[0]);
	_Vector1fSwap(d[1],s[1]);
}

inline void _Vector2fCopy(Vector2f d, const Vector2f s)
{
	memcpy(d, s, sizeof(float) * 2);
}
inline void _Vector2fMul(Vector2f c, const Vector2f a, const float b)
{
	c[0]=a[0]*b;
	c[1]=a[1]*b;
}
inline void _Vector2fAdd(Vector2f c, const Vector2f a, const Vector2f b)
{
	c[0]=a[0]+b[0];
	c[1]=a[1]+b[1];
}

inline void _Vector2fSub(Vector2f c, const Vector2f a, const Vector2f b)
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
}
inline void _Vector2fAvg(Vector2f c, const Vector2f a, const Vector2f b)
{
	c[0]=(a[0]+b[0])*0.5f;
	c[1]=(a[1]+b[1])*0.5f;
}

inline bool _Vector2fCmp(const Vector2f a, const Vector2f b)
{
	return (a[0]==b[0]&&a[1]==b[1]);
}


// basis 노멀방향에따른 평면의 U와 V 축을 대표하는 적절한 임의의 축 벡터를 찾기.
void _Vector3fFindBestAxis( Vector3f Axis1, Vector3f Axis2, const Vector3f basis );

#endif // !defined(USE_INLINE_VECTOR)

BOOL _Vector1fAlmostCmp(float a,float b,float epsilon=0.00001f);									
BOOL _Vector2fAlmostCmp(Vector2f a,Vector2f b,float epsilon=0.00001f);
BOOL _Vector3fAlmostCmp(Vector3f a,Vector3f b,float epsilon=0.00001f);
BOOL _Vector4fAlmostCmp(Vector4f a,Vector4f b,float epsilon=0.00001f);

BOOL _Vector3fAlmostCmpExceptY(Vector3f a,Vector3f b,float epsilon=0.00001f);

/**
a에서 b까지의 alpha 보간
*/
void _Vector4fInterpolate(Vector4f get, Vector4f a ,Vector4f b, float alpha);
/**
a에서 b까지의 alpha 보간
*/
void _Vector3fInterpolate(Vector3f get, Vector3f a ,Vector3f b, float alpha);
/**
a에서 b까지의 alpha 보간
*/
void _Vector2fInterpolate(Vector2f get, Vector2f a ,Vector2f b, float alpha);
/**
a에서 b까지의 alpha 보간
*/
void _Vector1fInterpolate(float *get, float a ,float b, float alpha);

#endif 
