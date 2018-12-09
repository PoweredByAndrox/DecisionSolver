#ifndef __TERRAIN__H_
#define __TERRAIN__H_
#pragma once

#include "pch.h"
#include "Shaders.h"
#include "File_system.h"

#include <WICTextureLoader.h>

namespace Engine
{
	class Terrain: public Shaders, public File_system
	{
	private:
		struct Vertex
		{
			Vector3 position = { 0.f, 0.f, 0.f };
			Vector2 texcoord = { 0.f, 0.f };
		} *vertices;
		struct MatrixBuffer
		{
			Matrix world;
			Matrix view;
			Matrix projection;
		};
		struct HeightMap
		{
			Vector3 Pos = { 0.f, 0.f, 0.f };
			Vector2 texcoord = { 0.f, 0.f };
		} *HMap;
		struct Model
		{
			Vector3 Pos = { 0.f, 0.f, 0.f };
			Vector2 texcoord = { 0.f, 0.f };
		} *model;

	public:
		Terrain() {}
		~Terrain() {}

		void Shutdown();
		void Render(Matrix, Matrix, Matrix);
		bool Initialize(Shaders *Shader, const char *HMapFile, const wchar_t *TextureTerrain);
		bool InitializeBuffers();

		int GetIndexCount() { return m_indexCount != 0 ? m_indexCount : throw exception("Terrain: m_indexCount == 0!!!"); }
		int GetVertexCount() { return m_vertexCount != 0 ? m_vertexCount : throw exception("Terrain: m_vertexCount == 0!!!"); }

	private:
		void ShutdownBuffers();
		void RenderBuffers(Matrix, Matrix, Matrix);

	private:
		HRESULT result = S_OK;
		
			// For texture!!!
		const int TEXTURE_REPEAT = 8;
		void CalculateTextureCoordinates();

			// Devices!!!
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }
		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;

		int m_terrainWidth = 257, m_terrainHeight = 257, m_vertexCount = 0, m_indexCount = 0;
		ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_matrixBuffer = nullptr;

		float m_heightScale = 6.f;

		ID3D10Blob *vertexShaderBuffer = nullptr, *pixelShaderBuffer = nullptr;
		D3D11_BUFFER_DESC matrixBufferDesc;
		ID3D11VertexShader *m_vertexShader = nullptr;
		ID3D11PixelShader *m_pixelShader = nullptr;
		ID3D11InputLayout *m_layout = nullptr;
		ID3D11SamplerState *m_sampleState = nullptr;
		ID3D11ShaderResourceView *m_texture;

		unique_ptr<Shaders> Shader;

			// Height Map!!!
		bool LoadBitmapHeightMap(const char *TerrainBMPfile);
		void ShutdownHeightMap();
		void SetTerrainCoordinates();
		bool BuildTerrainModel();
		void ShutdownTerrainModel();
	};
};
#endif // !__TERRAIN__H_