#include "pch.h"

#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "SDKmesh.h"

ID3D10Effect* g_pEffect = nullptr;
ID3D10InputLayout* g_pVertexLayout = nullptr;
ID3D10EffectTechnique* g_pTechnique = nullptr;
ID3D10Buffer* g_pVertexBuffer = nullptr;
ID3D10Buffer* g_pIndexBuffer = nullptr;
ID3D10EffectMatrixVariable* g_pWorldVariable = nullptr;
ID3D10EffectMatrixVariable* g_pViewVariable = nullptr;
ID3D10EffectMatrixVariable* g_pProjectionVariable = nullptr;
CDXUTSDKMesh g_Mesh;
D3DXMATRIX g_World;
D3DXMATRIX g_View;
D3DXMATRIX g_Projection;

HRESULT hr = S_OK;

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
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	if (FAILED(hr = D3DX10CreateEffectFromFileA("D://DecisionSolver//Engine//resource//shaders//Main.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, pd3dDevice, NULL, NULL,
		&g_pEffect, NULL, NULL)))
	{
		MessageBoxA(NULL,
			"The FX file cannot be located.  Please run this executable from the directory that contains the FX file.",
			"Error", MB_OK);
		V_RETURN(hr);
	}

	g_pTechnique = g_pEffect->GetTechniqueByName("Render");
	g_pWorldVariable = g_pEffect->GetVariableByName("World")->AsMatrix();
	g_pViewVariable = g_pEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionVariable = g_pEffect->GetVariableByName("Projection")->AsMatrix();

	// Define the input layout
	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
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

	// Initialize the world matrices
	D3DXMatrixIdentity(&g_World);

	// Initialize the view matrix
	D3DXVECTOR3 Eye(0.0f, 3.0f, -6.0f);
	D3DXVECTOR3 At(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_View, &Eye, &At, &Up);

	// Update Variables that never change
	g_pViewVariable->SetMatrix((float*)&g_View);
	return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	float fAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
	D3DXMatrixPerspectiveFovLH(&g_Projection, D3DX_PI * 0.25f, fAspect, 0.1f, 100.0f);
	g_pProjectionVariable->SetMatrix((float*)&g_Projection);
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

	//
	// Render the mesh
	//
	UINT Strides[1];
	UINT Offsets[1];
	ID3D10Buffer* pVB[1];
	pVB[0] = g_Mesh.GetVB10(0, 0);
	Strides[0] = (UINT)g_Mesh.GetVertexStride(0, 0);
	Offsets[0] = 0;
	pd3dDevice->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
	pd3dDevice->IASetIndexBuffer(g_Mesh.GetIB10(0), g_Mesh.GetIBFormat10(0), 0);

	D3D10_TECHNIQUE_DESC techDesc;
	g_pTechnique->GetDesc(&techDesc);
	SDKMESH_SUBSET* pSubset = NULL;
	ID3D10ShaderResourceView* pDiffuseRV = NULL;
	D3D10_PRIMITIVE_TOPOLOGY PrimType;

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (UINT subset = 0; subset < g_Mesh.GetNumSubsets(0); ++subset)
		{
			pSubset = g_Mesh.GetSubset(0, subset);

			PrimType = g_Mesh.GetPrimitiveType10((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
			pd3dDevice->IASetPrimitiveTopology(PrimType);

			pDiffuseRV = g_Mesh.GetMaterial(pSubset->MaterialID)->pDiffuseRV10;

			g_pTechnique->GetPassByIndex(p)->Apply(0);
			pd3dDevice->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
		}
	}

	//the mesh class also had a render method that allows rendering the mesh with the most common options
	g_Mesh.Render(pd3dDevice, g_pTechnique, nullptr);
}

void CALLBACK OnD3D10ReleasingSwapChain(void* pUserContext)
{
}

void CALLBACK OnD3D10DestroyDevice(void* pUserContext)
{
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pEffect);
	g_Mesh.Destroy();
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
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

	DXUTInit(true, true, NULL); 
	DXUTSetCursorSettings(true, true);
	DXUTCreateWindow(L"EngineProgram");
	DXUTCreateDevice(true, 640, 480);
	DXUTMainLoop(); 

	return DXUTGetExitCode();
}
