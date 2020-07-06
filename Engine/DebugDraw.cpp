#include "pch.h"
#include "DebugDraw.h"

//--------------------------------------------------------------------------------------
// File: DebugDraw.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Camera.h"
#include "Console.h"
#include "DebugDraw.h"
#include "Shaders.h"
#include "File_system.h"
#include "Render_Buffer.h"

void DebugDraw::BaseRender::Render()
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	if (g_pIndexBuffer)
		Application->getDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	Application->getDeviceContext()->IASetInputLayout(g_pVertexLayout);

	Application->getDeviceContext()->VSSetShader(g_pVertexShader, NULL, 0);
	Application->getDeviceContext()->PSSetShader(g_pPixelShader, NULL, 0);
}

void DebugDraw::BaseRender::Release()
{
	SAFE_RELEASE(g_pConstantBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
}

void DebugDraw::TriangleDraw::Draw(Matrix View, Matrix Proj)
{
	ConstantBuffer cb{};
	cb.World = XMMatrixTranspose(Matrix::CreateTranslation(0.f, 0.f, 0.f));
	cb.View = XMMatrixTranspose(View);
	cb.Proj = XMMatrixTranspose(Proj);
	Application->getDeviceContext()->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	Application->getDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	BaseRender::Render();
	Application->getDeviceContext()->Draw(SizeVrtx, 0);
}

void DebugDraw::TriangleDraw::Init(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color)
{
	vector<BaseRender::SimpleVertex> verts = 
	{
		SimpleVertex(pointA, color),
		SimpleVertex(pointB, color),
		SimpleVertex(pointC, color),
		SimpleVertex(pointA, color)
	};

	BaseRender::Init(verts, vector<WORD>());
	BaseRender::SizeVrtx = 4;
}

void DebugDraw::BaseRender::Init(vector<SimpleVertex> verts, vector<WORD> s_indices)
{
	vector<ID3DBlob *> Buffer_blob;
	vector<string> FileShaders =
	{
		Application->getFS()->GetFile(string("VertexShader.hlsl"))->PathA,
		Application->getFS()->GetFile(string("PixelShader.hlsl"))->PathA
	};
	vector<string> Functions =
	{
		string("DDraw_VS"),
		string("DDraw_PS")
	},
		Version =
	{
		string("vs_4_0"),
		string("ps_4_0")
	};
	vector<void *> Buffers = Shaders::CompileShaderFromFile(Buffer_blob =
		Shaders::CreateShaderFromFile(FileShaders, Functions, Version));
	g_pVertexShader = (ID3D11VertexShader *)Buffers[0]; // VS
	g_pPixelShader = (ID3D11PixelShader *)Buffers[1]; // PS

	Application->getDevice()->CreateVertexShader(Buffer_blob.at(0)->GetBufferPointer(), Buffer_blob.at(0)->GetBufferSize(),
		NULL, &g_pVertexShader);
	Application->getDevice()->CreatePixelShader(Buffer_blob.at(1)->GetBufferPointer(), Buffer_blob.at(1)->GetBufferSize(),
		NULL, &g_pPixelShader);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Application->getDevice()->CreateInputLayout(ied, 2, Buffer_blob.at(0)->GetBufferPointer(),
		Buffer_blob.at(0)->GetBufferSize(), &g_pVertexLayout);

	g_pConstantBuffer = Render_Buffer::CreateConstBuff(D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, sizeof(ConstantBuffer));

	SizeVrtx = s_indices.size();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * verts.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &verts[0];
	auto hr = Application->getDevice()->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	//if (FAILED(hr))
	//	ThrowIfFailed(hr);

	if (s_indices.empty()) return;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * s_indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &s_indices[0];
	hr = Application->getDevice()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	//if (FAILED(hr))
	//	ThrowIfFailed(hr);
}

void DebugDraw::Box::Draw(Vector3 Pos, Matrix View, Matrix Proj)
{
	ConstantBuffer cb{};
	cb.World = XMMatrixTranspose(Matrix::CreateTranslation(Pos));
	cb.View = XMMatrixTranspose(View);
	cb.Proj = XMMatrixTranspose(Proj);
	Application->getDeviceContext()->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	Application->getDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	BaseRender::Render();
	Application->getDeviceContext()->DrawIndexed(SizeVrtx, 0, 0);
}

void DebugDraw::Box::Init(BoundingBox Box, Vector4 color)
{
	_Box = Box;
	const vector<Vector3> s_verts =
	{
		{ -1.f, -1.f, -1.f },
		{  1.f, -1.f, -1.f },
		{  1.f, -1.f,  1.f },
		{ -1.f, -1.f,  1.f },
		{ -1.f,  1.f, -1.f },
		{  1.f,  1.f, -1.f },
		{  1.f,  1.f,  1.f },
		{ -1.f,  1.f,  1.f }
	};

	const vector<WORD> s_indices =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	vector<SimpleVertex> verts;
	for (size_t i = 0; i < s_verts.size(); ++i)
	{
		verts.push_back(SimpleVertex(Vector3::Transform(s_verts.at(i), Matrix::CreateScale(Vector3(Box.Extents))), color));
	}

	BaseRender::Init(verts, s_indices);
}

void DebugDraw::Sphere::Draw(Vector3 Pos, Matrix View, Matrix Proj)
{
	ConstantBuffer cb{};
	cb.World = XMMatrixTranspose(Matrix::CreateTranslation(Pos));
	cb.View = XMMatrixTranspose(View);
	cb.Proj = XMMatrixTranspose(Proj);
	Application->getDeviceContext()->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	Application->getDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	BaseRender::Render();
	Application->getDeviceContext()->DrawIndexed(SizeVrtx, 0, 0);
}

void DebugDraw::Sphere::Init(BoundingSphere Sphere, Vector4 color)
{
	_Sphere = Sphere;
	WORD tessellation = 3;
	WORD verticalSegments = 3;
	WORD horizontalSegments = tessellation * 2;

	float radius = 10.f / 2;
	
	vector<WORD> s_indices;
	vector<SimpleVertex> verts;

	// Create rings of vertices at progressively higher latitudes.
	for (WORD i = 0; i <= verticalSegments; i++)
	{
		float v = 1 - float(i) / float(verticalSegments);

		float latitude = (float(i) * XM_PI / float(verticalSegments)) - XM_PIDIV2;
		float dy, dxz;

		XMScalarSinCos(&dy, &dxz, latitude);

		// Create a single ring of vertices at this latitude.
		for (WORD j = 0; j <= horizontalSegments; j++)
		{
			float u = float(j) / float(horizontalSegments);

			float longitude = float(j) * XM_2PI / float(horizontalSegments);
			float dx, dz;

			XMScalarSinCos(&dx, &dz, longitude);

			dx *= dxz;
			dz *= dxz;

			verts.push_back(SimpleVertex(Vector3::Transform(Vector3(longitude), Matrix::CreateScale(Vector3(radius))),
				color));
		}
	}

	// Fill the index buffer with triangles joining each pair of latitude rings.
	WORD stride = horizontalSegments + 1;

	for (WORD i = 0; i < verticalSegments; i++)
	{
		for (WORD j = 0; j <= horizontalSegments; j++)
		{
			WORD nextI = i + 1;
			WORD nextJ = (j + 1) % stride;

			s_indices.push_back(i * stride + j);
			s_indices.push_back(nextI * stride + j);
			s_indices.push_back(i * stride + nextJ);

			s_indices.push_back(i * stride + nextJ);
			s_indices.push_back(nextI * stride + j);
			s_indices.push_back(nextI * stride + nextJ);
		}
	}
	for (auto it = s_indices.begin(); it != s_indices.end(); it += 3)
	{
		std::swap(*it, *(it + 2));
	}

	BaseRender::Init(verts, s_indices);
}

void DebugDraw::MainRender(Vector3 Pos, Matrix View, Matrix Proj)
{
	for (auto it: triangle)
	{
		it->Draw(View, Proj);
	}
	for (auto it: boxes)
	{
		it->Draw(Pos, View, Proj);
	}
	for (auto it: spheres)
	{
		it->Draw(Pos, View, Proj);
	}
}

void DebugDraw::AddTriangle(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color)
{
	triangle.push_back(make_shared<TriangleDraw>());
	triangle.back()->Init(pointA, pointB, pointC, color);
}

void DebugDraw::AddBox(Vector3 Pos, Vector3 Size, Vector4 color)
{
	boxes.push_back(make_shared<Box>());

	BoundingBox _Box;
	_Box.Center = Pos;
	_Box.Extents = Size;
	boxes.back()->Init(_Box, color);
}

void DebugDraw::AddSphere(Vector3 Pos, float Radius, Vector4 color)
{
	spheres.push_back(make_shared<Sphere>());

	BoundingSphere _Sphere;
	_Sphere.Center = Pos;
	_Sphere.Radius = Radius;
	spheres.back()->Init(_Sphere, color);
}

//void DebugDraw::DrawCube(const BoundingBox &box, Vector4 color)
//{
//
//}
//
//void DebugDraw::DrawPyramid(Vector4 color)
//{
//	//if (!m_states_Pyramid.operator bool() || !m_effect_Pyramid.operator bool() ||
//	//	!m_batch_Pyramid.operator bool() || !m_inputLayout_Pyramid)
//	//{
//	//	Engine::LogError("DebugDraw::DrawPyramid() Failed!", "DebugDraw::DrawPyramid() Failed!",
//	//		"DebugDraw: Something is wrong with draw Debug Pyramid!");
//	//	return;
//	//}
//
//	VertexCollection s_verts;
//	//{
//	//	{ XMFLOAT3(0.0f,  1.5f,  0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
//
//	//	{ XMFLOAT3(-1.0f,  0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
//
//	//	{ XMFLOAT3(1.0f,  0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
//
//	//	{ XMFLOAT3(-1.0f,  0.0f,  1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
//
//	//	{ XMFLOAT3(1.0f,  0.0f,  1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }
//	//};
//
//	IndexCollection s_indices;
//	//{
//	//	0,2,1,
//
//	//	0,3,4,
//
//	//	0,1,3,
//
//	//	0,4,2,
//
//	//	1,2,3,
//	//	2,4,3,
//	//};
//
//	DirectX::ComputeGeoSphere(s_verts, s_indices, 5.f, 20, false);
//
//	vector<SimpleVertex> verts;
//	for (auto it: s_verts)
//	{
//		verts.push_back(SimpleVertex(it.position, color));
//	}
//
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(SimpleVertex) * s_verts.size();
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = &s_verts[0];
//	auto hr = Application->getDevice()->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
//	if (FAILED(hr))
//		ThrowIfFailed(hr);
//
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(WORD) * s_indices.size();
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	InitData.pSysMem = &s_indices[0];
//	hr = Application->getDevice()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
//	if (FAILED(hr))
//		ThrowIfFailed(hr);
//
//	ConstantBuffer cb{};
//	cb.World = XMMatrixTranspose(Matrix::CreateTranslation(0.f, 0.f, 0.f));
//	cb.View = XMMatrixTranspose(Application->getCamera()->GetViewMatrix());
//	cb.Proj = XMMatrixTranspose(Application->getCamera()->GetProjMatrix());
//	Application->getDeviceContext()->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
//
//	UINT stride = sizeof(SimpleVertex);
//	UINT offset = 0;
//	Application->getDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
//	Application->getDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
//
//	Application->getDeviceContext()->VSSetShader(g_pVertexShader, NULL, 0);
//	Application->getDeviceContext()->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
//	Application->getDeviceContext()->PSSetShader(g_pPixelShader, NULL, 0);
//	Application->getDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
//	Application->getDeviceContext()->IASetInputLayout(g_pVertexLayout);
//	Application->getDeviceContext()->DrawIndexed(s_indices.size(), 0, 0);
//}
//
//void DebugDraw::Draw(const BoundingSphere &sphere, Vector4 color)
//{
//	if (!m_states_Sphere.operator bool() || !m_effect_Sphere.operator bool() ||
//		!m_batch_Sphere.operator bool() || !m_inputLayout_Sphere)
//	{
//		Engine::LogError("DebugDraw::Draw(const Sphere) Failed!", "DebugDraw::Draw(const Sphere) Failed!",
//			"DebugDraw: Something is wrong with draw Debug Sphere!");
//		return;
//	}
//
//	Vector3 origin = XMLoadFloat3(&sphere.Center);
//
//	const float radius = sphere.Radius;
//
//	Vector3 xaxis = g_XMIdentityR0 * radius;
//	Vector3 yaxis = g_XMIdentityR1 * radius;
//	Vector3 zaxis = g_XMIdentityR2 * radius;
//
//	DrawRing(origin, xaxis, zaxis, color);
//	DrawRing(origin, xaxis, yaxis, color);
//	DrawRing(origin, yaxis, zaxis, color);
//}
//
//void DebugDraw::Draw(const BoundingBox &box, Vector4 color)
//{
//	if (!m_states_Box.operator bool() || !m_effect_Box.operator bool() ||
//		!m_batch_Box.operator bool() || !m_inputLayout_Box)
//	{
//		Engine::LogError("DebugDraw::Draw(const Box) Failed!", "DebugDraw::Draw(const Box) Failed!",
//			"DebugDraw: Something is wrong with draw Debug Box!");
//		return;
//	}
//
//	m_effect_Box->SetMatrices(Matrix::Identity/*CreateScale(Vector3(box.Extents))
//		* Matrix::CreateTranslation(box.Center)*/,
//		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_Box->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Box->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_Box->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Box.Get());
//
//	m_batch_Box->Begin();
//
//	m_batch_Box->End();
//}
//
//void DebugDraw::Draw(const BoundingOrientedBox &obb, Vector4 color)
//{
//	if (!m_states_BBox.operator bool() || !m_effect_BBox.operator bool() ||
//		!m_batch_BBox.operator bool() || !m_inputLayout_BBox)
//	{
//		Engine::LogError("DebugDraw::Draw(const Cube) Failed!", "DebugDraw::Draw(const Cube) Failed!",
//			"DebugDraw: Something is wrong with draw Debug Cube!");
//		return;
//	}
//
//	m_effect_BBox->SetMatrices(Matrix::Identity,
//		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_BBox->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_BBox->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_BBox->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_BBox.Get());
//
//	m_batch_BBox->Begin();
//
//	m_batch_BBox->End();
//}
//
//void DebugDraw::Draw(const BoundingFrustum &frustum, Vector4 color)
//{
//	if (!m_states_Frustum.operator bool() || !m_effect_Frustum.operator bool() ||
//		!m_batch_Frustum.operator bool() || !m_inputLayout_Frustum)
//	{
//		Engine::LogError("DebugDraw::Draw(const Frustum) Failed!", "DebugDraw::Draw(const Frustum) Failed!",
//			"DebugDraw: Something is wrong with draw Debug Frustum!");
//		return;
//	}
//
//	m_effect_Frustum->SetMatrices(Matrix::Identity,
//		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_Frustum->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Frustum->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_Frustum->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Frustum.Get());
//
//	m_batch_Frustum->Begin();
//
//	Vector3 corners[BoundingFrustum::CORNER_COUNT];
//	frustum.GetCorners(corners);
//
//	VertexPositionColor verts[24] = {};
//	verts[0].position = corners[0];
//	verts[1].position = corners[1];
//	verts[2].position = corners[1];
//	verts[3].position = corners[2];
//	verts[4].position = corners[2];
//	verts[5].position = corners[3];
//	verts[6].position = corners[3];
//	verts[7].position = corners[0];
//
//	verts[8].position = corners[0];
//	verts[9].position = corners[4];
//	verts[10].position = corners[1];
//	verts[11].position = corners[5];
//	verts[12].position = corners[2];
//	verts[13].position = corners[6];
//	verts[14].position = corners[3];
//	verts[15].position = corners[7];
//
//	verts[16].position = corners[4];
//	verts[17].position = corners[5];
//	verts[18].position = corners[5];
//	verts[19].position = corners[6];
//	verts[20].position = corners[6];
//	verts[21].position = corners[7];
//	verts[22].position = corners[7];
//	verts[23].position = corners[4];
//
//	for (size_t j = 0; j < _countof(verts); ++j)
//	{
//		XMStoreFloat4(&verts[j].color, color);
//	}
//
//	m_batch_Frustum->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, _countof(verts));
//	m_batch_Frustum->End();
//}
//
//{
//	m_effect_Grid->SetMatrices(Matrix::Identity,
//		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_Grid->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Grid->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_Grid->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Grid.Get());
//
//	m_batch_Grid->Begin();
//
//
//	m_batch_Grid->End();
//}
//
//void DebugDraw::DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis, Vector4 color)
//{
//	if (!m_states_Sphere.operator bool() || !m_effect_Sphere.operator bool() ||
//		!m_batch_Sphere.operator bool() || !m_inputLayout_Sphere)
//	{
//		Engine::LogError("DebugDraw::Draw(const Ring) Failed!", "DebugDraw::Draw(const Ring) Failed!",
//			"DebugDraw: Something is wrong with draw Debug Ring!");
//		return;
//	}
//
//	m_effect_Sphere->SetMatrices(
//		Matrix::Identity, Application->getCamera()->GetViewMatrix(),
//		Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_Sphere->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Sphere->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_Sphere->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Sphere.Get());
//
//	m_batch_Sphere->Begin();
//
//	const size_t c_ringSegments = 32;
//
//	VertexPositionColor verts[c_ringSegments + 1];
//
//	float fAngleDelta = XM_2PI / float(c_ringSegments);
//	// Instead of calling cos/sin for each segment we calculate
//	// the sign of the angle delta and then incrementally calculate sin
//	// and cosine from then on.
//	Vector3 cosDelta = XMVectorReplicate(cosf(fAngleDelta));
//	Vector3 sinDelta = XMVectorReplicate(sinf(fAngleDelta));
//	Vector3 incrementalSin = XMVectorZero();
//	static const XMVECTORF32 s_initialCos =
//	{
//		1.f, 1.f, 1.f, 1.f
//	};
//	Vector3 incrementalCos = s_initialCos.v;
//	for (size_t i = 0; i < c_ringSegments; i++)
//	{
//		Vector3 pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
//		pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
//		XMStoreFloat3(&verts[i].position, pos);
//		XMStoreFloat4(&verts[i].color, color);
//		// Standard formula to rotate a vector.
//		Vector3 newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
//		Vector3 newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
//		incrementalCos = newCos;
//		incrementalSin = newSin;
//	}
//	verts[c_ringSegments] = verts[0];
//
//	m_batch_Sphere->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
//	m_batch_Sphere->End();
//}
//
//void DebugDraw::DrawRay(Vector3 origin, Vector3 direction, Vector4 color, bool normalize)
//{
//	if (!m_states_Ray.operator bool() || !m_effect_Ray.operator bool() ||
//		!m_batch_Ray.operator bool() || !m_inputLayout_Ray)
//	{
//		Engine::LogError("DebugDraw::Draw(const Ray) Failed!", "DebugDraw::Draw(const Ray) Failed!",
//			"DebugDraw: Something is wrong with draw Debug Ray!");
//		return;
//	}
//
//	m_effect_Ray->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
//		Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_Ray->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Ray->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_Ray->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Ray.Get());
//
//	m_batch_Ray->Begin();
//
//	VertexPositionColor verts[3];
//	XMStoreFloat3(&verts[0].position, origin);
//
//	Vector3 normDirection = XMVector3Normalize(direction);
//	Vector3 rayDirection = (normalize) ? normDirection : direction;
//
//	Vector3 perpVector = XMVector3Cross(normDirection, g_XMIdentityR1);
//
//	if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
//	{
//		perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
//	}
//	perpVector = XMVector3Normalize(perpVector);
//
//	XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, origin));
//	perpVector = XMVectorScale(perpVector, 0.0625f);
//	normDirection = XMVectorScale(normDirection, -0.25f);
//	rayDirection = XMVectorAdd(perpVector, rayDirection);
//	rayDirection = XMVectorAdd(normDirection, rayDirection);
//	XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, origin));
//
//	XMStoreFloat4(&verts[0].color, color);
//	XMStoreFloat4(&verts[1].color, color);
//	XMStoreFloat4(&verts[2].color, color);
//
//	m_batch_Ray->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
//	m_batch_Ray->End();
//}
//
//void DebugDraw::DrawTriangle()
//{
//	m_effect_Triangle->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
//		Application->getCamera()->GetProjMatrix());
//
//	Application->getDeviceContext()->OMSetBlendState(m_states_Triangle->Opaque(), nullptr, 0xFFFFFFFF);
//	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Triangle->DepthNone(), 0);
//	Application->getDeviceContext()->RSSetState(m_raster.Get());
//
//	m_effect_Triangle->Apply(Application->getDeviceContext());
//
//	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Triangle.Get());
//
//	m_batch_Triangle->Begin();
//
//
//	m_batch_Triangle->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);
//	m_batch_Triangle->End();
//}
//
//void DebugDraw::Release()
//{
//	m_states_Ray.release();
//	m_states_Box.release();
//	m_states_BBox.release();
//	m_states_Sphere.release();
//	m_states_Frustum.release();
//	m_states_Grid.release();
//	m_states_Triangle.release();
//	m_states_Pyramid.release();
//
//	m_effect_Ray.release();
//	m_effect_Box.release();
//	m_effect_BBox.release();
//	m_effect_Sphere.release();
//	m_effect_Frustum.release();
//	m_effect_Grid.release();
//	m_effect_Triangle.release();
//	m_effect_Pyramid.release();
//
//	m_batch_Ray.release();
//	m_batch_Box.release();
//	m_batch_BBox.release();
//	m_batch_Sphere.release();
//	m_batch_Frustum.release();
//	m_batch_Grid.release();
//	m_batch_Triangle.release();
//	m_batch_Pyramid.release();
//}
