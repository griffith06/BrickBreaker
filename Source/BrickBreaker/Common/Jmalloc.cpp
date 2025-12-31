#include "Jmalloc.h"

#ifdef  WIN32
#include <crtdbg.h>
#endif


//#define _TRACE_MEMORY_FOOT_PRINT

#ifndef WIN32
#define _aligned_malloc(a,b)		malloc(a)
#define _aligned_free				free
#endif

//---------------------------------- 동적인 메모리 관리 -------------------------------
static int64 Dmalloc_size = 0;
static uint32 DmallocCnt = 0;

/**
@brief 전체 할당된 메모리 사이즈
@return 메모리 사이즈를 리턴합니다.
*/
int GetDmallocSize()
{
	return Dmalloc_size;
}
/**
@brief 전체 할당된 메모리 갯수
@return 메모리 갯수를 리턴합니다.
*/
uint32 GetDmallocCnt()
{
	return DmallocCnt;
}


#ifdef _MEM_DEBUG	//메모리 디버깅할 필요가 있을때.
class CJMemDebug{
public:
	CJMemDebug();
	~CJMemDebug();
};

struct _JMEM_DEBUG{
	uint32 size;
	char *file;
	int line;
	void *fptr;
};
_JMEM_DEBUG *stJMemDbug;

static uint32 stJMemDbugMaxCnt,stJMemDbugCnt=0;

CJMemDebug::CJMemDebug()
{
	//LFH(Low Fragmentation Heap) 사용. 메모리 단편화방지라는데...
	HANDLE heaps[1025];
	uint32 nheaps = GetProcessHeaps(1024, heaps);
	for (uint32 i = 0; i < nheaps; i++)
	{
		ULONG  HeapFragValue = 2;
		HeapSetInformation(heaps[i],
			HeapCompatibilityInformation,
			&HeapFragValue,
			sizeof(HeapFragValue));
	}
}
CJMemDebug::~CJMemDebug()
{
}
static CJMemDebug tt;
/**
@brief 메모리 디버그 리스트에서 삭제한다.
@return 반환값이 없습니다.
@param id 메모리디버그 리스트의 인덱스 
*/
static void DeleteJMemDebugList(uint32 id)
{
	if( stJMemDbugCnt <= 0 )
		return;
	if((stJMemDbugCnt-id-1) > 0)
	{
		memcpy(&stJMemDbug[id],&stJMemDbug[id+1]
			,sizeof(_JMEM_DEBUG)*(stJMemDbugCnt-id-1));
	}
	stJMemDbugCnt--;
}

