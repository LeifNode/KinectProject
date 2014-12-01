#include "TextureManager.h"
#include "Texture.h"
#include "d3dApp.h"
#include "D3DRenderer.h"
#include "DDSTextureLoader.h"
#include <iostream>

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
	unloadAll();
}

Texture* TextureManager::loadTexture(const std::string& filePath)
{
	if (getExtensionFromPath(filePath.c_str()) != ".dds")
		return NULL;

	//return NULL;

	Texture* newTexture = new Texture();
	
	D3DRenderer* renderer = gpApplication->getRenderer();

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureResource;
	HRESULT result = CreateDDSTextureFromFile(renderer->device(), std::wstring(filePath.begin(), filePath.end()).c_str(), &texture, &textureResource);

	if (result != S_OK)
	{
		std::cout << "[TextureManager] Failed to load texture from " << filePath << std::endl; 
		//MessageBox(NULL, L"Could not load texture.", L"Error", 0);

		ReleaseCOM(texture);
		ReleaseCOM(textureResource);
		return NULL;
	}

	//newTexture->mpTexture = texture;
	newTexture->mpResourceView = textureResource;
	

	mTextureMap[filePath] = newTexture;
	return newTexture;
}

void TextureManager::unloadAll()
{
	for (auto it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
	{
		delete it->second;
	}

	mTextureMap.clear();
}

void TextureManager::unloadTexture(const std::string& filePath)
{
	auto it = mTextureMap.find(filePath);

	if (it != mTextureMap.end())
	{
		delete it->second;
		mTextureMap.erase(it);
	}
}

Texture* TextureManager::getTexture(const std::string& filePath)
{
	auto it = mTextureMap.find(filePath);

	if (it != mTextureMap.end())
		return it->second;
	else
		return loadTexture(filePath);
}

std::string TextureManager::getExtensionFromPath(const char* path)
{
	const char* currentPtr = path;

	while (*currentPtr != '\0')
		currentPtr++;

	while (*currentPtr != '.' && currentPtr != path)
		currentPtr--;

	return std::string(currentPtr);
}