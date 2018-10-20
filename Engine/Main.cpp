#include "pch.h"

#include "DXUT.h"

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
	return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
}

void CALLBACK OnD3D10FrameRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
	pd3dDevice->ClearRenderTargetView(DXUTGetD3D10RenderTargetView(), ClearColor);
	pd3dDevice->ClearDepthStencilView(DXUTGetD3D10DepthStencilView(), D3D10_CLEAR_DEPTH, 1.0, 0);
}

void CALLBACK OnD3D10ReleasingSwapChain(void* pUserContext)
{
}

void CALLBACK OnD3D10DestroyDevice(void* pUserContext)
{
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
