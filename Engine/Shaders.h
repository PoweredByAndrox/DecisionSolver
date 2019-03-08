#pragma once
#ifndef __SHADERS_H__
#define __SHADERS_H__
#include "pch.h"

#include "File_system.h"
class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

class Shaders
{
public:
	HRESULT CompileShaderFromFile(wstring *FileName, string *FunctionName, string *VersionShader, ID3DBlob **ppBlobOut);
	vector<void *> CompileShaderFromFile(vector<ID3DBlob *> Things);
	vector<ID3DBlob *> CreateShaderFromFile(vector<wstring> FileName, vector<string> FunctionName, vector<string> VersionShader, DWORD ShaderFlags = 0);

	Shaders() {}
	~Shaders() {}

private:
	HRESULT result = S_OK;

	ID3DBlob *pErrorBlob = nullptr;
};
#endif // !__SHADERS_H__