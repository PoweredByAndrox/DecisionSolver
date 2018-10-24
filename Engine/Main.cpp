#include "pch.h"

#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "DXUTCamera.h"
#include "DXUTgui.h"
#include "File_system.h"


//Included for use USES_CONVERSION (or A2W, W2A)
#include <AtlConv.h>

#include <windows.h>
#include <tchar.h>

//https://ru.stackoverflow.com/questions/414690/%D0%9A%D0%B0%D0%BA-%D1%83%D0%B7%D0%BD%D0%B0%D1%82%D1%8C-%D0%B4%D0%B8%D1%80%D0%B5%D0%BA%D1%82%D0%BE%D1%80%D0%B8%D1%8E-%D0%B8%D1%81%D0%BF%D0%BE%D0%BB%D0%BD%D1%8F%D0%B5%D0%BC%D0%BE%D0%B3%D0%BE-%D1%84%D0%B0%D0%B9%D0%BB%D0%B0-%D0%B2-windows
#include <stdio.h> 
#include <direct.h>

ID3D10Effect* g_pEffect = nullptr;
ID3D10InputLayout* g_pVertexLayout = nullptr;
ID3D10EffectTechnique* g_pTechnique = nullptr;
ID3D10Buffer* g_pVertexBuffer = nullptr;
ID3D10Buffer* g_pIndexBuffer = nullptr;
ID3D10EffectMatrixVariable* g_pWorldVariable = nullptr;
ID3D10EffectMatrixVariable* g_pViewVariable = nullptr;
ID3D10EffectTechnique* g_pRenderSky = nullptr;

ID3D10Texture2D* g_pPipeTexture = nullptr;
ID3D10ShaderResourceView* g_pPipeTexRV = nullptr;
ID3D10Texture2D* g_pSkyTexture = nullptr;
ID3D10ShaderResourceView* g_pSkyTexRV = nullptr;

CDXUTSDKMesh g_Mesh;
CDXUTSDKMesh g_SkyMesh;

D3DXMATRIX g_World;
D3DXMATRIX g_View;
D3DXMATRIX g_Projection;

D3DXVECTOR3 Eye(0.0f, 3.0f, -6.0f);
D3DXVECTOR3 At(0.0f, 1.0f, 0.0f);
D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);

CFirstPersonCamera g_Camera;
CDXUTDialogResourceManager g_DialogResourceManager;
CDXUTDialog g_HUD;
float g_fFOV = 80.0f * (D3DX_PI / 180.0f);

ID3D10EffectShaderResourceVariable* g_ptxDiffuse = nullptr;
ID3D10EffectShaderResourceVariable* g_ptxNormal = nullptr;
ID3D10EffectShaderResourceVariable* g_ptxHeight = nullptr;
ID3D10EffectShaderResourceVariable* g_ptxDirt = nullptr;
ID3D10EffectShaderResourceVariable* g_ptxGrass = nullptr;
ID3D10EffectShaderResourceVariable* g_ptxMask = nullptr;
ID3D10EffectShaderResourceVariable* g_ptxShadeNormals = nullptr;

ID3D10EffectMatrixVariable* g_pmWorldViewProj = nullptr;
ID3D10EffectMatrixVariable* g_pmWorld = nullptr;

HRESULT hr = S_OK;

File_system t;

void InitApp()
{
	g_HUD.Init(&g_DialogResourceManager);
	int iY = 10;
	g_HUD.AddButton(1, L"Some-Button#1", 35, iY, 125, 22);
	g_HUD.AddButton(3, L"Some-Button#2", 35, iY += 24, 125, 22, VK_F3);
	g_HUD.AddButton(4, L"Some-Button#3", 35, iY += 24, 125, 22, VK_F2);
	g_HUD.AddStatic(5, L"SomeText#1", 100, 120, 60, 50);
	//g_Camera.SetClipToBoundary(true, &D3DXVECTOR3(4, 6, 3), &D3DXVECTOR3(1, 2, 5));
	g_Camera.SetEnableYAxisMovement(false);
	g_Camera.SetScalers(0.001f, 4.0f);
	g_Camera.SetRotateButtons(true, true, true);
}

