#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__
#include "pch.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "File_system.h"

#include <WICTextureLoader.h>

class Mesh
{
public:
	Mesh() {}
	~Mesh() {}

	struct VERTEX
	{
		XMFLOAT3 Position;
		XMFLOAT2 texcoord;
	};

	struct Texture
	{
		string type;
		string path;
		ID3D11ShaderResourceView *texture;
	};

	vector<VERTEX> vertices;
	vector<UINT> indices;
	vector<Texture> textures;

	Mesh(vector<VERTEX> vertices, vector<UINT> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		GetD3DDevice();
		GetD3DDevice1();

		this->setupMesh();
	}

	void Draw()
	{
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		if (DeviceCon == nullptr)
			GetD3DDevice1();
		DeviceCon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		DeviceCon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		DeviceCon->PSSetShaderResources(0, 1, &textures[0].texture);

		DeviceCon->DrawIndexed(indices.size(), 0, 0);
	}
	void Close()
	{
		VertexBuffer->Release();
		IndexBuffer->Release();
	}
private:
	HRESULT hr = S_OK;

	ID3D11Device *Device = nullptr;
	ID3D11DeviceContext *DeviceCon = nullptr;
	ID3D11Buffer *VertexBuffer = nullptr, *IndexBuffer = nullptr;

	bool setupMesh()
	{
		D3D11_SUBRESOURCE_DATA Data;
		Data.pSysMem = &vertices[0];
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(VERTEX) * vertices.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		if (Device == nullptr)
			GetD3DDevice();

		if (FAILED(hr = Device->CreateBuffer(&bd, &Data, &VertexBuffer)))
			return false;

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(UINT) * indices.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		Data.pSysMem = &indices[0];

		if (FAILED(hr = Device->CreateBuffer(&bd, &Data, &IndexBuffer)))
			return false;

			return false;
	}

	void GetD3DDevice() { Device = DXUTGetD3D11Device(); }
	void GetD3DDevice1() { DeviceCon = DXUTGetD3D11DeviceContext(); }
};

class Models: public File_system, public Mesh
{
public:
	bool Load(string *Filename);

	void Draw();
	void Close();
	
	auto *getMeshes() { return &Meshes; }

	Models() {}
	~Models(){}

private:
	HRESULT hr = S_OK;

	ID3D11ShaderResourceView *texture;
	ID3D11Device *Device = nullptr;

	Assimp::Importer importer;
	const aiScene *pScene;

	vector<Mesh> Meshes;
	vector<Texture> Textures_loaded;
	string Textype;

	HWND hwnd;

	aiMesh *mesh;

	void processNode(aiNode *node, const aiScene *Scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *Scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, aiMaterial *mat);
	int getTextureIndex(aiString *str);
	
	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);

	void GetD3DDevice() { Device = DXUTGetD3D11Device(); }
	void GetD3DHWND() { hwnd = DXUTGetHWND(); }
};
#endif // !__MODELS_H__
