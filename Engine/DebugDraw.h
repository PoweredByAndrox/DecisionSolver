//--------------------------------------------------------------------------------------
// File: DebugDraw.h
//
// Helpers for drawing various debug shapes using PrimitiveBatch
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once

#include <DirectXCollision.h>

#include "PrimitiveBatch.h"
#include "VertexTypes.h"

class DebugDraw
{
public:
	DebugDraw() {}
	~DebugDraw() {}

	inline void DrawCube(PrimitiveBatch<VertexPositionColor> *batch, Matrix matWorld, Vector4 color);

	void Draw(PrimitiveBatch<VertexPositionColor> *batch,
		const BoundingSphere &sphere, Vector4 color);

	void Draw(PrimitiveBatch<VertexPositionColor> *batch,
		const BoundingBox &box, Vector4 color);

	void Draw(PrimitiveBatch<VertexPositionColor> *batch,
		const BoundingOrientedBox &obb, Vector4 color);

	void Draw(PrimitiveBatch<VertexPositionColor> *batch,
		const BoundingFrustum &frustum, Vector4 color);

	void DrawGrid(PrimitiveBatch<VertexPositionColor> *batch,
		Vector3 xAxis, Vector3 yAxis,
		Vector3 origin, size_t xdivs, size_t ydivs, Vector4 color);

	void DrawRing(PrimitiveBatch<VertexPositionColor> *batch,
		Vector3 origin, Vector3 majorAxis, Vector3 minorAxis, Vector4 color);

	void DrawRay(PrimitiveBatch<VertexPositionColor> *batch,
		Vector3 origin, Vector3 direction, Vector4 color, bool normalize = true);

	void DrawTriangle(PrimitiveBatch<VertexPositionColor> *batch,
		Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color);
};