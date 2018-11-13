#pragma once
#include "File_system.h"
class Shaders: public File_system
{
public:
	HRESULT CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoINT, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	Shaders() {}
	~Shaders() {}

private:
	HRESULT hr = S_OK;
};

