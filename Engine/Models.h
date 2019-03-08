#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__
#include "pch.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Shaders.h"
#include "File_system.h"
#include "Render_Buffer.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include <CommonStates.h>
#include <Effects.h>
#include <Model.h>

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

using namespace Assimp;

class Models
{
public:
	bool LoadFromFile(string *Filename);
	bool LoadFromFile(string *Filename, UINT Flags, bool ConvertToLH);

	bool LoadFromAllModels();
	bool LoadFromAllModels(vector<UINT> Flags, vector<bool> ConvertToLH);

	void Render(Matrix View, Matrix Proj, bool WF);

	Models(void) {}
	Models(string *Filename) { if (!LoadFromFile(Filename)) throw exception("Models::load == false!!!"); }

	void Release()
	{
		while (!textures.empty())
		{
			SAFE_DELETE(textures.at(0).texture);
			textures.erase(textures.begin());
		}
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
	void setupMesh();

	auto getIndices() { if (!indices.empty()) return indices; }
	auto getVertices() { if (!vertices.empty()) return vertices; }

	Vector3 getPosition() { return position.Translation(); }

	~Models() {}
protected:
#pragma pack(push, 1)
	struct Things
	{
		Vector3 Position;
		Vector4 Texcoord;
		Things(Vector3 Position, Vector4 Texcoord) : Position(Position), Texcoord(Texcoord) {}
	};
#pragma pack()

	struct Texture
	{
		string type, path;
		ID3D11ShaderResourceView *texture = nullptr;
	};

	HRESULT hr = S_OK;

	Assimp::Importer *importer = nullptr;
	const aiScene *pScene = nullptr;

	vector<Texture> Textures_loaded;
	string Textype = "";

	aiMesh *mesh = nullptr;

	int CountMaterial = 0;

	vector<Things> vertices;
	vector<UINT> indices;
	vector<Texture> textures;

	void processNode(aiNode *node, const aiScene *Scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, aiMaterial *mat);
	int getTextureIndex(aiString *str);

	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);

	Matrix position = XMMatrixIdentity();
	Matrix scale = XMMatrixIdentity();
	Matrix rotate = XMMatrixIdentity();
};
#endif // !__MODELS_H__