void SetSize(void * ptr, int size)
{
	if (size == 16)
	{
		int dfdf = 0;
	}
	((int *)ptr)[0] = size;
}
void SetSize64(void* ptr, int64 size)
{
	if (size == 16)
	{
		int dfdf = 0;
	}
	((int64*)ptr)[0] = size;
}
/**
@brief 디버거에 사용될 메모리 할당
@return 반환값이 없습니다.
@param size 사이즈
@param file 문자열
@param line 해당되는 라인

*/
inline void *DmallocDebug(int size, const char *file,int line)
{
	//Lock lock(g_MemCs);

	if(size == 0)
		return NULL;
	uint8 *ptr=(uint8 *)_malloc_dbg(size+4, _NORMAL_BLOCK, file, line);
	if( !ptr )
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;

//	if( ptr == (BYTE *)0x01E9ED80 )
//		int dfsdfsd=0;
//  	if(  size == 37380-4 || size == 67332-4 )// 262144은 demalloc 디버깅정보 잡는 양이다..ㅡㅡ
// 		int dfsdfsd=0;

#ifdef _TRACE_MEMORY_FOOT_PRINT
	if( stJMemDbug == NULL )
	{
		stJMemDbugMaxCnt=10000;
		stJMemDbugCnt=0;
		stJMemDbug = (_JMEM_DEBUG *)malloc(sizeof(_JMEM_DEBUG)*stJMemDbugMaxCnt);
	}

	stJMemDbug[stJMemDbugCnt].file = file;
	stJMemDbug[stJMemDbugCnt].line = line;
	stJMemDbug[stJMemDbugCnt].size = size;
	stJMemDbug[stJMemDbugCnt].fptr = (void*)(ptr+4);
	stJMemDbugCnt++;
	if( stJMemDbugCnt >= stJMemDbugMaxCnt )
	{
		_JMEM_DEBUG *temp_p = (_JMEM_DEBUG *)malloc(sizeof(_JMEM_DEBUG)*(stJMemDbugMaxCnt+5000));
		memcpy(temp_p,stJMemDbug,sizeof(_JMEM_DEBUG)*stJMemDbugMaxCnt);
		free(stJMemDbug);
		stJMemDbug = temp_p;
		stJMemDbugMaxCnt+=5000;
	}
#endif
	SetSize(ptr, size);
//	((int *)ptr)[0]=size;

    // 메모리 프로파일링
    // 할당된 메모리 정보를 수집
    // 첫 4바이트는 할당한 메모리 사이즈 정보를 저장하고 있다.
    // 사이즈 헤더 메모리 영역은 등록하지 않는다.
   // Gf_CAllocator::RegistAlloc(NULL, (void *)(ptr + 4), size);

	return((void *)(ptr+4));
}
inline void* Dmalloc64(int64 size, const char* file, int line)
{
	//Lock lock(g_MemCs);

	if (size == 0)
		return NULL;
	uint8* ptr = (uint8*)_malloc_dbg(size + 8, _NORMAL_BLOCK, file, line);
	if (!ptr)
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;

	SetSize64(ptr, size);

	return((void*)(ptr + 8));
}
void DMemoryReport()
{
	FILE *fp=fopen("dmemdump.txt","wt");

	uint32 i;
	for(i=0;i<stJMemDbugCnt;i++)
	{
		fprintf(fp, "%s(%d) ; %dByte\r\n", stJMemDbug[i].file, stJMemDbug[i].line, stJMemDbug[i].size);
	}

	fclose(fp);
}

void* DmallocAndMemset64(int64 size, const char* file, int line)
{
	if (size == 0)
		return NULL;

	uint8* ptr = (uint8*)Dmalloc64(size, file, line);

	if (!ptr)
		return NULL;
	memset(ptr, 0, size);
	ptr -= 8;

	return((void*)(ptr + 8));
}

void *DmallocDebugAndMemset(int size, const char *file,int line)
{
	if(size == 0)
		return NULL;

	uint8 *ptr=(uint8 *)DmallocDebug(size, file, line);
	
	if( !ptr )
		return NULL;
	memset(ptr, 0, size);
	ptr -= 4;
	
	return((void *)(ptr+4));
}

/**
@brief 할당된 메모리를 해제합니다.
@return 반환값이 없습니다.
@param fptr 메모리 주소
*/
void Dfree(void *fptr)
{
	if(fptr == NULL)
		return;
	
#ifdef _TRACE_MEMORY_FOOT_PRINT
	bool bFound=FALSE;
	for(uint32 i=0; i<stJMemDbugCnt; i++)
	{
		if( fptr == stJMemDbug[i].fptr )
		{
			bFound=TRUE;
			DeleteJMemDebugList(i);
			break;
		}
	}
	if( !bFound )
	{
		return;
	}
	else
#endif
	{
		Dmalloc_size-=((int *)fptr)[-1];
		DmallocCnt--;
	}

	
	if( DmallocCnt==0 && stJMemDbugCnt == 0)
 	{
 		free(stJMemDbug);
		stJMemDbug=NULL;
 		OutputDebugStringW(L"------- Gf_Engine  Dmalloc Cnt = 0, finish!---------\n");
 	}
 
    // 메모리 프로파일링: 할당된 메모리 정보를 수집
    //Gf_CAllocator::UnregistFree((char*)fptr);

	free(((char *)fptr-4));
}
void Dfree64(void* fptr)
{
	if (fptr == NULL)
		return;
	{
		Dmalloc_size -= ((int64*)fptr)[-1];
		DmallocCnt--;
	}


	if (DmallocCnt == 0 && stJMemDbugCnt == 0)
	{
		free(stJMemDbug);
		stJMemDbug = NULL;
		OutputDebugStringW(L"------- Gf_Engine  Dmalloc Cnt = 0, finish!---------\n");
	}

	free(((char*)fptr - 8));
}
#pragma warning( push )
#pragma warning ( disable : 4100 )		// warning C4100: 'line' : 참조되지 않은 형식 매개 변수입니다.

