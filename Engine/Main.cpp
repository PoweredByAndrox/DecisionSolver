#include "pch.h"

#include "DXUTCamera.h"
#include "DXUTgui.h"
#include "SDKmesh.h"

#include <d3d11_1.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "File_system.h"

#if defined(Never)
#include "Models.h"
#endif

#include "Physics.h"

#ifdef DEBUG
#include <d3d11sdklayers.h>
#endif
#include "d3dx11effect.h"
#pragma comment(lib, "Effects11d.lib")

ID3D11InputLayout  *g_pLayout = nullptr;

ID3D11VertexShader *g_pVS = nullptr;
ID3D11PixelShader  *g_pPS = nullptr;

D3DXVECTOR3 Eye(0.0f, 3.0f, -6.0f);
D3DXVECTOR3 At(0.0f, 1.0f, 0.0f);
D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);

auto file_system = make_unique<File_system>();

#if defined(Never)
auto Model = make_unique<Models>();
#endif

auto PhysX = make_unique<Physics>();

CFirstPersonCamera g_Camera;
CDXUTDialogResourceManager g_DialogResourceManager;
CDXUTDialog g_HUD;
float g_fFOV = 80.0f * (D3DX_PI / 180.0f);

HRESULT hr = S_OK;

static XMVECTORF32 Color = DirectX::Colors::DarkRed;

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

struct ConstantBuffer 
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

ID3D11SamplerState *TexSamplerState;
ID3D11Buffer *pConstantBuffer;

#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3
#define STATIC_TEXT 4
#define EDITBOX 5
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int iY = 10;

ID3D11InputLayout* g_pVertexLayout = NULL;

CDXUTSDKMesh g_Mesh;
D3DXMATRIX g_World;
D3DXMATRIX g_View;
D3DXMATRIX g_Projection;

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

void InitApp()
{
	g_HUD.Init(&g_DialogResourceManager);
	g_HUD.SetCallback(OnGUIEvent);
	
	g_HUD.AddButton(BUTTON_1, L"Change Buffer Color", 35, iY, 125, 22);
	g_HUD.AddButton(BUTTON_2, L"Do torque a phys box", 35, iY += 24, 125, 22, VK_F3);
	g_HUD.AddButton(BUTTON_3, L"Some-Button#3", 35, iY += 24, 125, 22, VK_F2);

	g_HUD.AddStatic(STATIC_TEXT, L"SomeText#1", 35, 90, 60, 50);

	//g_Camera.SetClipToBoundary(true, &D3DXVECTOR3(4, 6, 3), &D3DXVECTOR3(1, 2, 5));
	//g_Camera.SetEnableYAxisMovement(false);
	g_Camera.SetScalers(0.010f, 6.0f);
	g_Camera.SetRotateButtons(true, true, true, true);

	PhysX->Init();
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
	WCHAR wszOutput[1024];
	switch (nControlID)
	{
	case BUTTON_1:
		Color = _Color[rand() % 9 + 1];
		break;
	case BUTTON_2:
		PhysX->AddTorque(*PhysX->GetPhysObject().data(), PxVec3(10, 45, 33), PxForceMode::Enum::eIMPULSE);
		break;
	}
}

	// Need to move in shader class!!!
HRESULT CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoINT, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	if (FAILED(hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoINT, szShaderModel,
		dwShaderFlags, NULL, NULL, ppBlobOut, &pErrorBlob, NULL)))
	{
		if (pErrorBlob != NULL)
#ifdef DEBUG
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer()); 
#elif !defined(DEBUG)
			MessageBoxA(DXUTGetHWND(), 
				string(string("Error in 220. Shader compiller is failed with text: ") + 
				string((char*)pErrorBlob->GetBufferPointer())).c_str(), "Error log", MB_OK);
