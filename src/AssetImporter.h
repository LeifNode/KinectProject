#pragma once

#include "d3dStd.h"
#include "AssetTypes.h"
#include "AssetLoader.h"

class AssetImporter
{
public:
	AssetImporter();
	~AssetImporter();

	void ImportFile(const std::string& path);

	assettypes::Scene* getScene() const { return mpScene; }

private:
	std::string findExtension(const std::string& path);

private:
	assettypes::Scene* mpScene;
};