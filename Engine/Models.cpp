#include "pch.h"
#include "Models.h"

bool Models::Load(string *Filename)
{
	importer = new Assimp::Importer;
	pScene = importer->ReadFile(Filename->c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text:\n") 
				+ string(importer->GetErrorString()) + string(" Line: 8\n")).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}
	GetD3DDevice();
	GetD3DHWND();
	
	processNode(pScene->mRootNode, pScene);

	return true;
}

bool Models::Load(string *Filename, UINT Flags, bool ConvertToLH)
{
	importer = new Assimp::Importer;
	if (ConvertToLH)
		pScene = importer->ReadFile(Filename->c_str(), Flags | aiProcess_ConvertToLeftHanded);
	else
		pScene = importer->ReadFile(Filename->c_str(), Flags);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text:\n")
			+ string(importer->GetErrorString()) + string(" Line: 32\n")).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}
	GetD3DDevice();
	GetD3DHWND();

	processNode(pScene->mRootNode, pScene);

	return true;
}

void Models::Draw()
{
//	for (size_t i = 0; i < Meshes.size(); i++)
//		Meshes[i].Draw();
}

Mesh Models::processMesh(aiMesh *mesh, const aiScene *Scene)
{
	vector<VERTEX> vertices;
	vector<uint16_t> indices;
	vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = Scene->mMaterials[mesh->mMaterialIndex];

		if (Textype.empty()) 
			Textype = determineTextureType(Scene, mat);
	}

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.texcoord = XMFLOAT2(0.f, 0.f);

		vertices.push_back(vertex);
	}

	for (uint16_t i = 0; i < mesh->mNumFaces; i++)
		for (uint16_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			indices.push_back(mesh->mFaces[i].mIndices[j]);

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = Scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse", Scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			///It doesn't work!
		/*vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular", Scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	
		vector<Texture> normalMaps = loadMaterialTextures(material, 
			aiTextureType_HEIGHT, "texture_normal", Scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		vector<Texture> heightMaps = loadMaterialTextures(material, 
			aiTextureType_AMBIENT, "texture_height", Scene);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());*/
	}

	return Mesh(vertices, indices, textures);
}

vector<Mesh::Texture> Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene)
{
	vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (UINT j = 0; j < Textures_loaded.size(); j++)
		{
			if (strcmp(Textures_loaded[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(Textures_loaded[j]);
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
				if (Device == nullptr)
					GetD3DDevice();

				ThrowIfFailed(CreateWICTextureFromFile(Device, GetResPathW(&string(str.C_Str())).c_str(), nullptr, &texture.texture));
			}
			texture.type = typeName;
			texture.path = GetResPathA(&string(str.C_Str()))->c_str();
			textures.push_back(texture);
			this->Textures_loaded.push_back(texture);
		}
	}
	return textures;
}

void Models::Close()
{
	//for (int i = 0; i < Meshes.size(); i++)
		// Meshes[i].Close();

	if (Device != nullptr)
		Device->Release();
}

void Models::processNode(aiNode *node, const aiScene *scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
		Meshes.push_back(this->processMesh(scene->mMeshes[node->mMeshes[i]], scene));

	for (UINT i = 0; i < node->mNumChildren; i++)
		this->processNode(node->mChildren[i], scene);
}

string Models::determineTextureType(const aiScene *Scene, aiMaterial *mat)
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
}

int Models::getTextureIndex(aiString *str)
{
	string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

ID3D11ShaderResourceView *Models::getTextureFromModel(const aiScene *Scene, int Textureindex)
{
	HRESULT hr = S_OK;
	int* size = reinterpret_cast<int*>(&Scene->mTextures[Textureindex]->mWidth);

	if (Device == nullptr)
		GetD3DDevice();

	ThrowIfFailed(CreateWICTextureFromMemory(Device,
		reinterpret_cast<unsigned char*>(Scene->mTextures[Textureindex]->pcData), *size, nullptr, &texture));

	return texture;
}