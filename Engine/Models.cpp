#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "Models.h"
#include "Console.h"
#include "Shaders.h"
#include "File_system.h"

bool Models::LoadFromFile(string Filename)
{	
	importer = new Assimp::Importer;

	pScene = importer->ReadFile(Filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_JoinIdenticalVertices);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		Engine::LogError((boost::format("Model: Scene return nullptr with text: %s") % importer->GetErrorString()).str(),
			"Models::pScene == nullptr!!!",
			(boost::format("Model: Scene return nullptr with text:") % importer->GetErrorString()).str());
		return false;
	}

	processNode(pScene->mRootNode, pScene);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NOT_EQUAL;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Application->getDevice()->CreateSamplerState(&sampDesc, &TexSamplerState);

	vector<ID3DBlob *> Buffer_blob;
	vector<wstring> FileShaders =
	{
		Application->getFS()->GetFile("VertexShader.hlsl")->PathW,
		Application->getFS()->GetFile("PixelShader.hlsl")->PathW
	};
	vector<string> Functions =
	{
		"Vertex_model_VS",
		"Pixel_model_PS"
	},
		Version =
	{
		"vs_4_0",
		"ps_4_0"
	};
	vector<void *> Buffers = Shaders::CompileShaderFromFile(Buffer_blob =
		Shaders::CreateShaderFromFile(FileShaders, Functions, Version));
	VS = (ID3D11VertexShader *)Buffers[0]; // VS
	PS = (ID3D11PixelShader *)Buffers[1]; // PS

	Application->getDevice()->CreateVertexShader(Buffer_blob.at(0)->GetBufferPointer(), Buffer_blob.at(0)->GetBufferSize(),
		NULL, &VS);
	Application->getDevice()->CreatePixelShader(Buffer_blob.at(1)->GetBufferPointer(), Buffer_blob.at(1)->GetBufferSize(),
		NULL, &PS);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Application->getDevice()->CreateInputLayout(ied, 2, Buffer_blob.at(0)->GetBufferPointer(),
		Buffer_blob.at(0)->GetBufferSize(), &pLayout);

	pConstantBuffer = Render_Buffer::CreateConstBuff(D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, sizeof(cb));
	
	//	TM->EndTime();
	//Console::LogInfo((string("\nCreate Buffers And Shaders For Model Take:" + to_string(TM->GetResultTime().count())
	//	+ string(" Seconds")).c_str()));

	return true;
}

bool Models::LoadFromAllModels()
{
	auto Files = Application->getFS()->GetFileByType(_TypeOfFile::MODELS);
	for (size_t i = 0; i < Files.size(); i++)
	{
		importer = new Assimp::Importer;

		pScene = importer->ReadFile(Files.at(i)->PathA.c_str(), 
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded 
		| aiProcess_OptimizeMeshes | aiProcess_SortByPType | aiProcess_FindInvalidData
		| aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_OptimizeGraph);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
			Engine::LogError((boost::format("Model: Scene return nullptr with text: %s") % importer->GetErrorString()).str(),
				"Models::pScene == nullptr!!!",
				(boost::format("Model: Scene return nullptr with text:") % importer->GetErrorString()).str());
			return false;
		}

		processNode(pScene->mRootNode, pScene);

		SAFE_DELETE(importer);
	}

	return true;
}

void Models::Render(Matrix View, Matrix Proj)
{
	ConstantBuffer cb;
	auto Mrx = XMMatrixTranspose(scale) * XMMatrixTranspose(position) * XMMatrixTranspose(rotate);
	cb.World = Mrx;
	cb.View = XMMatrixTranspose(View);
	cb.Proj = XMMatrixTranspose(Proj);

	Application->getDeviceContext()->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	Application->getDeviceContext()->PSSetSamplers(0, 1, &TexSamplerState);
	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Application->getDeviceContext()->IASetInputLayout(pLayout);
	Application->getDeviceContext()->VSSetShader(VS, 0, 0);
	Application->getDeviceContext()->PSSetShader(PS, 0, 0);

	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes.at(i)->Draw();
	}
}

Models::Models(string Filename)
{
	if (Filename.empty())
		Engine::LogError((boost::format("Model File: %s not found And Can't Be Load!") % Filename).str(),
			"Models: File Not Found!\n",
			(boost::format("Model File: %s not found And Can't Be Load!") % Filename).str());
	if (!LoadFromFile(Filename))
		Engine::LogError((boost::format("Model File: %s Can't Be Load!") % Filename).str(),
			"Models::LoadFromFile == false!\n",
			(boost::format("Model File: %s Can't Be Load!") % Filename).str());
}

