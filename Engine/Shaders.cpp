#include "pch.h"

#include "Shaders.h"
#include "d3dx9effect.h"

HRESULT Engine::Shaders::CompileShaderFromFile(wstring *FileName, string *FunctionName, string *VersionShader, ID3DBlob **ppBlobOut)
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ThrowIfFailed(result = D3DX11CompileFromFileW(FileName->c_str(), NULL, NULL, 
		FunctionName->c_str(), VersionShader->c_str(), dwShaderFlags, NULL, NULL, ppBlobOut, &pErrorBlob, NULL));
	if (FAILED(result))
	{
		if (pErrorBlob != NULL)
		{
#ifdef DEBUG
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
			throw exception(strcat((char *)"Shaders->CompileShaderFromFile: D3DX11CompileFromFileW == E_FAILED!\n return: ",
				(char *)pErrorBlob->GetBufferPointer()));
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
	ID3D11PixelShader *m_pixelShader2 = nullptr;

	GetD3DDevice();
	Device->CreateVertexShader(Things.at(0)->GetBufferPointer(), Things.at(0)->GetBufferSize(), NULL, &m_vertexShader);
	Device->CreatePixelShader(Things.at(1)->GetBufferPointer(), Things.at(1)->GetBufferSize(), NULL, &m_pixelShader);
	if (Things.size() == 3)
		Device->CreatePixelShader(Things.at(2)->GetBufferPointer(), Things.at(2)->GetBufferSize(), NULL, &m_pixelShader2);

	ppBlobOut.push_back(m_vertexShader);
	ppBlobOut.push_back(m_pixelShader);
	if (m_pixelShader2)
		ppBlobOut.push_back(m_pixelShader2);

	return ppBlobOut;
}

vector<ID3DBlob *> Engine::Shaders::CreateShaderFromFile(vector<wstring> FileName, vector<string> FunctionName, vector<string> VersionShader, DWORD ShaderFlags)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	dwShaderFlags |= ShaderFlags;

	vector<ID3DBlob *> ppBlobOut;
	ID3DBlob *Cache = nullptr;

	int AllSize = FileName.size() + FunctionName.size() + VersionShader.size();
	if ((FileName.size() + FunctionName.size()+ VersionShader.size()) == AllSize)
	{
		for (int i = 0; i < FileName.size(); i++)
		{
			D3DCompileFromFile(FileName.at(i).c_str(), nullptr, nullptr, FunctionName.at(i).c_str(), VersionShader.at(i).c_str(),
				D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &Cache, &pErrorBlob);
			if (FAILED(result))
			{
				if (pErrorBlob != NULL)
				{
#ifdef DEBUG
					OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
					throw exception(strcat((char *)"Shaders->CreateShaderFromFile: D3DX11CompileFromFileW == E_FAILED!\n return: ",
						(char *)pErrorBlob->GetBufferPointer()));
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
		}
	}
	else
	{
		DebugTrace("Saders->CreateShaderFromFile: It's necessary to 3 arguments are equal!\n");
		throw exception("CreateShaderFromFile!!!");
	}
	return ppBlobOut;
}
