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

	V_RETURN(Device->CreateBuffer(&bd, NULL, &m_pConstBuffer));

	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	V_RETURN(Device->CreateInputLayout(L_Element, numElements, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(), &m_layout));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	V_RETURN(Device->CreateSamplerState(&samplerDesc, &m_sampleState));

	init = true;
	return hr;
}

ID3D11Buffer *Engine::Render_Buffer::CreateVB(int ByteWidth, void *vertices)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = ByteWidth;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	GetD3DDevice();

	ID3D11Buffer *VB = nullptr;

	ThrowIfFailed(Device->CreateBuffer(&vertexBufferDesc, &vertexData, &VB));
	return VB;
}
ID3D11Buffer *Engine::Render_Buffer::CreateIB(int ByteWidthInds, UINT *indices)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = ByteWidthInds;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	GetD3DDevice();
	ID3D11Buffer *IB = nullptr;

	ThrowIfFailed(Device->CreateBuffer(&indexBufferDesc, &indexData, &IB));
	return IB;
}

void Engine::Render_Buffer::RenderSimpleBuffer(Matrix World, Matrix View, Matrix Proj, int Indicies)
{
	cb.mWorld = XMMatrixTranspose(World);
	cb.mView = XMMatrixTranspose(View);
	cb.mProj = XMMatrixTranspose(Proj);

	GetD3DDeviceCon();
	DeviceCon->IASetInputLayout(m_layout);

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
	V_RETURN(Device->CreateBuffer(&bd, &Data, &m_vertexBuffer));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	Data.pSysMem = indices.data();

	V_RETURN(Device->CreateBuffer(&bd, &Data, &m_indexBuffer));

	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	V_RETURN(Device->CreateInputLayout(L_Element, numElements, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(), &m_layout));

	CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	init = true;
	return hr;
}

HRESULT Engine::Render_Buffer::InitModels(UINT SizeofVertex, void *vertices, vector<UINT> indices,
	vector<wstring> *ShaderFile, vector<string> *Func, vector<string> *VersionShader)
{
	InitSimpleBuffer(ShaderFile, Func, VersionShader);

	if (m_vertexBuffer)
		SAFE_RELEASE(m_vertexBuffer);

	if (m_indexBuffer)
		SAFE_RELEASE(m_indexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = SizeofVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	ZeroMemory(&Data, sizeof(Data));
	Data.pSysMem = vertices;

	GetD3DDevice();
	V_RETURN(Device->CreateBuffer(&bd, &Data, &m_vertexBuffer));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	Data.pSysMem = indices.data();

	V_RETURN(Device->CreateBuffer(&bd, &Data, &m_indexBuffer));

	const D3D11_INPUT_ELEMENT_DESC L_Element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(L_Element) / sizeof(L_Element[0]);

	V_RETURN(Device->CreateInputLayout(L_Element, numElements, Buffer_blob[0]->GetBufferPointer(), Buffer_blob[0]->GetBufferSize(), &m_layout));

	CreateConstBuff(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	init = true;
	return hr;
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
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	GetD3DDevice();
	V_RETURN(Device->CreateBuffer(&bd, NULL, &m_pConstBuffer));

	return S_OK;
}

HRESULT Engine::Render_Buffer::CreateTexture(const wchar_t *TextrFName)
{
	GetD3DDevice();
	ThrowIfFailed(hr = CreateWICTextureFromFile(Device, TextrFName, nullptr, &m_texture));
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

	if (RenderBuff[0])
		DeviceCon->IASetVertexBuffers(0, 1, &RenderBuff[0], &stride, &offset);
	if (RenderBuff[1])
		DeviceCon->IASetIndexBuffer(RenderBuff[1], DXGI_FORMAT_R32_UINT, 0);

	DeviceCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceCon->VSSetShader(m_vertexShader, NULL, 0);
	DeviceCon->PSSetShader(m_pixelShader, NULL, 0);

	DeviceCon->PSSetSamplers(0, 1, &m_sampleState);

	ThrowIfFailed(DeviceCon->Map(m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = (ConstantBuffer *)mappedResource.pData;

	dataPtr->mWorld = XMMatrixTranspose(World);
	dataPtr->mView = XMMatrixTranspose(View);
	dataPtr->mProj = XMMatrixTranspose(Proj);

	DeviceCon->Unmap(m_pConstBuffer, 0);

	DeviceCon->PSSetShaderResources(0, 1, &m_texture);

	DeviceCon->VSSetConstantBuffers(0, 1, &m_pConstBuffer);

	DeviceCon->DrawIndexed(Indices, 0, 0);
}

void Engine::Render_Buffer::RenderModels(Matrix World, Matrix View, Matrix Proj, UINT SizeIndices, ID3D11ShaderResourceView *RenderTextr, UINT stride)
{
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GetD3DDeviceCon();

	DeviceCon->IASetInputLayout(m_layout);

	DeviceCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	DeviceCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DeviceCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
