#include "pch.h"
#include "Shaders.h"
#include "Models.h"

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
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			throw exception(strcat((char*)"Shaders: line 17 return: ", (char*)pErrorBlob->GetBufferPointer()));
#elif !defined(DEBUG)
			MessageBoxA(DXUTGetHWND(),
				string(string("Shader compiller is failed with text:\n") +
					string((char*)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
		}
		SAFE_RELEASE(pErrorBlob);
		return result;
	}
	SAFE_RELEASE(pErrorBlob);
	return S_OK;
}

vector<ID3DBlob> *Engine::Shaders::CompileShaderFromFile(vector<wstring> *szFileName, vector<string> *szEntryPoint, vector<string> *szShaderModel)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	if(szFileName->size() != szEntryPoint->size() != szShaderModel->size())
	{
		return nullptr;
	}

	ID3DBlob *ppBlobOut;

	for (int i = 0; i < szFileName->size(); i++)
	{
		for (int i_1 = 0; i_1 < szEntryPoint->size(); i_1++)
		{
			for (int i_2 = 0; i_2 < szShaderModel->size(); i_2++)
			{
				ThrowIfFailed(result = D3DX11CompileFromFileW(szFileName->at(i).c_str(), NULL, NULL, 
					szEntryPoint->at(i_1).c_str(), szShaderModel->at(i_2).c_str(), dwShaderFlags, NULL, NULL, &ppBlobOut, &pErrorBlob, NULL));
				if (FAILED(result))
				{
					if (pErrorBlob != NULL)
#ifdef DEBUG
					{
						OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
						throw exception(strcat((char*)"Shaders: line 60 return: ", (char*)pErrorBlob->GetBufferPointer()));
#elif !defined(DEBUG)
						MessageBoxA(DXUTGetHWND(),
							string(string("Shader compiller is failed with text:\n") +
								string((char*)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
					}
				SAFE_RELEASE(pErrorBlob);
				return nullptr;
				}
			SAFE_RELEASE(pErrorBlob);
			}
		}
	}
	return S_OK;
}