#pragma once
#if !defined(__RENDER_BUFFER_H__)
#define __RENDER_BUFFER_H__
#include "pch.h"

#include "Shaders.h"
#include <Inc/WICTextureLoader.h>
#include "imgui.h"

class Render_Buffer
{
private:
public:
	static vector<ID3D11RasterizerState *> CreateWF(); // WireFrame

	static ID3D11Buffer *CreateVB(UINT ByteWidth, bool NeedVertice,
		D3D11_USAGE Usage, UINT CPUAccessFlags, void *vertices);
	static ID3D11Buffer *CreateIB(WORD ByteWidth, bool NeedIndices,
		D3D11_USAGE Usage, UINT CPUAccessFlags, void *indices);

	static ID3D11Buffer *CreateConstBuff(D3D11_USAGE Usage, UINT CPUAccessFlags, UINT sizeofStruct);
	static ID3D11InputLayout *CreateLayout(ID3DBlob *Buffer_blob, bool WithColor = true);
};
#endif // !__RENDER_BUFFER_H__