#include "InputSystem.h"
#include <Windows.h>
#include <WindowsX.h>
#include "MouseState.h"
#include "KeyboardState.h"
#include "Events.h"
#include "Memory.h"

InputSystem* InputSystem::mpInstance = NULL;

InputSystem::InputSystem()
{
	mpHydraManager = LE_NEW HydraManager();
	mpHydraManager->Initialize();
}

InputSystem::~InputSystem()
{
	SAFE_DELETE(mpHydraManager);
}

InputSystem* InputSystem::get()
{
	if (mpInstance)
		return mpInstance;
	else
		return mpInstance = LE_NEW InputSystem();
}

void InputSystem::Update(float dt)
{
	mpHydraManager->Update(dt);
	mPreviousKeyboardState = mKeyboardState;
	mPreviousMouseState = mMouseState;
}

bool InputSystem::processWindowsMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MouseButton button = MOUSE_NONE;

	switch(msg)
	{
	case WM_MOUSEMOVE:
		mMouseState.mX = GET_X_LPARAM(lParam);
		mMouseState.mY = GET_Y_LPARAM(lParam);
		enqueueEvent(IEventDataPtr(new EventData_MouseMove(mMouseState.mX, mMouseState.mY)));
		break;
	case WM_LBUTTONDOWN:
		mMouseState.mLeft = true;
		enqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_LEFT)));
		break;
	case WM_LBUTTONUP:
		mMouseState.mLeft = false;
		enqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_LEFT)));
		break;
	case WM_MBUTTONDOWN:
		mMouseState.mMiddle = true;
		enqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_MIDDLE)));
		break;
	case WM_MBUTTONUP:
		mMouseState.mMiddle = false;
		enqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_MIDDLE)));
		break;
	case WM_RBUTTONDOWN:
		mMouseState.mRight = true;
		enqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_RIGHT)));
		break;
	case WM_RBUTTONUP:
		mMouseState.mRight = false;
		enqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_RIGHT)));
		break;
	case WM_XBUTTONDOWN:
		if (GET_XBUTTON_WPARAM(wParam) == 1)
		{
			button = MOUSE_XBUTTON1;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == 2)
		{
			button = MOUSE_XBUTTON2;
		}

		enqueueEvent(IEventDataPtr(new EventData_MouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), button)));
		break;
	case WM_XBUTTONUP:
		if (GET_XBUTTON_WPARAM(wParam) == 1)
		{
			button = MOUSE_XBUTTON1;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == 2)
		{
			button = MOUSE_XBUTTON2;
		}

		enqueueEvent(IEventDataPtr(new EventData_MouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), button)));
		break;
	case WM_MOUSEWHEEL:
		mMouseState.mScroll = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	case WM_KEYDOWN:
		mKeyboardState.pressKey((KeyboardKey)wParam);
		enqueueEvent(IEventDataPtr(new EventData_KeyboardDown(wParam)));
		break;
	case WM_KEYUP:
		mKeyboardState.releaseKey((KeyboardKey)wParam);
		enqueueEvent(IEventDataPtr(new EventData_KeyboardUp(wParam)));
		break;
	default:
		return false;
	}

	return true;
}

void InputSystem::enqueueEvent(IEventDataPtr ev)
{
	EventSystem::get()->queueEvent(ev);
}