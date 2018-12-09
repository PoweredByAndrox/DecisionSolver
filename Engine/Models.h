#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__
#include "pch.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <Effects.h>
#include <GeometricPrimitive.h>
#include <CommonStates.h>
#include <WICTextureLoader.h>

#include "Shaders.h"
#include "File_system.h"

namespace Engine
{
	class Mesh: public File_system, public Shaders
	{
	public:
		struct VERTEX
		{
			Vector3 Position;
			Vector2 texcoord;
		};

		struct Texture
		{
			string type;
			string path;
			ID3D11ShaderResourceView *texture = nullptr;
		};

		struct ConstantBuffer
		{
			Matrix World;
			Matrix View;
			Matrix Proj;
		} cb;

		vector<VERTEX> vertices;
		vector<UINT> indices;
		vector<Texture> textures;

		Mesh() {};
		Mesh(vector<VERTEX> vertices, vector<UINT> indices, vector<Texture> textures)
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

		Matrix World = Matrix::Identity, View, Proj;

		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;
	
		ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr, *m_pConstantBuffer = nullptr;
		ID3D11VertexShader *m_vertexShader = nullptr;
		ID3D11PixelShader *m_pixelShader = nullptr;
		ID3D11InputLayout *m_layout = nullptr;
		ID3D11SamplerState *m_sampleState = nullptr;
		ID3D11ShaderResourceView *m_texture = nullptr;

		HRESULT InitShader();

		unique_ptr<Shaders> Shader = make_unique<Shaders>();

		void GetD3DDevice() { Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { DeviceCon = DXUTGetD3D11DeviceContext(); }

		void Rotation();
		auto Scale();
		void Position();

		bool setupMesh();
	};

	class Models: public Mesh
	{
	public:
		bool Load(string *Filename);
		bool Load(string *Filename, UINT Flags, bool ConvertToLH);

		void Render(Matrix World, Matrix View, Matrix Proj);

		Models() {}
		Models(string *Filename, Shaders *Shader) { if (!Load(Filename)) throw exception("Models::load == false!!!"); }
		Models(string *Filename, Shaders *Shader, UINT Flags, bool ConvertToLH)
		{
			if (!Load(Filename, Flags, ConvertToLH)) 
				throw exception("Models::load == false!!!");
		}

		void Rotation(vector<Mesh>);
		void Scale();
		void Position(vector<Mesh>);

		~Models() {}

	private:
		HRESULT hr = S_OK;

		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;

		Assimp::Importer *importer = nullptr;
		const aiScene *pScene = nullptr;

		vector<Texture> Textures_loaded;
		string Textype;

		aiMesh *mesh = nullptr;

		vector<Mesh> meshes;

		void processNode(aiNode *node, const aiScene *Scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *Scene);

		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene);
		string determineTextureType(const aiScene *Scene, aiMaterial *mat);
		int getTextureIndex(aiString *str);

		ID3D11ShaderResourceView *getTextureFromModel(const aiScene *Scene, int Textureindex);

		void GetD3DDevice() { Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { DeviceCon = DXUTGetD3D11DeviceContext(); }

		float *m_pHeightBits = new float[2.f * 2.f];

	};
};
#endif // !__MODELS_H__
