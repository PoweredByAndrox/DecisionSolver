#include "pch.h"

#include "Models.h"

bool Models::Load(string Filename)
{
	pScene = importer.ReadFile(Filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
		return false;

	GetD3DDevice();
	GetD3DHWND();
	
	processNode(pScene->mRootNode, pScene);

	return true;
}

void Models::Draw()
{
	for (size_t i = 0; i < Meshes.size(); i++)
		Meshes[i].Draw();
}

	// Function for work with PhysX (Need rework and connect to class physics)
vector<float> Models::GetVertices(void)//LPD3DXMESH  *g_pMesh
{
	vector<float> vertices;
	//DWORD stride = D3DXGetFVFVertexSize((*g_pMesh)->GetFVF());
	BYTE* vbptr = NULL;
	//(*g_pMesh)->LockVertexBuffer(0, (LPVOID*)&vbptr);
	int ii = -1;
	//for (int i = 0; i < (*g_pMesh)->GetNumVertices(); i++)
	//{
	//	ii++;
	//	D3DXVECTOR3* pos = (D3DXVECTOR3*)vbptr;
		//vertices.push_back(pos->x);
		//vertices.push_back(pos->y);
		//vertices.push_back(pos->z);
		//vbptr += stride;
	//}
	//(*g_pMesh)->UnlockVertexBuffer();
	return vertices;
}

	// It needs so
vector<short> Models::GetIndices(void) //LPD3DXMESH *g_pMesh
{
	LPVOID * ppData;
	DWORD stride = sizeof(short);
	BYTE* ibptr = NULL;
	//short* indices = new short[(*g_pMesh)->GetNumFaces() * 3];
	vector<short> copy;
	//(*g_pMesh)->LockIndexBuffer(0, (LPVOID*)&indices);
//	for (int i = 0; i < (*g_pMesh)->GetNumFaces() * 3; i++)
//		copy.push_back(indices[i]);

//	(*g_pMesh)->UnlockIndexBuffer();
	return copy;
}

Mesh Models::processMesh(aiMesh *mesh, const aiScene *Scene)
{
	// Data to fill
	vector<VERTEX> vertices;
	vector<UINT> indices;
	vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = Scene->mMaterials[mesh->mMaterialIndex];

		if (Textype.empty()) 
			Textype = determineTextureType(Scene, mat);
	}

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.X = mesh->mVertices[i].x;
		vertex.Y = mesh->mVertices[i].y;
		vertex.Z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = Scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", Scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<Texture> Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene)
{
	vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < Textures_loaded.size(); j++)
		{
			if (std::strcmp(Textures_loaded[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(Textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			HRESULT hr;
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

				if (FAILED(hr = CreateWICTextureFromFile(File_system::GetResPathW(&string(str.C_Str())).c_str(),
					nullptr, &texture.texture)))
				{
					if (HWND == NULL)
						GetD3DHWND();
					MessageBoxA(HWND, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);
				}
			}
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->Textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Models::Close()
{
	for (size_t i = 0; i < Meshes.size(); i++)
		Meshes[i].Close();

	if (Device != nullptr)
		Device->Release();
}

void Models::processNode(aiNode *node, const aiScene *scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Meshes.push_back(this->processMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		this->processNode(node->mChildren[i], scene);
}

string Models::determineTextureType(const aiScene *Scene, aiMaterial *mat)
{
	aiString textypeStr;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
	string textypeteststr = textypeStr.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5")
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

	if (FAILED(hr = CreateWICTextureFromMemory(
		reinterpret_cast<unsigned char*>(Scene->mTextures[Textureindex]->pcData),  *size, nullptr, &texture)))
		if (HWND == NULL)
			GetD3DHWND();
	MessageBoxA(HWND, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);

	return texture;
}