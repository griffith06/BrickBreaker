#ifndef __JMALLOC_H__
#define __JMALLOC_H__

/**
@file     JMalloc.h
@brief    메모리 잡고 해제하는 모듈.


Dmalloc을 통하면 동적인 메모리상황을 알수가 있다.\n\n
몇번에 걸쳐 얼마큼의 메모리를 잡는지를 알수있기 때문에 전체메모리 호출회수의 fragment도 알수 있다\n\n
(메모리는 fragment가 많을수록 크게 잡을수록 OS차원의 SWAP이라던지 프로그램의 시작과 종료속도가 느려진다던지 좋지않은 현상들이 발생한다.).\n\n
적은 메모리 호출과 적은 메모리 사용을 생활화하자\n\n
DEBUG 컴파일시 소스어느 위치에서 잡는지 정보도 포함하고 있다.\n\n

*/

#include "CommonDef.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_DEBUG) & (defined(WIN32) || defined(_WIN64))
#define _MEM_DEBUG		//릴리즈시 넣지말것.
#endif

#define SAFE_DFREE(p)	{ if (p) { Dfree (p);     (p)=NULL; } }

#ifdef _MEM_DEBUG
void *DmallocDebug(int size, const char *file = __FILE__,int line = __LINE__);
void *DmallocDebugAndMemset(int size,const char *file = __FILE__,int line = __LINE__);
#define Dmalloc(a) DmallocDebug(a,__FILE__,__LINE__)
#define DmallocAndMemset(a) DmallocDebugAndMemset(a,__FILE__,__LINE__)
void Dfree(void *fptr);
void Dfree64(void* fptr);
int GetDmallocSize();
uint32 GetDmallocCnt();
void *ReAllocDebug(void *buf,int old_size,int size, const char *file = __FILE__,int line = __LINE__);
void *ReAllocAndMemset(void *buf,int old_size,int size, const char *file = __FILE__,int line = __LINE__);
#define ReAlloc(a,b,c) ReAllocDebug(a,b,c,__FILE__,__LINE__)
void* DmallocAndMemset64(int64 size , const char* file = __FILE__, int line = __LINE__);
void* ReAllocAndMemset64(void* buf, int64 old_size, int64 size, const char* file = __FILE__, int line = __LINE__);

void DMemoryReport();

#else
//---------동적 메모리 관리를 위한 것.
void *Dmalloc(int size);
void *DmallocAndMemset(int size);
void *DmallocA16(int size);
void Dfree(void *fptr);
void Dfree64(void* fptr);
int GetDmallocSize();
uint32 GetDmallocCnt();
void *ReAlloc(void *buf,int old_size,int size);
void *ReAllocAndMemset(void *buf,int old_size,int size);
void* DmallocAndMemset64(int64 size);
void* ReAllocAndMemset64(void* buf, int64 old_size, int64 size);

void DMemoryReport();
#endif



//다이나믹 할당클래스. 벡터와 기능이 거의같고 
//간단한 선형구조의 메모리를 유지할수 있으며 적절한 양의 메모리를 개발자가 조절할 수있다.

struct DAlloc {
	int Cnt;
	int MaxCnt;
	int IncCnt;
	int StructSize;
	uint8 *Buf;
	DAlloc() { Clear(); }

