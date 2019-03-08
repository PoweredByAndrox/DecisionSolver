#pragma once
#if !defined(__RENDER_BUFFER_H__)
#define __RENDER_BUFFER_H__
#include "pch.h"

#include "Shaders.h"
#include <WICTextureLoader.h>

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

class Render_Buffer
{
private:
#pragma pack(push, 1)
	struct ConstantBuffer
	{
		Matrix mWorld, mView, mProj;
	} cb;
#pragma pack()

public:
	HRESULT InitSimpleBuffer(vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader, UINT VertexSize = 0U,
		void *Vertix = nullptr, UINT IndicesSize = 0U, void *Indix = nullptr, UINT SizeStruct = 0U);
#if defined(UseTerrain)
	HRESULT InitTerrain(shared_ptr<Engine> engine, UINT SizeofVertex, void *vertices,
		vector<UINT> indices, vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader);
#endif // UseTerrain

	HRESULT InitModels(UINT VertexSize, void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct,
		vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader);

	HRESULT CreateWF(); // WireFrame

	ID3D11Buffer *CreateVB(UINT ByteWidth, void *vertices);
	ID3D11Buffer *CreateIB(WORD ByteWidth, void *indices);
	HRESULT CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags);
	HRESULT CreateLayout(ID3DBlob *Buffer_blob);

	void RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies = 0, bool WF = false);
#if !defined(UseTerrain)
	void RenderTerrain(Matrix World, Matrix View, Matrix Proj, int Indices, vector<ID3D11Buffer *> RenderBuff, UINT stride);
#endif // UseTerrain

	void RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, UINT SizeStruct, ID3D11ShaderResourceView *RenderTextr, bool WF);

	Render_Buffer() {}
	~Render_Buffer() {}

	void Release();

	/// Getters!
	bool isInit() { return init; }
	ID3D11Buffer *getVB() { if (m_vertexBuffer) return m_vertexBuffer; return nullptr; }
	ID3D11Buffer *getIB() { if (m_indexBuffer)  return m_indexBuffer; return nullptr; }
protected:
	// **********
		// Init Class
	bool init = false;

	HRESULT hr = S_OK;

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

	Shaders *Shader = new Shaders;

	vector<ID3DBlob *> Buffer_blob;
	vector<void *> Buffers;
};
#endif // !__RENDER_BUFFER_H__