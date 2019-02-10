#include "pch.h"

#include "Models.h"

bool Engine::Models::LoadFromFile(string *Filename)
{
	importer = new Assimp::Importer;

	pScene = importer->ReadFile(Filename->c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text:\n") 
			+ string(importer->GetErrorString())).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}
	GetD3DDevice();
	
	processNode(pScene->mRootNode, pScene);
	return true;
}
bool Engine::Models::LoadFromFile(string *Filename, UINT Flags, bool ConvertToLH)
{
	importer = new Assimp::Importer;

	if (ConvertToLH)
		pScene = importer->ReadFile(Filename->c_str(), Flags | aiProcess_ConvertToLeftHanded);
	else
		pScene = importer->ReadFile(Filename->c_str(), Flags);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text:\n")
			+ string(importer->GetErrorString())).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}
	GetD3DDevice();

	processNode(pScene->mRootNode, pScene);
	return true;
}

bool Engine::Models::LoadFromAllModels()
{
	importer = new Assimp::Importer;

	auto Files = getFilesInFolder(&string("models"), ".obj");
	for (int i = 0; i < Files.size(); i++)
	{
		pScene = importer->ReadFile(Files.at(i).c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
			DebugTrace(string(string("Models: Error. Scene return nullptr with text:\n") + string(importer->GetErrorString())).c_str());
			throw exception("Models::pScene == nullptr!!!");
			return false;
		}
		GetD3DDevice();

		processNode(pScene->mRootNode, pScene);
	}

	return true;
}
bool Engine::Models::LoadFromAllModels(vector<UINT> Flags, vector<bool> ConvertToLH)
{
	importer = new Assimp::Importer;
	int i1 = 0;

	auto Files = getFilesInFolder(&string("models"), ".obj");
	for (int i = 0; i < Files.size(); i++)
	{
		if (!ConvertToLH.empty())
		{
			for (i1 = 0; i1 < ConvertToLH.size(); i1++)
				if (ConvertToLH.at(i1))
				{
					pScene = importer->ReadFile(Files.at(i).c_str(), Flags.at(i1) | aiProcess_ConvertToLeftHanded);
					break;
				}
				else
				{
					pScene = importer->ReadFile(Files.at(i).c_str(), Flags.at(i1));
					break;
				}
		}
		else
			pScene = importer->ReadFile(Files.at(i).c_str(), 0);

		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
			DebugTrace(string(string("Models: Error. Scene return nullptr with text:\n") + string(importer->GetErrorString())).c_str());
			throw exception("Models::pScene == nullptr!!!");
			return false;
		}
		GetD3DDevice();

		processNode(pScene->mRootNode, pScene);
	}

	return true;
}


void Engine::Models::Render(Matrix View, Matrix Proj)
{
	for (int i = 0; i < meshes.size(); i++)
		meshes.at(i).Draw(scale * rotate * position, View, Proj);
}

Engine::Mesh Engine::Models::processMesh(aiMesh *mesh, const aiScene *Scene)
{
	vector<VERTEX> verticesCache;
	vector<UINT> indicesCache;
	vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *mat = Scene->mMaterials[mesh->mMaterialIndex];

		if (Textype.empty())
			Textype = determineTextureType(Scene, mat);
	}

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		if (mesh->mTextureCoords[0])
			vertex.texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.texcoord = Vector2(0.f, 0.f);

		vertices.push_back(vertex);
		verticesCache.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
			indicesCache.push_back(mesh->mFaces[i].mIndices[j]);
		}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = Scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", Scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		///It doesn't work!
