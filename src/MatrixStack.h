#pragma once

#include "d3dStd.h"

class MatrixStack
{
public:
	MatrixStack();
	~MatrixStack();

	void push(const XMMATRIX&);
	void pop();
	XMMATRIX getTop() const;
	XMMATRIX getTopInverse() const;
	void multMatrixLocal(const XMMATRIX&);

private:
	mutable XMMATRIX mTopInverse;
	mutable bool mTopInverseDirty;
	std::list<XMMATRIX> mMatrixStack;
};