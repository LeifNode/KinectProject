#pragma once

#include "d3dStd.h"

class MatrixStack
{
public:
	MatrixStack();
	~MatrixStack();

	void push(const XMMATRIX&);
	void pop();
	XMMATRIX getTop();
	XMMATRIX getTopInverse();
	void multMatrixLocal(const XMMATRIX&);

private:
	XMMATRIX mTopInverse;
	bool mTopInverseDirty;
	std::list<XMMATRIX> mMatrixStack;
};