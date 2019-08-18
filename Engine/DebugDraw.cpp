#include "pch.h"
#include "DebugDraw.h"

//--------------------------------------------------------------------------------------
// File: DebugDraw.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "DebugDraw.h"
#include "Console.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "Camera.h"
void DebugDraw::Init()
{
	m_states_Ray = make_unique<CommonStates>(Application->getDevice());
	m_states_Box = make_unique<CommonStates>(Application->getDevice());
	m_states_Sphere = make_unique<CommonStates>(Application->getDevice());
	m_states_BBox = make_unique<CommonStates>(Application->getDevice());
	m_states_Grid = make_unique<CommonStates>(Application->getDevice());
	m_states_Frustum = make_unique<CommonStates>(Application->getDevice());
	m_states_Triangle = make_unique<CommonStates>(Application->getDevice());
	m_states_Pyramid = make_unique<CommonStates>(Application->getDevice());

	m_effect_Ray = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Ray->SetVertexColorEnabled(true);
	m_effect_Ray->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	void const *shaderByteCode;
	size_t byteCodeLength;

	m_effect_Ray->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_Box = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Box->SetVertexColorEnabled(true);
	m_effect_Box->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_Box->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_BBox = make_unique<BasicEffect>(Application->getDevice());
	m_effect_BBox->SetVertexColorEnabled(true);
	m_effect_BBox->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_BBox->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_Sphere = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Sphere->SetVertexColorEnabled(true);
	m_effect_Sphere->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_Sphere->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_Frustum = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Frustum->SetVertexColorEnabled(true);
	m_effect_Frustum->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_Frustum->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_Grid = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Grid->SetVertexColorEnabled(true);
	m_effect_Grid->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_Grid->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_Triangle = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Triangle->SetVertexColorEnabled(true);
	m_effect_Triangle->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_Triangle->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_effect_Pyramid = make_unique<BasicEffect>(Application->getDevice());
	m_effect_Pyramid->SetVertexColorEnabled(true);
	m_effect_Pyramid->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	m_effect_Pyramid->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,shaderByteCode, byteCodeLength,
		m_inputLayout_Ray.ReleaseAndGetAddressOf()));
	m_batch_Ray = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());
	
	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
		m_inputLayout_Box.ReleaseAndGetAddressOf()));
	m_batch_Box = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
		m_inputLayout_BBox.ReleaseAndGetAddressOf()));
	m_batch_BBox = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
		m_inputLayout_Sphere.ReleaseAndGetAddressOf()));
	m_batch_Sphere = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength
		, m_inputLayout_Grid.ReleaseAndGetAddressOf()));
	m_batch_Grid = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
		m_inputLayout_Frustum.ReleaseAndGetAddressOf()));
	m_batch_Frustum = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
		m_inputLayout_Triangle.ReleaseAndGetAddressOf()));
	m_batch_Triangle = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	ThrowIfFailed(Application->getDevice()->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
		m_inputLayout_Pyramid.ReleaseAndGetAddressOf()));
	m_batch_Pyramid = make_unique<PrimitiveBatch<VertexPositionColor>>(Application->getDeviceContext());

	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, false,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, true, false, true, true);

	ThrowIfFailed(Application->getDevice()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));
}

void DebugDraw::DrawCube(Matrix matWorld, Vector4 color, bool BBox)
{
	static const Vector3 s_verts[8] =
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

	static const WORD s_indices[] =
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

	vector<VertexPositionColor> verts;
	for (size_t i = 0; i < 8; ++i)
	{
		verts.push_back(VertexPositionColor(Vector3::Transform(s_verts[i], matWorld), color));
	}

	BBox ?
		m_batch_BBox->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), &verts[0], 8)
		:
		m_batch_Box->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), &verts[0], 8);
}

