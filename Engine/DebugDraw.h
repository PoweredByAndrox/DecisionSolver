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
	void MainRender(Matrix View, Matrix Proj);

	void AddTriangle(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color);
	void AddBox(Vector3 Pos, Vector3 Size, Vector4 color);

	//void DrawCube(const BoundingBox &box, Vector4 color);

	//void Draw(const BoundingSphere &sphere, Vector4 color);

	//void Draw(const BoundingBox &box, Vector4 color);

	//void Draw(const BoundingOrientedBox &obb, Vector4 color);

	//void Draw(const BoundingFrustum &frustum, Vector4 color);

	//void DrawGrid(Vector3 xAxis, Vector3 yAxis, Vector3 origin, float DisCells, Vector4 color);

	//void DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis, Vector4 color);

	//void DrawRay(Vector3 origin, Vector3 direction, Vector4 color, bool normalize = true);

	//void DrawTriangle(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color);

	//void DrawPyramid(Vector4 color = Vector4::Zero);

	//void Release();

private:
	class BaseRender
	{
	protected:
		struct SimpleVertex
		{
			SimpleVertex(Vector3 Pos, Vector4 Color) : Pos(Pos), Color(Color) {}
			Vector3 Pos;
			Vector4 Color;
		};

	public:
		void Init(vector<SimpleVertex> verts, vector<WORD> s_indices);
		void Render();
		void Release();

		void SetPos(Vector3 Pos) {}
		void SetRot(Vector3 Rot) {}
		void SetScale(Vector3 Scl) {}

	protected:
		struct ConstantBuffer
		{
			Matrix World, View, Proj;
		};

		Vector3 Pos = Vector3::Zero, Rot = Vector3::Zero, Scl = Vector3::Zero;

		ID3D11VertexShader *g_pVertexShader = nullptr;
		ID3D11PixelShader *g_pPixelShader = nullptr;
		ID3D11InputLayout *g_pVertexLayout = nullptr;
		ID3D11Buffer *g_pVertexBuffer = nullptr, *g_pIndexBuffer = nullptr, *g_pConstantBuffer = nullptr;

		UINT SizeVrtx = 0ul;
	};

	/*
	class Cobe: public BaseRender
	{
	public:

	private:

	};
	*/

	class TriangleDraw: public BaseRender
	{
	public:
		void Draw(Matrix View, Matrix Proj);
		void Init(Vector3 pointA, Vector3 pointB, Vector3 pointC, Vector4 color);
	};

	class Box: public BaseRender
	{
	public:
		void Draw(Matrix View, Matrix Proj);
		void Init(BoundingBox Box, Vector4 color);
	
	private:
		BoundingBox _Box;
	};

	vector<shared_ptr<TriangleDraw>> triangle;
	vector<shared_ptr<Box>> boxes;
};