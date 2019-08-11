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

#include "Inc/PrimitiveBatch.h"
#include "Inc/VertexTypes.h"
#include "Inc/CommonStates.h"
#include "Inc/Effects.h"

#include <wrl.h>

class DebugDraw
{
public:
	DebugDraw() {}
	~DebugDraw() { Release(); }

	void Init();

	inline void DrawCube(Matrix matWorld, Vector4 color, bool BBox);

	void Draw(const BoundingSphere &sphere, Vector4 color);

	void Draw(const BoundingBox &box, Vector4 color);

	void Draw(const BoundingOrientedBox &obb, Vector4 color);

	void Draw(const BoundingFrustum &frustum, Vector4 color);

	void DrawGrid(Vector3 xAxis, Vector3 yAxis, Vector3 origin, float DisCells, Vector4 color);

	void DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis, Vector4 color);

	void DrawRay(Vector3 origin, Vector3 direction, Vector4 color, bool normalize = true);

	void DrawTriangle(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color);

	void DrawPyramid(Matrix matWorld = Matrix::Identity, Vector4 color = Vector4::Zero);

	void Release();

private:
	unique_ptr<CommonStates> m_states_Ray, m_states_Box, m_states_BBox,
		m_states_Sphere, m_states_Frustum, m_states_Grid, m_states_Triangle,
		m_states_Pyramid;

	unique_ptr<BasicEffect> m_effect_Ray, m_effect_Box, m_effect_BBox,
		m_effect_Sphere, m_effect_Frustum, m_effect_Grid, m_effect_Triangle,
		m_effect_Pyramid;

	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_batch_Ray, m_batch_Box, m_batch_BBox,
		m_batch_Sphere, m_batch_Frustum, m_batch_Grid, m_batch_Triangle,
		m_batch_Pyramid;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout_Ray, m_inputLayout_Box, 
		m_inputLayout_BBox, m_inputLayout_Sphere, m_inputLayout_Frustum,
		m_inputLayout_Grid, m_inputLayout_Triangle,
		m_inputLayout_Pyramid;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;
};