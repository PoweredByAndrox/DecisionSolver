#include "pch.h"

#include "DXUTCamera.h"
#include "DXUTgui.h"

#include <d3d11_1.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "File_system.h"
#include "Models.h"

#ifdef DEBUG
#include <d3d11sdklayers.h>
#endif

ID3D11InputLayout  *g_pLayout = nullptr;

ID3D11VertexShader *g_pVS = nullptr;
ID3D11PixelShader  *g_pPS = nullptr;

//ID3D11Texture2D	   *g_pPipeTexture = nullptr;
//ID3D11ShaderResourceView *g_pPipeTexRV = nullptr;

D3DXMATRIX g_World;
D3DXMATRIX g_View;
D3DXMATRIX g_Projection;

D3DXVECTOR3 Eye(0.0f, 3.0f, -6.0f);
D3DXVECTOR3 At(0.0f, 1.0f, 0.0f);
D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);

File_system t;
//Models *Model;

CFirstPersonCamera g_Camera;
CDXUTDialogResourceManager g_DialogResourceManager;
CDXUTDialog g_HUD;
float g_fFOV = 80.0f * (D3DX_PI / 180.0f);

HRESULT hr = S_OK;

static XMVECTORF32 Color = DirectX::Colors::DarkRed;

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

XMVECTORF32 _Color[9] = 
{
	DirectX::Colors::AliceBlue,
	DirectX::Colors::Black,
	DirectX::Colors::Chartreuse,
	DirectX::Colors::DarkGreen,
	DirectX::Colors::Indigo,
	DirectX::Colors::LightSteelBlue,
	DirectX::Colors::Magenta,
	DirectX::Colors::OliveDrab,
	DirectX::Colors::SkyBlue
};

