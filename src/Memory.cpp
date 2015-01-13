#include "Memory.h"
#include "MallocTracker.h"

void* operator new(size_t size, const char* fileName, const char* functionName, int line)
{
	void* pMemory = ::operator new(size);

	MallocTracker::getInstance().alloc(pMemory, size, fileName, functionName, line);

	return pMemory;
}

void* operator new[](size_t size, const char* fileName, const char* functionName, int line)
{
	void* pMemory = ::operator new[](size);

	MallocTracker::getInstance().alloc(pMemory, size, fileName, functionName, line);

	return pMemory;
}

void operator delete(void * p, const char* fileName, const char* functionName, int line)
{
	MallocTracker::getInstance().freePtr(p);

	::operator delete(p);
}

void operator delete[](void * p, const char* fileName, const char* functionName, int line)
{
	MallocTracker::getInstance().freePtr(p);

	::operator delete[](p);
}

void allocTracker_free(void * p, const char* fileName, const char* functionName, int line)
{
	MallocTracker::getInstance().freePtr(p);
}