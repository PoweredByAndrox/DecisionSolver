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

void DebugDraw::DrawCube(PrimitiveBatch<VertexPositionColor> *batch, Matrix matWorld, Vector4 color)
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

	batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), &verts[0], 8);
}

void DebugDraw::Draw(PrimitiveBatch<VertexPositionColor> *batch,
	const BoundingSphere &sphere,
	Vector4 color)
{
	Vector3 origin = XMLoadFloat3(&sphere.Center);

	const float radius = sphere.Radius;

	Vector3 xaxis = g_XMIdentityR0 * radius;
	Vector3 yaxis = g_XMIdentityR1 * radius;
	Vector3 zaxis = g_XMIdentityR2 * radius;

	DrawRing(batch, origin, xaxis, zaxis, color);
	DrawRing(batch, origin, xaxis, yaxis, color);
	DrawRing(batch, origin, yaxis, zaxis, color);
}

void DebugDraw::Draw(PrimitiveBatch<VertexPositionColor> *batch,
	const BoundingBox &box,
	Vector4 color)
{
	DrawCube(batch, Matrix::CreateScale(Vector3(box.Extents)) * Matrix::CreateTranslation(box.Center), color);
}

void DebugDraw::Draw(PrimitiveBatch<VertexPositionColor> *batch,
	const BoundingOrientedBox &obb,
	Vector4 color)
{
	DrawCube(batch, Matrix::CreateScale(Vector3(obb.Extents)) * Matrix::CreateTranslation(obb.Center), color);
}

void DebugDraw::Draw(PrimitiveBatch<VertexPositionColor> *batch,
	const BoundingFrustum &frustum,
	Vector4 color)
{
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

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, _countof(verts));
}

void DebugDraw::DrawGrid(PrimitiveBatch<VertexPositionColor> *batch,
	Vector3 xAxis,
	Vector3 yAxis,
	Vector3 origin,
	size_t xdivs,
	size_t ydivs,
	Vector4 color)
{
	xdivs = std::max<size_t>(1, xdivs);
	ydivs = std::max<size_t>(1, ydivs);

	for (size_t i = 0; i <= xdivs; ++i)
	{
		float percent = float(i) / float(xdivs);
		percent = (percent * 2.f) - 1.f;
		Vector3 scale = XMVectorScale(xAxis, percent);
		scale = XMVectorAdd(scale, origin);

		VertexPositionColor v1(XMVectorSubtract(scale, yAxis), color);
		VertexPositionColor v2(XMVectorAdd(scale, yAxis), color);
		batch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= ydivs; i++)
	{
		float percent = float(i) / float(ydivs);
		percent = (percent * 2.f) - 1.f;
		Vector3 scale = XMVectorScale(yAxis, percent);
		scale = XMVectorAdd(scale, origin);

		VertexPositionColor v1(XMVectorSubtract(scale, xAxis), color);
		VertexPositionColor v2(XMVectorAdd(scale, xAxis), color);
		batch->DrawLine(v1, v2);
	}
}

void DebugDraw::DrawRing(PrimitiveBatch<VertexPositionColor> *batch,
	Vector3 origin,
	Vector3 majorAxis,
	Vector3 minorAxis,
	Vector4 color)
{
	static const size_t c_ringSegments = 32;

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

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
}

void DebugDraw::DrawRay(PrimitiveBatch<VertexPositionColor> *batch,
	Vector3 origin,
	Vector3 direction,
	Vector4 color,
	bool normalize)
{
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

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
}

void DebugDraw::DrawTriangle(PrimitiveBatch<VertexPositionColor> *batch,
	Vector3 pointA,
	Vector3 pointB,
	Vector3 pointC,
	Vector4 color)
{
	VertexPositionColor verts[4];
	XMStoreFloat3(&verts[0].position, pointA);
	XMStoreFloat3(&verts[1].position, pointB);
	XMStoreFloat3(&verts[2].position, pointC);
	XMStoreFloat3(&verts[3].position, pointA);

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);
	XMStoreFloat4(&verts[3].color, color);

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);
}