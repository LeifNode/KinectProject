#pragma once

void * operator new(size_t size, const char* fileName, const char* functionName, int line);
void * operator new[](size_t size, const char* fileName, const char* functionName, int line);

void operator delete(void * p, const char* fileName, const char* functionName, int line);
void operator delete[](void * p, const char* fileName, const char* functionName, int line);

//void allocTracker_malloc(void * p, const char* fileName, const char* functionName, int line);
void allocTracker_free(void * p, const char* fileName, const char* functionName, int line);

#define LE_NEW new(__FILE__, __FUNCTION__, __LINE__)

#define LE_DELETE delete(__FILE__, __FUNCTION__,__LINE__)

#define SAFE_DELETE(ptr) allocTracker_free(ptr, __FILE__, __FUNCTION__, __LINE__), delete ptr, ptr = 0
#define SAFE_DELETEARR(ptr) allocTracker_free(ptr, __FILE__, __FUNCTION__, __LINE__), delete[] ptr, ptr = 0