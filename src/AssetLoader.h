#pragma once

#include "d3dStd.h"
#include "AssetTypes.h"

class AssetLoader
{
public:
	AssetLoader();
	virtual ~AssetLoader();

	virtual assettypes::Scene* LoadFile(const std::string& path)=0;

protected:
};