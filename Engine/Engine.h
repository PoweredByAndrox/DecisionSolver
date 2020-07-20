#pragma once
#if !defined(__ENGINE_H__)
#define __ENGINE_H__
#include "pch.h"

#include <Inc/Keyboard.h>
#include <Inc/Mouse.h>
#include <Inc/GamePad.h>

#include "resource.h"
#include "Timer.h"

#include "Thread/ThreadPool.h"

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
class Multiplayer;

/*!
 * \class Engine Contains All The Classes
 */
class Engine
{
public:
	enum ThreadStatus
	{
		_Quit = 0,
		_ResizeWND,
		_Work,
		_Nothing
	};
private:
	static ThreadStatus ThState;

	static HWND hwnd;
	bool WireFrame = false,
		IsSimulation = false;
	static bool isQuit, IsLogError;
	MSG msg = {};

	XMVECTORF32 _ColorBuffer = DirectX::Colors::SkyBlue;
	HRESULT hr = S_OK;
	string NameWnd;
	wstring ClassWND;

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
	static shared_ptr<Timer> MainThread;

	// Multiplayer
	shared_ptr<Multiplayer> MPL;
	static shared_ptr<Mouse> mouse;
	static shared_ptr<Keyboard> keyboard;
	static shared_ptr<GamePad> gamepad;

	shared_ptr<DebugDraw> dDraw;

#if defined(Never_MainMenu)
	shared_ptr<MainMenu> Menu = make_unique<MainMenu>();
#endif

public:
/*!
 * \brief Initialized Here Window And DirectX
 *
 * \param ##1 name window
 * \param ##2 Instance of our window
 *
 * \return Init Function status of error codes
 */
	HRESULT Init(string NameWnd, HINSTANCE hInstance);

/*!
 * \brief Render It Every Class
 */
	void Render();

/*!
 * \brief Here's Releasing All Objects Of Window And DirectX
 */
	void Destroy();
	void Quit();
public:
	bool IsQuit() { return isQuit; }
	Engine() {}
	~Engine() {}

	ThreadStatus getThreadState() { return ThState; }

	static HWND GetHWND() { return hwnd; }

	string getNameWndA() { return NameWnd.c_str(); }

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

	shared_ptr<DebugDraw> getDebugDraw() { return dDraw; }
	shared_ptr<Multiplayer> getMPL() { return MPL; }
	shared_ptr<Timer> getMainThread() { return MainThread; }

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
	void setFS(shared_ptr<File_system> FS)
	{
		if (!this->FS.operator bool())
			this->FS = FS;
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
	void setMultiplayer(shared_ptr<Multiplayer> MPL)
	{
		if (!this->MPL.operator bool())
			this->MPL = MPL;
	}
	shared_ptr<Mouse> getMouse() { return mouse; }
	shared_ptr<Keyboard> getKeyboard() { return keyboard; }
	shared_ptr<GamePad> getGamepad() { return gamepad; }

	void setMessage(MSG msg) { this->msg = msg; }
	MSG getMessage() { return msg; }

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
	
/*!
 * \brief Here's Clear Render Target For DirectX + Depth Stensil Also Clear
 */
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

/*!
 * \brief Here's Error Handing For XML Files
 *
 * \param ##1 Error Debug Text If It's Debug Build And Text To File (Log)
 */
	static void StackTrace(LPCSTR Error);

	Mouse::ButtonStateTracker getTrackerMouse() { return TrackerMouse; }
	Keyboard::KeyboardStateTracker getTrackerKeyboard() { return TrackerKeyboard; }
	GamePad::ButtonStateTracker getTracherGamepad() { return TrackerGamepad; }

/*!
 * \brief Here's Error Handing All The Engine
 *
 * \param ##1 Debug Text If It's Debug Build
 * \param ##2 Only Works If Defined "ExceptionWhenEachError"
 * \param ##3 It'll Write In Log File This Text
 */
	static void LogError(string DebugText, string ExceptionText, string LogText);

	void SetLogErr(bool B) { IsLogError = B; }

/*!
 * \brief Here's Keybord Key Down Function For Lua
 *
 * \param ##1 Key Passed From Lua
 *
 * \return bool If It Was Down
*/
	static bool IsKeyboardDown(int Key);
		
/*!
 * \brief Here's Keybord Key Up Function For Lua
 *
 * \param ##1 Key Passed From Lua
 *
 * \return bool If It Was Up
 */
	static bool IsKeyboardUp(int Key);
	
/*!
 * \brief Here's Mouse Left Button Function For Lua
 *
 * \return bool If It Clicked Left
 */
	static bool IsMouseLeft();
	
/*!
 * \brief Here's Mouse Right Button Function For Lua
 *
 * \return bool If It Clicked Right
*/
	static bool IsMouseRight();
private:
/*!
 * \brief Here's Update Messages From System To Window
 *
 * \param ##1 Handle Current Window
 * \param ##2 Message
 * \param ##3 Additional Param
 * \param ##4 Additional Param
 */
	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	using ButtonState = Mouse::ButtonStateTracker::ButtonState;
	Mouse::ButtonStateTracker TrackerMouse;
	Keyboard::KeyboardStateTracker TrackerKeyboard;
	GamePad::ButtonStateTracker TrackerGamepad;

	HINSTANCE hInstance;
};
#endif // __ENGINE_H__