/**
@brief 메모리를 새로 할당받는다.
@return 반환값이 없습니다.
@param buf 할당받는 메모리 주소
@param old_size 이전 메모리 사이즈
@param size 할당받는 메모리 사이즈
@param file 문자열
@param line 해당라인

*/

void *ReAllocDebug(void *buf,int old_size,int size, const char *file,int line)
{
	//Lock lock(g_MemCs);

	uint8 *temp = (uint8 *)Dmalloc(size);
	old_size=Gf_min(old_size,size);		//만약 새로 할당 받는 사이즈가 적다면...
	memcpy(temp,buf,old_size);
	Dfree(buf);

	return (void *)temp;
}
/**
@brief 메모리를 새로 할당받는다
@return 반환값이 없습니다.
@param buf 메모리 주소
@param old_size 이번 메모리 사이즈
@param size 새로 할당된 사이즈
@param file 문자열
@param line 해당라인
*/
void *ReAllocAndMemset(void *buf,int old_size,int size, const char *file,int line)
{
	//Lock lock(g_MemCs);

	uint8 *temp = (uint8 *)Dmalloc(size);
	old_size=Gf_min(old_size,size);		//만약 새로 할당 받는 사이즈가 적다면...
	memcpy(temp,buf,old_size);
	Dfree(buf);
	memset( ((uint8*)temp+old_size), 0, size-old_size);

	return (void *)temp;
}

void* ReAllocAndMemset64(void* buf, int64 old_size, int64 size, const char* file, int line)
{
	uint8* temp = (uint8*)Dmalloc64(size, file, line);
	old_size = Gf_min(old_size, size);		//만약 새로 할당 받는 사이즈가 적다면...
	memcpy(temp, buf, old_size);
	Dfree64(buf);
	memset(((uint8*)temp + old_size), 0, size - old_size);

	return (void*)temp;
}

#pragma warning( pop )	//워닝 pop


#else
/**
@brief 메모리를 할당받는다.
@return 반환값은 할당된 메모리포인터입니다. 
@param size 메모리 사이즈

*/

void *Dmalloc(int size)
{
	//Lock lock(g_MemCs);

	if(size == 0)
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;
	uint8 *ptr=(uint8 *)malloc(size+4);
//	if( size >= 3810 && size < 3830)
//		int dfkjd=0;
//	if( (BYTE *)0x0053C960 == ptr )	//메모리 새는곳 디버그용으로 캡...
//		int ttest=0;
//	if( size == 2028 || size == 2112 )
//		int dfkjd=0;
	if( !ptr )
		return NULL;
	((int *)ptr)[0]=size;

    // 메모리 프로파일링
    // 할당된 메모리 정보를 수집
    // 첫 4바이트는 할당한 메모리 사이즈 정보를 저장하고 있다.
    // 사이즈 헤더 메모리 영역은 등록하지 않는다.
    //Gf_CAllocator::RegistAlloc(NULL, (void *)(ptr + 4), size);

	return((void *)(ptr+4));
}
void* Dmalloc64(int64 size)
{
	//Lock lock(g_MemCs);

	if (size == 0)
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;
	uint8* ptr = (uint8*)malloc(size + 8);
	if (!ptr)
		return NULL;
	((int64*)ptr)[0] = size;

	return((void*)(ptr + 8));
}

/**
@brief 메모리를 할당받는다.
@return 반환값이 메모리포인터입니다.
@param size 메모리 사이즈

*/

