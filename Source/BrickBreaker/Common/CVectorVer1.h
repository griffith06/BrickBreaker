#ifndef __C_VECTOR1_H__
#define __C_VECTOR1_H__

/**
@file     CVectorVer1.h
@brief    CVector 1.0

CVector 1.0에서는 Vector3fCopy(s,d) 처럼 dest가 뒤에 있기때문에(오래된 C방식이다)\n\n
2.0애서는 _Vector3fCopy(d,s)로_를 앞에 붙여서 dest를 앞으로 다 땡겼다.\n\n
호환성때문에 1.0 디파인들이 있는데 작업시 _Vector로 쓰길바란다.\n\n
*/

#define Vector4fCopy(s,d) {d[0]=s[0];d[1]=s[1];d[2]=s[2];d[3]=s[3];}

#define Vector3lCopy(s,d) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}

#define Vector3fDist(v) (sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))
#define Vector3fDist2(v) ((v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))

#define Vector3fCopy(s,d) {d[0]=s[0];d[1]=s[1];d[2]=s[2];}
#define Vector3fAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define Vector3fMul(a,b,c) {c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;}
#define Vector3fDiv(a,b,c) {c[0]=a[0]/b;c[1]=a[1]/b;c[2]=a[2]/b;}
#define Vector3fSub(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];} 
#define Vector3fScale(a,b,c) {c[0]=b*a[0];c[1]=b*a[1];c[2]=b*a[2];}

#ifdef _USE_INTEL_COMPILER 
	float   Vector3fDot(float *a,float *b);
#else
	#define Vector3fDot(a,b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#endif

#define Vector3fAvg(a,b,c) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;c[2]=(a[2]+b[2])*0.5f;}
#define Vector3fCmp(a,b) (a[0]==b[0]&&a[1]==b[1]&&a[2]==b[2])
#define Vector3fCross(a,b,r) {  r[0] = (a[1] * b[2]) - (a[2] * b[1]); r[1] = (a[2] * b[0]) - (a[0] * b[2]); r[2] = (a[0] * b[1]) - (a[1] * b[0]);}

#define Vector2fCopy(s,d) {d[0]=s[0];d[1]=s[1];}
#define Vector2fCmp(a,b) (a[0]==b[0]&&a[1]==b[1])
#define Vector2fAvg(a,b,c) {c[0]=(a[0]+b[0])*0.5f;c[1]=(a[1]+b[1])*0.5f;}

#define Vector1fCopy(s,d) {d=s;}
#define Vector1fAvg(a,b,c) {c=(a+b)*0.5f;}

#endif 
