#pragma once

#include "d3dStd.h"

enum SHADER_TYPE
{
	SHADER_TYPE_NONE = -1,
	SHADER_TYPE_PIXEL,
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_HULL,
	SHADER_TYPE_DOMAIN,
	SHADER_TYPE_COMPUTE,
	SHADER_TYPE_COUNT,
};

struct ShaderInfo
{
	SHADER_TYPE type;
	const char* entrypoint;
};

class D3DRenderer;

class Shader
{
	friend class D3DRenderer;

public:
	~Shader();

	std::string getName() const { return mName; }

	bool hasVertexShader() const { return mpVertexShader != nullptr; }
	bool hasPixelShader() const { return mpPixelShader != nullptr; }
	bool hasGeometryShader() const { return mpGeometryShader != nullptr; }
	bool hasComputeShader() const { return mpComputeShader != nullptr; }
	bool hasHullShader() const { return mpHullShader != nullptr; }
	bool hasDomainShader() const { return mpDomainShader != nullptr; }

private:
	Shader(const std::string& name);

	void bind(D3DRenderer* renderer);

private:
	std::string mName;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology;

	ID3D11InputLayout* mpInputLayout;

	ID3D11VertexShader* mpVertexShader;
	ID3D11PixelShader* mpPixelShader;
	ID3D11GeometryShader* mpGeometryShader;
	ID3D11ComputeShader* mpComputeShader;
	ID3D11HullShader* mpHullShader;
	ID3D11DomainShader* mpDomainShader;
};