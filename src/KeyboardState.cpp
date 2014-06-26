#include "KeyboardState.h"

KeyboardState::KeyboardState()
{
	mKeys.reserve(KEY_COUNT);

	for (int i = 0; i < KEY_COUNT; i++)
		mKeys.push_back(false);
}

KeyboardState::~KeyboardState()
{
}

bool KeyboardState::isKeyPressed(KeyboardKey key) const
{
	//return ((mKeys[key / 8] >> (key % 8)) & 1) != 0;
	return mKeys[key];
}

void KeyboardState::pressKey(KeyboardKey key)
{
	//mKeys[key / 8] |= (1 << (key % 8));
	mKeys[key] = true;
}

void KeyboardState::releaseKey(KeyboardKey key)
{
	//mKeys[key / 8] &= ~(1 << (key % 8));
	mKeys[key] = false;
}