#include "pch.h"

#include "Render_Buffer.h"

HRESULT Engine::Render_Buffer::InitSimpleBuffer(vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader, int ConstBuff_Width)
{
	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(*ShaderFile, *Func, *VersionShader));

	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *) Buffers[1]; // PS

	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	GetD3DDevice();

	V(Device->CreateBuffer(&bd, NULL, &m_pConstBuffer));

	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	V(Device->CreateInputLayout(L_Element, numElements, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(), &m_layout));

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

	V(Device->CreateSamplerState(&samplerDesc, &m_sampleState));

	CreateWF();

	init = true;
	return hr;
}

HRESULT Engine::Render_Buffer::CreateLayout(ID3DBlob *Buffer_blob)
{
	{
		const D3D11_INPUT_ELEMENT_DESC L_Element[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

		V(Device->CreateInputLayout(L_Element, numElements, Buffer_blob->GetBufferPointer(), Buffer_blob->GetBufferSize(), &m_layout));

		return S_OK;
	}
}

HRESULT Engine::Render_Buffer::CreateZBuff()
{
	GetD3DDevice();
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = DXUTGetDXGIBackBufferSurfaceDesc()->Width;
	descDepth.Height = DXUTGetDXGIBackBufferSurfaceDesc()->Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	V(Device->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil));
	if (FAILED(hr))
		return hr;

	// Создание z-буфреа
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = Device->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;
	GetD3DDeviceCon();

	auto RTV = DXUTGetD3D11RenderTargetView();
	DeviceCon->OMSetRenderTargets(1, &RTV, g_pDepthStencilView);

	return S_OK;
}

ID3D11Buffer *Engine::Render_Buffer::CreateVB(UINT ByteWidth, void *vertices)
{
	if (ByteWidth > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
	{
#ifdef DEBUG
		throw exception("VertexBuffer too large for DirectX 11");
#elif NDEBUG
		DebugTrace("WARNING!!!\nVertexBuffer too large for DirectX 11");
#endif
	}

	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		D3D11_SUBRESOURCE_DATA vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = ByteWidth;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		GetD3DDevice();

		ID3D11Buffer *VB = nullptr;

		V(Device->CreateBuffer(&vertexBufferDesc, &vertexData, &VB));

		return VB;
	}
}

ID3D11Buffer *Engine::Render_Buffer::CreateIB(WORD ByteWidth, void *indices)
{
	if (ByteWidth > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
	{
#ifdef DEBUG
		throw exception("VertexBuffer too large for DirectX 11");
#elif NDEBUG
		DebugTrace("WARNING!!!\nVertexBuffer too large for DirectX 11");
#endif
	}

	{
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory(&indexData, sizeof(indexData));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = ByteWidth;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexData.pSysMem = indices;

		GetD3DDevice();
		ID3D11Buffer *IB = nullptr;

		V(Device->CreateBuffer(&indexBufferDesc, &indexData, &IB));
		return IB;
	}
}

void Engine::Render_Buffer::RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies, bool WF)
{
	cb.mWorld = XMMatrixTranspose(World);
	cb.mView = XMMatrixTranspose(View);
	cb.mProj = XMMatrixTranspose(Proj);

	GetD3DDeviceCon();
	DeviceCon->IASetInputLayout(m_layout);

	if (WF && g_pRasWireFrame)
		DeviceCon->RSSetState(g_pRasWireFrame);
	else if (!WF && g_pRasStateSolid)
		DeviceCon->RSSetState(g_pRasStateSolid);

	DeviceCon->UpdateSubresource(m_pConstBuffer, 0, nullptr, &cb, 0, 0);

	DeviceCon->VSSetShader(m_vertexShader, 0, 0);
	DeviceCon->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	DeviceCon->PSSetShader(m_pixelShader, 0, 0);
	DeviceCon->PSSetSamplers(0, 1, &m_sampleState);
}

HRESULT Engine::Render_Buffer::InitTerrain(UINT SizeofVertex, void *vertices, vector<UINT> indices,
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

HRESULT Engine::Render_Buffer::InitModels(UINT VertexSize, void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct,
	vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader)
{
	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(*ShaderFile, *Func, *VersionShader));

	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

	V((m_vertexBuffer = CreateVB(SizeStruct * VertexSize, &Vertix)) == nullptr ? E_FAIL : S_OK);

	V((m_indexBuffer = CreateIB(sizeof(WORD) * IndicesSize, &Indix)) == nullptr ? E_FAIL : S_OK);

	V(CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE));

	V(CreateLayout(Buffer_blob[0]));

	{
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

		V(Device->CreateSamplerState(&samplerDesc, &m_sampleState));
	}

	CreateWF();
	CreateZBuff();

	GetD3DDeviceCon();
	DeviceCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	DeviceCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT offset = 0;
	DeviceCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &SizeStruct, &offset);

	init = true;
	return hr;
}

