#include "pch.h"

#include "Render_Buffer.h"

HRESULT Render_Buffer::InitSimpleBuffer(vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader, UINT VertexSize, 
	void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct)
{
	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(*ShaderFile, *Func, *VersionShader));

	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *) Buffers[1]; // PS

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	if (FAILED(Application->getDevice()->CreateBuffer(&bd, NULL, &m_pConstBuffer)))
	{
		DebugTrace("Render_Buffer::InitSimpleBuffer()->CreateBuffer() is failed");
		throw exception("Init is failed!!!");
		return E_FAIL;
	}

	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	if (FAILED(Application->getDevice()->CreateInputLayout(L_Element, numElements, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(),
		&m_layout)))
	{
		DebugTrace("Render_Buffer::InitSimpleBuffer()->CreateInputLayout() is failed");
		throw exception("Init is failed!!!");
		return E_FAIL;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
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

	if (FAILED(Application->getDevice()->CreateSamplerState(&samplerDesc, &m_sampleState)))
	{
		DebugTrace("Render_Buffer::InitSimpleBuffer()->CreateInputLayout() is failed");
		throw exception("Init is failed!!!");
		return E_FAIL;
	}

	CreateWF();

	m_vertexBuffer = CreateVB(SizeStruct * VertexSize, &Vertix);
	m_indexBuffer = CreateIB(sizeof(DWORD) * IndicesSize, &Indix);

	UINT offset = 0;
	Application->getDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &SizeStruct, &offset);
	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	init = true;
	return hr;
}

HRESULT Render_Buffer::CreateLayout(ID3DBlob *Buffer_blob)
{
	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	if (FAILED(Application->getDevice()->CreateInputLayout(L_Element, numElements, Buffer_blob->GetBufferPointer(), Buffer_blob->GetBufferSize(), &m_layout)))
	{
		DebugTrace("Render_Buffer::CreateLayout()->CreateInputLayout() is failed");
		throw exception("Create failed!!!");
		return E_FAIL;
	}

	return S_OK;
}

