#pragma once

#include "AssetProcess.h"

class CreateTangentsBitangentsProcess : public AssetProcess
{
public:
	virtual void Execute(assettypes::Scene* scene) override;
};