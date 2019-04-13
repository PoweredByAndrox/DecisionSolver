#include "pch.h"

#include "Shaders.h"
#include "File_system.h"
class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "Models.h"

vector<ID3DBlob *> Buffer_blob1;
vector<void *> Buffers1;
ID3D11InputLayout *pLayout;
ID3D11SamplerState *TexSamplerState;
ID3D11RasterizerState *g_pRasWireFrame, *g_pRasStateSolid;

bool Models::LoadFromFile(string Filename)
{
	vector<Things> vertices =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }
	};
	vector<UINT> indices =
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

	auto W = make_unique<Render_Buffer>();
	vector<wstring> FileShaders;
	FileShaders.push_back(wstring(L"D:/DecisionSolver/Engine/resource/shaders/shader.fx"));
	FileShaders.push_back(wstring(L"D:/DecisionSolver/Engine/resource/shaders/shader.fx"));

	vector<string> Functions, Version;
	Functions.push_back(string("VS"));
	Functions.push_back(string("PS"));

	Version.push_back(string("vs_4_0"));
	Version.push_back(string("ps_4_0"));

	Buffers1 = Application->getShader()->CompileShaderFromFile(Buffer_blob1 = Application->getShader()->CreateShaderFromFile(FileShaders, Functions, Version));
	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	Application->getDevice()->CreateInputLayout(layout, numElements, Buffer_blob1[0]->GetBufferPointer(), Buffer_blob1[0]->GetBufferSize(), &pLayout);
	SAFE_RELEASE(Buffer_blob1[0]);

	Application->getDeviceContext()->IASetInputLayout(pLayout);
	
	//D3D11_SAMPLER_DESC samplerDesc;
	//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MipLODBias = 0.0f;
	//samplerDesc.MaxAnisotropy = 1;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//samplerDesc.BorderColor[0] = 0;
	//samplerDesc.BorderColor[1] = 0;
	//samplerDesc.BorderColor[2] = 0;
	//samplerDesc.BorderColor[3] = 0;
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//if (FAILED(Application->getDevice()->CreateSamplerState(&samplerDesc, &m_sampleState)))
	//{
	//	DebugTrace("Render_Buffer::InitModels()->CreateSamplerState() is failed");
	//	throw exception("Init is failed!!!");
	//	return E_FAIL;
	//}


	//D3D11_SAMPLER_DESC samplerDesc;
	//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Application->getDevice()->CreateSamplerState(&samplerDesc, &TexSamplerState);

	auto WF = W->CreateWF();
	g_pRasWireFrame = WF.at(0);
	g_pRasStateSolid = WF.at(1);

	meshes.push_back(make_unique<Mesh>(vertices, indices, vector<Texture>(), ""));

	return true;

	importer = new Assimp::Importer;

	pScene = importer->ReadFile(Filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text: ") + string(importer->GetErrorString())).c_str());
		throw exception("Models::pScene == nullptr!!!");
		return false;
	}

	processNode(pScene->mRootNode, pScene);
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
		DebugTrace(string(string("Models: Error. Scene returned nullptr with text: ") + string(importer->GetErrorString())).c_str());
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
			DebugTrace(string(string("Models: Error. Scene return nullptr with text: ") + string(importer->GetErrorString())).c_str());
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
			DebugTrace(string(string("Models: Error. Scene return nullptr with text: ") + string(importer->GetErrorString())).c_str());
			throw exception("Models::pScene == nullptr!!!");
			return false;
		}

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}

void Models::Render(Matrix View, Matrix Proj)
{
	for (int i = 0; i < meshes.size(); i++)
	{//scale * rotate *
		meshes.at(i)->Draw(position, View, Proj);
	}
}

vector<Texture> Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene)
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
	vector<Texture> textures;

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		vector<Things> vertices;
		vector<UINT> indices;

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

			//if (mesh->mTextureCoords[0])
			//	vertex.Tex = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

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
HRESULT m_compileshaderfromfile(LPCWSTR FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	ShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFileW(FileName, NULL, NULL, EntryPoint, ShaderModel, ShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr) && pErrorBlob != NULL)
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

	SAFE_RELEASE(pErrorBlob);
	return hr;
}

