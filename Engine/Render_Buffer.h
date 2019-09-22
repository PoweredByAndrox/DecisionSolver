#pragma once
#if !defined(__RENDER_BUFFER_H__)
#define __RENDER_BUFFER_H__
#include "pch.h"

#include "Shaders.h"
#include <Inc/WICTextureLoader.h>
#include "imgui.h"

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
	HRESULT InitUI();
	void RenderUI(ImDrawData *draw_data, bool WF);

	static vector<ID3D11RasterizerState *> CreateWF(); // WireFrame

	static ID3D11Buffer *CreateVB(UINT ByteWidth, bool NeedVertice,
		D3D11_USAGE Usage, UINT CPUAccessFlags, void *vertices);
	static ID3D11Buffer *CreateIB(WORD ByteWidth, bool NeedIndices,
		D3D11_USAGE Usage, UINT CPUAccessFlags, void *indices);

	static ID3D11Buffer *CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags);
	static ID3D11InputLayout *CreateLayout(ID3DBlob *Buffer_blob, bool WithColor = true);

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
	ID3D11RasterizerState* g_pRasWireFrame = nullptr, *g_pRasStateSolid = nullptr;
	ID3D11Texture2D *g_pDepthStencil = nullptr;
	ID3D11DepthStencilView *g_pDepthStencilView = nullptr;
	ID3D11DepthStencilState *g_pDepthStencilState = nullptr;

	ID3D11SamplerState *g_pFontSampler = nullptr;
	ID3D11ShaderResourceView *g_pFontTextureView = nullptr;
	ID3D11BlendState *g_pBlendState = nullptr;

	// ***********
	// For UI
	int VBufferSize = 5000, IBufferSize = 10000;
};
#endif // !__RENDER_BUFFER_H__