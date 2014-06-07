/*
 *	
 */
#pragma once

#include <Windows.h>
#include <queue>
#include "EventSystem.h"
#include "KeyboardState.h"
#include "MouseState.h"

class D2DApp;

class InputSystem
{
	friend class D3DApp;

	static InputSystem* mpInstance;

public:
	void Update();

	const MouseState* getMouseState() const { return &mMouseState; }
	const MouseState* getPreviousMouseState() const { return &mPreviousMouseState; }
	const KeyboardState* getKeyboardState() const { return &mKeyboardState; }
	const KeyboardState* getPreviousKeyboardState() const { return &mPreviousKeyboardState; }

	static InputSystem* get();

private:
	InputSystem();
	~InputSystem();

	bool processWindowsMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	///<summary>
	///Adds an input event that will get dispatched upon update()
	///</summary>
	///<returns></returns>
	void enqueueEvent(IEventDataPtr);

private:
	MouseState mPreviousMouseState;
	MouseState mMouseState;

	KeyboardState mPreviousKeyboardState;
	KeyboardState mKeyboardState;
};