void Models::Mesh::Init(vector<Things> vertices, vector<UINT> indices, vector<Texture> textures, string ID)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->ID = ID;

	vertices.size() < 65535 ? type = uint16 : type = uint32;

	pVS = (ID3D11VertexShader *)Buffers1[0]; // VS
	pPS = (ID3D11PixelShader *)Buffers1[1]; // PS

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Things) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	Application->getDevice()->CreateBuffer(&bd, &InitData, &VertexBuffer);

	UINT stride = sizeof(Things);
	UINT offset = 0;
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices.data();
	Application->getDevice()->CreateBuffer(&bd, &InitData, &IndexBuffer);

	Application->getDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	Application->getDevice()->CreateBuffer(&bd, NULL, &pConstantBuffer);

	//HRESULT hr = S_OK;
	//ID3DBlob* pVSBlob = NULL;
	//hr = m_compileshaderfromfile(L"D:/DecisionSolver/Engine/resource/shaders/VertexShader.hlsl", "Vertex_model_VS", "vs_4_0", &pVSBlob);

	//hr = Application->getDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pVS);

	//D3D11_INPUT_ELEMENT_DESC layout[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	//UINT numElements = ARRAYSIZE(layout);

	//hr = Application->getDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pLayout);
	//SAFE_RELEASE(pVSBlob);

	//Application->getDeviceContext()->IASetInputLayout(pLayout);

	//ID3DBlob* pPSBlob = NULL;
	//hr = m_compileshaderfromfile(L"D:/DecisionSolver/Engine/resource/shaders/PixelShader.hlsl", "Pixel_model_PS", "ps_4_0", &pPSBlob);

	//hr = Application->getDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPS);
	//SAFE_RELEASE(pPSBlob);

	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(Things) * this->vertices.size();
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = &this->vertices[0];
	//hr = Application->getDevice()->CreateBuffer(&bd, &InitData, &VertexBuffer);

	//UINT stride = sizeof(Things);
	//UINT offset = 0;
	//Application->getDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(UINT) * this->indices.size();
	//bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	//InitData.pSysMem = &this->indices[0];
	//hr = Application->getDevice()->CreateBuffer(&bd, &InitData, &IndexBuffer);

	//Application->getDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	////bd.Usage = D3D11_USAGE_DEFAULT;
	////bd.ByteWidth = sizeof(ConstantBuffer);
	////bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	////bd.CPUAccessFlags = 0;
	////hr = Application->getDevice()->CreateBuffer(&bd, NULL, &pConstantBuffer);
}

void Models::Mesh::Draw(Matrix World, Matrix View, Matrix Proj)
{
	ConstantBuffer cb;
	cb.mMVP = XMMatrixTranspose(World * View * Proj);
	Application->getDeviceContext()->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);

	if (Application->IsWireFrame() && g_pRasWireFrame)
		Application->getDeviceContext()->RSSetState(g_pRasWireFrame);
	else if (!Application->IsWireFrame() && g_pRasStateSolid)
		Application->getDeviceContext()->RSSetState(g_pRasStateSolid);

	if (TexSamplerState)
		Application->getDeviceContext()->PSSetSamplers(0, 1, &TexSamplerState);

	Application->getDeviceContext()->VSSetShader(pVS, NULL, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	Application->getDeviceContext()->PSSetShader(pPS, NULL, 0);
	Application->getDeviceContext()->DrawIndexed(36, 0, 0);

	//UINT stride = sizeof(Things);
	//UINT offset = 0;

	//Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	////ConstantBuffer cb;
	////cb.mMVP = XMMatrixTranspose(World * View * Proj);
	////Application->getDeviceContext()->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);

	//Application->getDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	//Application->getDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Application->getDeviceContext()->VSSetShader(pVS, 0, 0);
	////Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	//Application->getDeviceContext()->PSSetShader(pPS, 0, 0);
	//Application->getDeviceContext()->PSSetSamplers(0, 1, &TexSamplerState);

	//if (!textures.empty())
	//	Application->getDeviceContext()->PSSetShaderResources(0, 1, &textures[0].texture);

	//Application->getDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}
