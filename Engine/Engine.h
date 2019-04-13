#pragma once
#if !defined(__ENGINE_H__)
#define __ENGINE_H__
#include "pch.h"

#include <Keyboard.h>
#include <Mouse.h>
#include <GamePad.h>

#define Never
//#define NEEDED_DEBUG_INFO

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
class Engine
{
private:
	struct DescWindow
	{
		DescWindow() {}
		DescWindow(LPCWSTR Title, float PosX, float PosY, float W, float H, bool Resizing): Title(Title), PosX(PosX), W(W), PosY(PosY), H(H),
			Resizing(Resizing) {}
		float PosX = 0.f,
			PosY = 0.f,
			W = 0.f,
			H = 0.f;
		bool Resizing = false;
		LPCWSTR Title = L"Engine";
	} *m_desc = nullptr;

	static HWND hwnd;
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
	wstring NameWnd = L"", ClassWND = L"";

	double countsPerSecond = 0.0, frameTime = 0.0;
	__int64 CounterStart = 0, frameTimeOld = 0;
	int frameCount = 0, fps = 0;

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
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

#if defined(NEEDED_DEBUG_INFO)
	ID3D11Debug *debug = nullptr;
#endif

	//#define Never_MainMenu

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
	shared_ptr<Mouse> mouse = make_unique<Mouse>();
	shared_ptr<Keyboard> keyboard = make_unique<Keyboard>();
	shared_ptr<GamePad> gamepad = make_unique<GamePad>();

	shared_ptr<DebugDraw>dDraw;

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> Menu = make_unique<MainMenu>();
#endif

public:
	HRESULT Init(wstring NameWnd, HINSTANCE hInstance);

	void Run();

	void Render();
	void Destroy(HINSTANCE hInstance);
	void Quit() { ::PostQuitMessage(0); }

	Engine() {}
	~Engine() {}

	static HWND GetHWND() { return hwnd; }
	wstring getNameWnd() { return NameWnd; }

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
	static void ResizeWindow()
	{
		if (!SwapChain)
			return;

		ID3D11Texture2D *pBackBuffer = nullptr;
		SAFE_RELEASE(RenderTargetView);
		SAFE_RELEASE(DepthStencilView);
		SAFE_RELEASE(DepthStencil);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = getWorkAreaSize(GetHWND()).x;
		sd.BufferDesc.Height = getWorkAreaSize(GetHWND()).y;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetHWND();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		//sd.Flags = !vsettings.windowed ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

		if (FAILED(SwapChain->ResizeBuffers(sd.BufferCount, sd.BufferDesc.Width, sd.BufferDesc.Height, sd.BufferDesc.Format, sd.Flags)))
		{
			DebugTrace("Engine::ResizeWindow->ResizeBuffers() failed.");
			throw exception("Resize failed!!!");
		}

		DXGI_MODE_DESC md;
		md.Width = sd.BufferDesc.Width;
		md.Height = sd.BufferDesc.Height;
		md.RefreshRate = sd.BufferDesc.RefreshRate;
		md.Format = sd.BufferDesc.Format;
		md.Scaling = sd.BufferDesc.Scaling;
		md.ScanlineOrdering = sd.BufferDesc.ScanlineOrdering;

		if (FAILED(SwapChain->ResizeTarget(&md)))
		{
			DebugTrace("Engine::ResizeWindow->ResizeTarget() failed.");
			throw exception("Resize failed!!!");
		}

		if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer))))
		{
			DebugTrace("Engine::ResizeWindow->GetBuffer() failed.");
			throw exception("Get failed!!!");
		}

		if (FAILED(Device->CreateRenderTargetView(pBackBuffer, 0, &RenderTargetView)))
		{
			DebugTrace("Engine::ResizeWindow->CreateRenderTargetView() failed.");
			throw exception("Create failed!!!");
		}

		UINT Count = 1;
		UINT Quality = 0;
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = sd.BufferDesc.Width;
		depthStencilDesc.Height = sd.BufferDesc.Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = Count;
		depthStencilDesc.SampleDesc.Quality = Quality;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		if (FAILED(Device->CreateTexture2D(&depthStencilDesc, 0, &DepthStencil)))
		{
			DebugTrace("Engine::ResizeWindow->CreateTexture2D() failed.");
			throw exception("Create failed!!!");
		}

		if (FAILED(Device->CreateDepthStencilView(DepthStencil, 0, &DepthStencilView)))
		{
			DebugTrace("Engine::ResizeWindow->CreateDepthStencilView() failed.");
			throw exception("Create failed!!!");
		}

		DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)getWorkAreaSize(GetHWND()).x;
		vp.Height = (FLOAT)getWorkAreaSize(GetHWND()).y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		DeviceContext->RSSetViewports(1, &vp);
		pBackBuffer->Release();
	}

	bool IsWireFrame() { return WireFrame; }

	float getFPS() { return fps; }
	static POINT getWorkAreaSize(HWND hwnd)
	{
		RECT rc;
		POINT Rect;
		GetClientRect(hwnd, &rc);
		Rect.x = rc.right - rc.left; // Width
		Rect.y = rc.bottom - rc.top; // Height
		return Rect;
	}

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

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> getMainMenu() { return Menu; }
#endif

	Mouse::ButtonStateTracker getTrackerMouse() { return TrackerMouse; }
	Keyboard::KeyboardStateTracker getTrackerKeyboard() { return TrackerKeyboard; }
	GamePad::ButtonStateTracker getTracherGamepad() { return TrackerGamepad; }
private:
	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	using ButtonState = Mouse::ButtonStateTracker::ButtonState;
	Mouse::ButtonStateTracker TrackerMouse;
	Keyboard::KeyboardStateTracker TrackerKeyboard;
	GamePad::ButtonStateTracker TrackerGamepad;
};
#endif // __ENGINE_H__
