#pragma once

#include "d3dStd.h"

namespace assettypes
{
	enum TextureType
	{
		TextureType_UNKNOWN = -1,
		TextureType_NONE,
		TextureType_DIFFUSE,
		TextureType_SPECULAR,
		TextureType_EMISSIVE,
		TextureType_HEIGHT,
		TextureType_NORMAL,
		TextureType_SHININESS,
		TextureType_LIGHTMAP,
	};

	struct Bone
	{
		unsigned mId;
		XMMATRIX mOffset;
	};

	struct VertexWeight
	{
		unsigned mVertexId;
	};

	struct BoneVertexWeights
	{
		//Unsigned is bone ID, float is weight
		std::vector<std::pair<unsigned, float>> boneWeights;
	};

	struct Material
	{
		XMFLOAT4 ambient;
		XMFLOAT4 emissive;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		float specularPow;

		std::map<TextureType, std::string> texturePaths;
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
		std::string mMeshId;

		unsigned mMaterialId;

		unsigned mVertexCount;
		unsigned mFaceCount;

		XMFLOAT3* mVertices;

		XMFLOAT3* mNormals;

		XMFLOAT3* mTangents;

		XMFLOAT3* mBitengents;

		XMFLOAT3* mTextureCoords;

		Face* mFaces;

		Mesh()
			:mVertexCount(0),
			mFaceCount(0),
			mVertices(NULL),
			mNormals(NULL),
			mTangents(NULL),
			mBitengents(NULL),
			mTextureCoords(NULL),
			mFaces(NULL)
		{
		}

		~Mesh()
		{
			delete [] mVertices;
			delete [] mNormals;
			delete [] mTangents;
			delete [] mBitengents;
			delete [] mTextureCoords;
			delete [] mFaces;
		}

		bool hasPositions() const { return mVertices != NULL && mVertexCount > 0; }
		bool hasFaces() const { return mFaces != NULL && mFaceCount > 0; }
		bool hasNormals() const { return mNormals != NULL && mVertexCount > 0; }
		bool hasTangentsAndBitangents() const { return mTangents != NULL && mBitengents != NULL && mVertexCount > 0; }
		bool hasTextureCoords() const { return mTextureCoords != NULL && mVertexCount > 0; }
	};

	struct Node
	{
		std::string mName;

		Node* mpParent;
		
		Node** mpChildren;
		unsigned mNumChildren;

		XMMATRIX mTransform;

		unsigned mNumMeshes;

		//Array of indices to meshes
		unsigned* mpMeshes;

		Node()
			:mpParent(NULL),
			mpChildren(NULL),
			mNumChildren(0),
			mNumMeshes(0),
			mpMeshes(NULL)
		{}

		~Node()
		{
			if (mpChildren != NULL)
			{
				for (unsigned int i = 0; i < mNumChildren; i++)
				{
					delete mpChildren[i];
				}

				delete [] mpChildren;
				delete [] mpMeshes;
			}
		}
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

	struct Scene
	{
		Node* mpRootNode;

		std::vector<Mesh*> mMeshes;
		std::vector<Material*> mMaterials;

		Scene()
			:mpRootNode(NULL)
		{
		}

		~Scene()
		{
			delete mpRootNode;

			for (int i = 0; i < mMeshes.size(); i++)
				delete mMeshes[i];

			for (int i = 0; i < mMaterials.size(); i++)
				delete mMaterials[i];
		}
	};
};