	void Clear() { memset(this, 0, sizeof(DAlloc)); }
	void Create(int start_cnt, int inc_cnt, int struct_size)
	{
		Cnt = 0;
		MaxCnt = start_cnt;
		StructSize = struct_size;
		IncCnt = inc_cnt;
		Buf = (uint8 *)DmallocAndMemset(StructSize*MaxCnt);
	}
	int GetUsedMemSize() 
	{ 
		return StructSize * MaxCnt; 
	}
	void Release()
	{
		Dfree(Buf);
		memset(this, 0, sizeof(DAlloc));
	}
	void SetCnt(int count) { Cnt = count; }
	void *Add(void* source)
	{
		memcpy(&Buf[StructSize*Cnt], source, StructSize);
		Increse();
		return &Buf[StructSize*(Cnt - 1)];
	}
	void Increse()
	{
		Cnt++;
		if (Cnt >= MaxCnt)
		{
			Buf = (uint8 *)ReAllocAndMemset(Buf, StructSize*MaxCnt, StructSize*(MaxCnt + IncCnt));
			MaxCnt += IncCnt;
		}
	}
	void ResizeTo(int re_size)	//increase에서 체크하므로 +1을 가지고는 있어야 함.
	{
		Buf = (uint8 *)ReAllocAndMemset(Buf, StructSize*Cnt, StructSize*(re_size + 1));
		MaxCnt = re_size + 1;
	}
	int Num()	const { return Cnt; }
	int size()	const { return Cnt; }
	void Delete(int index, int count = 1)
	{
		if (Cnt == 0)
		{
			return;
		}
		if (count > Cnt - index)
		{
			count = Cnt - index;
		}
		if (Cnt - index - count > 0)
		{
			memcpy(&Buf[StructSize*index], &Buf[StructSize*(index + count)], StructSize*(Cnt - index - count));
		}
		Cnt -= count;
		if (Cnt < 0)
		{
			Cnt = 0;
		}
	}
	void *GetPtr(int index = 0)	const { return (void*)(&Buf[StructSize*index]); }
	bool IsValid(int index)		const { if (index >= Cnt || index < 0) return false;	return true;
}
	int GetIndex(void *ptr)		const { return (int)((uint8*)ptr - Buf)/StructSize; }
	void *GetNowPtr()			const   { return (void*)(&Buf[StructSize*Cnt]);	 }
	void *GetNowPtr(int struct_size)	
	{ 
		if( !Buf )
			Create(1,1,struct_size);
		return GetNowPtr();
	}
	//버퍼크기는 확보후 복사한다.
	void Copy(const void *src, int count)	
	{
		memcpy(Buf, src, StructSize*count);
		Cnt = count;
	}

	// WARNING : 이전 코드는 Buf에 메모리가 이미 할당 되어 있어도 Delete하지 않고
	// 그대로 덮어씌운다. 따라서 메모리 누수가 발생한다.
	// 20220517 :이를 방지하기 위해 if문으로 먼저 릴리즈한다.
	// 만약 문제가 발생할 경우, Copy자체를 고민해 볼 것.
	void Copy(const DAlloc *src)
	{
		if (Buf != nullptr)
			Release();

		Create(src->Num(), 1, src->StructSize);
		Copy(src->GetPtr(), src->Num());
	}
};

struct DAlloc64 {
	int64 Cnt;
	int64 MaxCnt;
	int64 IncCnt;
	int64 StructSize;
	uint8* Buf;
	DAlloc64() { Clear(); }

	void Clear() { memset(this, 0, sizeof(DAlloc64)); }
	void Create(int64 start_cnt, int64 inc_cnt, int64 struct_size)
	{
		Cnt = 0;
		MaxCnt = start_cnt;
		StructSize = struct_size;
		IncCnt = inc_cnt;
		Buf = (uint8*)DmallocAndMemset64(StructSize * MaxCnt);
	}
	int64 GetUsedMemSize()
	{
		return StructSize * MaxCnt;
	}
	void Release()
	{
		Dfree64(Buf);
		memset(this, 0, sizeof(DAlloc64));
	}
	void SetCnt(int64 count) { Cnt = count; }
	void* Add(void* source)
	{
		memcpy(&Buf[StructSize * Cnt], source, StructSize);
		Increse();
		return &Buf[StructSize * (Cnt - 1)];
	}
	void Increse()
	{
		Cnt++;
		if (Cnt >= MaxCnt)
		{
			Buf = (uint8*)ReAllocAndMemset64(Buf, StructSize * MaxCnt, StructSize * (MaxCnt + IncCnt));
			MaxCnt += IncCnt;
		}
	}
	void ResizeTo(int64 re_size)	//increase에서 체크하므로 +1을 가지고는 있어야 함.
	{
		Buf = (uint8*)ReAllocAndMemset64(Buf, StructSize * Cnt, StructSize * (re_size + 1));
		MaxCnt = re_size + 1;
	}
	int64 Num()	const { return Cnt; }
	int64 size()	const { return Cnt; }
	void Delete(int64 index, int64 count = 1)
	{
		if (Cnt == 0)
		{
			return;
		}
		if (count > Cnt - index)
		{
			count = Cnt - index;
		}
		if (Cnt - index - count > 0)
		{
			memcpy(&Buf[StructSize * index], &Buf[StructSize * (index + count)], StructSize * (Cnt - index - count));
		}
		Cnt -= count;
		if (Cnt < 0)
		{
			Cnt = 0;
		}
	}
	void* GetPtr(int64 index = 0)	const { return (void*)(&Buf[StructSize * index]); }
	bool IsValid(int64 index)		const {
		if (index >= Cnt || index < 0) return false;	return true;
	}
	int64 GetIndex(void* ptr)		const { return (int64)((uint8*)ptr - Buf) / StructSize; }
	void* GetNowPtr()			const { return (void*)(&Buf[StructSize * Cnt]); }
	void* GetNowPtr(int64 struct_size)
	{
		if (!Buf)
			Create(1, 1, struct_size);
		return GetNowPtr();
	}
	//버퍼크기는 확보후 복사한다.
	void Copy(const void* src, int64 count)
	{
		memcpy(Buf, src, StructSize * count);
		Cnt = count;
	}

