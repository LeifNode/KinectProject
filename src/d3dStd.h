#pragma once

#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <cassert>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <list>
#include <string>
#include <algorithm>
#include "..\3rdParty\include\FastDelegate\FastDelegate.h"
#include "dxerr.h"
#include <NuiApi.h> //Kinect API
#include "LoggingSystem.h"

using namespace DirectX;

using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

//Testing speed impact of disabling SIMD on the DirectXMath library
//#ifndef _XM_NO_INTRINSICS_
//#define _XM_NO_INTRINSICS_
//#endif

#ifndef ReleaseCOM
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) delete x; x = 0;
#endif

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(const_cast<WCHAR*>((WCHAR*)__FILE__), (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 