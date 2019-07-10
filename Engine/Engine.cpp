#include "pch.h"

#include "UI.h"
#include "Models.h"
#include "Actor.h"
#include "Shaders.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"
//#include "CLua.h"
//#include "Picking.h"
#include "DebugDraw.h"

ID3D11Device *Engine::Device = nullptr;
ID3D11DeviceContext *Engine::DeviceContext = nullptr;
IDXGISwapChain *Engine::SwapChain = nullptr;
ID3D11RenderTargetView *Engine::RenderTargetView = nullptr;
ID3D11Texture2D *Engine::DepthStencil = nullptr;
ID3D11DepthStencilView *Engine::DepthStencilView = nullptr;
HWND Engine::hwnd = nullptr;

ID3D11Device1 *Engine::Device1 = nullptr;
ID3D11DeviceContext1 *Engine::DeviceContext1 = nullptr;
IDXGISwapChain1 *Engine::SwapChain1 = nullptr;
IDXGIFactory1 *Engine::dxgiFactory = nullptr;
IDXGIFactory2 *Engine::dxgiFactory2 = nullptr;
DXGI_SWAP_CHAIN_DESC Engine::SCD;
DXGI_SWAP_CHAIN_DESC1 Engine::SCD1;
D3D11_TEXTURE2D_DESC Engine::descDepth;
D3D11_VIEWPORT Engine::vp;

WNDCLASSEXW wnd;

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

bool DrawGrid = true, DrawCamSphere;

HRESULT Engine::Init(wstring NameWnd, HINSTANCE hInstance)
{
	try
	{
		this->hInstance = hInstance;
		wnd.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_CLASSDC;
		wnd.lpfnWndProc = (WNDPROC)Engine::WndProc;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hInstance = hInstance;
		wnd.hIcon = LoadIconW(hInstance, IDI_WINLOGO);
		wnd.hIconSm = wnd.hIcon;
		wnd.hCursor = LoadCursorW(hInstance, IDC_ARROW);
		wnd.hbrBackground = (HBRUSH)nullptr;
		wnd.lpszMenuName = L"";
		wnd.lpszClassName = L"WND_ENGINE";
		wnd.cbSize = sizeof(WNDCLASSEXW);

		if (!RegisterClassExW(&wnd))
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->RegisterClassEx() Init is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->RegisterClassEx() Init is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create the main window class!");
			return E_FAIL;
		}

#if defined (DEBUG)
		if (!(hwnd = CreateWindowW(wnd.lpszClassName, NameWnd.c_str(), WS_MAXIMIZE | WS_POPUP, 392 /*1024/2-120*/, 160 /*768-608*/,
			1024, 768, NULL, NULL, hInstance, NULL)))
#else
		if (!(hwnd = CreateWindowW(wnd.lpszClassName, NameWnd.c_str(), WS_MAXIMIZE | WS_POPUP, 0, 0, 1024, 768,
			NULL, NULL, hInstance, NULL)))
#endif
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->CreateWindow() Init is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->CreateWindow() Init is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create the main window!");
			return E_FAIL;
		}

		this->NameWnd = NameWnd;
		ClassWND = wnd.lpszClassName;

		UINT createDeviceFlags = 0;
