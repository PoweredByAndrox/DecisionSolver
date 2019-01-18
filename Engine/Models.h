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

namespace Engine
{
	class Render_Buffer;
	class Models;
	class Mesh
	{
	public:
		struct VERTEX
		{
			Vector3 Position;
			Vector2 texcoord;
		};

		struct Texture
		{
			string type, path;
			ID3D11ShaderResourceView *texture = nullptr;
		};

		vector<VERTEX> vertices;
		vector<UINT> indices;
		vector<Texture> textures;

		Mesh(void) {};
		Mesh(vector<VERTEX> vertices, vector<UINT> indices, vector<Texture> textures, Models *model)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			this->setupMesh();
		}

		void Draw(Matrix World, Matrix View, Matrix Proj);

		void Close();

	private:
		HRESULT hr = S_OK;

		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }

		void setupMesh();

		Render_Buffer *render = new Render_Buffer;
	};
	class Models: public Mesh, public Render_Buffer
	{
	public:
		bool Load(string *Filename);
		bool Load(string *Filename, UINT Flags, bool ConvertToLH);

		void Render(Matrix View, Matrix Proj);

		Models(void) {}
		Models(string *Filename) { if (!Load(Filename)) throw exception("Models::load == false!!!"); else AllModel.push_back(*this); }
		Models(string *Filename, UINT Flags, bool ConvertToLH)
		{
			if (!Load(Filename, Flags, ConvertToLH))
				throw exception("Models::load == false!!!");
			else
				AllModel.push_back(*this);
		}

		void Release()
		{
			SAFE_RELEASE(Device);
			if (DeviceCon)
			{
				DeviceCon->ClearState();
				DeviceCon->Flush();
				SAFE_RELEASE(DeviceCon);
			}
			SAFE_DELETE(model);

			int Cache = 0;
			if (!AllModel.empty())
				while (AllModel.size() != 0)
				{
					AllModel.at(Cache++).Close();
					AllModel.at(Cache).Release();
					AllModel.erase(AllModel.begin());
				}
		}

		void Rotation(Vector3 rotaxis, float Angel);
		void Scale(Vector3 Scale);
		void Position(Vector3 Pos);

		auto getIndices() { if (!indices.empty()) return indices; }
		auto getVertices() { if (!vertices.empty()) return vertices; }

		auto getMeshes() { if (!meshes.empty()) return meshes; }

		int getCountModels() { if (!AllModel.empty()) return AllModel.size(); }

		Vector3 getPosition() { return position.Invert().Translation(); }

		~Models() {}
	private:
		HRESULT hr = S_OK;

		Assimp::Importer *importer = nullptr;
		const aiScene *pScene = nullptr;

		vector<Texture> Textures_loaded;
		string Textype;

		aiMesh *mesh = nullptr;

		vector<Mesh> meshes;
		vector<VERTEX> vertices;
		vector<UINT> indices;

		vector<Models> AllModel;

		void processNode(aiNode *node, const aiScene *Scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *Scene);

		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
		string determineTextureType(const aiScene *Scene, aiMaterial *mat);
		int getTextureIndex(aiString *str);

		ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);

		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }

		Matrix position = XMMatrixIdentity();
		Matrix scale = XMMatrixIdentity();
		Matrix rotate = XMMatrixIdentity();

		Mesh *model = new Mesh;
	};
};
#endif // !__MODELS_H__