HRESULT LoadTextureArray(ID3D10Device* pd3dDevice, vector<wstring> szTextureNames, int iNumTextures,
	ID3D10Texture2D** ppTex2D, ID3D10ShaderResourceView** ppSRV)
{
	HRESULT hr = S_OK;
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D10_TEXTURE2D_DESC));

	WCHAR str[MAX_PATH];
	for (int i = 0; i < iNumTextures; i++)
	{
		//V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, szTextureNames[i]));
		ID3D10Resource* pRes = NULL;
		D3DX10_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO));
		loadInfo.Width = D3DX_FROM_FILE;
		loadInfo.Height = D3DX_FROM_FILE;
		loadInfo.Depth = D3DX_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 1;
		loadInfo.Usage = D3D10_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = MAKE_TYPELESS(DXGI_FORMAT_R8G8B8A8_UNORM);
		loadInfo.Filter = D3DX10_FILTER_NONE;
		loadInfo.MipFilter = D3DX10_FILTER_NONE;

		//V_RETURN(
		D3DX10CreateTextureFromFile(pd3dDevice, szTextureNames[i].c_str(), &loadInfo, NULL, &pRes, NULL);
		if (pRes)
		{
			ID3D10Texture2D* pTemp;
			pRes->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&pTemp);
			pTemp->GetDesc(&desc);

			D3D10_MAPPED_TEXTURE2D mappedTex2D;

			if (desc.MipLevels > 4)
				desc.MipLevels -= 4;
			if (!(*ppTex2D))
			{
				desc.Usage = D3D10_USAGE_DEFAULT;
				desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.ArraySize = iNumTextures;
				V_RETURN(pd3dDevice->CreateTexture2D(&desc, NULL, ppTex2D));
			}

			for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
			{
				pTemp->Map(iMip, D3D10_MAP_READ, 0, &mappedTex2D);

				pd3dDevice->UpdateSubresource((*ppTex2D),
					D3D10CalcSubresource(iMip, i, desc.MipLevels),
					NULL,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					0);

				pTemp->Unmap(iMip);
			}

			SAFE_RELEASE(pRes);
			SAFE_RELEASE(pTemp);
		}
		else
		{
			return false;
		}
	}

	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = MAKE_SRGB(DXGI_FORMAT_R8G8B8A8_UNORM);
	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	SRVDesc.Texture2DArray.ArraySize = iNumTextures;
	V_RETURN(pd3dDevice->CreateShaderResourceView(*ppTex2D, &SRVDesc, ppSRV));

	return hr;
}

bool CALLBACK IsD3D10DeviceAcceptable(UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
	DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}

bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}

LPCTSTR g_szSkyTextureName[6] =
{
		t.GetResPathW(L"sky_bot.dds"),
		t.GetResPathW(L"sky_top.dds"),
		t.GetResPathW(L"sky_side.dds"),
		t.GetResPathW(L"sky_side.dds"),
		t.GetResPathW(L"sky_side.dds"),
		t.GetResPathW(L"sky_side.dds")
};

vector<wstring> g_szSkyTextures[6];


HRESULT CALLBACK OnD3D10CreateDevice(ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	// Read the D3DX effect file
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DXSHADER_DEBUG;

	UINT uFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_DEBUG;

	V_RETURN(g_DialogResourceManager.OnD3D10CreateDevice(pd3dDevice));

	V_RETURN(D3DX10CreateEffectFromFile(L"D://DecisionSolver//Engine//resource//shaders//SomeFile.fx",NULL, NULL, "fx_4_0", dwShaderFlags,
		0, pd3dDevice, NULL, NULL, &g_pEffect, NULL, NULL));

	g_pTechnique = g_pEffect->GetTechniqueByName("RenderMesh");
	g_pRenderSky = g_pEffect->GetTechniqueByName("RenderSkybox");
	g_pViewVariable = g_pEffect->GetVariableByName("g_mWorldViewProj")->AsMatrix();
	g_ptxDiffuse = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();

	// Define the input layout
	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
		&g_pVertexLayout));


	// Set the input layout
	pd3dDevice->IASetInputLayout(g_pVertexLayout);

	V_RETURN(g_Mesh.Create(pd3dDevice, t.GetResPathW(&wstring(L"tiny.sdkmesh")), true));
	V_RETURN(LoadTextureArray(pd3dDevice, t.GetResPathW(wstring(L"Tiny_skin.dds")), 1, &g_pPipeTexture, &g_pPipeTexRV));

	V_RETURN(g_SkyMesh.Create(pd3dDevice, L"D://DecisionSolver//Engine//resource//models//tiny.sdkmesh"));
	
	//for (int i = 0; i < 6; i++)
	//	g_szSkyTextures[i].push_back(g_szSkyTextureName[i]);

	//V_RETURN(
	//LoadTextureArray(pd3dDevice, t.GetResPathW(g_szSkyTextures), 6, &g_pSkyTexture, &g_pSkyTexRV)
	//);
	
	// Initialize the world matrices
	D3DXMatrixIdentity(&g_World);

	// Initialize the view matrix
	D3DXMatrixLookAtLH(&g_View, &Eye, &At, &Up);

	// Update Variables that never change
	g_pViewVariable->SetMatrix((float*)&g_View);

	// Setup the camera's view parameters
	//g_Camera.SetViewParams(&Eye, &At);
	return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	V_RETURN(g_DialogResourceManager.OnD3D10ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DX_PI / 3, fAspectRatio, 0.5f, 5000.0f);

	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera.FrameMove(fElapsedTime);
}

