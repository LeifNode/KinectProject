#include "KeyboardState.h"

KeyboardState::KeyboardState()
{
	ZeroMemory(this, sizeof(this));
}

KeyboardState::~KeyboardState()
{
}

bool KeyboardState::isKeyPressed(KeyboardKey key) const
{
	return ((mKeys[key / 8] >> (key % 8)) & 1) != 0;
}

void KeyboardState::pressKey(KeyboardKey key)
{
	mKeys[key / 8] |= (1 << (key % 8));
}

void KeyboardState::releaseKey(KeyboardKey key)
{
	mKeys[key / 8] &= ~(1 << (key % 8));
}