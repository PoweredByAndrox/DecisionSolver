#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "Shaders.h"
#include "d3dx9effect.h"
#include "File_system.h"

HRESULT Shaders::CompileShaderFromFile(wstring *FileName, string *FunctionName, string *VersionShader, ID3DBlob **ppBlobOut)
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG)
	dwShaderFlags |= D3DXSHADER_DEBUG;
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ThrowIfFailed(result = D3DX11CompileFromFileW(FileName->c_str(), NULL, NULL, 
		FunctionName->c_str(), VersionShader->c_str(), dwShaderFlags, NULL, NULL, ppBlobOut, &pErrorBlob, NULL));
	if (FAILED(result))
	{
		if (pErrorBlob)
		{
#if defined(DEBUG)
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
			throw exception(string(string("Shaders->CompileShaderFromFile: D3DX11CompileFromFileW == E_FAILED!\nreturn: ") +
				string((char *)pErrorBlob->GetBufferPointer())).c_str());
#elif !defined(DEBUG)
			MessageBoxA(Application->GetHWND(),
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

vector<void *> Shaders::CompileShaderFromFile(vector<ID3DBlob *> Things)
{
	vector<void *> ppBlobOut;
	ID3D11VertexShader *m_vertexShader = nullptr;
	ID3D11PixelShader *m_pixelShader = nullptr;
	ID3D11PixelShader *m_pixelShader2 = nullptr;

	Application->getDevice()->CreateVertexShader(Things.at(0)->GetBufferPointer(), Things.at(0)->GetBufferSize(), NULL, &m_vertexShader);
	Application->getDevice()->CreatePixelShader(Things.at(1)->GetBufferPointer(), Things.at(1)->GetBufferSize(), NULL, &m_pixelShader);
	if (Things.size() == 3)
		Application->getDevice()->CreatePixelShader(Things.at(2)->GetBufferPointer(), Things.at(2)->GetBufferSize(), NULL, &m_pixelShader2);

	ppBlobOut.push_back(m_vertexShader);
	ppBlobOut.push_back(m_pixelShader);
	if (m_pixelShader2)
		ppBlobOut.push_back(m_pixelShader2);

	return ppBlobOut;
}

vector<ID3DBlob *> Shaders::CreateShaderFromFile(vector<wstring> FileName, vector<string> FunctionName, vector<string> VersionShader, DWORD ShaderFlags)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	dwShaderFlags |= ShaderFlags;

	vector<ID3DBlob *> ppBlobOut;
	ID3DBlob *Cache = nullptr;

	if ((FileName.size() == 3) & (FunctionName.size() == 3) & (VersionShader.size() == 3)
		|| (FileName.size() == 2) & (FunctionName.size() == 2) & (VersionShader.size() == 2))
	{
		for (size_t i = 0; i < FileName.size(); i++)
		{
			result = D3DX11CompileFromFileW(FileName.at(i).c_str(), nullptr, nullptr, FunctionName.at(i).c_str(),
				VersionShader.at(i).c_str(), dwShaderFlags, 0, nullptr, &Cache, &pErrorBlob, &result);
			if (FAILED(result))
			{
				if (pErrorBlob)
				{
#if defined(DEBUG)
					OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
					throw exception(string(string("Shaders->CreateShaderFromFile: D3DX11CompileFromFileW == E_FAILED!\nreturn: ") +
						string((char *)pErrorBlob->GetBufferPointer())).c_str());
#elif !defined(DEBUG)
					MessageBoxA(Application->GetHWND(),
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
		DebugTrace("Saders->CreateShaderFromFile() It's necessary to 3 arguments are equal!\n");
		throw exception("CreateShaderFromFile!!!");
	}
	return ppBlobOut;
}