void *DmallocAndMemset(int size)
{
	//Lock lock(g_MemCs);

	if(size == 0)
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;
	uint8 *ptr=(uint8 *)malloc(size+4);
	if( !ptr )
		return NULL;
	memset(ptr,0,size+4);
	((int *)ptr)[0]=size;

    // 메모리 프로파일링
    // 할당된 메모리 정보를 수집
    // 첫 4바이트는 할당한 메모리 사이즈 정보를 저장하고 있다.
    // 사이즈 헤더 메모리 영역은 등록하지 않는다.
    //Gf_CAllocator::RegistAlloc(NULL, (void *)(ptr + 4), size);

	return((void *)(ptr+4));
}
void* DmallocAndMemset64(int64 size)
{
	//Lock lock(g_MemCs);

	if (size == 0)
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;
	uint8* ptr = (uint8*)malloc(size + 4);
	if (!ptr)
		return NULL;
	memset(ptr, 0, size + 8);
	((int64*)ptr)[0] = size;

	// 메모리 프로파일링
	// 할당된 메모리 정보를 수집
	// 첫 4바이트는 할당한 메모리 사이즈 정보를 저장하고 있다.
	// 사이즈 헤더 메모리 영역은 등록하지 않는다.
	//Gf_CAllocator::RegistAlloc(NULL, (void *)(ptr + 4), size);

	return((void*)(ptr + 8));
}

/**
@brief 16바이트 정렬된 메모리를 할당받는다.
@return 반환값이 없습니다. 
@param size 메모리 사이즈

*/
void *DmallocA16(int size)
{
	//Lock lock(g_MemCs);

	if(size == 0)
		return NULL;
	DmallocCnt++;
	Dmalloc_size += size;
	uint8 *ptr=(uint8 *)_aligned_malloc(size+16,16);
	if( !ptr )
		return NULL;
	((int *)ptr)[0]=size;

    // 메모리 프로파일링
    // 할당된 메모리 정보를 수집
    //Gf_CAllocator::RegistAlloc(NULL, (void *)(ptr + 16), size);

	return((void *)(ptr+16));
}

/**
@brief 메모리를 해제합니다.
@return 반환값이 없습니다.
@param fptr 메모리 주소

*/

void DfreeA16(void *fptr)
{
	if(fptr == NULL)
		return;
	Dmalloc_size-=((int *)fptr)[-1];
	DmallocCnt--;

    // 메모리 프로파일링: 할당된 메모리 정보를 수집
    //Gf_CAllocator::UnregistFree((char*)fptr);

	_aligned_free(((char *)fptr-16));
}
/**
@brief 메모리를 해제합니다.
@return 반환값이 없습니다.
@param fptr 메모리 주소

*/

void Dfree(void *fptr)
{
	//Lock lock(g_MemCs);

	if(fptr == NULL)
		return;
	Dmalloc_size-=((int *)fptr)[-1];
	DmallocCnt--;
	//	if( DmallocCnt < 10)
	//		int dfdfdf=0;

    // 메모리 프로파일링: 할당된 메모리 정보를 수집
    //Gf_CAllocator::UnregistFree( ((char *)fptr-4) );

	free(((char *)fptr-4));
}
void Dfree64(void* fptr)
{
	if (fptr == NULL)
		return;
	{
		Dmalloc_size -= ((int64*)fptr)[-1];
		DmallocCnt--;
	}

	free(((char*)fptr - 8));
}
/**
@brief 메모리 재할당받는다.
@return 반환값이 없습니다.
@param buf 메모리 주소
@param old_size 이전 사이즈
@param size 새로할당받는 사이즈
*/

void *ReAlloc(void *buf,int old_size,int size)
{
	//Lock lock(g_MemCs);

	uint8 *temp = (uint8 *)Dmalloc(size);
	old_size=Gf_min(old_size,size);		//만약 새로 할당 받는 사이즈가 적다면...
	memcpy(temp,buf,old_size);
//	if( (BYTE *)0x0051B030 == temp )	//메모리 새는곳 디버그용으로 캡...
//		int ttest=0;
//	if( size == 2028 || size == 2112 )
//		int dfkjd=0;
	Dfree(buf);

	return (void *)temp;
}
/**
@brief 메모리 재할당받는다.
@return 반환값이 없습니다.
@param buf 메모리 주소
@param old_size 이전 사이즈
@param size 새로할당받는 사이즈
*/
void *ReAllocAndMemset(void *buf,int old_size,int size)
{
	//Lock lock(g_MemCs);

	uint8 *temp = (uint8 *)Dmalloc(size);
	old_size=Gf_min(old_size,size);		//만약 새로 할당 받는 사이즈가 적다면...
	memcpy(temp,buf,old_size);
	Dfree(buf);
	memset( ((uint8*)temp+old_size), 0, size-old_size);

	return (void *)temp;
}
void* ReAllocAndMemset64(void* buf, int64 old_size, int64 size)
{
	//Lock lock(g_MemCs);

	uint8* temp = (uint8*)Dmalloc64(size);
	old_size = Gf_min(old_size, size);		//만약 새로 할당 받는 사이즈가 적다면...
	memcpy(temp, buf, old_size);
	Dfree64(buf);
	memset(((uint8*)temp + old_size), 0, size - old_size);

	return (void*)temp;
}


