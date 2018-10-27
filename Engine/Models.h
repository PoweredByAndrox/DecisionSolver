#pragma once
#include "pch.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Textures.h"
#include "Mesh.h"
#include "File_system.h"

using namespace std;

class Models: public File_system
{
public:
	bool Load(string Filename);
	void Draw();

	void Close();
	Models() {}
	~Models(){}

private:
	ID3D11Device *Device = nullptr;
	Assimp::Importer importer;
	const aiScene *pScene;
	vector<Mesh> Meshes;
	vector<Texture> Textures_loaded;
	string Textype;
	HWND HWND;
	ID3D11ShaderResourceView *texture;

	void processNode(aiNode *node, const aiScene *Scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *Scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, aiMaterial *mat);
	int getTextureIndex(aiString *str);
	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);

	void GetD3DDevice()
	{
		this->Device = DXUTGetD3D11Device();
	}

	void GetD3DHWND()
	{
		this->HWND = DXUTGetHWND();
	}
};

