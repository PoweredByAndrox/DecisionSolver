#ifndef __TERRAIN__H_
#define __TERRAIN__H_
#pragma once

#include "pch.h"
#include "Shaders.h"
#include "File_system.h"
#include "Camera.h"
#include "Render_Buffer.h"

namespace Engine
{
	class Terrain;
	class Render_Buffer;
	class QuadTerrain: public Frustum
	{
		private:
			struct Vertex
			{
				Vector3 position = { 0.f, 0.f, 0.f };
				Vector2 texture = { 0.f, 0.f };
			} *m_vertexList;
			struct NT
			{
				Vector2 texcoord = { 0.f, 0.f };
				float width = 0.f;
				int triangleCount = 0;
				vector<Vector3> vertexArray;
				ID3D11Buffer *vertexBuffer = nullptr, *indexBuffer = nullptr;
				NT *nodes[4];
			} *m_parentNode;

		public:
			QuadTerrain()  {}
			~QuadTerrain() {}

			bool Initialize(Terrain *terrain, Frustum *frustum);
			void Shutdown();
			void Render(Matrix World, Matrix View, Matrix Proj);

			void Release()
			{
				SAFE_RELEASE(Device);
				if (DeviceCon)
				{
					DeviceCon->ClearState();
					DeviceCon->Flush();
				}
			}

			bool GetHeightAtPosition(float positionX, float positionZ, float &height);
		private:
			const int MAX_TRIANGLES = 10000;
			int m_triangleCount = 0;

			void CalculateMeshDimensions(int, float&, float&, float&);
			void CreateTreeNode(NT *node, Vector2 Pos, float width);
			int CountTriangles(Vector2 Pos, float);
			bool IsTriangleContained(int, Vector2 Pos, float);
			void ReleaseNode(NT *node);
			void RenderNode(NT *node, Matrix World, Matrix View, Matrix Proj);

			void FindNode(NT *node, float x, float z, float &height);
			bool CheckHeightOfTriangle(float x, float z, float &height, float v0[3], float v1[3], float v2[3]);

			unique_ptr<Terrain> terrain;
			unique_ptr<Frustum> frustum;
			unique_ptr<Render_Buffer> render;

				// Devices!!!
			void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
			void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }
			ID3D11Device *Device = nullptr;
			ID3D11DeviceContext *DeviceCon = nullptr;
	};
	class Terrain: public QuadTerrain, public Render_Buffer
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
		void Render(Matrix World, Matrix View, Matrix Proj);
		bool Initialize(Frustum *frustum, const char *HMapFile, const wchar_t *TextureTerrain);
		bool InitializeBuffers();

		void Release()
		{
			SAFE_RELEASE(Device);
			if (DeviceCon)
			{
				DeviceCon->ClearState();
				DeviceCon->Flush();
			}
		}

		int GetIndexCount() { return m_indexCount != 0 ? m_indexCount : throw exception("Terrain: m_indexCount == 0!!!"); }
		int GetVertexCount() { return m_vertexCount != 0 ? m_vertexCount : throw exception("Terrain: m_vertexCount == 0!!!"); }

		void CopyVertexArray(void *vertexList) { memcpy(vertexList, vertices, sizeof(Vertex) * m_vertexCount); }

		auto getQTerrain(float X, float Z, float &H) 
		{
			if (QTerrain.operator bool()) 
				return QTerrain->GetHeightAtPosition(X, Z, H);
			else 
				false;
		}

		auto getRenderObj() { if (render.operator bool()) return render.get(); }

		auto getVertices() { if (vertices) return vertices; }
		auto getIndices() { if (indices.size() > 0) return indices; }

	private:
		HRESULT result = S_OK;
		
			// For texture!!!
		const int TEXTURE_REPEAT = 16;
		void CalculateTextureCoordinates();

			// Devices!!!
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }
		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;

		ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_matrixBuffer = nullptr;

		int m_terrainWidth = 257, m_terrainHeight = 257, m_vertexCount = 0, m_indexCount = 0;
		float m_heightScale = 6.f;

		ID3D10Blob *vertexShaderBuffer = nullptr, *pixelShaderBuffer = nullptr;
		D3D11_BUFFER_DESC matrixBufferDesc;
		ID3D11VertexShader *m_vertexShader = nullptr;
		ID3D11PixelShader *m_pixelShader = nullptr;
		ID3D11InputLayout *m_layout = nullptr;
		ID3D11SamplerState *m_sampleState = nullptr;
		ID3D11ShaderResourceView *m_texture = nullptr;

		unique_ptr<QuadTerrain> QTerrain;
		unique_ptr<Render_Buffer> render;

			// Height Map!!!
		bool LoadBitmapHeightMap(const char *TerrainBMPfile);
		void ShutdownHeightMap();
		void SetTerrainCoordinates();
		bool BuildTerrainModel();
		void ShutdownTerrainModel();

		vector<UINT> indices;
	};
};
#endif // !__TERRAIN__H_