#include "MatrixStack.h"

MatrixStack::MatrixStack()
	:mMatrixStack(),
	mTopInverseDirty(true)
{
	mMatrixStack.push_back(XMMatrixIdentity());
}

MatrixStack::~MatrixStack()
{
	mMatrixStack.clear();
}

void MatrixStack::push(const XMMATRIX& mat)
{
	XMMATRIX newTop = mat * mMatrixStack.back();
	mMatrixStack.push_back(newTop);

	mTopInverseDirty = true;
}

void MatrixStack::multMatrixLocal(const XMMATRIX& mat)
{
	XMMATRIX topMat = mMatrixStack.back();
	topMat = XMMatrixMultiply(topMat, mat);
	(*mMatrixStack.rbegin()) = topMat; //replace top
}

void MatrixStack::pop()
{
	mMatrixStack.pop_back();

	if (mMatrixStack.empty())
		mMatrixStack.push_back(XMMatrixIdentity());

	mTopInverseDirty = true;
}

XMMATRIX MatrixStack::getTop()
{
	return mMatrixStack.back();
}

XMMATRIX MatrixStack::getTopInverse()
{
	if (mTopInverseDirty)
	{
		mTopInverse = XMMatrixInverse(NULL, mMatrixStack.back());
		mTopInverseDirty = false;
	}

	return mTopInverse;
}