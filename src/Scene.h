#pragma once

#include "d3dStd.h"

class Scene
{
public:
	struct Bone
	{
		unsigned id;
		XMFLOAT4 rotation;
		XMFLOAT3 position;
	};

	struct BoneVertexWeights
	{
		//Unsigned is bone ID, float is weight
		std::vector<std::pair<unsigned, float>> boneWeights;
	};

	struct Material
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 specular;
		float specularPow;
		XMFLOAT3 emissive;

		std::string diffuseTexturePath;
		std::string specularTexturePath;
		std::string bumpmapTexturePath;
		std::string emissiveTexturePath;
	};

	struct Face
	{
		unsigned mIndexCount;
		unsigned* mIndices;

		Face()
			:mIndexCount(0),
			mIndices(NULL)
		{
		}

		~Face()
		{
			delete [] mIndices;
		}

		Face(const Face& other)
			:mIndices(NULL)
		{
			*this = other;
		}

		Face& operator= (const Face& other)
		{
			if (&other == this)
				return *this;

			delete [] mIndices;
			mIndexCount = other.mIndexCount;

			if (mIndexCount)
			{
				mIndices = new unsigned[mIndexCount];
				memcpy(mIndices, other.mIndices, mIndexCount * sizeof(unsigned));
			}
			else
				mIndices = NULL;

			return *this;
		}

		bool operator== (const Face& other)
		{
			if (mIndices == other.mIndices)return true;
			else if (mIndices && mIndexCount == other.mIndexCount)
			{
				for (unsigned int i = 0;i < this->mIndexCount;++i)
					if (mIndices[i] != other.mIndices[i])
						return false;
				return true;
			}
			return false;
		}

		bool operator!= (const Face& other)
		{
			return !(*this == other);
		}
	};

	struct Mesh
	{
		unsigned mPrimitiveType;
		unsigned mVertexCount;
		unsigned mFaceCount;

		XMFLOAT3* mVertices;
	};

	struct Model
	{
		std::string modelId;
		Material* material;

		/*std::vector<XMFLOAT3> vertexPositions;
		std::vector<XMFLOAT3> vertexTexCoords;
		std::vector<XMFLOAT3> vertexNormals;
		std::vector<XMFLOAT3> vertexTangents;
		std::vector<XMFLOAT3> vertexBiTangents;
		std::vector<BoneVertexWeights> vertexBoneWeights;*/


		//bool hasBoneWeights() const { return vertexBoneWeights.size() != 0; }
	};

	struct DirectionalLight
	{
		XMFLOAT3 direction;
		XMFLOAT3 diffuse;
		XMFLOAT3 ambient;
	};

	struct PointLight
	{
		XMFLOAT3 position;
		XMFLOAT3 diffuse;
		XMFLOAT3 ambient;
		float falloffLinear, falloffSquare;
		float range;
	};

	struct SpotLight
	{

	};


};