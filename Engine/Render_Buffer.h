#pragma once
#ifndef __RENDER_BUFFER_H__
#define __RENDER_BUFFER_H__
#include "pch.h"

#include "Shaders.h"
#include "File_system.h"

#include <WICTextureLoader.h>

namespace Engine
{
	class Render_Buffer: public Shaders, public File_system
	{
	private:
		struct ConstantBuffer
		{
			Matrix mWorld, mView, mProj;
		} cb;

	public:
		HRESULT InitSimpleBuffer(vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader, int ConstBuff_Width = 0);

		HRESULT InitTerrain(UINT SizeofVertex, void *vertices,
			vector<UINT> indices, vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader);
		HRESULT InitModels(UINT SizeofVertex, void *vertices,
			vector<UINT> indices, vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader);

		HRESULT CreateTexture(const wchar_t *TextrFName);

		ID3D11Buffer *CreateVB(int ByteWidth, void *vertices);
		ID3D11Buffer *CreateIB(int ByteWidthInds, UINT *indices);
		HRESULT CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags);

		void RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies = 0);
		void RenderTerrain(Matrix World, Matrix View, Matrix Proj, int Indices, vector<ID3D11Buffer *> RenderBuff, UINT stride);
		void RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, ID3D11ShaderResourceView *RenderTextr, UINT stride);

		Render_Buffer()  {}
		~Render_Buffer() {}

		void Release();

			/// Getters!
		bool isInit() { return init; }
		ID3D11Buffer *getVB() { if (m_vertexBuffer) return m_vertexBuffer; return nullptr; }
		ID3D11Buffer *getIB() { if (m_indexBuffer)  return m_indexBuffer; return nullptr; }
		File_system *getFS() { if (FS) return FS; return nullptr; }

	protected:
		// **********
			// Init Class
		bool init = false;

		HRESULT hr = S_OK;

		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }

		ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_pConstBuffer = nullptr, *m_matrixBuffer = nullptr;
		ID3D11VertexShader *m_vertexShader = nullptr;
		ID3D11PixelShader *m_pixelShader = nullptr;

		ID3D11InputLayout *m_layout = nullptr;
		ID3D11SamplerState *m_sampleState = nullptr;
		ID3D11ShaderResourceView *m_texture = nullptr;
		D3D11_BUFFER_DESC bd;

		Shaders *Shader = new Shaders;
		File_system *FS = new File_system;

		vector<ID3DBlob *> Buffer_blob;
		vector<void *> Buffers;
	};
};
#endif // !__RENDER_BUFFER_H__