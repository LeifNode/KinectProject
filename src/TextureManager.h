#pragma once

#include "d3dStd.h"

class Texture;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	Texture* getTexture(const std::string& filePath);
	void preloadTextures(const std::vector<std::string>& filePaths);

	void unloadAll();

private:
	Texture* loadTexture(const std::string& filePath);
	void unloadTexture(const std::string& filePath);

	std::string getExtensionFromPath(const char* path);

private:
	std::map<std::string, Texture*> mTextureMap;
};