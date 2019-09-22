#pragma once
#if !defined(__ENGINE_H__)
#define __ENGINE_H__
#include "pch.h"

#include <Inc/Keyboard.h>
#include <Inc/Mouse.h>
#include <Inc/GamePad.h>

#include "StepTimer.h"
#include "resource.h"

#define Never
//#define NEEDED_DEBUG_INFO

//#define ExceptionWhenEachError

class DebugDraw;

class CLua;
class File_system;
class UI;
class Models;
class Camera;
class Actor;
class Audio;
class Console;
class Physics;
class Picking;
class Levels;
class CutScene;
class Engine
{
private:
	static HWND hwnd;
	bool WireFrame = false,
		IsSimulation = false;

	XMVECTORF32 _ColorBuffer = DirectX::Colors::SkyBlue;
	HRESULT hr = S_OK;
	wstring NameWnd = L"", ClassWND = L"";

	float fps = 0.f, frameTime = 0.f;

	static ID3D11Device *Device;
	static ID3D11DeviceContext *DeviceContext;
	static ID3D11Device1 *Device1;
	static ID3D11DeviceContext1 *DeviceContext1;
	static IDXGISwapChain *SwapChain;
	static IDXGISwapChain1 *SwapChain1;
	static ID3D11RenderTargetView *RenderTargetView;
	static ID3D11Texture2D *DepthStencil;
	ID3D11DepthStencilState *m_depthStencilState = nullptr;

	static ID3D11DepthStencilView *DepthStencilView;
	static IDXGIFactory1 *dxgiFactory;
	static IDXGIFactory2 *dxgiFactory2;

	D3D_FEATURE_LEVEL *featureLevel = nullptr;
	static DXGI_SWAP_CHAIN_DESC SCD;
	static DXGI_SWAP_CHAIN_DESC1 SCD1;
	static D3D11_TEXTURE2D_DESC descDepth;
	static D3D11_VIEWPORT vp;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState *RsWF = nullptr, *RsNoWF = nullptr;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	DXGI_ADAPTER_DESC pDesc;

	UINT m4xMsaaQuality = 0ul;

#if defined(NEEDED_DEBUG_INFO)
	ID3D11Debug *debug = nullptr;
#endif

	shared_ptr<File_system> FS;
	shared_ptr<Models> model;
	shared_ptr<Audio> Sound;
	shared_ptr<Console> console;
	shared_ptr<UI> ui;
	shared_ptr<CLua> lua;
	shared_ptr<CutScene> CScene;
	shared_ptr<Picking> Pick;
	//shared_ptr<Frustum> frustum;
	shared_ptr<Levels> Level;
	shared_ptr<Actor> mainActor;
	shared_ptr<Physics> PhysX;
	shared_ptr<Camera> camera;
	static shared_ptr<Mouse> mouse;
	static shared_ptr<Keyboard> keyboard;
	static shared_ptr<GamePad> gamepad;

	shared_ptr<StepTimer> Timer = make_shared<StepTimer>();

	shared_ptr<DebugDraw> dDraw;

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> Menu = make_unique<MainMenu>();
#endif

public:
	HRESULT Init(wstring NameWnd, HINSTANCE hInstance);

	void Render();
	void Destroy();
	static void Quit() { ::PostQuitMessage(0); }

	Engine() {}
	~Engine() {}

	static HWND GetHWND() { return hwnd; }

	wstring getNameWndW() { return NameWnd; }
	string getNameWndA()
	{
		USES_CONVERSION;
		return W2A(NameWnd.c_str());
	}

	shared_ptr<File_system> getFS() { return FS; }
	shared_ptr<Models> getModel() { return model; }
	shared_ptr<Audio> getSound() { return Sound; }
	shared_ptr<UI> getUI() { return ui; }
	shared_ptr<Picking> getPick() { return Pick; }
	//shared_ptr<Frustum> getFrustum() { return frustum; }
	shared_ptr<Levels> getLevel() { return Level; }
	shared_ptr<Actor> getActor() { return mainActor; }
	shared_ptr<Physics> getPhysics() { return PhysX; }
	shared_ptr<Camera> getCamera() { return camera; }
	shared_ptr<Console> getConsole() { return console; }
	shared_ptr<CLua> getCLua() { return lua; }
	shared_ptr<CutScene> getCScene() { return CScene; }

	shared_ptr<StepTimer> getTimer() { return Timer; }

	shared_ptr<DebugDraw> getDebugDraw() { return dDraw; }

