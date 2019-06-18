#pragma once
#if !defined(__ENGINE_H__)
#define __ENGINE_H__
#include "pch.h"

#include <Inc/Keyboard.h>
#include <Inc/Mouse.h>
#include <Inc/GamePad.h>

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
class Shaders;
class Audio;
class Console;
class Physics;
//class Picking;
class Engine
{
private:
	static HWND hwnd;
	bool WireFrame = false,
		IsSimulation = false;
	XMVECTORF32 _ColorBuffer = DirectX::Colors::Black;
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
	HRESULT hr = S_OK;
	wstring NameWnd = L"", ClassWND = L"";

	__int64 CounterStart = 0, frameTimeOld = 0;
	float frameCount = 0.f, fps = 0.f, countsPerSecond = 0.f, frameTime = 0.f;

	WNDCLASSEXW wnd;

	static ID3D11Device *Device;
	static ID3D11DeviceContext *DeviceContext;
	static ID3D11Device1 *Device1;
	static ID3D11DeviceContext1 *DeviceContext1;
	static IDXGISwapChain *SwapChain;
	static IDXGISwapChain1 *SwapChain1;
	static ID3D11RenderTargetView *RenderTargetView;
	static ID3D11Texture2D *DepthStencil;
	static ID3D11DepthStencilView *DepthStencilView;
	static IDXGIFactory1 *dxgiFactory;
	static IDXGIFactory2 *dxgiFactory2;

	D3D_FEATURE_LEVEL *featureLevel = nullptr;
	static DXGI_SWAP_CHAIN_DESC SCD;
	static DXGI_SWAP_CHAIN_DESC1 SCD1;
	static D3D11_TEXTURE2D_DESC descDepth;
	static D3D11_VIEWPORT vp;

	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState *rasterState = nullptr;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

	UINT m4xMsaaQuality = 0l;

#if defined(NEEDED_DEBUG_INFO)
	ID3D11Debug *debug = nullptr;
#endif

	shared_ptr<File_system> FS;
	shared_ptr<Models> model;
	shared_ptr<Audio> Sound;
	shared_ptr<Console> console;
	shared_ptr<UI> ui;
	shared_ptr<CLua> lua;
	//shared_ptr<Picking> Pick;
	// shared_ptr<Terrain> terrain;
	//shared_ptr<Frustum> frustum;
	shared_ptr<Actor> mainActor;
	shared_ptr<Physics> PhysX;
	//shared_ptr<Levels> Level;
	shared_ptr<Camera> camera;
	shared_ptr<Shaders> shader;
	shared_ptr<Mouse> mouse = make_shared<Mouse>();
	shared_ptr<Keyboard> keyboard = make_shared<Keyboard>();
	shared_ptr<GamePad> gamepad = make_shared<GamePad>();

	shared_ptr<DebugDraw> dDraw;

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> Menu = make_unique<MainMenu>();
#endif

public:
	HRESULT Init(wstring NameWnd, HINSTANCE hInstance);

	void Render();
	void Destroy();
	void Quit() { ::PostQuitMessage(0); }

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
	//shared_ptr<Picking> getPick() { return Pick; }
	//shared_ptr<Frustum> getFrustum() { return frustum; }
	shared_ptr<Actor> getActor() { return mainActor; }
	shared_ptr<Physics> getPhysics() { return PhysX; }
	//shared_ptr<Levels> getLevel() { return Level; }
	shared_ptr<Camera> getCamera() { return camera; }
	shared_ptr<Shaders> getShader() { return shader; }
	shared_ptr<Console> getConsole() { return console; }
	shared_ptr<CLua> getCLua() { return lua; }

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
	void setShader(shared_ptr<Shaders> shader)
	{
		if (!this->shader.operator bool())
			this->shader = shader;
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
	/*
	void setPick(shared_ptr<Picking> Pick)
	{
		if (!this->Pick.operator bool())
			this->Pick = Pick;
	}
	void setFrustum(shared_ptr<Frustum> frustum)
	{
		if (!this->frustum.operator bool())
			this->frustum = frustum;
	}
	void setLevel(shared_ptr<Levels> Level)
	{
		if (!this->Level.operator bool())
			this->Level = Level;
	}
	*/

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
	void ClearRenderTarget()
	{
		DeviceContext->ClearRenderTargetView(RenderTargetView, _ColorBuffer);
		DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
	static HRESULT ResizeWindow(WPARAM wParam);

	bool IsWireFrame() { return WireFrame; }
	void SetWireFrame(bool WF) { WireFrame = WF; }

	bool IsSimulatePhysics() { return IsSimulation; }
	void SetPausePhysics(bool Pause) { IsSimulation = Pause; }

	float getFPS() { return fps; }
	static POINT getWorkAreaSize(HWND hwnd)
	{
		RECT rc = { 0, 0, 0, 0 };
		POINT Rect = { 0, 0 };
		GetClientRect(hwnd, &rc);
		Rect.x = rc.right - rc.left; // Width
		Rect.y = rc.bottom - rc.top; // Height
		return Rect;
	}

	float getframeTime() { return frameTime; }

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> getMainMenu() { return Menu; }
#endif

	static void StackTrace(LPCSTR Error);

	Mouse::ButtonStateTracker getTrackerMouse() { return TrackerMouse; }
	Keyboard::KeyboardStateTracker getTrackerKeyboard() { return TrackerKeyboard; }
	GamePad::ButtonStateTracker getTracherGamepad() { return TrackerGamepad; }

	auto getAllThreadGroup() { return ThreadGroups; }
private:
	// Work with Time!
	void StartTimer()
	{
		LARGE_INTEGER frequencyCount;
		QueryPerformanceFrequency(&frequencyCount);

		countsPerSecond = float(frequencyCount.QuadPart);

		QueryPerformanceCounter(&frequencyCount);
		CounterStart = frequencyCount.QuadPart;
	}
	double GetTime()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
	}
	float GetFrameTime()
	{
		LARGE_INTEGER currentTime;
		__int64 tickCount;
		QueryPerformanceCounter(&currentTime);

		tickCount = currentTime.QuadPart - frameTimeOld;
		frameTimeOld = currentTime.QuadPart;

		if (tickCount < 0)
			tickCount = 0;

		return float(tickCount) / countsPerSecond;
	}
	void CountFPS()
	{
		frameCount++;
		if (GetTime() > 1.0f)
		{
			fps = frameCount;
			frameCount = 0;
			StartTimer();
		}

		frameTime = GetFrameTime();
	}

	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	using ButtonState = Mouse::ButtonStateTracker::ButtonState;
	Mouse::ButtonStateTracker TrackerMouse;
	Keyboard::KeyboardStateTracker TrackerKeyboard;
	GamePad::ButtonStateTracker TrackerGamepad;

	shared_ptr<boost::thread_group> ThreadGroups;
	HINSTANCE hInstance;
};
#endif // __ENGINE_H__