void DebugDraw::DrawPyramid(Matrix matWorld, Vector4 color)
{
	if (!m_states_Pyramid.operator bool() || !m_effect_Pyramid.operator bool() ||
		!m_batch_Pyramid.operator bool() || !m_inputLayout_Pyramid)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::DrawPyramid() Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::DrawPyramid() Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Pyramid!");
		return;
	}

	static const Vector3 s_verts[] =
	{
	{ XMFLOAT3(-1.0f,  1.0f, -1.0f) },
	{ XMFLOAT3(1.0f,  1.0f, -1.0f) },
	{ XMFLOAT3(1.0f,  1.0f,  1.0f) },
	{ XMFLOAT3(-1.0f,  1.0f,  1.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f) },
	{ XMFLOAT3(1.0f, -1.0f,  1.0f) },
	{ XMFLOAT3(-1.0f, -1.0f,  1.0f) }
	};

	static const WORD s_indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		0, 5, 4,
		1, 5, 0,

		3, 4, 7,
		0, 4, 3,

		1, 6, 5,
		2, 6, 1,

		2, 7, 6,
		3, 7, 2,

		6, 4, 5,
		7, 4, 6
	};

	vector<VertexPositionColor> verts;
	for (size_t i = 0; i < 5; ++i)
	{
		verts.push_back(VertexPositionColor(Vector3::Transform(s_verts[i], matWorld), color));
	}

	m_effect_Pyramid->SetMatrices(Matrix::Identity,
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Pyramid->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Pyramid->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Pyramid->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Pyramid.Get());

	m_batch_Pyramid->Begin();
	m_batch_Pyramid->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, s_indices, 18, &verts[0], 5);
	m_batch_Pyramid->End();
}

void DebugDraw::Draw(const BoundingSphere &sphere, Vector4 color)
{
	if (!m_states_Sphere.operator bool() || !m_effect_Sphere.operator bool() ||
		!m_batch_Sphere.operator bool() || !m_inputLayout_Sphere)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Sphere) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Sphere) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Sphere!");
		return;
	}

	Vector3 origin = XMLoadFloat3(&sphere.Center);

	const float radius = sphere.Radius;

	Vector3 xaxis = g_XMIdentityR0 * radius;
	Vector3 yaxis = g_XMIdentityR1 * radius;
	Vector3 zaxis = g_XMIdentityR2 * radius;

	DrawRing(origin, xaxis, zaxis, color);
	DrawRing(origin, xaxis, yaxis, color);
	DrawRing(origin, yaxis, zaxis, color);
}

void DebugDraw::Draw(const BoundingBox &box, Vector4 color)
{
	if (!m_states_Box.operator bool() || !m_effect_Box.operator bool() ||
		!m_batch_Box.operator bool() || !m_inputLayout_Box)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Box) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Box) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Box!");
		return;
	}

	m_effect_Box->SetMatrices(Matrix::Identity/*CreateScale(Vector3(box.Extents))
		* Matrix::CreateTranslation(box.Center)*/,
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Box->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Box->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Box->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Box.Get());

	m_batch_Box->Begin();

	DrawCube(Matrix::CreateScale(Vector3(box.Extents)), color, false);

	m_batch_Box->End();
}

void DebugDraw::Draw(const BoundingOrientedBox &obb, Vector4 color)
{
	if (!m_states_BBox.operator bool() || !m_effect_BBox.operator bool() ||
		!m_batch_BBox.operator bool() || !m_inputLayout_BBox)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Cube) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Cube) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Cube!");
		return;
	}

	m_effect_BBox->SetMatrices(Matrix::Identity,
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_BBox->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_BBox->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_BBox->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_BBox.Get());

	m_batch_BBox->Begin();

	DrawCube(Matrix::CreateScale(Vector3(obb.Extents)) * Matrix::CreateTranslation(obb.Center), color, true);

	m_batch_BBox->End();
}

