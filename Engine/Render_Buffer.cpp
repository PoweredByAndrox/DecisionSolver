#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Render_Buffer.h"

ID3D11InputLayout *Render_Buffer::CreateLayout(ID3DBlob *Buffer_blob, bool WithColor)
{
	UINT numElements;

	if (WithColor)
	{
		const D3D11_INPUT_ELEMENT_DESC L_Element_UI[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		numElements = sizeof(L_Element_UI) / sizeof(L_Element_UI[0]);

		ID3D11InputLayout *Layout = nullptr;
		if (FAILED(Application->getDevice()->CreateInputLayout(L_Element_UI, numElements,
			Buffer_blob->GetBufferPointer(), Buffer_blob->GetBufferSize(), &Layout)))
		{
//			DebugTrace("Render_Buffer::CreateLayout()->CreateInputLayout() is failed");
			throw exception("Create failed!!!");
			return nullptr;
		}

		Layout->SetPrivateData(WKPDID_D3DDebugObjectName, string("LayoutUI").size() - 1, string("LayoutUI").c_str());

		return Layout;
	}
	else
	{
		const D3D11_INPUT_ELEMENT_DESC L_Element_Model[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof(L_Element_Model) / sizeof(L_Element_Model[0]);

		ID3D11InputLayout *Layout = nullptr;
		if (FAILED(Application->getDevice()->CreateInputLayout(L_Element_Model, numElements,
			Buffer_blob->GetBufferPointer(), Buffer_blob->GetBufferSize(), &Layout)))
		{
//			DebugTrace("Render_Buffer::CreateLayout()->CreateInputLayout() is failed");
			throw exception("Create failed!!!");
			return nullptr;
		}

		Layout->SetPrivateData(WKPDID_D3DDebugObjectName, string("Layout").size() - 1, string("Layout").c_str());

		return Layout;
	}

	return nullptr;
}

ID3D11Buffer *Render_Buffer::CreateVB(UINT ByteWidth, bool NeedVertice, D3D11_USAGE Usage,
	UINT CPUAccessFlags, void *vertices)
{
	if (ByteWidth > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
	{
#if defined (DEBUG)
		throw exception("VertexBuffer too large for DirectX 11");
#elif defined(NDEBUG)
//		DebugTrace("WARNING!!!\nVertexBuffer too large for DirectX 11");
#endif
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = Usage;
	vertexBufferDesc.ByteWidth = ByteWidth;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = CPUAccessFlags;

	if (NeedVertice)
	{
		ZeroMemory(&vertexData, sizeof(vertexData));
		vertexData.pSysMem = vertices;
	}

	ID3D11Buffer *VB = nullptr;

	if (FAILED(Application->getDevice()->CreateBuffer(&vertexBufferDesc, NeedVertice ? &vertexData : nullptr, &VB)))
	{
//		DebugTrace("Render_Buffer::CreateVB()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	VB->SetPrivateData(WKPDID_D3DDebugObjectName, string("VB").size() - 1, string("VB").c_str());

	return VB;
}

ID3D11Buffer *Render_Buffer::CreateIB(WORD ByteWidth, bool NeedIndices, D3D11_USAGE Usage,
	UINT CPUAccessFlags, void *indices)
{
	if (ByteWidth > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
	{
#if defined (DEBUG)
		throw exception("VertexBuffer too large for DirectX 11");
#elif defined(NDEBUG)
//		DebugTrace("WARNING!!!\nVertexBuffer too large for DirectX 11");
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
//		DebugTrace("Render_Buffer::CreateIB()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	IB->SetPrivateData(WKPDID_D3DDebugObjectName, string("IB").size() - 1, string("IB").c_str());

	return IB;
}

vector<ID3D11RasterizerState *> Render_Buffer::CreateWF()
{
	D3D11_RASTERIZER_DESC RasterDesc;
	ZeroMemory(&RasterDesc, sizeof(RasterDesc));
	RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.DepthClipEnable = true;

	vector<ID3D11RasterizerState *> WF_buff;
	ID3D11RasterizerState *RsWF = nullptr, *RsNoWF = nullptr;
	if (FAILED(Application->getDevice()->CreateRasterizerState(&RasterDesc, &RsWF)))
	{
//		DebugTrace("Render_Buffer::CreateWF()->CreateRasterizerState() is failed");
		throw exception("Create failed!!!");
	}
	WF_buff.push_back(RsWF);

	RsWF->SetPrivateData(WKPDID_D3DDebugObjectName, string("WireFrame").size() - 1, string("WireFrame").c_str());

	ZeroMemory(&RasterDesc, sizeof(RasterDesc));
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_BACK;
	RasterDesc.DepthClipEnable = true;

	if (FAILED(Application->getDevice()->CreateRasterizerState(&RasterDesc, &RsNoWF)))
	{
//		DebugTrace("Render_Buffer::CreateWF()->CreateRasterizerState() is failed");
		throw exception("Create failed!!!");
	}
	WF_buff.push_back(RsNoWF);

	RsNoWF->SetPrivateData(WKPDID_D3DDebugObjectName, string("noWireFrame").size() - 1, string("noWireFrame").c_str());

	return WF_buff;
}

ID3D11Buffer *Render_Buffer::CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags, UINT sizeofStruct)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = Usage;
	bd.ByteWidth = sizeofStruct;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = CPUAccessFlags;

	ID3D11Buffer *ConstBuff = nullptr;
	if (FAILED(Application->getDevice()->CreateBuffer(&bd, NULL, &ConstBuff)))
	{
//		DebugTrace("Render_Buffer::CreateConstBuff()->CreateBuffer() is failed");
		throw exception("Create failed!!!");
		return nullptr;
	}

	ConstBuff->SetPrivateData(WKPDID_D3DDebugObjectName, string("ConstBuff").size() - 1, string("ConstBuff").c_str());

	return ConstBuff;
}