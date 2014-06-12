#pragma once

#include "d3dStd.h"

template <class T>
class Bitmap
{
public:
	Bitmap(int width, int height);
	~Bitmap();

	UINT getPitch();

	void putPixel(int x, int y, const T& pixel);
	T* getArray() const { return mpPixelArray; }

private:
	int mWidth, mHeight;
	T* mpPixelArray;
};

template <class T>
Bitmap<T>::Bitmap(int width, int height)
{
	mpPixelArray = new T[width * height]();
}

template <class T>
Bitmap<T>::~Bitmap()
{
	SAFE_DELETE(mpPixelArray);
}

template <class T>
UINT Bitmap<T>::getPitch()
{
	return mWidth  * sizeof(T);
}

template <class T>
void Bitmap<T>::putPixel(int x, int y, const T& pixel)
{
	mpPixelArray[x + y * mWidth] = pixel;
}

