#include "pch.h"

#include "Render_Buffer.h"

void Render_Buffer::SetShadersFile(vector<wstring> ShaderFile, vector<string> Func, vector<string> VersionShader)
{
	this->ShaderFile = ShaderFile;
	this->Func = Func;
	this->VersionShader = VersionShader;
}

HRESULT Render_Buffer::InitSimpleBuffer(UINT VertexSize, void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct)
{
	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(ShaderFile, Func, VersionShader));
	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

	m_layout = CreateLayout(Buffer_blob[0]);

	Application->getDeviceContext()->IASetInputLayout(m_layout);

	m_vertexBuffer = CreateVB(SizeStruct * VertexSize, Vertix);

	UINT offset = 0;
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &SizeStruct, &offset);

	m_indexBuffer = CreateIB(sizeof(WORD) * IndicesSize, Indix);

	auto WF = CreateWF();
	g_pRasWireFrame = WF.at(0);
	g_pRasStateSolid = WF.at(1);

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

	Application->getDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pConstBuffer = CreateConstBuff(D3D11_USAGE_DEFAULT, 0);

	init = true;
	return hr;
}

ID3D11InputLayout *Render_Buffer::CreateLayout(ID3DBlob *Buffer_blob)
{
	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	ID3D11InputLayout *Layout = nullptr;
	if (FAILED(Application->getDevice()->CreateInputLayout(L_Element, numElements,
		Buffer_blob->GetBufferPointer(), Buffer_blob->GetBufferSize(), &Layout)))
	{
		DebugTrace("Render_Buffer::CreateLayout()->CreateInputLayout() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return Layout;
}

ID3D11Buffer *Render_Buffer::CreateVB(UINT ByteWidth, bool NeedVertice, D3D11_USAGE Usage, UINT CPUAccessFlags, void *vertices)
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
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = Usage;
	vertexBufferDesc.ByteWidth = ByteWidth;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (NeedVertice)
	{
		ZeroMemory(&vertexData, sizeof(vertexData));
		vertexData.pSysMem = vertices;
	}

	ID3D11Buffer *VB = nullptr;

	if (FAILED(Application->getDevice()->CreateBuffer(&vertexBufferDesc, NeedVertice ? &vertexData : nullptr, &VB)))
	{
		DebugTrace("Render_Buffer::CreateVB()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return VB;
}

ID3D11Buffer *Render_Buffer::CreateIB(WORD ByteWidth, bool NeedIndices, D3D11_USAGE Usage, UINT CPUAccessFlags, void *indices)
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

	indexBufferDesc.Usage = Usage;
	indexBufferDesc.ByteWidth = ByteWidth;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = CPUAccessFlags;

	if (NeedIndices)
	{
		ZeroMemory(&indexData, sizeof(indexData));
		indexData.pSysMem = indices;
	}

	ID3D11Buffer *IB = nullptr;
	if (FAILED(Application->getDevice()->CreateBuffer(&indexBufferDesc, NeedIndices ? &indexData : nullptr, &IB)))
	{
		DebugTrace("Render_Buffer::CreateIB()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return IB;
}

void Render_Buffer::RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies, ID3D11ShaderResourceView *RenderTextr, bool WF)
{
	ConstantBuffer cb;
	XMMATRIX WVP = World * View * Proj;
	cb.mMVP = XMMatrixTranspose(WVP);
	Application->getDeviceContext()->UpdateSubresource(m_pConstBuffer, 0, NULL, &cb, 0, 0);

	Application->getDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	Application->getDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	if (m_sampleState)
		Application->getDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);
	
	if (WF && g_pRasWireFrame)
		Application->getDeviceContext()->RSSetState(g_pRasWireFrame);
	else if (!WF && g_pRasStateSolid)
		Application->getDeviceContext()->RSSetState(g_pRasStateSolid);

	if (RenderTextr)
		Application->getDeviceContext()->PSSetShaderResources(0, 1, &RenderTextr);

	Application->getDeviceContext()->DrawIndexed(Indicies, 0, 0);
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

HRESULT Render_Buffer::InitModels(UINT VertexSize, void *Vertix, UINT IndicesSize, void *Indix, UINT SizeStruct)
{
	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(ShaderFile,Func, VersionShader));

	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

	m_layout = CreateLayout(Buffer_blob[0]);

	auto WF = CreateWF();
	g_pRasWireFrame = WF.at(0);
	g_pRasStateSolid = WF.at(1);

	m_vertexBuffer = CreateVB(SizeStruct * VertexSize, &Vertix);
	m_indexBuffer = CreateIB(sizeof(UINT) * IndicesSize, &Indix);

	UINT offset = 0;
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &SizeStruct, &offset);

	Application->getDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pConstBuffer = CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	init = true;
	return hr;
}

vector<ID3D11RasterizerState *> Render_Buffer::CreateWF()
{
	D3D11_RASTERIZER_DESC RasterDesc;
	ZeroMemory(&RasterDesc, sizeof(RasterDesc));
	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterDesc.DepthClipEnable = true;
	RasterDesc.ScissorEnable = true;
	RasterDesc.AntialiasedLineEnable = true;
	RasterDesc.MultisampleEnable = true;

	vector<ID3D11RasterizerState *> WF_buff;
	ID3D11RasterizerState *RsWF = nullptr, *RsNoWF = nullptr;
	if (FAILED(Application->getDevice()->CreateRasterizerState(&RasterDesc, &RsWF)))
	{
		DebugTrace("Render_Buffer::CreateWF()->CreateRasterizerState() is failed");
		throw exception("Create failed!!!");
	}
	WF_buff.push_back(RsWF);

	ZeroMemory(&RasterDesc, sizeof(RasterDesc));
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.DepthClipEnable = true;
	RasterDesc.ScissorEnable = true;
	RasterDesc.AntialiasedLineEnable = true;
	RasterDesc.MultisampleEnable = true;

	if (FAILED(Application->getDevice()->CreateRasterizerState(&RasterDesc, &RsNoWF)))
	{
		DebugTrace("Render_Buffer::CreateWF()->CreateRasterizerState() is failed");
		throw exception("Create failed!!!");
	}
	WF_buff.push_back(RsNoWF);

	return WF_buff;
}

ID3D11Buffer *Render_Buffer::CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags)
{
	if (m_pConstBuffer)
		SAFE_RELEASE(m_pConstBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = Usage;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = CPUAccessFlags;

	ID3D11Buffer *ConstBuff = nullptr;
	if (FAILED(Application->getDevice()->CreateBuffer(&bd, NULL, &ConstBuff)))
	{
		DebugTrace("Render_Buffer::CreateConstBuff()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	return ConstBuff;
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

void Render_Buffer::RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, ID3D11ShaderResourceView *RenderTextr, bool WF)
{
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

	XMMATRIX WVP = World * View * Proj;
	dataPtr->mMVP = XMMatrixTranspose(WVP);

	if (m_pConstBuffer)
	{
		Application->getDeviceContext()->Unmap(m_pConstBuffer, 0);
		Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	}

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
	if (g_pFontTextureView)
	{
		SAFE_RELEASE(g_pFontTextureView);
		ImGui::GetIO().Fonts->TexID = NULL;
	}
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_pConstBuffer);
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(g_pRasWireFrame);
	SAFE_RELEASE(g_pRasStateSolid);
	SAFE_RELEASE(g_pDepthStencil);
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencilState);
	SAFE_RELEASE(g_pFontSampler);
	SAFE_RELEASE(g_pBlendState);
	while (!Buffer_blob.empty())
	{
		SAFE_RELEASE(Buffer_blob.at(0));
		Buffer_blob.erase(Buffer_blob.begin());
	}
	Buffers.clear();
}

HRESULT Render_Buffer::InitUI()
{
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "EngineUI";

	if (g_pFontSampler)
	{
		if (g_pFontTextureView)
		{
			SAFE_RELEASE(g_pFontTextureView);
			ImGui::GetIO().Fonts->TexID = NULL;
		}
		SAFE_RELEASE(m_vertexBuffer);
		SAFE_RELEASE(m_indexBuffer);
		SAFE_RELEASE(m_pConstBuffer);
		SAFE_RELEASE(m_vertexShader);
		SAFE_RELEASE(m_pixelShader);
		SAFE_RELEASE(m_layout);
		SAFE_RELEASE(m_sampleState);
		SAFE_RELEASE(m_texture);
		SAFE_RELEASE(g_pRasWireFrame);
		SAFE_RELEASE(g_pRasStateSolid);
		SAFE_RELEASE(g_pDepthStencil);
		SAFE_RELEASE(g_pDepthStencilView);
		SAFE_RELEASE(g_pDepthStencilState);
		SAFE_RELEASE(g_pFontSampler);
		SAFE_RELEASE(g_pBlendState);
		while (!Buffer_blob.empty())
		{
			SAFE_RELEASE(Buffer_blob.at(0));
			Buffer_blob.erase(Buffer_blob.begin());
		}
		Buffers.clear();
	}

	D3D11_INPUT_ELEMENT_DESC local_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Buffers = Shader->CompileShaderFromFile(Buffer_blob = Shader->CreateShaderFromFile(ShaderFile, Func, VersionShader));
	m_vertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	m_pixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

	if (Application->getDevice()->CreateInputLayout(local_layout, 3, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(), &m_layout) != S_OK)
		return false;

	m_pConstBuffer = CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	Application->getDevice()->CreateBlendState(&BlendDesc, &g_pBlendState);

	auto WF = CreateWF();
	g_pRasWireFrame = WF.at(0);
	g_pRasStateSolid = WF.at(1);

	D3D11_DEPTH_STENCIL_DESC DepthDesc;
	ZeroMemory(&DepthDesc, sizeof(DepthDesc));
	DepthDesc.DepthEnable = false;
	DepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	DepthDesc.StencilEnable = false;
	DepthDesc.FrontFace.StencilFailOp = DepthDesc.FrontFace.StencilDepthFailOp = DepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DepthDesc.BackFace = DepthDesc.FrontFace;
	Application->getDevice()->CreateDepthStencilState(&DepthDesc, &g_pDepthStencilState);

	UCHAR *pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	D3D11_TEXTURE2D_DESC TexrtDesc;
	ZeroMemory(&TexrtDesc, sizeof(TexrtDesc));
	TexrtDesc.Width = width;
	TexrtDesc.Height = height;
	TexrtDesc.MipLevels = 1;
	TexrtDesc.ArraySize = 1;
	TexrtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TexrtDesc.SampleDesc.Count = 1;
	TexrtDesc.Usage = D3D11_USAGE_DEFAULT;
	TexrtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TexrtDesc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = nullptr;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = pixels;
	subResource.SysMemPitch = TexrtDesc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	Application->getDevice()->CreateTexture2D(&TexrtDesc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = TexrtDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Application->getDevice()->CreateShaderResourceView(pTexture, &srvDesc, &g_pFontTextureView);
	pTexture->Release();

	// Store our identifier
	io.Fonts->TexID = (ImTextureID)g_pFontTextureView;

	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0.f;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0.f;
	desc.MaxLOD = 0.f;
	Application->getDevice()->CreateSamplerState(&desc, &g_pFontSampler);

	return true;
}

void Render_Buffer::RenderUI(ImDrawData *draw_data, bool WF)
{
	if (!m_vertexBuffer || VBufferSize < draw_data->TotalVtxCount)
	{
		SAFE_RELEASE(m_vertexBuffer);
		VBufferSize = draw_data->TotalVtxCount + 5000;

		assert(m_vertexBuffer = CreateVB(VBufferSize * sizeof(ImDrawVert), false, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE));
	}
	if (!m_indexBuffer || IBufferSize < draw_data->TotalIdxCount)
	{
		SAFE_RELEASE(m_indexBuffer);
		assert(m_indexBuffer = CreateIB(IBufferSize * sizeof(ImDrawIdx), false, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE));
	}

	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	if (Application->getDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
		return;
	if (Application->getDeviceContext()->Map(m_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
		return;
	ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	Application->getDeviceContext()->Unmap(m_vertexBuffer, 0);
	Application->getDeviceContext()->Unmap(m_indexBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	if (Application->getDeviceContext()->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
		return;
	ConstantBuffer *constant_buffer = (ConstantBuffer *)mapped_resource.pData;
	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	float mvp[4][4] =
	{
		{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
		{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
		{ 0.0f,         0.0f,           0.5f,       0.0f },
		{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
	};
	Matrix mMVP(*mvp);

	constant_buffer->mMVP = XMMatrixTranspose(mMVP);

	Application->getDeviceContext()->Unmap(m_pConstBuffer, 0);

	D3D11_VIEWPORT vp;
	memset(&vp, 0, sizeof(D3D11_VIEWPORT));
	vp.Width = draw_data->DisplaySize.x;
	vp.Height = draw_data->DisplaySize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0;
	Application->getDeviceContext()->RSSetViewports(1, &vp);

	UINT offset = 0, stride = sizeof(ImDrawVert);
	Application->getDeviceContext()->IASetInputLayout(m_layout);
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	Application->getDeviceContext()->IASetIndexBuffer(m_indexBuffer, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	Application->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Application->getDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstBuffer);
	Application->getDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);
	Application->getDeviceContext()->PSSetSamplers(0, 1, &g_pFontSampler);

	// Setup render state
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	Application->getDeviceContext()->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
	Application->getDeviceContext()->OMSetDepthStencilState(g_pDepthStencilState, 0);

	if (WF && g_pRasWireFrame)
		Application->getDeviceContext()->RSSetState(g_pRasWireFrame);

	else if (!WF && g_pRasStateSolid)
		Application->getDeviceContext()->RSSetState(g_pRasStateSolid);

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList *cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				ImVec2 clip_off = draw_data->DisplayPos;
				const D3D11_RECT r = 
				{
					(long)(pcmd->ClipRect.x - clip_off.x),
					(long)(pcmd->ClipRect.y - clip_off.y),
					(long)(pcmd->ClipRect.z - clip_off.x),
					(long)(pcmd->ClipRect.w - clip_off.y)
				};
				Application->getDeviceContext()->RSSetScissorRects(1, &r);

				ID3D11ShaderResourceView *texture_srv = (ID3D11ShaderResourceView *)pcmd->TextureId;
				Application->getDeviceContext()->PSSetShaderResources(0, 1, &texture_srv);
				Application->getDeviceContext()->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
}