/*
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", Scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", Scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", Scene);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
*/
	}

	return Mesh(verticesCache, indicesCache, textures, this);
}

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
vector<Engine::Mesh::Texture> Engine::Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene)
{
	USES_CONVERSION;

	vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (int j = 0; j < Textures_loaded.size(); j++)
		{
			if (strcmp(Textures_loaded.at(j).path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(Textures_loaded.at(j));
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			if (Textype == "embedded compressed texture")
			{
				int Textureindex = getTextureIndex(&str);
				texture.texture = getTextureFromModel(Scene, Textureindex);
			}
			else
			{
				GetD3DDevice();
				if (FindSubStr(GetFile(string(str.C_Str()))->ExtA, string(".dds")))
					V(CreateDDSTextureFromFile(Device, GetFile(string(str.C_Str()))->PathW.c_str(), nullptr, &texture.texture))
				else
					V(CreateWICTextureFromFile(Device, GetFile(string(str.C_Str()))->PathW.c_str(), nullptr, &texture.texture));
			}
			texture.type = typeName;
			texture.path = GetFile(string(str.C_Str()))->PathA.c_str();

			textures.push_back(texture);

			this->Textures_loaded.push_back(texture);
		}
	}
	return textures;
}

void Engine::Models::processNode(aiNode *node, const aiScene *Scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
		meshes.push_back(this->processMesh(Scene->mMeshes[node->mMeshes[i]], Scene));

	for (int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], Scene);
}

string Engine::Models::determineTextureType(const aiScene *Scene, aiMaterial *mat)
{
	aiString textypeStr;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
	string textypeteststr = textypeStr.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2"
		|| textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5")
	{
		if (Scene->mTextures[0]->mHeight == 0)
			return "embedded compressed texture";
		else
			return "embedded non-compressed texture";
	}
	if (textypeteststr.find('.') != string::npos)
		return "textures are on disk";

	return "";
}

int Engine::Models::getTextureIndex(aiString *str)
{
	string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

ID3D11ShaderResourceView *Engine::Models::getTextureFromModel(const aiScene *Scene, int Textureindex)
{
	ID3D11ShaderResourceView *texture;
	int* size = reinterpret_cast<int*>(&Scene->mTextures[Textureindex]->mWidth);

	GetD3DDevice();
	V(CreateWICTextureFromMemory(Device, reinterpret_cast<unsigned char*>(Scene->mTextures[Textureindex]->pcData), *size, nullptr, &texture));

	return texture;
}

void Engine::Models::setRotation(Vector3 rotaxis)
{
	rotate = Matrix::CreateFromQuaternion(Quaternion(rotaxis.x, rotaxis.y, rotaxis.z, 1.f));
}

void Engine::Models::setScale(Vector3 Scale)
{
	scale = Matrix::CreateScale(Scale);
}

void Engine::Models::setPosition(Vector3 Pos)
{
	position = Matrix::CreateTranslation(Pos);
}

void Engine::Mesh::Draw(Matrix World, Matrix View, Matrix Proj)
{
	if (!textures.empty())
		render->RenderModels(World, View, Proj, indices.size(), textures[0].texture, sizeof(VERTEX));
	else
		render->RenderModels(World, View, Proj, indices.size(), nullptr, sizeof(VERTEX));
}

void Engine::Mesh::Close()
{
	SAFE_RELEASE(Device);
	if (DeviceCon)
	{
		DeviceCon->ClearState();
		DeviceCon->Flush();
		SAFE_RELEASE(DeviceCon);
	}

	for (int i = 0; i < textures.size(); i++)
		 textures.at(i).texture->Release();

	SAFE_DELETE(render);
}

void Engine::Mesh::setupMesh()
{
	vector<wstring> FileShaders;
	FileShaders.push_back(render->GetFile(string("VertexShader.hlsl"))->PathW);
	FileShaders.push_back(render->GetFile(string("PixelShader.hlsl"))->PathW);

	vector<string> Functions, Version;
	Functions.push_back(string("main"));
	Functions.push_back(string("main"));

	Version.push_back(string("vs_4_0"));
	Version.push_back(string("ps_4_0"));

	render->InitModels(sizeof(VERTEX) * vertices.size(), vertices.data(), indices, &FileShaders, &Functions, &Version);
}