#if defined (DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			auto m_driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &Device, &*featureLevel, &DeviceContext);
			if (SUCCEEDED(hr))
				break;
		}
		if (FAILED(hr))
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->D3D11CreateDeviceAndSwapChain() Init is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->D3D11CreateDeviceAndSwapChain() Init is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create Device And Swap Chain Buffer!");
			return hr;
		}

		Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);

		IDXGIDevice *dxgiDevice = nullptr;
		hr = Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter *adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&dxgiFactory));
				SAFE_RELEASE(adapter);
			}
			SAFE_RELEASE(dxgiDevice);
		}
		else
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->DXGI Factory couldn't be obtained!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->DXGI Factory couldn't be obtained!");
#endif
			Console::LogError("Engine::Init->DXGI Factory couldn't be obtained!");
			return hr;
		}

		hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory2));
		if (dxgiFactory2)
		{
			// DirectX 11.1 or later
			hr = Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void **>(&Device1));
			if (SUCCEEDED(hr))
				DeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void **>(&DeviceContext1));

			ZeroMemory(&SCD1, sizeof(SCD1));
			SCD1.Width = getWorkAreaSize(hwnd).x;
			SCD1.Height = getWorkAreaSize(hwnd).y;
			SCD1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			SCD1.SampleDesc.Count = 4;
			SCD1.SampleDesc.Quality = m4xMsaaQuality - 1;
			SCD1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SCD1.BufferCount = 1;

			hr = dxgiFactory2->CreateSwapChainForHwnd(Device, hwnd, &SCD1, nullptr, nullptr, &SwapChain1);
			if (SUCCEEDED(hr))
				hr = SwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void **>(&SwapChain));

			SAFE_RELEASE(dxgiFactory2);
		}
		else
		{
			// DirectX 11.0 systems
			ZeroMemory(&SCD, sizeof(SCD));
			SCD.BufferCount = 1;
			SCD.BufferDesc.Width = getWorkAreaSize(hwnd).x;
			SCD.BufferDesc.Height = getWorkAreaSize(hwnd).y;
			SCD.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			SCD.BufferDesc.RefreshRate.Numerator = 60;
			SCD.BufferDesc.RefreshRate.Denominator = 1;
			SCD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SCD.OutputWindow = hwnd;
			SCD.SampleDesc.Count = 1;
			SCD.SampleDesc.Quality = m4xMsaaQuality - 1;
			SCD.Windowed = true;

			hr = dxgiFactory->CreateSwapChain(Device, &SCD, &SwapChain);
		}

		ID3D11Texture2D *pBackBuffer = nullptr;
		if (FAILED(hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer)))
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->GetBuffer() Get is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->GetBuffer() Get is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create Back Buffer!");
			return hr;
		}

		if (FAILED(hr = Device->CreateRenderTargetView(pBackBuffer, NULL, &RenderTargetView)))
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->CreateRenderTargetView() Init is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->CreateRenderTargetView() Init is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create Render Target View Buffer!");
			return hr;
		}
		SAFE_RELEASE(pBackBuffer);

		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = getWorkAreaSize(hwnd).x;
		descDepth.Height = getWorkAreaSize(hwnd).y;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = m4xMsaaQuality - 1;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		if (FAILED(hr = Device->CreateTexture2D(&descDepth, NULL, &DepthStencil)))
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->CreateTexture2D() Get is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->CreateTexture2D() Get is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create Deph Stencil Buffer!");
			return hr;
		}

		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Texture2D.MipSlice = 0;

		if (FAILED(hr = Device->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView)))
		{
#if defined (DEBUG)
			DebugTrace("Engine::Init->CreateDepthStencilView() Init is failed!");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("Engine::Init->CreateDepthStencilView() Init is failed!");
#endif
			Console::LogError("Engine: Something is wrong with create Deph Stencil Buffer!");
			return hr;
		}

		DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = true;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		Device->CreateRasterizerState(&rasterDesc, &rasterState);
		DeviceContext->RSSetState(rasterState);

		vp.Width = (float)getWorkAreaSize(hwnd).x;
		vp.Height = (float)getWorkAreaSize(hwnd).y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		DeviceContext->RSSetViewports(1, &vp);

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
	}
	catch (const exception &Catch)
	{
#if defined (DEBUG)
		DebugTrace(string(string("Engine::Init()->catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("Engine::Init()->catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
		Console::LogError(string(string("Engine: Something is wrong with create Init Function!\nReturn Error Text:")
			+ Catch.what()).c_str());
		return hr;
	}

	return S_OK;
}

ToDo("Need To Rework This!")
void Engine::CountFPS()
{
	if (ui.operator bool() && ui->GetIO())
		fps = ui->GetIO()->Framerate;
}

#include <chrono>
void Engine::Render()
{
	if (!DeviceContext || !Device)
		return;

	CountFPS();

	auto start = chrono::system_clock::now();

	if (keyboard->IsConnected())
	{
		auto state = keyboard->GetState();
		TrackerKeyboard.Update(state);

		if (TrackerKeyboard.pressed.F1)
			ChangeColorBuffer(_Color[rand() % 9 + 1]);

		if (TrackerKeyboard.pressed.F2)
			if (WireFrame)
				WireFrame = false;
			else
				WireFrame = true;

		if (TrackerKeyboard.pressed.F3)
			if (IsSimulation)
				IsSimulation = false;
			else
				IsSimulation = true;

		if (TrackerKeyboard.pressed.F5)
			PhysX->AddNewActor(camera->GetEyePt(), Vector3::One, 1);

		if (console.operator bool() && TrackerKeyboard.pressed.OemTilde)
			console->OpenConsole();

		if (TrackerKeyboard.pressed.Escape)
			Quit();

		if (TrackerKeyboard.pressed.F6)
			if (DrawGrid)
				DrawGrid = false;
			else
				DrawGrid = true;

		if (TrackerKeyboard.pressed.F7)
			if (DrawCamSphere)
				DrawCamSphere = false;
			else
				DrawCamSphere = true;
	}
	if (gamepad->GetState(0).IsConnected())
	{
		auto state = gamepad->GetState(0);
		TrackerGamepad.Update(state);
	}

	if (Sound.operator bool())
		Sound->Update();

	//lua->Update();

	ClearRenderTarget();

	mainActor->Render(getframeTime());

#if defined (DEBUG)
	if (DrawGrid)
		dDraw->DrawGrid(Vector3(500.f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 500.f), Vector3::Zero, 300,
		(Vector4)Colors::Teal);

	if (Sound.operator bool())
	{
		BoundingSphere sphere;
		sphere.Center = Sound->getSoundPosition();
		dDraw->Draw(sphere, (Vector4)Colors::Red);
	}
#endif

	PhysX->Simulation(getframeTime());

#if defined (DEBUG)
	ui->Begin();

	auto Dial1 = ui->getDialog("Main");
	if (Dial1.operator bool() && !Dial1->GetTitle().empty())
	{
		float CamPos[3] = { mainActor->getPosition().x, mainActor->getPosition().y, mainActor->getPosition().z };
		Dial1->getComponents()->Label.front()->ChangeText(string((boost::format(
			string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)\nIs WireFrame? : %b\nIs Simulation PhysX : %b\n") +
			string("Resolution Window: W:%f, H:%f")) % fps % CamPos[0] % CamPos[1] % CamPos[2] % WireFrame % !IsSimulation %
			getWorkAreaSize(hwnd).x % getWorkAreaSize(hwnd).y).str()));

		if (Sound.operator bool())
		{
			if (Dial1->getComponents()->CollpsHeader.back()->getComponent()->Btn.front()->IsClicked())
				Sound->doPlay();
			if (Dial1->getComponents()->CollpsHeader.back()->getComponent()->Btn.at(1)->IsClicked())
				Sound->doStop();
			if (Dial1->getComponents()->CollpsHeader.back()->getComponent()->Btn.back()->IsClicked())
				Sound->doPause();
		}

		Dial1->Render();
	}
	auto Dial2 = ui->getDialog("List Of Game Objects");
	if (Dial2.operator bool() && !Dial2->GetTitle().empty())
		Dial2->Render();

	if (ui->getDialog("Console").operator bool() && !ui->getDialog("Console")->GetTitle().empty())
		console->Render();

	ui->FrameEnd();
#endif

	if (model.operator bool())
	{
		model->setPosition(Vector3::One);
		model->Render(camera->GetViewMatrix(), camera->GetProjMatrix());
	}

#if defined(NEEDED_DEBUG_INFO)
	Device->QueryInterface(IID_ID3D11Debug, (void **) &debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

	SwapChain->Present(0, 0);

	auto end = chrono::system_clock::now();

	chrono::duration<float> elapsed_seconds = end - start;
	frameTime = elapsed_seconds.count();
}

void Engine::Destroy()
{
	::DestroyWindow(hwnd);
	::UnregisterClassW(ClassWND.c_str(), hInstance);

	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(SwapChain1);

	SAFE_RELEASE(DeviceContext1);
	SAFE_RELEASE(Device1);

	SAFE_RELEASE(DepthStencil);
	SAFE_RELEASE(DepthStencilView);

	SAFE_RELEASE(dxgiFactory);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);

	::CoUninitialize();
}

ID3D11Device *Engine::getDevice()
{
	if (Device)
		return Device;
	else
	{
#if defined (DEBUG)
		DebugTrace("Engine::GetDevice() Get is failed");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("GetDevice() Get is failed!!!");
#endif
		Console::LogError("Engine: Error with get Main Device Context! Maybe You Forgotten To Create Main Device?");
		return nullptr;
	}
}
ID3D11DeviceContext *Engine::getDeviceContext()
{
	if (DeviceContext)
		return DeviceContext;
	else
	{
#if defined (DEBUG)
		DebugTrace("Engine::GetDeviceContext() Get is failed");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("GetDeviceContext() Get is failed!!!");
#endif
		Console::LogError("Engine: Error with get Device Context! Maybe You Forgotten To Create Devices?");
		return nullptr;
	}
}

IDXGISwapChain *Engine::getSwapChain()
{
	if (SwapChain)
		return SwapChain;
	else
	{
#if defined (DEBUG)
		DebugTrace("Engine::GetSwapChain() Get is failed");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("GetSwapChain() Get is failed!!!");
#endif
		Console::LogError("Engine: Error with get Swap Chain Buffer! Maybe You Forgotten To Create Devices?");
		return nullptr;
	}
}

ID3D11RenderTargetView *Engine::getTargetView()
{
	if (RenderTargetView)
		return RenderTargetView;
	else
	{
#if defined (DEBUG)
		DebugTrace("Engine::GetTargetView() Get is failed");
#endif
#if defined (ExceptionWhenEachError)
		throw exception("GetTargetView() Get is failed!!!");
#endif
		Console::LogError("Engine: Error with get Target View Buffer! Maybe You Forgotten To Create Devices?");
		return nullptr;
	}
}

HRESULT Engine::ResizeWindow(WPARAM wParam)
{
	HRESULT hr = S_OK;
	if (!SwapChain || wParam == SIZE_MINIMIZED)
		return E_FAIL;

	ID3D11Texture2D *pBackBuffer = nullptr;
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DepthStencilView);
	SAFE_RELEASE(DepthStencil);

	//sd.Flags = !vsettings.windowed ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

	SCD.BufferDesc.Width = getWorkAreaSize(hwnd).x;
	SCD.BufferDesc.Height = getWorkAreaSize(hwnd).y;

	if (FAILED(hr = SwapChain->ResizeBuffers(
		SCD.BufferCount, SCD.BufferDesc.Width,
		SCD.BufferDesc.Height, SCD.BufferDesc.Format,
		SCD.Flags)))
	{
#if defined (DEBUG)
		DebugTrace(string(string("ResizeWindow()->ResizeBuffers() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("ResizeWindow()->ResizeBuffers() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
		Console::LogError(string(string("ResizeWindow: Something is wrong with Resize Buffers Swap Chain!\n")
			+ string("Return Error Text:") + to_string(hr)).c_str());
		return hr;
	}

	DXGI_MODE_DESC md;
	md.Width = SCD.BufferDesc.Width;
	md.Height = SCD.BufferDesc.Height;
	md.RefreshRate = SCD.BufferDesc.RefreshRate;
	md.Format = SCD.BufferDesc.Format;
	md.Scaling = SCD.BufferDesc.Scaling;
	md.ScanlineOrdering = SCD.BufferDesc.ScanlineOrdering;

	if (FAILED(hr = SwapChain->ResizeTarget(&md)))
	{
#if defined (DEBUG)
		DebugTrace(string(string("ResizeWindow()->ResizeTarget() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("ResizeWindow()->ResizeTarget() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
		Console::LogError(string(string("ResizeWindow: Something is wrong with Resize Target Swap Chain!\n")
			+ string("Return Error Text:") + to_string(hr)).c_str());
		return hr;
	}

	if (FAILED(hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer))))
	{
#if defined (DEBUG)
		DebugTrace(string(string("ResizeWindow()->GetBuffer() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("ResizeWindow()->GetBuffer() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
		Console::LogError(string(string("ResizeWindow: Something is wrong with create Swap Chain!\n")
			+ string("Return Error Text:") + to_string(hr)).c_str());
		return hr;
	}

	if (FAILED(hr = Device->CreateRenderTargetView(pBackBuffer, 0, &RenderTargetView)))
	{
#if defined (DEBUG)
		DebugTrace(string(string("ResizeWindow()->CreateRenderTargetView() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("ResizeWindow()->CreateRenderTargetView() Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
		Console::LogError(string(string("ResizeWindow: Something is wrong with create Render Target Buffer!\n")
			+ string("Return Error Text:") + to_string(hr)).c_str());
		return hr;
	}

	descDepth.Width = SCD.BufferDesc.Width;
	descDepth.Height = SCD.BufferDesc.Height;

	if (FAILED(hr = Device->CreateTexture2D(&descDepth, 0, &DepthStencil)))
	{
#if defined (DEBUG)
		DebugTrace(string(string("ResizeWindow()->CreateTexture2D() Get Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("ResizeWindow()->CreateTexture2D() Get Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
		Console::LogError(string(string("ResizeWindow: Something is wrong with create Depth Stencil Buffer!\n")
			+ string("Return Error Text:") + to_string(hr)).c_str());
		return hr;
	}

	if (FAILED(hr = Device->CreateDepthStencilView(DepthStencil, 0, &DepthStencilView)))
	{
#if defined (DEBUG)
		DebugTrace(string(string("ResizeWindow()->CreateDepthStencilView() Init Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("ResizeWindow()->CreateDepthStencilView() Init Is Failed!\nReturn Error Text:")
			+ to_string(hr)).c_str());
#endif
		Console::LogError(string(string("ResizeWindow: Something is wrong with create Depth Stencil Buffer!\n")
			+ string("Return Error Text:") + to_string(hr)).c_str());
		return hr;
	}

	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	vp.Width = (float)getWorkAreaSize(GetHWND()).x;
	vp.Height = (float)getWorkAreaSize(GetHWND()).y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);
	pBackBuffer->Release();

	return S_OK;
}

POINT Engine::getWorkAreaSize(HWND hwnd)
{
	RECT rc = { 0, 0, 0, 0 };
	POINT Rect = { 0, 0 };
	GetClientRect(hwnd, &rc);
	Rect.x = rc.right - rc.left; // Width
	Rect.y = rc.bottom - rc.top; // Height
	return Rect;
}

float Engine::getframeTime()
{
	if (frameTime == 0.f)
		return 0.3f;
	else
		return frameTime;
}

void Engine::StackTrace(LPCSTR Error)
{
#if defined (DEBUG)
	DebugTrace("\n***********ERROR IN XML FILE***********\n");
	DebugTrace(string(Error).c_str());
	DebugTrace("\n***********ERROR IN XML FILE***********\n");
#endif
#if defined (ExceptionWhenEachError)
	throw exception(string(string("ERROR IN XML FILE!\n") + Error).c_str());
#endif
	Console::LogError(string("Engine: ERROR IN XML FILE!\n") + Error);
}

LRESULT Engine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Application->getUI().operator bool())
		if (UI::MsgProc(hWnd, uMsg, wParam, lParam))
			return true;

	switch (uMsg)
	{
	case WM_SIZE:
		if (uMsg != WM_DESTROY && Application->getUI().operator bool())
		{
			ThrowIfFailed(ResizeWindow(wParam));
			UI::ResizeWnd();
		}
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(uMsg, wParam, lParam);
		break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
