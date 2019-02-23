#include "pch.h"

#include "Models.h"

using namespace Engine;

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

	GetD3DDevice();
	GetD3DDeviceCon();

	//m_states = make_unique<CommonStates>(Device);

	//m_fxFactory = make_unique<EffectFactory>(Device);

	//m_model = Model::CreateFromCMO(Device, L"D:/DecisionSolver/Engine/resource/models/rope.cmo", *m_fxFactory);

	//processNode(pScene->mRootNode, pScene);

	SAFE_DELETE(importer);

	setupMesh();
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
	GetD3DDevice();

	processNode(pScene->mRootNode, pScene);

	SAFE_DELETE(importer);
	return true;
}

bool Models::LoadFromAllModels()
{
	auto Files = getFilesInFolder(&string("models"), ".obj");
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
		GetD3DDevice();

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}
bool Models::LoadFromAllModels(vector<UINT> Flags, vector<bool> ConvertToLH)
{
	int i1 = 0;

	auto Files = getFilesInFolder(&string("models"), ".obj");
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
		GetD3DDevice();

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}

void Models::Render(Matrix View, Matrix Proj, bool WF)
{
	//	for (int i = 0; i < meshes.size(); i++)
	//			meshes.at(i).Draw(scale * rotate * position, View, Proj);
	if (!textures.empty())
		render->RenderModels(scale * rotate * position, View, Proj, 36 /*indices.size()*/, sizeof(Things), textures[0].texture, WF);
	else
		render->RenderModels(scale * rotate * position, View, Proj, 36 /*indices.size()*/, sizeof(Things), nullptr, WF);

	//m_model->Draw(DeviceCon, *m_states, scale * rotate * position, View, Proj);
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

void Models::processNode(aiNode *node, const aiScene *Scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		mesh = Scene->mMeshes[node->mMeshes[i]];
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *mat = Scene->mMaterials[mesh->mMaterialIndex];

			CountMaterial = Scene->mNumMaterials;

			if (Textype.empty())
				Textype = determineTextureType(Scene, mat);
		}

		//for (int i = 0; i < mesh->mNumVertices; i++)
		//{
		//	if (mesh->mTextureCoords[0])
		//		vertices.push_back(Things(Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z), Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)));
		//}

		for (int i = 0; i < mesh->mNumFaces; i++)
			for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				indices.push_back(mesh->mFaces[i].mIndices[j]);
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
	int* size = reinterpret_cast<int*>(&Scene->mTextures[Textureindex]->mWidth);

	GetD3DDevice();
	V(CreateWICTextureFromMemory(Device, reinterpret_cast<unsigned char*>(Scene->mTextures[Textureindex]->pcData), *size, nullptr, &texture));

	return texture;
}

void Models::setRotation(Vector3 rotaxis)
{
	rotate = Matrix::CreateFromQuaternion(Quaternion(rotaxis.x, rotaxis.y, rotaxis.z, 1.f));
}

void Models::setScale(Vector3 Scale)
{
	scale = Matrix::CreateScale(Scale);
}

void Models::setPosition(Vector3 Pos)
{
	position = Matrix::CreateTranslation(Pos);
}

void Models::setupMesh()
{
	vector<wstring> FileShaders;
	FileShaders.push_back(render->GetFile(string("VertexShader.hlsl"))->PathW);
	FileShaders.push_back(render->GetFile(string("PixelShader.hlsl"))->PathW);

	vector<string> Functions, Version;
	Functions.push_back(string("VS"));
	Functions.push_back(string("PS"));

	Version.push_back(string("vs_4_0"));
	Version.push_back(string("ps_4_0"));

	vector<Things> Some;
	Some.push_back(Things(Vector3(-1.0f, 1.0f, -1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)));
	Some.push_back(Things(Vector3(1.0f, 1.0f, -1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)));
	Some.push_back(Things(Vector3(1.0f, 1.0f, 1.0f), Vector4(0.0f, 1.0f, 1.0f, 1.0f)));
	Some.push_back(Things(Vector3(-1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	Some.push_back(Things(Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f)));
	Some.push_back(Things(Vector3(1.0f, -1.0f, -1.0f), Vector4(1.0f, 1.0f, 0.0f, 1.0f)));
	Some.push_back(Things(Vector3(1.0f, -1.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)));
	Some.push_back(Things(Vector3(-1.0f, -1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f)));

	WORD indices[] =
	{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
	};

	render->InitModels(Some.size(), &Some.at(0), 36, &indices[0], sizeof(Things), &FileShaders, &Functions, &Version);
}