	void setUI(shared_ptr<UI> ui)
	{
		if (!this->ui.operator bool())
			this->ui = ui;
	}
	void setConsole(shared_ptr<Console> console)
	{
		if (!this->console.operator bool())
			this->console = console;
	}
	void setFS(shared_ptr<File_system> Pick)
	{
		if (!this->FS.operator bool())
			this->FS = Pick;
	}
	void setModel(shared_ptr<Models> model)
	{
		if (!this->model.operator bool())
			this->model = model;
	}
	void setCamera(shared_ptr<Camera> camera)
	{
		if (!this->camera.operator bool())
			this->camera = camera;
	}
	void setActor(shared_ptr<Actor> mainActor)
	{
		if (!this->mainActor.operator bool())
			this->mainActor = mainActor;
	}
	void setSound(shared_ptr<Audio> Sound)
	{
		if (!this->Sound.operator bool())
			this->Sound = Sound;
	}
	void setPhysics(shared_ptr<Physics> PhysX)
	{
		if (!this->PhysX.operator bool())
			this->PhysX = PhysX;
	}
	void setDebugDraw(shared_ptr<DebugDraw> dDraw)
	{
		if (!this->dDraw.operator bool())
			this->dDraw = dDraw;
	}
	void setCLua(shared_ptr<CLua> lua)
	{
		if (!this->lua.operator bool())
			this->lua = lua;
	}
	void setCScene(shared_ptr<CutScene> CScene)
	{
		if (!this->CScene.operator bool())
			this->CScene = CScene;
	}
	void setPick(shared_ptr<Picking> Pick)
	{
		if (!this->Pick.operator bool())
			this->Pick = Pick;
	}
	/*
	void setFrustum(shared_ptr<Frustum> frustum)
	{
		if (!this->frustum.operator bool())
			this->frustum = frustum;
	}
	*/
	void setLevel(shared_ptr<Levels> Level)
	{
		if (!this->Level.operator bool())
			this->Level = Level;
	}
	shared_ptr<Mouse> getMouse() { return mouse; }
	shared_ptr<Keyboard> getKeyboard() { return keyboard; }
	shared_ptr<GamePad> getGamepad() { return gamepad; }

	ID3D11Device *getDevice();
	ID3D11DeviceContext *getDeviceContext();
	IDXGISwapChain *getSwapChain();
	ID3D11RenderTargetView *getTargetView();

	DXGI_SWAP_CHAIN_DESC getSwapChainDesc() { return SCD; }
	DXGI_SWAP_CHAIN_DESC1 getSwapChainDesc1() { return SCD1; }
	D3D11_TEXTURE2D_DESC getDescDepth() { return descDepth; }
	D3D11_VIEWPORT getViewPort() { return vp; }

	UINT getMsaaQuality() { return m4xMsaaQuality; }

	void ChangeColorBuffer(XMVECTORF32 Color) { _ColorBuffer = Color; }
	void ClearRenderTarget();
	static HRESULT ResizeWindow(WPARAM wParam);

	bool IsWireFrame() { return WireFrame; }
	ID3D11RasterizerState *GetWireFrame() { return RsWF; }
	ID3D11RasterizerState *GetNormalFrame() { return RsNoWF; }
	void SetWireFrame(bool WF) { WireFrame = WF; }

	bool IsSimulatePhysics() { return IsSimulation; }
	void SetPausePhysics(bool Pause) { IsSimulation = Pause; }

	float getFPS() { return fps; }
	static POINT getWorkAreaSize(HWND hwnd);

	float getframeTime();

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> getMainMenu() { return Menu; }
#endif

	static void StackTrace(LPCSTR Error);

	Mouse::ButtonStateTracker getTrackerMouse() { return TrackerMouse; }
	Keyboard::KeyboardStateTracker getTrackerKeyboard() { return TrackerKeyboard; }
	GamePad::ButtonStateTracker getTracherGamepad() { return TrackerGamepad; }

	auto getAllThreadGroup() { return ThreadGroups; }

	static void LogError(string DebugText, string ExceptionText, string LogText);

	//static bool IsKeyboardDown(Keyboard::Keys Key);
	//static bool IsKeyboardUp(Keyboard::Keys Key);
	//static bool IsMouseDown(Keyboard::Keys Key);
	//static bool IsMouseUp(Keyboard::Keys Key);
private:
	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	using ButtonState = Mouse::ButtonStateTracker::ButtonState;
	Mouse::ButtonStateTracker TrackerMouse;
	Keyboard::KeyboardStateTracker TrackerKeyboard;
	GamePad::ButtonStateTracker TrackerGamepad;

	shared_ptr<boost::thread_group> ThreadGroups;
	HINSTANCE hInstance;
};
#endif // __ENGINE_H__