#endif
		SAFE_RELEASE(pErrorBlob);
		return hr;
	}
	SAFE_RELEASE(pErrorBlob);
	return S_OK;
}

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, 
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{

	InitApp();

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS;
	
#ifdef DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, DXUTGetD3D11DeviceContext()));

	ID3DBlob *VS = nullptr, *PS = nullptr;
	V_RETURN(D3DCompileFromFile(file_system->GetResPathW(&wstring(L"VertexShader.hlsl")), 0,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", dwShaderFlags, 0, &VS, nullptr));

	V_RETURN(D3DCompileFromFile(file_system->GetResPathW(&wstring(L"PixelShader.hlsl")), 0,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_4_0", dwShaderFlags, 0, &PS, nullptr));

	V_RETURN(pd3dDevice->CreateVertexShader(VS->GetBufferPointer(),
		VS->GetBufferSize(), NULL, &g_pVS));

	V_RETURN(pd3dDevice->CreatePixelShader(PS->GetBufferPointer(),
		PS->GetBufferSize(), NULL, &g_pPS));

#ifdef DEBUG
	DXUT_SetDebugName(g_pVS, "VS");
	DXUT_SetDebugName(g_pPS, "PS");
#endif

	// Define the input layout
	const D3D11_INPUT_ELEMENT_DESC layout1[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(layout1) / sizeof(layout1[0]);
	//// Create the input layout
	V_RETURN(pd3dDevice->CreateInputLayout(layout1, numElements, VS->GetBufferPointer(),
		VS->GetBufferSize(), &g_pLayout));

#ifdef DEBUG
	DXUT_SetDebugName(g_pLayout, "Vertices Shader");
#endif

#if defined(Never)
	Model = new Models;
	if (!Model->Load(file_system->GetResPathA(&string("New.obj"))))
		file_system->GetPath(); // TEST FUNC!
#endif

	// Initialize the world matrices
	D3DXMatrixIdentity(&g_World);

	// Initialize the view matrix
	D3DXMatrixLookAtLH(&g_View, &Eye, &At, &Up);

	// Setup the camera's view parameters
    //g_Camera.SetViewParams(XMVectorSet(Eye.x, Eye.y, Eye.z, 1.f), XMVectorSet(At.x, At.y, At.z, 1.f));
	SAFE_RELEASE(VS);
	SAFE_RELEASE(PS);
	
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	V_RETURN(pd3dDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer));

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	V_RETURN(pd3dDevice->CreateSamplerState(&sampDesc, &TexSamplerState));

	V_RETURN(g_Mesh.Create(pd3dDevice, file_system->GetResPathW(&wstring(L"tiny.sdkmesh"))));
	
	return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DX_PI / 3, fAspectRatio, 0.5f, 5000.0f);
	int X = pBackBufferSurfaceDesc->Width - 170;
	int Y = 15;
	g_HUD.GetButton(1)->SetLocation(X, Y);
	g_HUD.GetButton(2)->SetLocation(X, Y += 25);
	g_HUD.GetButton(3)->SetLocation(X, Y += 20);
		
	//g_HUD.GetButton(i)->SetSize(170, 170);

	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, 
	void* pUserContext)
{
	// Заполняем матрицу вращения
	//D3DXMatrixRotationY(&g_World, t);
	g_Camera.FrameMove(fElapsedTime);
}

vector<XMVECTOR> Mass;

XMMATRIX GetMatrix(D3DMATRIX Thing)
{
	return XMMatrixSet(Thing._11, Thing._12, Thing._13, Thing._14, Thing._21,
		Thing._22, Thing._23, Thing._24, Thing._31, Thing._32, Thing._33,
		Thing._34, Thing._41, Thing._42, Thing._43, Thing._44);
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{
	USES_CONVERSION;
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
	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout);
		
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_Camera.GetWorldMatrix()); //GetMatrix(g_World) );
	cb.mView = XMMatrixTranspose(g_Camera.GetViewMatrix()); //GetMatrix(g_View));
	cb.mProjection = XMMatrixTranspose(g_Camera.GetProjMatrix()); //GetMatrix(g_Projection));
	pd3dImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);

	pd3dImmediateContext->VSSetShader(g_pVS, 0, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	pd3dImmediateContext->PSSetShader(g_pPS, 0, 0);
	pd3dImmediateContext->PSSetSamplers(0, 1, &TexSamplerState);

	g_Mesh.Render(pd3dImmediateContext);

#if defined(Never)
	Model->Draw();
#endif
	
	V(g_HUD.OnRender(fElapsedTime));

#ifdef DEBUG
	ID3D11Debug* debug = 0;
	pd3dDevice->QueryInterface(IID_ID3D11Debug, (void**)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

	PhysX->Simulation();

	Mass = { g_Camera.GetEyePt() };
	char buff[100];

	snprintf(buff, sizeof(buff), "\nCam Pos: X:%.1f, Y:%.1f, Z:%.1f\n",
		Mass.data()->m128_f32[0],  Mass.data()->m128_f32[1], Mass.data()->m128_f32[2]);

	g_HUD.GetStatic(STATIC_TEXT)->SetText(A2W(buff));
											//UP  DOWN
	g_HUD.GetStatic(STATIC_TEXT)->SetLocation(0, 10);
	Mass.clear();
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
	SAFE_RELEASE(g_pVertexLayout);
	pConstantBuffer->Release();

	g_Mesh.Destroy();
	PhysX->Destroy();

#if defined(Never)
	Model->Close();
#endif
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
