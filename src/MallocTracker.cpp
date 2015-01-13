#include "MallocTracker.h"
#include "Logger.h"
#include <string>
#include <sstream>
#include <iostream>
#include <Windows.h>

void MallocTracker::alloc(void* p, size_t size, const char* fileName, const char* functionName, int lineNum)
{
	//if (free(p))

	MemAllocation memAlloc;
	memAlloc.Size = size;
	memAlloc.FileName = fileName;
	memAlloc.FunctionName = functionName;
	memAlloc.LineNum = lineNum;

	mAllocations[p] = memAlloc;

	mTotalMemory += size;
}

bool MallocTracker::freePtr(void* p)
{
	auto it = mAllocations.find(p);

	if (it != mAllocations.end())
	{
		mTotalMemory -= it->second.Size;

		mAllocations.erase(it);

		//std::cout << "Freed allocated memory at: " << p << std::endl;

		return true;
	}

	//std::cout << "Attempted to free non-allocated memory at: " << p << std::endl;

	return false;
}

void MallocTracker::dumpTrackedMemory()
{
	char sizeStr[15];

	sprintf(sizeStr, "%fKB\n", mTotalMemory / 1024.0f);

	OutputDebugStringA("Total memory allocated: ");
	OutputDebugStringA(sizeStr);
	OutputDebugStringA("Remaining memory allocations:\n");

	for (auto it = mAllocations.begin(); it != mAllocations.end(); ++it)
	{
		std::stringstream debugString;
		
		debugString << "Size: " << it->second.Size << "B, File: " << it->second.FileName << ", Function: " << it->second.FunctionName << ", Line: " << it->second.LineNum << std::endl; 

		OutputDebugStringA(debugString.str().c_str());
	}
}