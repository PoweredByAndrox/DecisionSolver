#include "pch.h"
#include "Shaders.h"

HRESULT Shaders::CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoINT, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	ThrowIfFailed(hr = D3DX11CompileFromFile(GetResPathW(szFileName).data()->c_str(), NULL, NULL, szEntryPoINT, szShaderModel,
		dwShaderFlags, NULL, NULL, ppBlobOut, &pErrorBlob, NULL));
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
#ifdef DEBUG
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
#elif !defined(DEBUG)
			MessageBoxA(DXUTGetHWND(),
				string(string("Shader compiller is failed with text: ") +
					string((char*)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
		SAFE_RELEASE(pErrorBlob);
		return hr;
	}
	SAFE_RELEASE(pErrorBlob);
	return S_OK;
}
