#pragma once

struct Rect
{
	float x;
	float y;
	float width;
	float height;

	Rect()
	{
		x = y = width = height = 0.0f;
	}
};