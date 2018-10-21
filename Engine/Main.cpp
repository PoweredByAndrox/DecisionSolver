#include "pch.h"

#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "DXUTCamera.h"
#include "DXUTgui.h"

ID3D10Effect* g_pEffect = nullptr;
ID3D10InputLayout* g_pVertexLayout = nullptr;
ID3D10EffectTechnique* g_pTechnique = nullptr;
ID3D10Buffer* g_pVertexBuffer = nullptr;
ID3D10Buffer* g_pIndexBuffer = nullptr;
ID3D10EffectMatrixVariable* g_pWorldVariable = nullptr;
ID3D10EffectMatrixVariable* g_pViewVariable = nullptr;
//ID3D10EffectMatrixVariable* g_pProjectionVariable = nullptr;
ID3D10EffectTechnique* g_pRenderSky = nullptr;


CDXUTSDKMesh g_Mesh;
CDXUTSDKMesh g_SkyMesh;

D3DXMATRIX g_World;
D3DXMATRIX g_View;
D3DXMATRIX g_Projection;

CModelViewerCamera g_Camera;
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


void InitApp()
{
	g_HUD.Init(&g_DialogResourceManager);
	int iY = 10;
	g_HUD.AddButton(1, L"Some-Button#1", 35, iY, 125, 22);
	g_HUD.AddButton(3, L"Some-Button#2", 35, iY += 24, 125, 22, VK_F3);
	g_HUD.AddButton(4, L"Some-Button#3", 35, iY += 24, 125, 22, VK_F2);
	g_HUD.AddStatic(5, L"SomeText#1", 100, 120, 60, 50);
}

void RenderSky(ID3D10Device* pd3dDevice)
{
	D3DXMATRIX mWorld;
	D3DXVECTOR3 vEye;
	D3DXVECTOR3 vDir;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	D3DXMatrixRotationY(&mWorld, -D3DX_PI / 2.5f);

	mView._41 = mView._42 = mView._43 = 0.0f;
	D3DXMATRIX mWVP = mWorld * *g_Camera.GetWorldMatrix() * *g_Camera.GetViewMatrix() * *g_Camera.GetProjMatrix();

	g_pmWorldViewProj->SetMatrix((float*)&mWVP);
	g_pmWorld->SetMatrix((float*)&mWorld);

	pd3dDevice->IASetInputLayout(g_pVertexLayout);
	g_SkyMesh.Render(pd3dDevice, g_pRenderSky, g_ptxDiffuse);
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

HRESULT CALLBACK OnD3D10CreateDevice(ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	// Read the D3DX effect file
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DXSHADER_DEBUG;

	UINT uFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_DEBUG;

	V_RETURN(g_DialogResourceManager.OnD3D10CreateDevice(pd3dDevice));

	V_RETURN(D3DX10CreateEffectFromFile(L"D://DecisionSolver//Engine//resource//shaders//SomeFile.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, pd3dDevice, NULL, NULL,
		&g_pEffect, NULL, NULL));

	g_pTechnique = g_pEffect->GetTechniqueByName("RenderMesh");
	g_pWorldVariable = g_pEffect->GetVariableByName("g_mWorld")->AsMatrix();
	g_pViewVariable = g_pEffect->GetVariableByName("g_mWorldViewProj")->AsMatrix();
	
	///////////////////////////////////////////////////////////////////////////////////////////
	//Rework IT
	g_pRenderSky = g_pEffect->GetTechniqueByName("RenderSkybox");
	g_ptxDiffuse = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();
	g_ptxNormal = g_pEffect->GetVariableByName("g_txNormal")->AsShaderResource();
	g_ptxHeight = g_pEffect->GetVariableByName("g_txHeight")->AsShaderResource();
	g_ptxDirt = g_pEffect->GetVariableByName("g_txDirt")->AsShaderResource();
	g_ptxGrass = g_pEffect->GetVariableByName("g_txGrass")->AsShaderResource();
	g_ptxMask = g_pEffect->GetVariableByName("g_txMask")->AsShaderResource();
	g_ptxShadeNormals = g_pEffect->GetVariableByName("g_txShadeNormals")->AsShaderResource();
	//Rework IT
	///////////////////////////////////////////////////////////////////////////////////////////


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

	V_RETURN(g_Mesh.Create(pd3dDevice, L"D://DecisionSolver//Engine//resource//models//tiny.sdkmesh", true));
	V_RETURN(g_SkyMesh.Create(pd3dDevice, L"D://DecisionSolver//Engine//resource//models//cloud_skybox.sdkmesh"));
	// Initialize the world matrices
	D3DXMatrixIdentity(&g_World);

	// Initialize the view matrix
	D3DXVECTOR3 Eye(0.0f, 3.0f, -6.0f);
	D3DXVECTOR3 At(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_View, &Eye, &At, &Up);

	// Update Variables that never change
	g_pViewVariable->SetMatrix((float*)&g_View);

	// Setup the camera's view parameters
	g_Camera.SetViewParams(&Eye, &At);
	return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	V_RETURN(g_DialogResourceManager.OnD3D10ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(g_fFOV, fAspectRatio, 0.1f, 1000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);

	
	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
}

void CALLBACK OnD3D10FrameRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
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
	
	g_SkyMesh.Render(pd3dDevice, g_pRenderSky);


	V(g_HUD.OnRender(fElapsedTime));
	//V(g_SampleUI.OnRender(fElapsedTime));

	//the mesh class also had a render method that allows rendering the mesh with the most common options
	g_Mesh.Render(pd3dDevice, g_pTechnique);
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
	DXUTSetCursorSettings(true, true);
	DXUTCreateWindow(L"EngineProgram");
	DXUTCreateDevice(true, 640, 480);
	DXUTMainLoop(); 

	return DXUTGetExitCode();
}
