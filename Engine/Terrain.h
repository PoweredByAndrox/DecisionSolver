#ifndef __TERRAIN__H_
#define __TERRAIN__H_
#pragma once

#include "pch.h"
#include "Shaders.h"
#include "File_system.h"

namespace Engine
{
	class Terrain: public Shaders, public File_system
	{
	private:
		struct VertexType
		{
			Vector3 position;
			Vector4 color;
		};

	public:
		Terrain() {}
		~Terrain() {}

		void Shutdown();
		void Render(ID3D11DeviceContext*, Matrix, Matrix, Matrix);
		bool InitializeBuffers(Shaders *Shader);

		int GetIndexCount() { return m_indexCount != 0 ? m_indexCount : 0; }
		int GetVertexCount() { return m_vertexCount != 0 ? m_vertexCount : 0; }

	private:
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*, Matrix, Matrix, Matrix);

	private:
		HRESULT result = S_OK;

		int m_terrainWidth = 100, m_terrainHeight = 100;
		int m_vertexCount = 0, m_indexCount = 0;
		ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_matrixBuffer = nullptr;

		ID3D10Blob* vertexShaderBuffer = nullptr, *pixelShaderBuffer = nullptr;
		UINT numElements;
		D3D11_BUFFER_DESC matrixBufferDesc;

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11InputLayout* m_layout = nullptr;

		struct MatrixBufferType
		{
			Matrix world;
			Matrix view;
			Matrix projection;
		};

	};
};
#endif // !__TERRAIN__H_