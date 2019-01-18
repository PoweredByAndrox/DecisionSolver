#pragma once
#ifndef __SHADERS_H__
#define __SHADERS_H__
#include "pch.h"

#include "File_system.h"

namespace Engine
{
	class Shaders
	{
	public:
		HRESULT CompileShaderFromFile(wstring *FileName, string *FunctionName, string *VersionShader, ID3DBlob **ppBlobOut);
		vector<void *> CompileShaderFromFile(vector<ID3DBlob *> Things);
		vector<ID3DBlob *> CreateShaderFromFile(vector<wstring> FileName, vector<string> FunctionName, vector<string> VersionShader, DWORD ShaderFlags = 0);

		void Release()
		{
			SAFE_RELEASE(Device); 
			if (DeviceCon)
			{
				DeviceCon->ClearState();
				DeviceCon->Flush();
				SAFE_DELETE(DeviceCon);
			}
		}

		Shaders() {}
		~Shaders() {}

	private:
		HRESULT result = S_OK;

		ID3DBlob *pErrorBlob = nullptr;

		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }
	};
};

#endif // !__SHADERS_H__