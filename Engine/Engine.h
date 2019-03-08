#pragma once
#if !defined(__ENGINE_H__)
#define __ENGINE_H__
#include "pch.h"

#include <Keyboard.h>
#include <Mouse.h>

#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#endif
#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p) = nullptr; } }
#endif
#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
	#define SAFE_release(p) { if (p) { (p)->release(); (p) = nullptr; } }
#endif

#if defined(DEBUG) || defined(_DEBUG)
	#ifndef V
		#define V(x) { hr = (x); if (FAILED(hr)) { DXUTTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); } }
	#endif
	#ifndef V_RETURN
		#define V_RETURN(x) { hr = (x); if (FAILED(hr)) { return DXUTTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); } }
	#endif
#else
	#if !defined(V)
		#define V(x) { hr = (x); }
	#endif
	#if !defined(V_RETURN)
		#define V_RETURN(x) { hr = (x); if (FAILED(hr)) { return hr; } }
	#endif
#endif

#define Never

class Render_Buffer;
class File_system;
class UI;
class Models;
class Camera;
class Actor;
class Shaders;
class Engine
{
private:
	struct DescWindow
	{
		DescWindow() {}
		DescWindow(LPCWSTR Title, float PosX, float PosY, float W, float H, bool Resizing) : Title(Title), PosX(PosX), W(W), PosY(PosY), H(H),
			Resizing(Resizing) {}
		float PosX = 0.f,
			PosY = 0.f,
			W = 0.f,
			H = 0.f;
		bool Resizing = false;
		LPCWSTR Title = L"Engine";
	} *m_desc = nullptr;

	HWND hwnd = nullptr;
	bool WireFrame = false,
		PauseSimulation = false;
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
	POINT Desktop = { 0, 0 };
	LPCWSTR NameWnd = L"";

	double countsPerSecond = 0.0, frameTime = 0.0;
	__int64 CounterStart = 0, frameTimeOld = 0;
	int frameCount = 0, fps = 0;

	ID3D11Device *Device = nullptr;
	ID3D11DeviceContext *DeviceContext = nullptr;
	IDXGISwapChain *SwapChain = nullptr;
	ID3D11RenderTargetView *RenderTargetView = nullptr;
	ID3D11DepthStencilView *DepthStencilView = nullptr;
	ID3D11DepthStencilState *DepthStencilState = nullptr,
		*DepthDisabledStencilState = nullptr;
	ID3D11BlendState *AlphaEnableBlendingState = nullptr,
		*AlphaDisableBlendingState = nullptr;
	ID3D11Texture2D *TextrDepthStencil = nullptr;

	D3D_FEATURE_LEVEL *featureLevel = nullptr;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

#if defined(NEEDED_DEBUG_INFO)
	ID3D11Debug *debug = nullptr;
#endif

	//#define Never_MainMenu
	//#define UseConsole

	shared_ptr<File_system> FS;
	shared_ptr<Models> model;
	//shared_ptr<Audio> Sound;
	shared_ptr<UI> ui;
	//shared_ptr<Picking> Pick;
	// shared_ptr<Terrain> terrain;
	//shared_ptr<Frustum> frustum;
	shared_ptr<Render_Buffer> R_buffer;
	shared_ptr<Actor> mainActor;
	//shared_ptr<Physics> PhysX;
	//shared_ptr<Levels> Level;
	shared_ptr<Camera> camera;
	shared_ptr<Shaders> shader;
	shared_ptr<Mouse> mouse = make_unique<Mouse>();
	shared_ptr<Keyboard> keyboard = make_unique<Keyboard>();

#ifdef UseConsole
	shared_ptr<Console> console = make_unique<Console>();
#endif
#ifdef Never_MainMenu
	shared_ptr<MainMenu> Menu = make_unique<MainMenu>();
#endif

public:
	HRESULT Init(LPCWSTR NameWnd, HINSTANCE hInstance);

	void Run();

	void Render();
	void Destroy();
	Engine() {}
	~Engine() {}

	HWND GetHWND() { return hwnd; }
	LPCWSTR getNameWnd() { return NameWnd; }

	shared_ptr<File_system> getFS() { return FS; }
	shared_ptr<Models> getModel() { return model; }
	//shared_ptr<Audio> getSound() { return Sound; }
	shared_ptr<UI> getUI() { return ui; }
	//shared_ptr<Picking> getPick() { return Pick; }
	//shared_ptr<Frustum> getFrustum() { return frustum; }
	shared_ptr<Render_Buffer> getRender_Buffer() { return R_buffer; }
	shared_ptr<Actor> getActor() { return mainActor; }
	//shared_ptr<Physics> getPhysics() { return PhysX; }
	//shared_ptr<Levels> getLevel() { return Level; }
	shared_ptr<Camera> getCamera() { return camera; }
	shared_ptr<Shaders> getShader() { return shader; }

