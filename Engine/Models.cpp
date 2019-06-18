#include "pch.h"

#include "Shaders.h"
#include "File_system.h"
class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "Models.h"
#include "Console.h"

ID3D11InputLayout *pLayout;
ID3D11SamplerState *TexSamplerState;
ID3D11RasterizerState *g_pRasWireFrame, *g_pRasStateSolid;
ID3DBlob *VS, *PS;
ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;
ID3D11Buffer *Matrices = nullptr;
shared_ptr<Render_Buffer> RB = make_shared<Render_Buffer>();

HRESULT	CompileShaderFromFile(LPCWSTR pFileName, const D3D_SHADER_MACRO* pDefines, LPCSTR pEntryPoint, LPCSTR pShaderModel, ID3DBlob** ppBytecodeBlob)
{
	UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = NULL;

	HRESULT result = D3DCompileFromFile(pFileName, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntryPoint, pShaderModel, compileFlags, 0, ppBytecodeBlob, &pErrorBlob);
	if (FAILED(result))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((LPCSTR)pErrorBlob->GetBufferPointer());
	}

	if (pErrorBlob != NULL)
		pErrorBlob->Release();

	return result;
}

bool Models::LoadFromFile(string Filename)
{	
	importer = new Assimp::Importer;

	pScene = importer->ReadFile(Filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
#if defined (_DEBUG)
		DebugTrace(string(string("Model: Scene return nullptr with text:") + importer->GetErrorString()).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception("Models::pScene == nullptr!!!");
#endif
		Console::LogError(string("Model: Scene return nullptr with text:") + importer->GetErrorString());
		return false;
	}

	processNode(pScene->mRootNode, pScene);

	CompileShaderFromFile(Application->getFS()->GetFile("VertexShader.hlsl")->PathW.c_str(), 0, "Vertex_model_VS", "vs_4_0", &VS);
	CompileShaderFromFile(Application->getFS()->GetFile("PixelShader.hlsl")->PathW.c_str(), 0, "Pixel_model_PS", "ps_4_0", &PS);

	Application->getDevice()->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	Application->getDevice()->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Application->getDevice()->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	Matrices = RB->CreateConstBuff(D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	return true;
}
bool Models::LoadFromFile(string Filename, UINT Flags, bool ConvertToLH)
{
	importer = new Assimp::Importer;

	if (ConvertToLH)
		pScene = importer->ReadFile(Filename.c_str(), Flags 
		| aiProcess_ConvertToLeftHanded | aiProcess_OptimizeMeshes 
		| aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FindInvalidData
		| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph
		| aiProcess_FindDegenerates | aiProcess_ImproveCacheLocality);
	else
		pScene = importer->ReadFile(Filename.c_str(), Flags);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
#if defined (_DEBUG)
		DebugTrace(string(string("Model: Scene return nullptr with text:") + importer->GetErrorString()).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception("Models::pScene == nullptr!!!");
#endif
		Console::LogError(string("Model: Scene return nullptr with text:") + importer->GetErrorString());
		return false;
	}

	processNode(pScene->mRootNode, pScene);

	SAFE_DELETE(importer);
	return true;
}

bool Models::LoadFromAllModels()
{
	auto Files = Application->getFS()->getFilesInFolder("", _TypeOfFile::MODELS);
	for (size_t i = 0; i < Files.size(); i++)
	{
		importer = new Assimp::Importer;

		pScene = importer->ReadFile(Files.at(i).c_str(), 
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded 
		| aiProcess_OptimizeMeshes | aiProcess_SortByPType | aiProcess_FindInvalidData
		| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
#if defined (_DEBUG)
			DebugTrace(string(string("Model: Scene return nullptr with text:") + importer->GetErrorString()).c_str());
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Models::pScene == nullptr!!!");
#endif
			Console::LogError(string("Model: Scene return nullptr with text:") + importer->GetErrorString());
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

		// Need To Check It!!!
	auto Files = Application->getFS()->getFilesInFolder(".obj");
	for (size_t i = 0; i < Files.size(); i++)
	{
		importer = new Assimp::Importer;

		if (!ConvertToLH.empty())
		{
			for (size_t i1 = 0; i1 < ConvertToLH.size(); i1++)
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
#if defined (_DEBUG)
			DebugTrace(string(string("Model: Scene return nullptr with text:") + importer->GetErrorString()).c_str());
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Models::pScene == nullptr!!!");
#endif
			Console::LogError(string("Model: Scene return nullptr with text:") + importer->GetErrorString());
			return false;
		}

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}

void Models::Render(Matrix View, Matrix Proj)
{
	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Application->getDeviceContext()->VSSetShader(pVS, 0, 0);
	Application->getDeviceContext()->PSSetShader(pPS, 0, 0);

	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes.at(i)->Draw(scale * rotate * position, View, Proj);
	}
}

Models::Models(string Filename)
{
	if (Filename.empty())
	{
#if defined (ExceptionWhenEachError)
		throw exception("Models: File Not Found!\n");
#endif
#if defined (_DEBUG)
		DebugTrace(string(string("Model File: ") + Filename + string("not found And Can't Be Load!")).c_str());
#endif
		Console::LogError(string("Model File: ") + Filename + string("not found And Can't Be Load!"));

		return;
	}
	if (!LoadFromFile(Filename))
	{
#if defined (ExceptionWhenEachError)
		throw exception("Models::LoadFromFile == false!\n");
#endif
#if defined (_DEBUG)
		DebugTrace(string(string("Model: ") + Filename + string(" Can't Be Load!")).c_str());
#endif
		Console::LogError(string("Model: ") + Filename + string(" Can't Be Load!"));
	}
}

vector<Texture> Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName,
	const aiScene *Scene)
{
	vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (size_t j = 0; j < Textures_loaded.size(); j++)
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
				auto textr = Application->getFS()->GetFile(string(str.C_Str()));
				if (FindSubStr(textr->ExtA, string(".dds")))
				{
					if (FAILED(CreateDDSTextureFromFile(Application->getDevice(), textr->PathW.c_str(), nullptr, &texture.texture)))
					{
#if defined (_DEBUG)
						DebugTrace("Models::CreateDDSTextureFromFile() create failed");
#endif
#if defined (ExceptionWhenEachError)
						throw exception("Create failed!!!");
#endif
						Console::LogInfo(string("Something is wrong with this texture: ") + textr->FileA);
					}
				}
				else
				{
					if (FAILED(CreateWICTextureFromFile(Application->getDevice(), textr->PathW.c_str(), nullptr, &texture.texture)))
					{
#if defined (_DEBUG)
						DebugTrace("Models::CreateWICTextureFromFile() create failed");
#endif
#if defined (ExceptionWhenEachError)
						throw exception("Create failed!!!");
#endif
						Console::LogInfo(string("Something is wrong with this texture: ") + textr->FileA);
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
	for (UINT i = 0; i < node->mNumMeshes; i++)
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

		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			Things vertex;

			vertex.Pos = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->mTextureCoords[0])
				vertex.Tex = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			vertices.push_back(vertex);
		}

		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (UINT j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
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

		meshes.push_back(make_shared<Mesh>(vertices, indices, textures));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
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
#if defined (_DEBUG)
		DebugTrace("Models::CreateDDSTextureFromFile() create failed");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("Create failed!!!");
#endif
		Console::LogInfo(string("Something is wrong with this texture: ") + Scene->mTextures[Textureindex]->mFilename.C_Str());

		return nullptr;
	}

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

void Models::Mesh::Init(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Things) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];

	Application->getDevice()->CreateBuffer(&vbd, &initData, &VertexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &indices[0];

	Application->getDevice()->CreateBuffer(&ibd, &initData, &IndexBuffer);
}

void Models::Mesh::Draw(Matrix World, Matrix View, Matrix Proj)
{
	UINT stride = sizeof(Things);
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (Matrices)
		ThrowIfFailed(Application->getDeviceContext()->Map(Matrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = (ConstantBuffer *)mappedResource.pData;

	Matrix WVP = World * View * Proj;
	dataPtr->mMVP = XMMatrixTranspose(WVP);

	if (Matrices)
	{
		Application->getDeviceContext()->Unmap(Matrices, 0);
		Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &Matrices);
	}

	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	Application->getDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	if (!textures.empty() && textures[0].texture)
		Application->getDeviceContext()->PSSetShaderResources(0, 1, &textures[0].texture);

	Application->getDeviceContext()->IASetInputLayout(pLayout);

	Application->getDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}