vector<Texture> Models::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName,
	const aiScene *Scene)
{
	vector<Texture> textures;
	string PathTexture = "";

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
				texture.TextureSHRes = getTextureFromModel(Scene, getTextureIndex(&str));
			else
			{
				auto textr = Application->getFS()->GetFile(string(str.C_Str()));
				if (textr.operator bool())
				{
					PathTexture = textr->PathA;

					if (FindSubStr(textr->ExtA, ".dds"))
					{
						if (FAILED(CreateDDSTextureFromFile(Application->getDevice(), textr->PathW.c_str(),
							&texture.TextureRes, &texture.TextureSHRes)))
						{
#if defined (DEBUG)
//							DebugTrace("Models::CreateDDSTextureFromFile() create failed");
#endif
#if defined (ExceptionWhenEachError)
							throw exception("Create failed!!!");
#endif
							Console::LogInfo(string("Something is wrong with this texture: ") + textr->FileA);
						}
					}
					else
					{
						if (FAILED(CreateWICTextureFromFile(Application->getDevice(), textr->PathW.c_str(),
							&texture.TextureRes, &texture.TextureSHRes)))
						{
#if defined (DEBUG)
//							DebugTrace("Models::CreateWICTextureFromFile() create failed");
#endif
#if defined (ExceptionWhenEachError)
							throw exception("Create failed!!!");
#endif
							Console::LogInfo(string("Something is wrong with Create the texture: ") + textr->FileA);
						}
					}
				}
			}
			texture.type = typeName;
			texture.path = PathTexture.c_str();

			textures.push_back(texture);

			Textures_loaded.push_back(texture);
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
		Vector3 Max;
		
		mesh = Scene->mMeshes[node->mMeshes[i]];
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *mat = Scene->mMaterials[mesh->mMaterialIndex];
			
			if (Textype.empty())
				Textype = determineTextureType(Scene, string(mat->GetName().C_Str()), mat);
		}

		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			Things vertex;

			vertex.Pos = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			//Max = Vector3(max(float(mesh->mVertices[i].x), Max.x), max(float(mesh->mVertices[i].y), Max.y),
			//	max(float(mesh->mVertices[i].z), Max.z));
			if (mesh->mTextureCoords[0])
				vertex.Tex = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

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

			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
				"texture_diffuse", Scene);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			///It doesn't work!
			/*
			vector<Texture> Opacity = loadMaterialTextures(material, aiTextureType_OPACITY,
			"texture_opacity", Scene);
			textures.insert(textures.end(), Opacity.begin(), Opacity.end());

			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
			"texture_specular", Scene);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
			"texture_normal", Scene);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,
			"texture_height", Scene);
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
			*/
		}

		meshes.push_back(make_shared<Mesh>(vertices, indices, textures));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], Scene);
}

aiTextureType Models::getTextureType(string TypeName)
{
	if (contains(TypeName, "diffuse"))
		return aiTextureType_DIFFUSE;
	else if (contains(TypeName, "opacity"))
		return aiTextureType_OPACITY;
	return aiTextureType_UNKNOWN;
}

string Models::determineTextureType(const aiScene *Scene, string TypeName, aiMaterial *mat)
{
	aiString textypeStr;
	mat->GetTexture(Models::getTextureType(TypeName), 0, &textypeStr);
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
#if defined (DEBUG)
//		DebugTrace("Models::CreateWICTextureFromFile() create failed");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("Create failed!!!");
#endif
		Console::LogInfo(string("Something is wrong with this texture: ") +
			Scene->mTextures[Textureindex]->mFilename.C_Str());

		return nullptr;
	}

	return texture;
}
inline XMMATRIX XMMatrixRotationAxis1
(
	Vector3 Axis,
	float     Angle
)
{
	// assert(!XMVector3Equal(Axis, XMVectorZero()));
	assert(!XMVector3IsInfinite(Axis));

	//Vector3 Normal = XMVector3Normalize(Axis);
	return XMMatrixRotationNormal(Axis, Angle);
}

void Models::setRotation(Vector3 rotaxis)
{
	rotate = XMMatrixRotationAxis1(rotaxis, 0.1f);
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

void Models::Mesh::Draw()
{
	UINT stride = sizeof(Things);
	UINT offset = 0;

	//2
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	//4
	Application->getDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	if (!textures.empty() && textures[0].TextureSHRes)
		//8
		Application->getDeviceContext()->PSSetShaderResources(0, 1, &textures[0].TextureSHRes);

	if (Application->IsWireFrame())
		Application->getDeviceContext()->RSSetState(Application->GetWireFrame());
	else
		Application->getDeviceContext()->RSSetState(Application->GetNormalFrame());

	Application->getDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}