ID3D11Buffer *Render_Buffer::CreateVB(UINT ByteWidth, void *vertices)
{
	if (ByteWidth > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
	{
#if defined(DEBUG) || defined(_DEBUG)
		throw exception("VertexBuffer too large for DirectX 11");
#elif defined(NDEBUG)
		DebugTrace("WARNING!!!\nVertexBuffer too large for DirectX 11");
#endif
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = ByteWidth;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexData.pSysMem = vertices;

	ID3D11Buffer *VB = nullptr;

	if (FAILED(Application->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &VB)))
	{
		DebugTrace("Render_Buffer::CreateVB()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return VB;
}

ID3D11Buffer *Render_Buffer::CreateIB(WORD ByteWidth, void *indices)
{
	if (ByteWidth > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
	{
#if defined(DEBUG) || defined(_DEBUG)
		throw exception("VertexBuffer too large for DirectX 11");
#elif defined(NDEBUG)
		DebugTrace("WARNING!!!\nVertexBuffer too large for DirectX 11");
#endif
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = ByteWidth;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexData.pSysMem = indices;

	ID3D11Buffer *IB = nullptr;
	if (FAILED(Application->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &IB)))
	{
		DebugTrace("Render_Buffer::CreateIB()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return IB;
}

void Render_Buffer::RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies, bool WF)
{
	cb.mWorld = XMMatrixTranspose(World);
	cb.mView = XMMatrixTranspose(View);
	cb.mProj = XMMatrixTranspose(Proj);

	if (m_layout)
		Application->getDeviceContext()->IASetInputLayout(m_layout);

	if (WF && g_pRasWireFrame)
		Application->getDeviceContext()->RSSetState(g_pRasWireFrame);
	else if (!WF && g_pRasStateSolid)
		Application->getDeviceContext()->RSSetState(g_pRasStateSolid);

	if (m_pConstBuffer)
	{
		Application->getDeviceContext()->UpdateSubresource(m_pConstBuffer, 0, nullptr, &cb, 0, 0);
		Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	}

	if (m_vertexShader)
		Application->getDeviceContext()->VSSetShader(m_vertexShader, 0, 0);

	if (m_pixelShader)
		Application->getDeviceContext()->PSSetShader(m_pixelShader, 0, 0);

	if (m_sampleState)
		Application->getDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	Application->getDeviceContext()->Draw(Indicies, 0);
}

#if defined(UseTerrain)
HRESULT Render_Buffer::InitTerrain(shared_ptr<Engine> engine, UINT SizeofVertex, void *vertices, vector<UINT> indices,
	vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader)
{
	InitSimpleBuffer(ShaderFile, Func, VersionShader);

	if (m_vertexBuffer)
		SAFE_RELEASE(m_vertexBuffer);

	if (m_indexBuffer)
		SAFE_RELEASE(m_indexBuffer);

	if (m_layout)
		SAFE_RELEASE(m_layout);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = SizeofVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	ZeroMemory(&Data, sizeof(Data));
	Data.pSysMem = vertices;

	GetD3DDevice();
	V(Device->CreateBuffer(&bd, &Data, &m_vertexBuffer));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	Data.pSysMem = indices.data();

	V(Device->CreateBuffer(&bd, &Data, &m_indexBuffer));

	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	V(Device->CreateInputLayout(L_Element, numElements, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(), &m_layout));

	CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	init = true;
	return hr;
}
#endif // UseTerrain

HRESULT Render_Buffer::InitModels(UINT VertexSize, void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct,
	vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader)
{
	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(*ShaderFile, *Func, *VersionShader));

	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

	m_vertexBuffer = CreateVB(SizeStruct * VertexSize, &Vertix);

	m_indexBuffer = CreateIB(sizeof(WORD) * IndicesSize, &Indix);

	CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	CreateLayout(Buffer_blob[0]);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
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

	if (FAILED(Application->getDevice()->CreateSamplerState(&samplerDesc, &m_sampleState)))
	{
		DebugTrace("Render_Buffer::InitModels()->CreateSamplerState() is failed");
		throw exception("Init is failed!!!");
		return E_FAIL;
	}

	CreateWF();
	Application->getDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT offset = 0;
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &SizeStruct, &offset);

	init = true;
	return hr;
}

HRESULT Render_Buffer::CreateWF()
{
	D3D11_RASTERIZER_DESC descRast;
	ZeroMemory(&descRast, sizeof(descRast));
	descRast.CullMode = D3D11_CULL_NONE;
	descRast.FillMode = D3D11_FILL_WIREFRAME;
	if (FAILED(Application->getDevice()->CreateRasterizerState(&descRast, &g_pRasWireFrame)))
	{
		DebugTrace("Render_Buffer::CreateWF()->CreateRasterizerState() is failed");
		throw exception("Create failed!!!");
		return E_FAIL;
	}

	D3D11_RASTERIZER_DESC RasterDesc;
	ZeroMemory(&RasterDesc, sizeof(RasterDesc));
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.DepthClipEnable = true;
	if (FAILED(Application->getDevice()->CreateRasterizerState(&RasterDesc, &g_pRasStateSolid)))
	{
		DebugTrace("Render_Buffer::CreateWF()->CreateRasterizerState() is failed");
		throw exception("Create failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Render_Buffer::CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags)
{
	if (m_pConstBuffer)
		SAFE_RELEASE(m_pConstBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = Usage;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = CPUAccessFlags;

	if (FAILED(Application->getDevice()->CreateBuffer(&bd, NULL, &m_pConstBuffer)))
	{
		DebugTrace("Render_Buffer::CreateConstBuff()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return E_FAIL;
	}

	return S_OK;
}

#if defined(UseTerrain)
void Render_Buffer::RenderTerrain(Matrix World, Matrix View, Matrix Proj, int Indices, vector<ID3D11Buffer *> RenderBuff, UINT stride)
{
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	engine->getDeviceContext()->IASetInputLayout(m_layout);

	//if (WF && g_pRasWireFrame)
	//	DeviceCon->RSSetState(g_pRasWireFrame);
	//else if (!WF && g_pRasStateSolid)
	//	DeviceCon->RSSetState(g_pRasStateSolid);

	if (RenderBuff[0])
		engine->getDeviceContext()->IASetVertexBuffers(0, 1, &RenderBuff[0], &stride, &offset);
	if (RenderBuff[1])
		engine->getDeviceContext()->IASetIndexBuffer(RenderBuff[1], DXGI_FORMAT_R32_UINT, 0);

	engine->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine->getDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	engine->getDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	engine->getDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	V(DeviceCon->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = (ConstantBuffer *)mappedResource.pData;

	dataPtr->mWorld = XMMatrixTranspose(World);
	dataPtr->mView = XMMatrixTranspose(View);
	dataPtr->mProj = XMMatrixTranspose(Proj);

	engine->getDeviceContext()->Unmap(m_pConstBuffer, 0);

	engine->getDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	engine->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);

	engine->getDeviceContext()->DrawIndexed(Indices, 0, 0);
}
#endif // UseTerrain

void Render_Buffer::RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, UINT SizeStruct, ID3D11ShaderResourceView *RenderTextr, bool WF)
{
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (m_layout)
		Application->getDeviceContext()->IASetInputLayout(m_layout);

	if (WF && g_pRasWireFrame)
		Application->getDeviceContext()->RSSetState(g_pRasWireFrame);
	else if (!WF && g_pRasStateSolid)
		Application->getDeviceContext()->RSSetState(g_pRasStateSolid);

	if (m_pConstBuffer)
		ThrowIfFailed(Application->getDeviceContext()->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = (ConstantBuffer *)mappedResource.pData;

	dataPtr->mWorld = XMMatrixTranspose(World);
	dataPtr->mView = XMMatrixTranspose(View);
	dataPtr->mProj = XMMatrixTranspose(Proj);

	if (m_pConstBuffer)
	{
		Application->getDeviceContext()->Unmap(m_pConstBuffer, 0);
		Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	}

	if (m_sampleState)
		Application->getDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	if (m_vertexShader)
		Application->getDeviceContext()->VSSetShader(m_vertexShader, 0, 0);
	if (m_pixelShader)
		Application->getDeviceContext()->PSSetShader(m_pixelShader, 0, 0);

	if (RenderTextr)
		Application->getDeviceContext()->PSSetShaderResources(0, 1, &RenderTextr);
	Application->getDeviceContext()->DrawIndexed(SizeIndices, 0, 0);
}

void Render_Buffer::Release()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_pConstBuffer);
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);

	SAFE_DELETE(Shader);
}
