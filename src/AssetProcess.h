#pragma once

#include "AssetTypes.h"

class AssetProcess
{
public:
	AssetProcess();

	virtual void Execute(assettypes::Scene* scene)=0;
};