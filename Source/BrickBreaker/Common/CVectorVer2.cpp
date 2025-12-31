#include "CVectorVer2.h"
#include "windows.h"


/**
@brief 두벡터를 비교한다epsilon 범위까지는 같은걸로 친다.
@return 같으면 TRUE,틀리면 FALSE
@param a 비교대상1
@param b 배교대상2

*/
BOOL _Vector1fAlmostCmp(float a,float b,float epsilon)									
{
	if( a < b + epsilon && a > b - epsilon)
		return TRUE;												
	return FALSE;												
}
/**
@brief 두벡터를 비교한다.epsilon 범위까지는 같은걸로 친다.
@return 같으면 TRUE,틀리면 FALSE
@param a 비교대상1
@param b 배교대상2

*/
BOOL _Vector2fAlmostCmp(Vector2f a,Vector2f b,float epsilon)									
{																
	int almosti;												
	for(almosti=0; almosti<2; almosti++)						
	{															
		if( !_Vector1fAlmostCmp(a[almosti],b[almosti],epsilon) )
			return FALSE;												
	}															
	return TRUE;												
}																

/**
@brief 두벡터를 비교한다.epsilon 범위까지는 같은걸로 친다.
@return 같으면 TRUE,틀리면 FALSE
@param a 비교대상1
@param b 배교대상2

*/
BOOL _Vector3fAlmostCmp(Vector3f a,Vector3f b,float epsilon)									
{																
	int almosti;												
	for( almosti=0; almosti<3; almosti++)						
	{															
		if( !_Vector1fAlmostCmp(a[almosti],b[almosti],epsilon) )
			return FALSE;												
	}															
	return TRUE;												
}																
/**
@brief 두벡터를 비교한다.0.0001 일경우 0.0002 또는 0.0000까지는 같은걸로 친다.
@return 같으면 TRUE,틀리면 FALSE
@param a 비교대상1
@param b 배교대상2

*/
BOOL _Vector4fAlmostCmp(Vector4f a,Vector4f b,float epsilon)									
{																
	int almosti;												
	for( almosti=0; almosti<4; almosti++)						
	{															
		if( !_Vector1fAlmostCmp(a[almosti],b[almosti],epsilon) )
			return FALSE;												
	}															
	return TRUE;												
}			
/**
@brief 두벡터를 y값을 제외하고 비교한다.0.0001 일경우 0.0002 또는 0.0000까지는 같은걸로 친다.
@return 같으면 TRUE,틀리면 FALSE
@param a 비교대상1
@param b 배교대상2

*/
BOOL _Vector3fAlmostCmpExceptY(Vector3f a,Vector3f b,float epsilon)									
{																
	int almosti;												
	for( almosti=0; almosti<3; almosti+=2)						
	{															
		if( !_Vector1fAlmostCmp(a[almosti],b[almosti],epsilon) )
			return FALSE;												
	}															
	return TRUE;												
}	
/**
@brief a에서 b까지의 alpha 보간
@return 첫번째 매개변수로 리턴
@param get 리턴값
@param a 벡터1
@param b 벡터2
@param alpha 보간값

*/
void _Vector4fInterpolate(Vector4f get, Vector4f a ,Vector4f b, float alpha)
{
	Vector4f temp;
	_Vector4fSub(temp,b,a);
	_Vector4fMul(temp,temp,alpha);
	_Vector4fAdd(get,temp,a);
}
/**
@brief a에서 b까지의 alpha 보간
@return 첫번째 매개변수로 리턴
@param get 리턴값
@param a 벡터1
@param b 벡터2
@param alpha 보간값

*/

void _Vector3fInterpolate(Vector3f get, Vector3f a ,Vector3f b, float alpha)
{
	Vector3f temp;
	_Vector3fSub(temp,b,a);
	_Vector3fMul(temp,temp,alpha);
	_Vector3fAdd(get,temp,a);
}
/**
@brief a에서 b까지의 alpha 보간
@return 첫번째 매개변수로 리턴
@param get 리턴값
@param a 벡터1
@param b 벡터2
@param alpha 보간값

*/

void _Vector1fInterpolate(float *get, float a ,float b, float alpha)
{
	float temp;
	temp=b-a;
	temp*=alpha;
	*get=temp+a;
}

/**
@brief a에서 b까지의 alpha 보간
@return 첫번째 매개변수로 리턴
@param get 리턴값
@param a 벡터1
@param b 벡터2
@param alpha 보간값

*/

void _Vector2fInterpolate(Vector2f get, Vector2f a ,Vector2f b, float alpha)
{
	_Vector1fInterpolate(&get[0],a[0],b[0],alpha);
	_Vector1fInterpolate(&get[1],a[1],b[1],alpha);
}

/** 
평면의 U와 V 축을 대표하는 적절한 임의의 축 벡터를 찾기.
*/
void _Vector3fFindBestAxis( Vector3f Axis1, Vector3f Axis2, const Vector3f basis )
{
	const FLOAT NX = (float)fabs(basis[0]);
	const FLOAT NY = (float)fabs(basis[1]);
	const FLOAT NZ = (float)fabs(basis[2]);

	// Find best basis vectors.
	if( NZ>NX && NZ>NY )	
	{
		Axis1[0] = 1;		Axis1[1] = 0;		Axis1[2] = 0;
	}
	else
	{
		Axis1[0] = 0;		Axis1[1] = 0;		Axis1[2] = 1;
	}
	Vector3f Temp;
	float dotAB =_Vector3fDot(Axis1,basis);
	_Vector3fMul(Temp,basis,dotAB);
	_Vector3fSub(Axis1,Axis1,Temp);
	_Vector3fNormalize(Axis1);
//	_Vector3fCross(Axis2,basis,Axis1);
	_Vector3fCross(Axis2,Axis1,basis);
}