void DMemoryReport()
{

}


#endif


#ifdef WIN32

//공용메모리 사용하기위한.. 함수...
struct _FMALLOC{
	void *ptr;
	HANDLE handle;
};

static int stMaxFCnt=16;
static int stFCnt=0;
_FMALLOC *stFmalloc=NULL;

/**
@brief 파일메모리 할당받는다.
@return 반환값이 없습니다.
@param size 파일메모리 사이즈
@param identity 매핑오브젝트의 문자열

*/
void *Fmalloc(int size,char *identity)
{
	void *ptr;
	if( !stFmalloc )
	{
		stFmalloc = (_FMALLOC *)Dmalloc(sizeof(_FMALLOC)*stMaxFCnt);
		stFCnt = 0;
	}

	HANDLE handle = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, identity);
	if( handle == 0 )
		return 0;
	ptr = (void *)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if( ptr == 0 )
		return 0;

	stFmalloc[stFCnt].handle = handle;
	stFmalloc[stFCnt].ptr = ptr;
	stFCnt++;
	if( stFCnt >= stMaxFCnt )
	{
		stFmalloc = (_FMALLOC *)ReAlloc(stFmalloc,sizeof(_FMALLOC)*stMaxFCnt,sizeof(_FMALLOC)*(stMaxFCnt*2));
		stMaxFCnt = stMaxFCnt*2;
	}

	return ptr;
}
/**
@brief 파일메모리 할당받는다.
@return 반환값이 없습니다.
@param size 파일메모리 사이즈
@param identity 매핑오브젝트의 문자열

*/
void *Fmalloc(int size,wchar_t *identity)
{
	void *ptr;
	if( !stFmalloc )
	{
		stFmalloc = (_FMALLOC *)Dmalloc(sizeof(_FMALLOC)*stMaxFCnt);
		stFCnt = 0;
	}

	HANDLE handle = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, identity);
	if( handle == 0 )
		return 0;
	ptr = (void *)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if( ptr == 0 )
		return 0;

	stFmalloc[stFCnt].handle = handle;
	stFmalloc[stFCnt].ptr = ptr;
	stFCnt++;
	if( stFCnt >= stMaxFCnt )
	{
		stFmalloc = (_FMALLOC *)ReAlloc(stFmalloc,sizeof(_FMALLOC)*stMaxFCnt,sizeof(_FMALLOC)*(stMaxFCnt*2));
		stMaxFCnt = stMaxFCnt*2;
	}

	return ptr;
}
/**
@brief 파일메모리를 해제합니다.
@return 반환값이 없습니다.
@param ptr 주소값

*/

void Ffree(void *ptr)
{
	int i;

	for(i=0; i<stFCnt; i++)
	{
		if( stFmalloc[i].ptr == ptr )
		{
			UnmapViewOfFile(stFmalloc[i].ptr);
			CloseHandle(stFmalloc[i].handle);
			memcpy(&stFmalloc[i],&stFmalloc[i+1]
				,sizeof(_FMALLOC)*(stFCnt-i-1));
			stFCnt--;
			break;
		}
	}

	if( stFCnt==0 )
	{
		Dfree(stFmalloc);
		stFmalloc=0;
	}
}


#endif

//----------------------------------------------- Dmalloc 을 조그맣고 잦은 카운팅을 할때 메모리 카운트횟수를 줄여주는 관리자이다.
#define Gf_SetForceDmallocFlag(buf,num)			(((BYTE*)buf)[num>>3]|=(1<<(num&7)))
#define Gf_UnSetForceDmallocFlag(buf,num)		(((BYTE*)buf)[num>>3]&=(~(1<<(num&7))) )	//끈다.
#define Gf_IsSetForceDmallocFlag(buf,num)		(((BYTE*)buf)[num>>3]&(1<<(num&7)))


