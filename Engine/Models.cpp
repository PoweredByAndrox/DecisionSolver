#include "pch.h"
#include "Models.h"

bool Engine::Models::Load(string *Filename)
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
	
	processNode(pScene->mRootNode, pScene);

	return true;
}

bool Engine::Models::Load(string *Filename, UINT Flags, bool ConvertToLH)
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

	processNode(pScene->mRootNode, pScene);

	return true;
}

void Engine::Models::Render(Matrix World, Matrix View, Matrix Proj)
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(World, View, Proj);
}

Engine::Mesh Engine::Models::processMesh(aiMesh *mesh, const aiScene *Scene)
{
	vector<VERTEX> vertices;
	vector<UINT> indices;
	vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = Scene->mMaterials[mesh->mMaterialIndex];

		if (Textype.empty()) 
			Textype = determineTextureType(Scene, mat);
	}

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);


		if (mesh->mTextureCoords[0])
			vertex.texcoord = Vector2((float)mesh->mTextureCoords[0][i].x, (float)mesh->mTextureCoords[0][i].y);
		else
			vertex.texcoord = Vector2(0.f, 0.f);

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
		 for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			  indices.push_back(mesh->mFaces[i].mIndices[j]);

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = Scene->mMaterials[mesh->mMaterialIndex];
		
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", Scene);
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
				textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
				*/
	}
	return Mesh(vertices, indices, textures);
}

vector<Engine::Mesh::Texture> Engine::Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, const aiScene *Scene)
{
	vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (int j = 0; j < Textures_loaded.size(); j++)
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

void Engine::Models::processNode(aiNode *node, const aiScene *scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
		meshes.push_back(this->processMesh(scene->mMeshes[node->mMeshes[i]], scene));

	for (int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
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

	if (Device == nullptr)
		GetD3DDevice();

	ThrowIfFailed(CreateWICTextureFromMemory(Device, reinterpret_cast<unsigned char*>(Scene->mTextures[Textureindex]->pcData), *size, nullptr, &texture));

	return texture;
}

//void Engine::Models::SetShaderParameters()
//{
//	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//
//
//	XMMATRIX WVP = PO.World * View * Proj;
//}

HRESULT Engine::Mesh::InitShader()
{
	ID3DBlob *vertexShaderBuffer = nullptr, *pixelShaderBuffer = nullptr;
	V_RETURN(hr = Shader->CompileShaderFromFile(Engine::Mesh::GetResPathW(&wstring(L"VertexShader.hlsl")), &string("main"), &string("vs_4_0"), &vertexShaderBuffer));
	V_RETURN(hr = Shader->CompileShaderFromFile(Engine::Mesh::GetResPathW(&wstring(L"PixelShader.hlsl")), &string("main"), &string("ps_4_0"), &pixelShaderBuffer));

	if (Device == nullptr)
		GetD3DDevice();

	V_RETURN(hr = Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader));
	V_RETURN(hr = Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader));

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	V_RETURN(hr = Device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout));

	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	V_RETURN(hr = Device->CreateSamplerState(&samplerDesc, &m_sampleState));
}

void Engine::Mesh::Rotation()
{

}

auto Engine::Mesh::Scale()
{
return XMMatrixScaling(2.0f, 2.0f, 2.0f);
}

void Engine::Mesh::Position()
{
	cb.World = XMMatrixTranspose(World * XMMatrixTranslation(rand() % 10 + 1, rand() % 8 + 1, rand() % 3 + 1));
}

void Engine::Models::Rotation(vector<Mesh> Obj)
{

}

void Engine::Models::Scale()
{
	cb.View = XMMatrixTranspose(XMMatrixTranslation(2.0f, 0.0001f, 0.1f));
}

void Engine::Models::Position(vector<Mesh> Obj)
{
}

// CreateRotation, ChangePositionModel And Clear ConstBuff Ever Frame Update

void Engine::Mesh::Draw(Matrix World, Matrix View, Matrix Proj)
{
	UINT stride = sizeof(VERTEX), offset = 0;
	this->World = World; // Cache Variable For Matrix World
	this->View = View; // Cache Variable For Matrix View
	this->Proj = Proj; // Cache Variable For Matrix Proj

	cb.World = XMMatrixTranspose(World);
	cb.View = XMMatrixTranspose(View);
	cb.Proj = XMMatrixTranspose(Proj);
	
	//Scale();

	if (DeviceCon == nullptr)
		GetD3DDeviceCon();

	DeviceCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	DeviceCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceCon->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);
	DeviceCon->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	DeviceCon->PSSetShaderResources(0, 1, &textures[0].texture);

	DeviceCon->IASetInputLayout(m_layout);
	DeviceCon->VSSetShader(m_vertexShader, NULL, 0);
	DeviceCon->PSSetShader(m_pixelShader, NULL, 0);
	DeviceCon->PSSetSamplers(0, 1, &m_sampleState);

	DeviceCon->DrawIndexed(indices.size(), 0, 0);
}

void Engine::Mesh::Close()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);
	for (int i = 0; i < textures.size(); i++)
		 textures[i].texture->Release();
}

bool Engine::Mesh::setupMesh()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	ZeroMemory(&Data, sizeof(Data));
	Data.pSysMem = vertices.data();

	if (Device == nullptr)
		GetD3DDevice();

	V_RETURN(hr = Device->CreateBuffer(&bd, &Data, &m_vertexBuffer));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	Data.pSysMem = indices.data();

	V_RETURN(hr = Device->CreateBuffer(&bd, &Data, &m_indexBuffer));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	V_RETURN(hr = Device->CreateBuffer(&bd, NULL, &m_pConstantBuffer));

	InitShader();
}
