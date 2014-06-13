//Based off of code by Jukka Jylänki and http://www.blackpawn.com/texts/lightmaps/default.html
#pragma once

#include "d3dStd.h"

class RectangleBinPacker
{
public:
	struct Node
	{
		Node();
		~Node();

		Node* left;
		Node* right;

		int x, y;
		int width, height;
	};

public:
	RectangleBinPacker();
	~RectangleBinPacker();

	void Initialize(int width, int height);

	Node* insert(int width, int height);

	int getWidth() const { return mBaseWidth; }
	int getHeight() const { return mBaseHeight; } 

	int getFilledArea();

	float getFillPercent();

private:
	Node* insert(Node* node, int width, int height);

private:
	Node mRoot;

	int mFilledArea;
	int mBaseWidth;
	int mBaseHeight;
};