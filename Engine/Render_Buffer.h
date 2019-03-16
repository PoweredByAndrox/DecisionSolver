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
		Matrix mMVP;
	} cb;
#pragma pack()

public:
	void SetShadersFile(vector<wstring> ShaderFile, vector<string> Func, vector<string> VersionShader);

	HRESULT InitSimpleBuffer(UINT VertexSize = 0U, void *Vertix = nullptr, UINT IndicesSize = 0U, void *Indix = nullptr, UINT SizeStruct = 0U);
#if defined(UseTerrain)
	HRESULT InitTerrain(shared_ptr<Engine> engine, UINT SizeofVertex, void *vertices,
		vector<UINT> indices, vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader);
#endif // UseTerrain

	HRESULT InitModels(UINT VertexSize, void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct);

	HRESULT InitUI();
	void RenderUI(ImDrawData *draw_data, bool WF);

	vector<ID3D11RasterizerState *> CreateWF(); // WireFrame

	ID3D11Buffer *CreateVB(UINT ByteWidth, bool NeedVertice = false,
		D3D11_USAGE Usage = D3D11_USAGE_DEFAULT, UINT CPUAccessFlags = 0, void *vertices = nullptr);
	ID3D11Buffer *CreateIB(WORD ByteWidth, bool NeedIndices = false,
		D3D11_USAGE Usage = D3D11_USAGE_DEFAULT, UINT CPUAccessFlags = 0, void *indices = nullptr);

	ID3D11Buffer *CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags);
	ID3D11InputLayout *CreateLayout(ID3DBlob *Buffer_blob);

	void RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies = 0, ID3D11ShaderResourceView *RenderTextr = nullptr, bool WF = false);
#if !defined(UseTerrain)
	void RenderTerrain(Matrix World, Matrix View, Matrix Proj, int Indices, vector<ID3D11Buffer *> RenderBuff, UINT stride);
#endif // UseTerrain

	void RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, ID3D11ShaderResourceView *RenderTextr, bool WF);

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

	ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_pConstBuffer = nullptr;
	ID3D11VertexShader *m_vertexShader = nullptr;
	ID3D11PixelShader *m_pixelShader = nullptr;

	ID3D11InputLayout *m_layout = nullptr;
	ID3D11SamplerState *m_sampleState = nullptr;
	ID3D11ShaderResourceView *m_texture = nullptr;
	D3D11_BUFFER_DESC bd;
	ID3D11RasterizerState* g_pRasWireFrame = nullptr, *g_pRasStateSolid = nullptr;
	ID3D11Texture2D *g_pDepthStencil = nullptr;
	ID3D11DepthStencilView *g_pDepthStencilView = nullptr;
	ID3D11DepthStencilState *g_pDepthStencilState = nullptr;

	ID3D11SamplerState *g_pFontSampler = nullptr;
	ID3D11ShaderResourceView *g_pFontTextureView = nullptr;
	ID3D11BlendState *g_pBlendState = nullptr;

	shared_ptr<Shaders> Shader = make_unique<Shaders>();

	vector<ID3DBlob *> Buffer_blob;
	vector<void *> Buffers;

	// ***********
	// For UI
	int VBufferSize = 5000, IBufferSize = 10000;

	vector<wstring> ShaderFile;
	vector<string> Func, VersionShader;
};
#endif // !__RENDER_BUFFER_H__