	// WARNING : 이전 코드는 Buf에 메모리가 이미 할당 되어 있어도 Delete하지 않고
	// 그대로 덮어씌운다. 따라서 메모리 누수가 발생한다.
	// 20220517 :이를 방지하기 위해 if문으로 먼저 릴리즈한다.
	// 만약 문제가 발생할 경우, Copy자체를 고민해 볼 것.
	void Copy(const DAlloc64* src)
	{
		if (Buf != nullptr)
			Release();

		Create(src->Num(), 1, src->StructSize);
		Copy(src->GetPtr(), src->Num());
	}
};


//공용메모리 사용하기위한.. 함수...F = File 파일메모리다...ㅋ
void *Fmalloc(int size,char *identity);
void *Fmalloc(int size,wchar_t *identity);
void Ffree(void *ptr);











/**
	Dmalloc 을 조그맣고 잦은 카운팅을 할때 메모리 카운트횟수를 줄여주는 관리자이다.
	사용법은 다음과 같다.

	Gf_DmallocManager Test(sizeof(DWORD),128);   //한번에 통짜 블럭이 128개이다.하나의블럭사이즈 = sizeof(DWORD)*128
	그리고 Dmalloc쓰던곳을 Test.GetDmalloc(); 로 교체하고 Dfree쓰는곳을 Test.ReleaseDfree(ptr)로 교체한다.
	
	위의경우 총 65개의 Dmalloc를 호출하게 되는 경우. 내부에서 통짜블럭이 두개생긴다. 즉 총메모리는 sizeof(DWORD)*128*2 = 1024byte + 약간의 캐쉬플래그사이즈이다.
	메모리호출횟수가 많을수록 필요이상의 추가메모리블럭과 원치않는 하드스왑이 생길수가 있다.
	따라서 잦은 메모리호출과 전체메모리 사용량을 줄이는데 노력해야한다.
*/


struct Gf_ForceDMallocComp{
	uint8 *Buf;
	uint32 *State;
};
class Gf_DmallocManager{
	int mStructSize;			///< 메모리할당 구조체 사이즈
	int mWidth;					///< 한개의 통짜블럭의 갯수.  하나의블럭사이즈 = mStructSize*mWidth
	int mPage;					///< 페이즈 갯수. 하나의 블럭사이즈를 넘게되면 다음페이지를 오픈한다.
	Gf_ForceDMallocComp *mComp;	///< 실제 버퍼와 할당과 릴리즈여부를 알수있는 카운트.

public:
	Gf_DmallocManager(int struct_size,int width);
	~Gf_DmallocManager();
	uint8 *GetDmalloc();
	void ReleaseDfree(void *ptr);

	bool IsRelease(void *ptr);
};
class Gf_First {
	int bBegin;
public:	
	Gf_First();
	~Gf_First();
};


#endif
