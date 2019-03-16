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

class Mesh;
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
#pragma pack(push, 1)
	struct Things
	{
		Vector3 Position;
		Vector2 Texcoord;
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

	vector<Mesh> Meshes;

	void processNode(aiNode *node, const aiScene *Scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
	string determineTextureType(const aiScene *Scene, aiMaterial *mat);
	int getTextureIndex(aiString *str);

	ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);
};

class Mesh: public Models
{
private:
	vector<Things> vertices;
	vector<UINT> indices;
	vector<Texture> textures;
	ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_pConstBuffer = nullptr, *m_matrixBuffer = nullptr;
	ID3D11VertexShader *m_vertexShader = nullptr;
	ID3D11PixelShader *m_pixelShader = nullptr;

	ID3D11InputLayout *m_layout = nullptr;
	ID3D11SamplerState *m_sampleState = nullptr;
	ID3D11ShaderResourceView *m_texture = nullptr;
	D3D11_BUFFER_DESC bd;
	ID3D11RasterizerState* g_pRasWireFrame = nullptr, *g_pRasStateSolid = nullptr;
	ID3D11Texture2D *g_pDepthStencil = nullptr;
	ID3D11DepthStencilView *g_pDepthStencilView = nullptr;

#pragma pack(push, 1)
	struct ConstantBuffer
	{
		Matrix mMVP;
	} cb;
#pragma pack()

	enum Type
	{
		uint16 = 0,
		uint32
	} type = uint16;
public:
	Mesh(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures)
	{
		vertices.size() < 65535 ? type = uint16 : type = uint32;

		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->setupMesh();
	}
	Matrix position, scale, rotate;

	void Draw(Matrix View, Matrix Proj, bool WF)
	{
		//if (type == uint32)
		//	Application->getRender_Buffer()->RenderModels(scale * rotate * position, View, Proj,
		//		indices.size(), textures[0].texture, WF);
		//else if (type == uint16)
		//	Application->getRender_Buffer()->RenderSimpleBuffer(scale * rotate * position, View, Proj, 
		//		indices.size(), textures[0].texture, WF);
		/*
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (m_layout)
			Application->getDeviceContext()->IASetInputLayout(m_layout);

		if (WF && g_pRasWireFrame)
			Application->getDeviceContext()->RSSetState(g_pRasWireFrame);
		else if (!WF && g_pRasStateSolid)
			Application->getDeviceContext()->RSSetState(g_pRasStateSolid);

		if (m_pConstBuffer)
			ThrowIfFailed(Application->getDeviceContext()->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

		auto dataPtr = (ConstantBuffer *)mappedResource.pData;

		XMMATRIX WVP = (scale * rotate * position) * View * Proj;
		dataPtr->mMVP = XMMatrixTranspose(WVP);

		if (m_pConstBuffer)
		{
			Application->getDeviceContext()->Unmap(m_pConstBuffer, 0);
			Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
		}

		if (m_vertexShader)
			Application->getDeviceContext()->VSSetShader(m_vertexShader, 0, 0);
		if (m_pixelShader)
			Application->getDeviceContext()->PSSetShader(m_pixelShader, 0, 0);

		Application->getDeviceContext()->PSSetShaderResources(0, 1, &textures[0].texture);
		Application->getDeviceContext()->DrawIndexed(indices.size(), 0, 0);
		*/
	}

	void Close()
	{
		while (!textures.empty())
		{
			SAFE_DELETE(textures.at(0).texture);
			textures.erase(textures.begin());
		}
	}
private:

	void setupMesh()
	{
		vector<wstring> FileShaders;
		FileShaders.push_back(Application->getFS()->GetFile(string("VertexShader.hlsl"))->PathW);
		FileShaders.push_back(Application->getFS()->GetFile(string("PixelShader.hlsl"))->PathW);

		vector<string> Functions, Version;
		Functions.push_back(string("VS"));
		Functions.push_back(string("PS"));

		Version.push_back(string("vs_4_0"));
		Version.push_back(string("ps_4_0"));

		//if (type == uint32)
		//	Application->getRender_Buffer()->InitModels(vertices.size(), &vertices, indices.size(), &indices, sizeof(Things));
		//else if (type == uint16)
		//	Application->getRender_Buffer()->InitSimpleBuffer(vertices.size(), &vertices, indices.size(), &indices, sizeof(Things));

		/*
		vector<ID3DBlob *> Buffer_blob;
		vector<void *> Buffers;
		Buffers = Application->getShader()->CompileShaderFromFile(Buffer_blob = Application->getShader()->CreateShaderFromFile(FileShaders, Functions, Version));
		m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
		m_pixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

		m_layout = Application->getRender_Buffer()->CreateLayout(Buffer_blob[0]);

		Application->getDeviceContext()->IASetInputLayout(m_layout);

		m_vertexBuffer = Application->getRender_Buffer()->CreateVB(sizeof(Things) * vertices.size(), &vertices[0]);

		UINT offset = 0;
		UINT stride = sizeof(Things);
		Application->getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		m_indexBuffer = Application->getRender_Buffer()->CreateIB(type == uint16 ? sizeof(UINT16) : sizeof(UINT32) * indices.size(), &indices[0]);

		auto WF = Application->getRender_Buffer()->CreateWF();
		g_pRasWireFrame = WF.at(0);
		g_pRasStateSolid = WF.at(1);

		Application->getDeviceContext()->IASetIndexBuffer(m_indexBuffer, type == uint16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

		Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pConstBuffer = Application->getRender_Buffer()->CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		*/
	}
};
#endif // !__MODELS_H__