Gf_DmallocManager::Gf_DmallocManager(int struct_size,int width)
{
//	if( width < 32 )
//		JError("Too small size","The Number is More than 32");
	width = Gf_max(32,width);
	mStructSize=struct_size;
	mPage=1;
	mWidth = width;
	mComp=(Gf_ForceDMallocComp*)DmallocAndMemset(sizeof(Gf_ForceDMallocComp)*mPage);
	mComp[0].Buf=(uint8*)DmallocAndMemset(struct_size*mWidth);
	mComp[0].State=(uint32*)DmallocAndMemset(sizeof(uint32)*((mWidth+31)/32));
}


Gf_DmallocManager::~Gf_DmallocManager()
{
	int i;
	for(i=0; i<mPage; i++)
	{
		if( mComp[i].Buf )
			Dfree(mComp[i].Buf);
		mComp[i].Buf=0;
		if( mComp[i].State )
			Dfree(mComp[i].State);
		mComp[i].State=0;
	}

	if(mComp)
		Dfree(mComp);
	mComp=0;
}

bool Gf_DmallocManager::IsRelease(void *ptr)
{
	int i;

	for(i=0; i<mPage; i++)
	{
		if( mComp[i].Buf <= ptr &&  mComp[i].Buf + mWidth*mStructSize > ptr )
		{
			return Gf_IsSetForceDmallocFlag(mComp[i].State,((uint8*)ptr-mComp[i].Buf)/mStructSize);
		}
	}
	return TRUE;	//없으니까 릴리즈 되지 않았을까..
}

uint8 *Gf_DmallocManager::GetDmalloc()
{
	int i,j,k;
	for(i=0; i<mPage; i++)
	{
		for(j=0; j<(mWidth+31)/32; j++)
		{
			if( mComp[i].State[j] == 0xffffffff )
				continue;
			for(k=0; k<32; k++)
			{
				if( Gf_IsSetForceDmallocFlag(mComp[i].State,k) )
					continue;
				Gf_SetForceDmallocFlag(mComp[i].State,k);
				memset(&mComp[i].Buf[ (j*32 + k)*mStructSize ],0,mStructSize);
				return &mComp[i].Buf[ (j*32 + k)*mStructSize ];
			}
		}
	}
	//빈공간이 없으므로 추가.
	mComp=(Gf_ForceDMallocComp*)ReAllocAndMemset(mComp,sizeof(Gf_ForceDMallocComp)*mPage,sizeof(Gf_ForceDMallocComp)*(mPage+1));
	mComp[mPage].Buf=(uint8*)DmallocAndMemset(mStructSize*mWidth);
	mComp[mPage].State=(uint32*)DmallocAndMemset(sizeof(uint32)*((mWidth+31)/32));
	Gf_SetForceDmallocFlag(mComp[mPage].State,0);
	mPage++;
	return mComp[mPage-1].Buf;
}
void Gf_DmallocManager::ReleaseDfree(void *ptr)
{
	int i,j;
	for(i=0; i<mPage; i++)
	{
		if( mComp[i].Buf <= ptr &&  mComp[i].Buf + mWidth*mStructSize > ptr )
		{
			Gf_UnSetForceDmallocFlag(mComp[i].State,((uint8*)ptr-mComp[i].Buf)/mStructSize);
			for(j=0; j<(mWidth+31)/32; j++)
			{
				if( mComp[i].State[j] != 0 )
					return;
			}
			//Dfree해야할듯.
			Dfree(mComp[i].Buf);
			Dfree(mComp[i].State);
			mComp[i].Buf=0;
			mComp[i].State=0;
			if(i != mPage-1 )
			{
				memcpy(&mComp[i],&mComp[i+1],sizeof(Gf_ForceDMallocComp)*(mPage-i-1));
			}
			mPage--;
			return;
		}
	}
	// 먼가 잘못 되었다. 유효하지 않은 포인터를 넘겼다.
//	JWarning("Wrong pointer!","<- Can't Free Pointer!","","force dmalloc module",_GF_LOG_MOUDLETYPE_FORCE_DMALLOC);
}


Gf_First::Gf_First()
{
	bBegin = 1;
}

Gf_First::~Gf_First()
{
	bBegin = 0;
}
