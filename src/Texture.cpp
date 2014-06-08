#include "Texture.h"

Texture::Texture()
	:mWidth(0),
	mHeight(0),
	mpTexture(NULL),
	mpResourceView(NULL)
{}
Texture::~Texture() 
{
	ReleaseCOM(mpResourceView);
	ReleaseCOM(mpTexture);
}
//
//D3D11_RESOURCE_DIMENSION Texture::getType() const
//{
//	D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
//	
//	if (mpTexture)
//		mpTexture->GetType(&dimension);
//
//	return dimension;
//}