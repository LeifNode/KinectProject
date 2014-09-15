#pragma once

#include "d3dStd.h"
#include "AssetLoader.h"

class OBJLoader : public AssetLoader
{
public:
	OBJLoader();
	~OBJLoader();

	virtual assettypes::Scene* LoadFile(const std::string& path) override;

private:
};