void DebugDraw::Draw(const BoundingFrustum &frustum, Vector4 color)
{
	if (!m_states_Frustum.operator bool() || !m_effect_Frustum.operator bool() ||
		!m_batch_Frustum.operator bool() || !m_inputLayout_Frustum)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Frustum) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Frustum) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Frustum!");
		return;
	}

	m_effect_Frustum->SetMatrices(Matrix::Identity,
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Frustum->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Frustum->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Frustum->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Frustum.Get());

	m_batch_Frustum->Begin();

	Vector3 corners[BoundingFrustum::CORNER_COUNT];
	frustum.GetCorners(corners);

	VertexPositionColor verts[24] = {};
	verts[0].position = corners[0];
	verts[1].position = corners[1];
	verts[2].position = corners[1];
	verts[3].position = corners[2];
	verts[4].position = corners[2];
	verts[5].position = corners[3];
	verts[6].position = corners[3];
	verts[7].position = corners[0];

	verts[8].position = corners[0];
	verts[9].position = corners[4];
	verts[10].position = corners[1];
	verts[11].position = corners[5];
	verts[12].position = corners[2];
	verts[13].position = corners[6];
	verts[14].position = corners[3];
	verts[15].position = corners[7];

	verts[16].position = corners[4];
	verts[17].position = corners[5];
	verts[18].position = corners[5];
	verts[19].position = corners[6];
	verts[20].position = corners[6];
	verts[21].position = corners[7];
	verts[22].position = corners[7];
	verts[23].position = corners[4];

	for (size_t j = 0; j < _countof(verts); ++j)
	{
		XMStoreFloat4(&verts[j].color, color);
	}

	m_batch_Frustum->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, _countof(verts));
	m_batch_Frustum->End();
}

void DebugDraw::DrawGrid(Vector3 xAxis, Vector3 yAxis, Vector3 origin, float DisCells, Vector4 color)
{
	if (!m_states_Grid.operator bool() || !m_effect_Grid.operator bool() ||
		!m_batch_Grid.operator bool() || !m_inputLayout_Grid)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Grid) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Grid) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Grid!");
		return;
	}

	m_effect_Grid->SetMatrices(Matrix::Identity,
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Grid->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Grid->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Grid->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Grid.Get());

	m_batch_Grid->Begin();

	size_t divisions = (size_t)max<float>(1, DisCells);

	for (size_t i = 0; i <= divisions; ++i)
	{
		float fPercent = float(i) / float(divisions);
		fPercent = (fPercent * 2.0f) - 1.0f;

		Vector3 scale = xAxis * fPercent + origin;

		VertexPositionColor v1(scale - yAxis, color);
		VertexPositionColor v2(scale + yAxis, color);
		m_batch_Grid->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divisions; i++)
	{
		float fPercent = float(i) / float(divisions);
		fPercent = (fPercent * 2.0f) - 1.0f;

		Vector3 scale = yAxis * fPercent + origin;

		VertexPositionColor v1(scale - xAxis, color);
		VertexPositionColor v2(scale + xAxis, color);
		m_batch_Grid->DrawLine(v1, v2);
	}

	m_batch_Grid->End();
}

void DebugDraw::DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis, Vector4 color)
{
	if (!m_states_Sphere.operator bool() || !m_effect_Sphere.operator bool() ||
		!m_batch_Sphere.operator bool() || !m_inputLayout_Sphere)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Ring) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Ring) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Ring!");
		return;
	}

	m_effect_Sphere->SetMatrices(
		Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Sphere->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Sphere->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Sphere->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Sphere.Get());

	m_batch_Sphere->Begin();

	const size_t c_ringSegments = 32;

	VertexPositionColor verts[c_ringSegments + 1];

	float fAngleDelta = XM_2PI / float(c_ringSegments);
	// Instead of calling cos/sin for each segment we calculate
	// the sign of the angle delta and then incrementally calculate sin
	// and cosine from then on.
	Vector3 cosDelta = XMVectorReplicate(cosf(fAngleDelta));
	Vector3 sinDelta = XMVectorReplicate(sinf(fAngleDelta));
	Vector3 incrementalSin = XMVectorZero();
	static const XMVECTORF32 s_initialCos =
	{
		1.f, 1.f, 1.f, 1.f
	};
	Vector3 incrementalCos = s_initialCos.v;
	for (size_t i = 0; i < c_ringSegments; i++)
	{
		Vector3 pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
		pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
		XMStoreFloat3(&verts[i].position, pos);
		XMStoreFloat4(&verts[i].color, color);
		// Standard formula to rotate a vector.
		Vector3 newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
		Vector3 newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
		incrementalCos = newCos;
		incrementalSin = newSin;
	}
	verts[c_ringSegments] = verts[0];

	m_batch_Sphere->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
	m_batch_Sphere->End();
}

