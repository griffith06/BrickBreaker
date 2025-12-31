#pragma once

typedef unsigned short	wchar_fix;	//고정 wide char.. window WCHAR 때문에 필요함.

typedef unsigned long	utf32;	/* at least 32 bits */
typedef unsigned short	utf16;	/* at least 16 bits */
typedef unsigned char	utf8;	/* typically 8 bits */

typedef unsigned char 		uint8;		///< 8-bit  unsigned.
typedef unsigned short int	uint16;		///< 16-bit unsigned.
typedef unsigned int		uint32;		///< 32-bit unsigned.
typedef unsigned long long	uint64;		///< 64-bit unsigned.

										// Signed base types.
typedef	signed char			int8;		///< 8-bit  signed.
typedef signed short int	int16;		///< 16-bit signed.
typedef signed int	 		int32;		///< 32-bit signed.
typedef signed long long	int64;		///< 64-bit signed.


#define Gf_max(a,b)            (((a) > (b)) ? (a) : (b))
#define Gf_min(a,b)            (((a) < (b)) ? (a) : (b))

#define _Gf_STOP_ 	while(1){}

#if defined(WIN32) || defined(_WIN64)

#define _PLATFORM_WINDOWS
#include <Windows.h>
#define Gf_GetTickCount		GetTickCount
#define Gf_strlwr			strlwr
#endif

#ifdef ANDROID
#define _PLATFORM_ANDROID
#endif

#ifdef iOS
#define _PLATFORM_iOS
#endif


#define		DEC_SINGLE( Type )\
	public:\
	static Type* Ins;\
	static Type* GetThis(void);\
//로컬 static방식. 스태틱 객체라 프로그램 종료시 객체가 남아있음
#define		DEF_SINGLE( Type )\
	Type* Type::Ins=NULL;\
	Type* Type::GetThis(void)\
	{\
		static Type StaticIns;\
		Ins = &StaticIns;\
		return &StaticIns; \
	}

/*		new 방식. 알아서 종료해야함.
private:\
static Type*	m_pThis;\
public:\
static Type* GetThis(void);\
static void Destroy(void);
public:\
#define		DEF_SINGLE( Type )\
	Type* Type::m_pThis = NULL;\
	Type* Type::GetThis(void)\
	{\
		if(!m_pThis)\
			m_pThis = new Type;\
		return m_pThis;\
	}\
	void Type::Destroy(void)\
	{\
		if(m_pThis)\
		{\
			delete m_pThis;\
			m_pThis = NULL;\
		}\
	}
*/

#define GET_SINGLE( T )			T::GetThis()
#define DESTROY_SINGLE( T )		T::Destroy()


#define GET_SET_VALUE( Type, name, Func )\
	protected:\
		Type	name;\
	public:\
		Type	Get##Func() { return name; }\
		void	Set##Func( Type a ) { name = a; }

#define GETTER_PTR( Type, Name )\
	protected:\
		Type	Name;\
	public:\
		Type*	Get##Name() { return &Name; }\

#define GETTER(T,member) T Get##member() { return member; }
#define SETTER(T, member) void Set##member(T value) { member =value; }
#define GSETTER(T, member) T member;  SETTER(T,member);  GETTER(T,member)
#define PGSETTER(T, member) public: T member;  SETTER(T,member); GETTER(T,member)



