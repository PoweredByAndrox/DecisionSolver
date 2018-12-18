#include "pch.h"
#include "Shaders.h"

#include "d3dx9effect.h"

HRESULT Engine::Shaders::CompileShaderFromFile(wstring *szFileName, string *szEntryPoint, string *szShaderModel, ID3DBlob **ppBlobOut)
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ThrowIfFailed(result = D3DX11CompileFromFileW(szFileName->c_str(), NULL, NULL, 
		szEntryPoint->c_str(), szShaderModel->c_str(), dwShaderFlags, NULL, NULL, ppBlobOut, &pErrorBlob, NULL));
	if (FAILED(result))
	{
		if (pErrorBlob != NULL)
#ifdef DEBUG
		{
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
			throw exception(strcat((char *)"Shaders: line 17 return: ", (char *)pErrorBlob->GetBufferPointer()));
#elif !defined(DEBUG)
			MessageBoxA(DXUTGetHWND(),
				string(string("Shader compiller is failed with text:\n") +
					string((char *)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
		}
		SAFE_RELEASE(pErrorBlob);
		return result;
	}
	SAFE_RELEASE(pErrorBlob);
	return S_OK;
}

vector<void *> Engine::Shaders::CompileShaderFromFile(vector<ID3DBlob *> Things)
{
	vector<void *> ppBlobOut;
	ID3D11VertexShader *m_vertexShader = nullptr;
	ID3D11PixelShader *m_pixelShader = nullptr;

	GetD3DDevice();
	Device->CreateVertexShader(Things.at(0)->GetBufferPointer(), Things.at(0)->GetBufferSize(), NULL, &m_vertexShader);
	Device->CreatePixelShader(Things.at(1)->GetBufferPointer(), Things.at(1)->GetBufferSize(), NULL, &m_pixelShader);

	ppBlobOut.clear();
	ppBlobOut.push_back(m_vertexShader);
	ppBlobOut.push_back(m_pixelShader);

	return ppBlobOut;
}

vector<ID3DBlob *> Engine::Shaders::CreateShaderFromFile(vector<wstring> szFileName, vector<string> szEntryPoint, vector<string> szShaderModel)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	vector<ID3DBlob *> ppBlobOut;
	ID3DBlob *Cache = nullptr;
	ThrowIfFailed(result = D3DX11CompileFromFileW(szFileName.at(0).c_str(), NULL, NULL,
		szEntryPoint.at(0).c_str(), szShaderModel.at(0).c_str(), dwShaderFlags, NULL, NULL, &Cache, &pErrorBlob, NULL));
	if (FAILED(result))
	{
		if (pErrorBlob != NULL)
#ifdef DEBUG
		{
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
			throw exception(strcat((char *)"Shaders: line 67 return: ", (char *)pErrorBlob->GetBufferPointer()));
#elif !defined(DEBUG)
			MessageBoxA(DXUTGetHWND(),
				string(string("Shader compiller is failed with text:\n") +
					string((char *)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
		}
	SAFE_RELEASE(pErrorBlob);
	}
	SAFE_RELEASE(pErrorBlob);
	ppBlobOut.push_back(Cache);

	ThrowIfFailed(result = D3DX11CompileFromFileW(szFileName.at(1).c_str(), NULL, NULL,
		szEntryPoint.at(1).c_str(), szShaderModel.at(1).c_str(), dwShaderFlags, NULL, NULL, &Cache, &pErrorBlob, NULL));
	if (FAILED(result))
	{
		if (pErrorBlob != NULL)
#ifdef DEBUG
		{
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
			throw exception(strcat((char *)"Shaders: line 88 return: ", (char *)pErrorBlob->GetBufferPointer()));
#elif !defined(DEBUG)
			MessageBoxA(DXUTGetHWND(),
				string(string("Shader compiller is failed with text:\n") +
					string((char *)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
		}
	SAFE_RELEASE(pErrorBlob);
	}
	SAFE_RELEASE(pErrorBlob);
	ppBlobOut.push_back(Cache);

	return ppBlobOut;
}