void DebugDraw::DrawRay(Vector3 origin, Vector3 direction, Vector4 color, bool normalize)
{
	if (!m_states_Ray.operator bool() || !m_effect_Ray.operator bool() ||
		!m_batch_Ray.operator bool() || !m_inputLayout_Ray)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Ray) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Ray) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Ray!");
		return;
	}

	m_effect_Ray->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Ray->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Ray->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Ray->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Ray.Get());

	m_batch_Ray->Begin();

	VertexPositionColor verts[3];
	XMStoreFloat3(&verts[0].position, origin);

	Vector3 normDirection = XMVector3Normalize(direction);
	Vector3 rayDirection = (normalize) ? normDirection : direction;

	Vector3 perpVector = XMVector3Cross(normDirection, g_XMIdentityR1);

	if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
	{
		perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
	}
	perpVector = XMVector3Normalize(perpVector);

	XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, origin));
	perpVector = XMVectorScale(perpVector, 0.0625f);
	normDirection = XMVectorScale(normDirection, -0.25f);
	rayDirection = XMVectorAdd(perpVector, rayDirection);
	rayDirection = XMVectorAdd(normDirection, rayDirection);
	XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, origin));

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);

	m_batch_Ray->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
	m_batch_Ray->End();
}

void DebugDraw::DrawTriangle(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color)
{
	if (!m_states_Triangle.operator bool() || !m_effect_Triangle.operator bool() ||
		!m_batch_Triangle.operator bool() || !m_inputLayout_Triangle)
	{
#if defined (DEBUG)
		DebugTrace("DebugDraw::Draw(const Triangle) Is Failed!");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("DebugDraw::Draw(const Triangle) Is Failed!");
#endif
		Console::LogError("DebugDraw: Something is wrong with draw Debug Triangle!");
		return;
	}

	m_effect_Triangle->SetMatrices(Matrix::Identity, Application->getCamera()->GetViewMatrix(),
		Application->getCamera()->GetProjMatrix());

	Application->getDeviceContext()->OMSetBlendState(m_states_Triangle->Opaque(), nullptr, 0xFFFFFFFF);
	Application->getDeviceContext()->OMSetDepthStencilState(m_states_Triangle->DepthNone(), 0);
	Application->getDeviceContext()->RSSetState(m_raster.Get());

	m_effect_Triangle->Apply(Application->getDeviceContext());

	Application->getDeviceContext()->IASetInputLayout(m_inputLayout_Triangle.Get());

	m_batch_Triangle->Begin();

	VertexPositionColor verts[4];
	XMStoreFloat3(&verts[0].position, pointA);
	XMStoreFloat3(&verts[1].position, pointB);
	XMStoreFloat3(&verts[2].position, pointC);
	XMStoreFloat3(&verts[3].position, pointA);

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);
	XMStoreFloat4(&verts[3].color, color);

	m_batch_Triangle->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);
	m_batch_Triangle->End();
}

void DebugDraw::Release()
{
	m_states_Ray.release();
	m_states_Box.release();
	m_states_BBox.release();
	m_states_Sphere.release();
	m_states_Frustum.release();
	m_states_Grid.release();
	m_states_Triangle.release();
	m_states_Pyramid.release();

	m_effect_Ray.release();
	m_effect_Box.release();
	m_effect_BBox.release();
	m_effect_Sphere.release();
	m_effect_Frustum.release();
	m_effect_Grid.release();
	m_effect_Triangle.release();
	m_effect_Pyramid.release();

	m_batch_Ray.release();
	m_batch_Box.release();
	m_batch_BBox.release();
	m_batch_Sphere.release();
	m_batch_Frustum.release();
	m_batch_Grid.release();
	m_batch_Triangle.release();
	m_batch_Pyramid.release();
}
