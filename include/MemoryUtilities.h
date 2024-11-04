#pragma once 

#include <crtdbg.h>
#include <Windows.h>
#include <psapi.h>

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC  // Maps malloc/free to their debug versions
    #define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )  // Replace new with debug version
#endif


class HeapChecker {
private:
	const char *m_pFunction;
	const char *m_pFile;
public:
	HeapChecker(const char *pFun, const char *pFile) : m_pFunction(pFun), m_pFile(pFile){}
	~HeapChecker();
};


#ifdef _DEBUG
	#define HEAPCHECK HeapChecker instance{__FUNCSIG__, __FILE__};
#else
	#define HEAPCHECK
#endif


class MemoryCheckpoint
{
public:
	MemoryCheckpoint(const char* pFile, const char* pFunction);
	~MemoryCheckpoint();

private:
	_CrtMemState m_Begin;
	const char *m_pFile;
	const char *m_pFunction;
};

#ifdef _DEBUG
#define MEMORYCHECKPOINT MemoryCheckpoint cp{__FILE__, __FUNCSIG__};
#else
#define MEMORYCHECKPOINT 
#endif // _DEBUG

void printMemoryUsage();