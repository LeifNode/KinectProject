#pragma once

#include <map>
#include <string>

class MallocTracker
{
	struct MemAllocation
	{
		size_t Size;
		std::string FileName;
		std::string FunctionName;
		int LineNum;
	};

public:
	static MallocTracker& getInstance()
    {
        static MallocTracker instance;

        return instance;
    }

	void alloc(void* p, size_t size, const char* fileName, const char* functionName, int lineNum);
	bool freePtr(void* p);

	void dumpTrackedMemory();

	unsigned int getUsedMemory() const { return mTotalMemory; }

private:
	MallocTracker() {}
	~MallocTracker() {}

private:
	std::map<void*, MemAllocation> mAllocations;

	unsigned int mTotalMemory;
};