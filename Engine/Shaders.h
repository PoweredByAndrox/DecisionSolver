#pragma once
#ifndef __SHADERS_H__
#define __SHADERS_H__
#include "pch.h"

class Shaders
{
public:
	static HRESULT CompileShaderFromFile(string FileName, string FunctionName, string VersionShader,
		ID3DBlob **ppBlobOut);
	static vector<void *> CompileShaderFromFile(vector<ID3DBlob *> Things);
	static vector<ID3DBlob *> CreateShaderFromFile(vector<string> FileName, vector<string> FunctionName,
		vector<string> VersionShader, DWORD ShaderFlags = 0);

private:
	static HRESULT result;

	static ID3DBlob *pErrorBlob;
};
#endif // !__SHADERS_H__