HRESULT Engine::Render_Buffer::CreateWF()
{
	{
		D3D11_RASTERIZER_DESC descRast;
		ZeroMemory(&descRast, sizeof(descRast));
		descRast.CullMode = D3D11_CULL_NONE;
		descRast.FillMode = D3D11_FILL_WIREFRAME;

		GetD3DDevice();
		V(Device->CreateRasterizerState(&descRast, &g_pRasWireFrame));
		DXUT_SetDebugName(g_pRasWireFrame, "WireFrame");

		D3D11_RASTERIZER_DESC RasterDesc;
		ZeroMemory(&RasterDesc, sizeof(RasterDesc));
		RasterDesc.FillMode = D3D11_FILL_SOLID;
		RasterDesc.CullMode = D3D11_CULL_NONE;
		RasterDesc.DepthClipEnable = TRUE;
		V(Device->CreateRasterizerState(&RasterDesc, &g_pRasStateSolid));
		DXUT_SetDebugName(g_pRasStateSolid, "Solid_WireFrame");
	}
	return S_OK;
}

HRESULT Engine::Render_Buffer::CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags)
{
	if (m_pConstBuffer)
		SAFE_RELEASE(m_pConstBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = Usage;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = CPUAccessFlags;

	GetD3DDevice();
	V(Device->CreateBuffer(&bd, NULL, &m_pConstBuffer));

	return S_OK;
}

HRESULT Engine::Render_Buffer::CreateTexture(const wchar_t *TextrFName)
{
	GetD3DDevice();
	V(hr = CreateWICTextureFromFile(Device, TextrFName, nullptr, &m_texture));
	if (FAILED(hr))
	{
		throw exception("Render_Buffer::CreateTexture == E_FAIL!!!");
		return hr;
	}

	return S_OK;
}

void Engine::Render_Buffer::RenderTerrain(Matrix World, Matrix View, Matrix Proj, int Indices, vector<ID3D11Buffer *> RenderBuff, UINT stride)
{
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GetD3DDeviceCon();
	DeviceCon->IASetInputLayout(m_layout);

	//if (WF && g_pRasWireFrame)
	//	DeviceCon->RSSetState(g_pRasWireFrame);
	//else if (!WF && g_pRasStateSolid)
	//	DeviceCon->RSSetState(g_pRasStateSolid);

	if (RenderBuff[0])
		DeviceCon->IASetVertexBuffers(0, 1, &RenderBuff[0], &stride, &offset);
	if (RenderBuff[1])
		DeviceCon->IASetIndexBuffer(RenderBuff[1], DXGI_FORMAT_R32_UINT, 0);

	DeviceCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceCon->VSSetShader(m_vertexShader, NULL, 0);
	DeviceCon->PSSetShader(m_pixelShader, NULL, 0);

	DeviceCon->PSSetSamplers(0, 1, &m_sampleState);

	V(DeviceCon->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = (ConstantBuffer *)mappedResource.pData;

	dataPtr->mWorld = XMMatrixTranspose(World);
	dataPtr->mView = XMMatrixTranspose(View);
	dataPtr->mProj = XMMatrixTranspose(Proj);

	DeviceCon->Unmap(m_pConstBuffer, 0);

	DeviceCon->PSSetShaderResources(0, 1, &m_texture);

	DeviceCon->VSSetConstantBuffers(0, 1, &m_pConstBuffer);

	DeviceCon->DrawIndexed(Indices, 0, 0);
}

void Engine::Render_Buffer::RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, UINT SizeStruct, ID3D11ShaderResourceView *RenderTextr, bool WF)
{
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GetD3DDeviceCon();
	DeviceCon->IASetInputLayout(m_layout);

	if (WF && g_pRasWireFrame)
		DeviceCon->RSSetState(g_pRasWireFrame);
	else if (!WF && g_pRasStateSolid)
		DeviceCon->RSSetState(g_pRasStateSolid);

	ThrowIfFailed(DeviceCon->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = (ConstantBuffer *)mappedResource.pData;

	dataPtr->mWorld = XMMatrixTranspose(World);
	dataPtr->mView = XMMatrixTranspose(View);
	dataPtr->mProj = XMMatrixTranspose(Proj);

	DeviceCon->Unmap(m_pConstBuffer, 0);

	DeviceCon->VSSetConstantBuffers(0, 1, &m_pConstBuffer);

	DeviceCon->VSSetShader(m_vertexShader, 0, 0);

	DeviceCon->PSSetShader(m_pixelShader, 0, 0);
	DeviceCon->PSSetSamplers(0, 1, &m_sampleState);

	DeviceCon->PSSetShaderResources(0, 1, &RenderTextr);

	DeviceCon->DrawIndexed(SizeIndices, 0, 0);
}

void Engine::Render_Buffer::Release()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_pConstBuffer);
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);

	SAFE_RELEASE(Device);
	if (DeviceCon)
	{
		DeviceCon->ClearState();
		DeviceCon->Flush();
		SAFE_RELEASE(DeviceCon);
	}
	SAFE_DELETE(Shader);
}
