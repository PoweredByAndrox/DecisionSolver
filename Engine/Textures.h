#pragma once
#include "pch.h"

#ifdef _MSC_VER
#pragma once
#endif

#pragma warning(push)
#pragma warning(disable: 4005)
#include <stdint.h>
#pragma warning(pop)

HRESULT CreateWICTextureFromMemory(_In_bytecount_(wicDataSize) const uint8_t* wicData,
	_In_ size_t wicDataSize,
	_Out_opt_ ID3D11Resource** texture,
	_Out_opt_ ID3D11ShaderResourceView** textureView,
	_In_ size_t maxsize = 0);

HRESULT CreateWICTextureFromFile(_In_z_ const wchar_t* szFileName,
	_Out_opt_ ID3D11Resource** texture,
	_Out_opt_ ID3D11ShaderResourceView** textureView,
	_In_ size_t maxsize = 0);
