#include "RenderTarget.h"

RenderTarget::RenderTarget()
	:mpRenderTargetTexture(NULL),
	mpDepthTexture(NULL),
	mpRenderTargetView(NULL),
	mpDepthView(NULL)
{

}

RenderTarget::~RenderTarget()
{
	ReleaseCOM(mpRenderTargetView);
	ReleaseCOM(mpDepthView);

	SAFE_DELETE(mpRenderTargetTexture);
	SAFE_DELETE(mpDepthTexture);
}