	void setUI(shared_ptr<UI> ui)
	{
		if (!this->ui.operator bool())
			this->ui = ui;
	}
	void setRender_Buffer(shared_ptr<Render_Buffer> R_buffer)
	{
		if (!this->R_buffer.operator bool())
			this->R_buffer = R_buffer;
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
	void setPhysics(shared_ptr<Physics> PhysX)
	{
		if (!this->PhysX.operator bool())
			this->PhysX = PhysX;
	}
	void setLevel(shared_ptr<Levels> Level)
	{
		if (!this->Level.operator bool())
			this->Level = Level;
	}
	void setSound(shared_ptr<Audio> Sound)
	{
		if (!this->Sound.operator bool())
			this->Sound = Sound;
	}
	*/

	shared_ptr<Mouse> getMouse() { return mouse; }
	shared_ptr<Keyboard> getKeyboard() { return keyboard; }

	ID3D11Device *getDevice()
	{
		if (Device)
			return Device;
		else
		{
			DebugTrace("Engine::GetDevice() get is failed");
			throw exception("Get is failed!!!");
			return nullptr;
		}
	}
	ID3D11DeviceContext *getDeviceContext()
	{
		if (DeviceContext)
			return DeviceContext;
		else
		{
			DebugTrace("Engine::GetDeviceContext() get is failed");
			throw exception("Get is failed!!!");
			return nullptr;
		}
	}
	IDXGISwapChain *getSwapChain()
	{
		if (SwapChain)
			return SwapChain;
		else
		{
			DebugTrace("Engine::GetSwapChain() get is failed");
			throw exception("Get is failed!!!");
			return nullptr;
		}
	}
	ID3D11RenderTargetView *getTargetView()
	{
		if (RenderTargetView)
			return RenderTargetView;
		else
		{
			DebugTrace("Engine::GetTargetView() get is failed");
			throw exception("Get is failed!!!");
			return nullptr;
		}
	}

	void ChangeColorBuffer(XMVECTORF32 Color) { _ColorBuffer = Color; }
	void ClearRenderTarget()
	{
		DeviceContext->ClearRenderTargetView(RenderTargetView, _ColorBuffer);
		DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0.f);
	}
	bool IsWireFrame() { return WireFrame; }

	float getFPS() { return fps; }
	POINT getWorkAreaSize()
	{
		RECT rc;
		POINT Rect;
		GetClientRect(hwnd, &rc);
		Rect.x = rc.right - rc.left; // Width
		Rect.y = rc.bottom - rc.top; // Height
		return Rect;
	}
	POINT getPosCursor()
	{
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(hwnd, &ptCursor);
		return ptCursor;
	}

	POINT getResolutionMonitor() { return Desktop; }

	// Work with Time!
	void StartTimer()
	{
		LARGE_INTEGER frequencyCount;
		QueryPerformanceFrequency(&frequencyCount);

		countsPerSecond = double(frequencyCount.QuadPart);

		QueryPerformanceCounter(&frequencyCount);
		CounterStart = frequencyCount.QuadPart;
	}
	double GetTime()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
	}
	double GetFrameTime()
	{
		LARGE_INTEGER currentTime;
		__int64 tickCount;
		QueryPerformanceCounter(&currentTime);

		tickCount = currentTime.QuadPart - frameTimeOld;
		frameTimeOld = currentTime.QuadPart;

		if (tickCount < 0.0f)
			tickCount = 0.0f;

		return float(tickCount) / countsPerSecond;
	}

#ifdef UseConsole
	shared_ptr<Console> getConsole() { return console; }
#endif
#ifdef Never_MainMenu
	shared_ptr<MainMenu> getMainMenu() { return Menu; }
#endif

	Mouse::ButtonStateTracker getTrackerMouse() { return TrackerMouse; }
	Keyboard::KeyboardStateTracker getTrackerKeyboard() { return TrackerKeyboard; }

	void ResizibleWnd();
private:
	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	using ButtonState = Mouse::ButtonStateTracker::ButtonState;
	Mouse::ButtonStateTracker TrackerMouse;
	Keyboard::KeyboardStateTracker TrackerKeyboard;
};
#endif // __ENGINE_H__
