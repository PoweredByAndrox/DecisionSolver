#pragma once
#ifndef __MESH_H__
#define __MESH_H__
#include "pch.h"

class Mesh
{
public:
	Mesh() {}
	~Mesh() {}

	struct VERTEX
	{
		FLOAT X, Y, Z;
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
	ID3D11Device *Device = nullptr;
	ID3D11DeviceContext *DeviceCon = nullptr;

	/*  Render data  */
	ID3D11Buffer *VertexBuffer, *IndexBuffer;

/*  Functions    */
// Initializes all the buffer objects/arrays
	bool setupMesh()
	{
		HRESULT hr;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(VERTEX) * vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertices[0];

		if (Device == nullptr)
			GetD3DDevice();

		if (FAILED(hr = Device->CreateBuffer(&vbd, &initData, &VertexBuffer)))
			return false;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * indices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;

		initData.pSysMem = &indices[0];
		if (Device == nullptr)
			GetD3DDevice();
		if (FAILED(hr = Device->CreateBuffer(&ibd, &initData, &IndexBuffer)))
			return false;
	}

	void GetD3DDevice()
	{
		this->Device = DXUTGetD3D11Device();
	}

	void GetD3DDevice1()
	{
		this->DeviceCon = DXUTGetD3D11DeviceContext();
	}
};
#endif // !__MESH_H__
