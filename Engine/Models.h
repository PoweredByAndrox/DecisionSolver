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
		Vector3 Position;
		Vector2 texcoord;
	};

	struct Texture
	{
		string type;
		string path;
		ID3D11ShaderResourceView *texture;
	};

	vector<VERTEX> vertices;
	vector<uint16_t> indices;
	vector<Texture> textures;

	Mesh(vector<VERTEX> vertices, vector<uint16_t> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		GetD3DDevice();
		GetD3DDevice1();

		this->setupMesh();
	}

	/*
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
	*/
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

#include <CommonStates.h>
#include <Effects.h>
#include <GeometricPrimitive.h>

class Models: public File_system, public Mesh
{
public:
	bool Load(string *Filename);
	bool Load(string *Filename, UINT Flags, bool ConvertToLH);

	void Draw();
	void Close();
	
	auto *getMeshes() { return &Meshes; }

	void test(Models *ObjClas, Matrix World, Matrix View, Matrix Proj)
	{
		for (int i = 0; i < m_shape.size(); i++)
			for (int i1 = 0; i1 < ObjClas->getMeshes()->size(); i1++)
				for (int i2 = 0; i2 < ObjClas->getMeshes()->at(i1).textures.size(); i2++)
					m_shape.at(i)->Draw(World, View, Proj, Colors::White, ObjClas->getMeshes()->at(i1).textures.at(i2).texture);
	}

	void Setting(Models *ObjClas)
	{
		vector<VertexPositionNormalTexture> Vertes;
		vector<uint16_t> indices;
		vector<GeometricPrimitive::VertexType> S;
		GeometricPrimitive::VertexType VT;
		for (int i = 0; i < ObjClas->Meshes.size(); i++)
		{
			for (int ii = 0; ii < ObjClas->Meshes.at(i).vertices.size(); ii++)
			{
				VT.position = XMFLOAT3(ObjClas->Meshes.at(i).vertices.at(ii).Position.x,
					ObjClas->Meshes.at(i).vertices.at(ii).Position.y,
					ObjClas->Meshes.at(i).vertices.at(ii).Position.z);

				VT.normal = XMFLOAT3(0, 0, 0);

				VT.textureCoordinate = XMFLOAT2(ObjClas->Meshes.at(i).vertices.at(ii).texcoord.x,
					ObjClas->Meshes.at(i).vertices.at(ii).texcoord.y);

				S.push_back(VT);
			}

			for (int i1 = 0; i1 < Meshes.at(i).indices.size(); i1++)
				indices.push_back(Meshes.at(i).indices.at(i1));
		}

		for (int i = 0; i < S.size(); i++)
			Vertes.push_back(S.at(i));

		if (DeviceCon == nullptr)
			GetD3DDeviceCon();
		m_shape.push_back(GeometricPrimitive::CreateCustom(DeviceCon, Vertes, indices));
	}

	Models() {}
	Models(string *Filename) { if (!Load(Filename)) throw exception("Models::load == false!!!"); }
	Models(string *Filename, UINT Flags, bool ConvertToLH) { if (!Load(Filename, Flags, ConvertToLH)) throw exception("Models::load == false!!!"); }


	~Models(){}

private:
	HRESULT hr = S_OK;

	ID3D11ShaderResourceView *texture = nullptr;
	ID3D11Device *Device = nullptr;
	ID3D11DeviceContext *DeviceCon = nullptr;

	Assimp::Importer *importer = nullptr;
	const aiScene *pScene = nullptr;

	vector<Mesh> Meshes;
	vector<Texture> Textures_loaded;
	string Textype;

	HWND hwnd = nullptr;

	aiMesh *mesh = nullptr;

	void processNode(aiNode *node, const aiScene *Scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *Scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, aiMaterial *mat);
	int getTextureIndex(aiString *str);
	
	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);

	unique_ptr<DirectX::CommonStates> m_states;
	unique_ptr<DirectX::BasicEffect> m_effect;
	vector<unique_ptr<DirectX::GeometricPrimitive>> m_shape;
	//unique_ptr<PrimitiveBatch<VertexPositionColor>> m_batch;
	ID3D11InputLayout *m_inputLayout = nullptr;

	void GetD3DDevice() { Device = DXUTGetD3D11Device(); }
	void GetD3DDeviceCon() { DeviceCon = DXUTGetD3D11DeviceContext(); }
	void GetD3DHWND() { hwnd = DXUTGetHWND(); }
};
#endif // !__MODELS_H__