struct ConstantBuffer {
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3
#define STATIC_TEXT 4

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

void InitApp()
{
	g_HUD.Init(&g_DialogResourceManager);
	g_HUD.SetCallback(OnGUIEvent);
	int iY = 10;
	g_HUD.AddButton(BUTTON_1, L"Change Buffer Color", 35, iY, 125, 22);
	g_HUD.AddButton(BUTTON_2, L"Some-Button#2", 35, iY += 24, 125, 22, VK_F3);
	g_HUD.AddButton(BUTTON_3, L"Some-Button#3", 35, iY += 24, 125, 22, VK_F2);
	g_HUD.AddStatic(STATIC_TEXT, L"SomeText#1", 100, 120, 60, 50);

	//g_Camera.SetClipToBoundary(true, &D3DXVECTOR3(4, 6, 3), &D3DXVECTOR3(1, 2, 5));
	g_Camera.SetEnableYAxisMovement(false);
	g_Camera.SetScalers(0.001f, 4.0f);
	g_Camera.SetRotateButtons(true, true, true);
}

HRESULT LoadTextureArray(ID3D11Device* pd3dDevice, vector<wstring> szTextureNames, int iNumTextures,
	ID3D11Texture2D** ppTex2D, ID3D11ShaderResourceView** ppSRV)
{
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	for (int i = 0; i < iNumTextures; i++)
	{
		//V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, szTextureNames[i]));
		ID3D11Resource* pRes = NULL;
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		loadInfo.Width = D3DX_FROM_FILE;
		loadInfo.Height = D3DX_FROM_FILE;
		loadInfo.Depth = D3DX_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 1;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = MAKE_TYPELESS(DXGI_FORMAT_R8G8B8A8_UNORM);
		loadInfo.Filter = D3DX11_FILTER_NONE;
		loadInfo.MipFilter = D3DX11_FILTER_NONE;

		//V_RETURN(
		D3DX11CreateTextureFromFile(pd3dDevice, szTextureNames[i].c_str(), &loadInfo, NULL, &pRes, NULL);
		if (pRes)
		{
			ID3D11Texture2D* pTemp;
			pRes->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&pTemp);
			pTemp->GetDesc(&desc);

			//D3D11_TEXTURE2D_DESC mappedTex2D;

			if (desc.MipLevels > 4)
				desc.MipLevels -= 4;
			if (!(*ppTex2D))
			{
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.ArraySize = iNumTextures;
				V_RETURN(pd3dDevice->CreateTexture2D(&desc, NULL, ppTex2D));
			}

			/*
			for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
			{
				pTemp->Map(iMip, D3D11_MAP_READ, 0, &mappedTex2D);

				DXUTGetD3D11DeviceContext()->UpdateSubresource((*ppTex2D),
					D3D10CalcSubresource(iMip, i, desc.MipLevels),
					NULL,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					0);

				pTemp->Unmap(iMip);
			}
			*/

			SAFE_RELEASE(pRes);
			SAFE_RELEASE(pTemp);
		}
		else
		{
			return false;
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = MAKE_SRGB(DXGI_FORMAT_R8G8B8A8_UNORM);
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	SRVDesc.Texture2DArray.ArraySize = iNumTextures;
	V_RETURN(pd3dDevice->CreateShaderResourceView(*ppTex2D, &SRVDesc, ppSRV));

	return hr;
}

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo,
	UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, 
	void* pUserContext)
{
	return true;
}

bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID,
	CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case BUTTON_1:
		Color = _Color[rand() % 9 + 1];
		break;
	}
}

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, 
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DXSHADER_DEBUG;

	UINT uFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, DXUTGetD3D11DeviceContext()));

	ID3DBlob *VS = nullptr, *PS = nullptr;
	V_RETURN(D3DCompileFromFile(t.GetResPathW(&wstring(L"VertexShader.hlsl")), 0, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", uFlags, 0, &VS, nullptr));
	
	V_RETURN(D3DCompileFromFile(t.GetResPathW(&wstring(L"PixelShader.hlsl")), 0,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_4_0", uFlags, 0, &PS, nullptr));

	V_RETURN(pd3dDevice->CreateVertexShader(VS->GetBufferPointer(),
		VS->GetBufferSize(), NULL, &g_pVS));

	V_RETURN(pd3dDevice->CreatePixelShader(PS->GetBufferPointer(),
		PS->GetBufferSize(), NULL, &g_pPS));
	
	DXUT_SetDebugName(g_pVS, "VS");
	DXUT_SetDebugName(g_pPS, "PS");

	// Define the input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	//// Create the input layout
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, VS->GetBufferPointer(),
		VS->GetBufferSize(), &g_pLayout));
	DXUT_SetDebugName(g_pLayout, "Vertices Shader");

	//Model = new Models;
	//if (!Model->Load(t.GetResPathA(&string("New.obj"))))
	//	t.GetPath();

	// Initialize the world matrices
	D3DXMatrixIdentity(&g_World);

	// Initialize the view matrix
	D3DXMatrixLookAtLH(&g_View, &Eye, &At, &Up);

	//// Setup the camera's view parameters
    g_Camera.SetViewParams(XMVectorSet(0.f, 0.f, -300.f, 0.f), XMVectorSet(10.0f, 20.0f, 0.0f, 0.f));

	SAFE_RELEASE(VS);
	SAFE_RELEASE(PS);

	return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DX_PI / 3, fAspectRatio, 0.5f, 5000.0f);

	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, 
	void* pUserContext)
{
	g_Camera.FrameMove(fElapsedTime);
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{
	//
	// Clear the back buffer
	//
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Color);

	//
	// Clear the depth stencil
	//
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	//
	// Set the Vertex Layout
	//
	pd3dImmediateContext->IASetInputLayout(g_pLayout);

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWorldViewProj;

	//Model->Draw();
	V(g_HUD.OnRender(fElapsedTime));

#ifdef DEBUG
	ID3D11Debug* debug = 0;
	pd3dDevice->QueryInterface(IID_ID3D11Debug, (void**)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_RELEASE(g_pLayout);
	SAFE_RELEASE(g_pVS);
	SAFE_RELEASE(g_pPS);

//	Model->Close();
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
	try
	{
		DXUTSetCallbackFrameMove(OnFrameMove);
		DXUTSetCallbackKeyboard(OnKeyboard);
		DXUTSetCallbackMouse(OnMouse);
		DXUTSetCallbackMsgProc(MsgProc);
		DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
		DXUTSetCallbackDeviceRemoved(OnDeviceRemoved);

		DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
		DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
		DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
		DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
		DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
		DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);

		InitApp();

		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		DXUTInit(true, true, NULL);

		DXUTSetCursorSettings(true, true);
		DXUTCreateWindow(L"EngineProgram");
		DXUTCreateDevice(D3D_FEATURE_LEVEL_9_2, true, SCREEN_WIDTH, SCREEN_HEIGHT);
		DXUTMainLoop();
	}

	catch (const std::exception	&ex)
	{
		MessageBoxA(DXUTGetHWND(), string("Log initialization failed: " + *ex.what()).c_str(), "Error", MB_OK);
	}
	return DXUTGetExitCode();
}
