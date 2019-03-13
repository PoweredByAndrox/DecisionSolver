#include "pch.h"

#include "Models.h"

bool Models::LoadFromFile(string *Filename)
{
	importer = new Assimp::Importer;

	pScene = importer->ReadFile(Filename->c_str(),
		aiProcess_ConvertToLeftHanded | aiProcess_OptimizeMeshes
		| aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FindInvalidData
		| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices
		| aiProcess_FindDegenerates | aiProcess_ImproveCacheLocality
	);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text:\n")
			+ string(importer->GetErrorString())).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}

	processNode(pScene->mRootNode, pScene);
	return true;
}
bool Models::LoadFromFile(string *Filename, UINT Flags, bool ConvertToLH)
{
	importer = new Assimp::Importer;

	if (ConvertToLH)
		pScene = importer->ReadFile(Filename->c_str(), Flags 
		| aiProcess_ConvertToLeftHanded | aiProcess_OptimizeMeshes 
		| aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FindInvalidData
		| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph
		| aiProcess_FindDegenerates | aiProcess_ImproveCacheLocality);
	else
		pScene = importer->ReadFile(Filename->c_str(), Flags);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text:\n")
			+ string(importer->GetErrorString())).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}

	processNode(pScene->mRootNode, pScene);

	SAFE_DELETE(importer);
	return true;
}

bool Models::LoadFromAllModels()
{
	auto Files = Application->getFS()->getFilesInFolder(".obj");
	for (int i = 0; i < Files.size(); i++)
	{
		importer = new Assimp::Importer;

		pScene = importer->ReadFile(Files.at(i).c_str(), 
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded 
		| aiProcess_OptimizeMeshes | aiProcess_SortByPType | aiProcess_FindInvalidData
		| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
			DebugTrace(string(string("Models: Error. Scene return nullptr with text:\n") + string(importer->GetErrorString())).c_str());
			throw exception("Models::pScene == nullptr!!!");
			return false;
		}

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}
bool Models::LoadFromAllModels(vector<UINT> Flags, vector<bool> ConvertToLH)
{
	int i1 = 0;

	auto Files = Application->getFS()->getFilesInFolder(".obj");
	for (int i = 0; i < Files.size(); i++)
	{
		importer = new Assimp::Importer;

		if (!ConvertToLH.empty())
		{
			for (i1 = 0; i1 < ConvertToLH.size(); i1++)
			{
				if (ConvertToLH.at(i1))
				{
					pScene = importer->ReadFile(Files.at(i).c_str(), Flags.at(i1)
						| aiProcess_ConvertToLeftHanded | aiProcess_OptimizeMeshes
						| aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FindInvalidData
						| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph);
					break;
				}
				else
				{
					pScene = importer->ReadFile(Files.at(i).c_str(), Flags.at(i1));
					break;
				}
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

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}

void Models::Render(Matrix View, Matrix Proj, bool WF)
{
	for (int i = 0; i < Meshes.size(); i++)
		Meshes.at(i).Draw(View, Proj, WF);
}

vector<Models::Texture> Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene)
{
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
				texture.texture = getTextureFromModel(Scene, getTextureIndex(&str));
			else
			{
				if (FindSubStr(Application->getFS()->GetFile(string(str.C_Str()))->ExtA, string(".dds")))
				{
					if (FAILED(CreateDDSTextureFromFile(Application->getDevice(),
						Application->getFS()->GetFile(string(str.C_Str()))->PathW.c_str(), nullptr, &texture.texture)))
					{
						DebugTrace("Models::CreateDDSTextureFromFile() create failed");
						throw exception("Create failed!!!");
					}
				}
				else
				{
					if (FAILED(CreateWICTextureFromFile(Application->getDevice(),
						Application->getFS()->GetFile(string(str.C_Str()))->PathW.c_str(), nullptr, &texture.texture)))
					{
						DebugTrace("Models::CreateWICTextureFromFile() create failed");
						throw exception("Create failed!!!");
					}
				}
			}
			texture.type = typeName;
			texture.path = Application->getFS()->GetFile(string(str.C_Str()))->PathA.c_str();

			textures.push_back(texture);

			this->Textures_loaded.push_back(texture);
		}
	}
	return textures;
}

void Models::processNode(aiNode *node, const aiScene *Scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		vector<Things> vertices;
		vector<UINT> indices;
		vector<Texture> textures;

		mesh = Scene->mMeshes[node->mMeshes[i]];
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *mat = Scene->mMaterials[mesh->mMaterialIndex];

			CountMaterial = Scene->mNumMaterials;

			if (Textype.empty())
				Textype = determineTextureType(Scene, mat);
		}

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Things t;
			t.Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->mTextureCoords[0])
				t.Texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			vertices.push_back(t);
		}
		
		for (UINT i = 0; i < mesh->mNumFaces; i++)
			for (UINT j = 0; j < mesh->mFaces[i].mNumIndices; j++)
				indices.push_back(mesh->mFaces[i].mIndices[j]);

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

		Meshes.push_back(Mesh(vertices, indices, textures));
	}

	for (int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], Scene);
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

	return "";
}

int Models::getTextureIndex(aiString *str)
{
	return stoi(string(str->C_Str()).substr(1));
}

ID3D11ShaderResourceView *Models::getTextureFromModel(const aiScene *Scene, int Textureindex)
{
	ID3D11ShaderResourceView *texture;
	int *size = reinterpret_cast<int *>(&Scene->mTextures[Textureindex]->mWidth);

	if (FAILED(CreateWICTextureFromMemory(Application->getDevice(),
		reinterpret_cast<unsigned char*>(Scene->mTextures[Textureindex]->pcData), *size, nullptr, &texture)))
	{
		DebugTrace("Models::CreateWICTextureFromMemory() create failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return texture;
}

void Models::setRotation(Vector3 rotaxis)
{
	for (int i = 0; i < Meshes.size(); i++)
		Meshes.at(i).rotate = Matrix::CreateFromQuaternion(Quaternion(rotaxis.x, rotaxis.y, rotaxis.z, 1.f));
}

void Models::setScale(Vector3 Scale)
{
	for (int i = 0; i < Meshes.size(); i++)
		Meshes.at(i).scale = Matrix::CreateScale(Scale);
}

void Models::setPosition(Vector3 Pos)
{
	for (int i = 0; i < Meshes.size(); i++)
		Meshes.at(i).position = Matrix::CreateTranslation(Pos);
}
