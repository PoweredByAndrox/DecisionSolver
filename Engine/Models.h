#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__
#include "pch.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Render_Buffer.h"

#include <Inc/WICTextureLoader.h>
#include <Inc/DDSTextureLoader.h>

#include <Inc/CommonStates.h>
#include <Effects.h>

using namespace Assimp;

struct Texture
{
	string type, path;
	ID3D11ShaderResourceView *TextureSHRes = nullptr;
	ID3D11Resource *TextureRes = nullptr;
};
#pragma pack(push, 1)
struct Things
{
	Vector3 Pos;
	Vector2 Tex;
};
#pragma pack()

class Models
{
private:
	class Mesh
	{
	public:
		Mesh(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures, Vector3 Min, Vector3 Max)
		{
			Init(vertices, indices, textures, Min, Max);
		}
		Mesh() {}
		~Mesh() {}

		void Init(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures, Vector3 Min, Vector3 Max);
		void Draw(Matrix World, Matrix View, Matrix Proj);

		vector<Things> getVertices() { return vertices; }
		vector<UINT> getIndices() { return indices; }
		Vector3 getMinAABB() { return MinAABB; }
		Vector3 getMaxAABB() { return MaxAABB; }
	private:
		vector<Things> vertices;
		vector<UINT> indices;
		vector<Texture> textures;

		ID3D11Buffer *VertexBuffer, *IndexBuffer, *pConstantBuffer;
		ID3D11VertexShader *pVS;
		ID3D11PixelShader *pPS;
		Vector3 MinAABB = Vector3::Zero, MaxAABB = Vector3::Zero;
	};
	vector<shared_ptr<Mesh>> meshes;

public:
	bool LoadFromFile(string Filename);
	bool LoadFromAllModels();

	void Render(Matrix View, Matrix Proj);

	Models() {}
	Models(string Filename);

	void Release()
	{
		while (!Textures_loaded.empty())
		{
			SAFE_DELETE(Textures_loaded.front().TextureRes);
			SAFE_DELETE(Textures_loaded.front().TextureSHRes);
			Textures_loaded.erase(Textures_loaded.begin());
		}

		SAFE_DELETE(importer);
		SAFE_DELETE(pScene);
		SAFE_DELETE(mesh);
	}

	void setRotation(Vector3 rotaxis);
	void setScale(Vector3 Scale);
	void setPosition(Vector3 Pos);

//	Vector3 getPosition() { return position.Translation(); }
	vector<shared_ptr<Mesh>> getMeshes() { return meshes; }

	~Models() {}
protected:
	Matrix position = Matrix::Identity, scale = Matrix::Identity, rotate = Matrix::Identity;

#pragma pack(push, 1)
	struct ConstantBuffer
	{
		Matrix mMVP = Matrix::Identity;
	} cb;
#pragma pack()

	HRESULT hr = S_OK;

	Assimp::Importer *importer = nullptr;
	const aiScene *pScene = nullptr;

	vector<Texture> Textures_loaded;
	string Textype = "";

	aiMesh *mesh = nullptr;

	void processNode(aiNode *node, const aiScene *Scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, string TypeName, aiMaterial *mat);
	int getTextureIndex(aiString *str);

	static aiTextureType getTextureType(string TypeName);

	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);
};
#endif // !__MODELS_H__
