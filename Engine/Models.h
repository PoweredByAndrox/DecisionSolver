#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__
#include "pch.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Render_Buffer.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include <CommonStates.h>
#include <Effects.h>
#include <Model.h>

using namespace Assimp;

struct Texture
{
	string type, path;
	ID3D11ShaderResourceView *texture = nullptr;
};
#pragma pack(push, 1)
struct Things
{
	Vector3 Pos;
	Vector2 Tex;
};
#pragma pack()

//class Mesh;
class Models
{
private:
	class Mesh
	{
	public:
		Mesh(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures)
		{
			Init(vertices, indices, textures);
		}
		Mesh() {}
		~Mesh() {}

		void Init(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures);
		void Draw(Matrix World, Matrix View, Matrix Proj);
	private:
		vector<Things> vertices;
		vector<UINT> indices;
		vector<Texture> textures;

		ID3D11Buffer *VertexBuffer, *IndexBuffer, *pConstantBuffer;
		ID3D11VertexShader *pVS;
		ID3D11PixelShader *pPS;
	};
	vector<shared_ptr<Mesh>> meshes;

public:
	bool LoadFromFile(string Filename);
	bool LoadFromFile(string Filename, UINT Flags, bool ConvertToLH);

	bool LoadFromAllModels();
	bool LoadFromAllModels(vector<UINT> Flags, vector<bool> ConvertToLH);

	void Render(Matrix View, Matrix Proj);

	Models(void) {}
	Models(string Filename) { if (!LoadFromFile(Filename)) throw exception("Models::load == false!!!"); }

	void Release()
	{
		while (!Textures_loaded.empty())
		{
			SAFE_DELETE(Textures_loaded.at(0).texture);
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

	~Models() {}
protected:
	Matrix position, scale, rotate;

#pragma pack(push, 1)
	struct ConstantBuffer
	{
		Matrix mMVP;
	} cb;
#pragma pack()

	HRESULT hr = S_OK;

	Assimp::Importer *importer = nullptr;
	const aiScene *pScene = nullptr;

	vector<Texture> Textures_loaded;
	string Textype = "";

	aiMesh *mesh = nullptr;

	int CountMaterial = 0;

	//vector<Mesh> Meshes;

	shared_ptr<Render_Buffer> Buff = make_unique<Render_Buffer>();

	void processNode(aiNode *node, const aiScene *Scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, aiMaterial *mat);
	int getTextureIndex(aiString *str);

	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);
};
#endif // !__MODELS_H__