void CALLBACK OnD3D10FrameRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.1f, 1.f, 0.7f, 1.0f }; // red, green, blue, alpha
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	pd3dDevice->ClearRenderTargetView(pRTV, ClearColor);

	//
	// Clear the depth stencil
	//
	ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
	pd3dDevice->ClearDepthStencilView(pDSV, D3D10_CLEAR_DEPTH, 1.0, 0);

	//
    //
	// Set the Vertex Layout
	//
	pd3dDevice->IASetInputLayout(g_pVertexLayout);

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWorldViewProj;
	mProj = *g_Camera.GetProjMatrix();
	mView = *g_Camera.GetViewMatrix();

	// Render the skybox
	D3DXMATRIX mViewSkybox = mView;
	mViewSkybox._41 = 0.0f;
	mViewSkybox._42 = 0.0f;
	mViewSkybox._43 = 0.0f;
	D3DXMatrixMultiply(&mWorldViewProj, &mViewSkybox, &mProj);
	g_pViewVariable->SetMatrix((float*)&mWorldViewProj);
	g_ptxDiffuse->SetResource(g_pPipeTexRV);
	//g_SkyMesh.Render(pd3dDevice, g_pRenderSky, g_ptxDiffuse);

	// Render the Mesh
	g_ptxDiffuse->SetResource(g_pPipeTexRV);
	D3DXMatrixMultiply(&mWorldViewProj, &mView, &mProj);
	g_pViewVariable->SetMatrix((float*)&mWorldViewProj);
	g_Mesh.Render(pd3dDevice, g_pTechnique, g_ptxDiffuse);

	V(g_HUD.OnRender(fElapsedTime));
}

void CALLBACK OnD3D10ReleasingSwapChain(void* pUserContext)
{
	g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}

void CALLBACK OnD3D10DestroyDevice(void* pUserContext)
{
	g_DialogResourceManager.OnD3D10DestroyDevice();
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pEffect);

	SAFE_RELEASE(g_pPipeTexture);
	SAFE_RELEASE(g_pPipeTexRV);
	SAFE_RELEASE(g_pSkyTexture);
	SAFE_RELEASE(g_pSkyTexRV);

	g_SkyMesh.Destroy();
	g_Mesh.Destroy();
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);

	return 0;
}

void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
}

void CALLBACK OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
	bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
	int xPos, int yPos, void* pUserContext)
{
	g_Camera.SetEnablePositionMovement(true);
}

bool CALLBACK OnDeviceRemoved(void* pUserContext)
{
	return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	DXUTSetCallbackFrameMove(OnFrameMove);
	DXUTSetCallbackKeyboard(OnKeyboard);
	DXUTSetCallbackMouse(OnMouse);
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackDeviceRemoved(OnDeviceRemoved);

	DXUTSetCallbackD3D10DeviceAcceptable(IsD3D10DeviceAcceptable);
	DXUTSetCallbackD3D10DeviceCreated(OnD3D10CreateDevice);
	DXUTSetCallbackD3D10SwapChainResized(OnD3D10ResizedSwapChain);
	DXUTSetCallbackD3D10FrameRender(OnD3D10FrameRender);
	DXUTSetCallbackD3D10SwapChainReleasing(OnD3D10ReleasingSwapChain);
	DXUTSetCallbackD3D10DeviceDestroyed(OnD3D10DestroyDevice);

	InitApp();

	DXUTInit(true, true, NULL); 

	//t.GetPath();

	DXUTSetCursorSettings(true, true);
	DXUTCreateWindow(L"EngineProgram");
	DXUTCreateDevice(true, 640, 480);
	DXUTMainLoop(); 

	return DXUTGetExitCode();
}
