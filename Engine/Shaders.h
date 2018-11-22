#pragma once
#ifndef __SHADERS_H__
#define __SHADERS_H__
#include "pch.h"

#include "File_system.h"


class Shaders
{
public:
	HRESULT CompileShaderFromFile(wstring *szFileName, string *szEntryPoint, string *szShaderModel, ID3DBlob **ppBlobOut);
	vector<ID3DBlob> *CompileShaderFromFile(vector<wstring> *szFileName, vector<string> *szEntryPoint,
		vector<string> *szShaderModel);

	Shaders() {}
	~Shaders() {}

private:
	HRESULT result = S_OK;

	ID3DBlob *pErrorBlob = nullptr;

	ID3D11Device *Device = nullptr;
	ID3D11DeviceContext *DevCon = nullptr;

	void getD3DDevice() { Device = DXUTGetD3D11Device(); }
	void getD3DDeviceContext() { DevCon = DXUTGetD3D11DeviceContext(); }
};

#endif // !__SHADERS_H__