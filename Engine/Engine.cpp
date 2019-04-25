#include "pch.h"

#include "UI.h"
#include "Models.h"
#include "Actor.h"
#include "Shaders.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"
#include "CLua.h"
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

HRESULT Engine::Init(wstring NameWnd, HINSTANCE hInstance)
{
	try
	{
		wnd.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_CLASSDC;
		wnd.lpfnWndProc = (WNDPROC)Engine::WndProc;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hInstance = hInstance;
		wnd.hIcon = LoadIconW(hInstance, IDI_WINLOGO);
		wnd.hIconSm = wnd.hIcon;
		wnd.hCursor = LoadCursorW(hInstance, IDC_ARROW);
		wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wnd.lpszMenuName = NULL;
		wnd.lpszClassName = L"WND_ENGINE";
		wnd.cbSize = sizeof(WNDCLASSEXW);

		if (!RegisterClassExW(&wnd))
		{
			DebugTrace("Engine::Init()->RegisterClassEx() is failed");
			throw exception("Init is failed!!!");
			return E_FAIL;
		}

		if (!(hwnd = CreateWindowW(wnd.lpszClassName, NameWnd.c_str(), WS_CLIPSIBLINGS | WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
			WS_SYSMENU | WS_CAPTION, 392 /*1024/2-120*/, 160 /*768-608*/, 1024, 768, NULL, NULL, hInstance, NULL)))
		{
			DebugTrace("Engine::Init()->CreateWindow() is failed");
			throw exception("Init is failed!!!");
			return E_FAIL;
		}

		this->NameWnd = NameWnd;
		ClassWND = wnd.lpszClassName;

		UINT createDeviceFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
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
			DebugTrace("Engine::Init->D3D11CreateDeviceAndSwapChain() failed.");
			throw exception("Create failed!!!");
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
				adapter->Release();
			}
			SAFE_RELEASE(dxgiDevice);
		}
		else
		{
			DebugTrace("Engine::Init->DXGI Factory couldn't be obtained.");
			throw exception("Create failed!!!");
		}

		hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory2));
		if (dxgiFactory2)
		{
			// DirectX 11.1 or later
			hr = Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void **>(&Device1));
			if (SUCCEEDED(hr))
				(void)DeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void **>(&DeviceContext1));

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
			{
				hr = SwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void **>(&SwapChain));
			}

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
		if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBuffer)))
		{
			DebugTrace("Engine::Init->GetBuffer() failed.");
			throw exception("Get failed!!!");
		}

		if (FAILED(Device->CreateRenderTargetView(pBackBuffer, NULL, &RenderTargetView)))
		{
			DebugTrace("Engine::Init->CreateRenderTargetView() failed.");
			throw exception("Create failed!!!");
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

		if (FAILED(Device->CreateTexture2D(&descDepth, NULL, &DepthStencil)))
		{
			DebugTrace("Engine::Init->CreateTexture2D() failed.");
			throw exception("Create failed!!!");
		}

		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Texture2D.MipSlice = 0;

		if (FAILED(Device->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView)))
		{
			DebugTrace("Engine::Init->CreateDepthStencilView() failed.");
			throw exception("Create failed!!!");
		}

		DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
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

		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	}
	catch (const exception &Catch)
	{
		DebugTrace(string(string("Engine: Init is failed. ") + string(Catch.what())).c_str());
		throw exception("Init is failed!!!");
		return E_FAIL;
	}
}

void Engine::Run()
{
	ClearRenderTarget();

	Render();

	mainActor->Render(frameTime);

	PhysX->Simulation(frameTime);

	//ui->Begin();

	//ui->Render();

	//ui->getDialogs().front()->getLabels().front()->ChangeText(string((boost::format(
	//	string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)\nIs WireFrame? : %b\nIs Simulation PhysX : %b\n"))
	//	% fps % mainActor->getPosition().x % mainActor->getPosition().y % mainActor->getPosition().z % WireFrame % !PauseSimulation).str()));

	//if (ui->getDialogs().front()->getCollapsHeaders().back()->getButtons().front()->IsClicked())
	//	Sound->doPlay();

	//if (ui->getDialogs().front()->getCollapsHeaders().back()->getButtons().at(1)->IsClicked())
	//	Sound->doStop();

	//if (ui->getDialogs().front()->getCollapsHeaders().back()->getButtons().back()->IsClicked())
	//	Sound->doPause();

	//console->Render();

	//ui->FrameEnd();

	SwapChain->Present(0, 0);

	Sound->Update();

	lua->Update();
}

void Engine::Render()
{
	if (!DeviceContext)
		return;

	frameCount++;
	if (GetTime() > 1.0f)
	{
		fps = frameCount;
		frameCount = 0;
		StartTimer();
	}

	frameTime = GetFrameTime();

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
			if (PauseSimulation)
				PauseSimulation = false;
			else
				PauseSimulation = true;

		if (TrackerKeyboard.pressed.OemTilde && console.operator bool())
			console->OpenConsole();
	}
	if (gamepad->GetState(0).IsConnected())
	{
		auto state = gamepad->GetState(0);
		TrackerGamepad.Update(state);
	}

	/*
	Pick->tick();
	if (mouse->GetState().leftButton)
	{
		if (!Pick->isPicked())
			Pick->UpdatePick();
		else
			Pick->ReleasePick();
	}
	*/

#if defined(VISUALIZE_PICKING_RAYS)
	if (mPicking)
	{
		const vector<Picking::Ray> &rays = Pick->getRays();
		PxU32 nbRays = rays.size();
		const RendererColor color(255, 0, 0);
		for (PxU32 i = 0; i < nbRays; i++)
		{
			dDraw->DrawRay(m_batch.get(), rays[i].origin, rays[i].origin + rays[i].dir * 1000.0f, color);
		}
	}
#endif

	//model->setPosition(Vector3::One);
	//model->Render(camera->GetViewMatrix(), camera->GetProjMatrix());

	dDraw->DrawGrid(Vector3(800.f, 0.f, 0.f), Vector3(0.f, 0.f, 800.f), Vector3(0.f, 0.7f, 0.f), 250, (Vector4)Colors::OldLace);

#if defined(NEEDED_DEBUG_INFO)
	Device->QueryInterface(IID_ID3D11Debug, (void **) &debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
}

void Engine::Destroy(HINSTANCE hInstance)
{
	::DestroyWindow(hwnd);
	::UnregisterClassW(ClassWND.c_str(), hInstance);

	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);
	SAFE_RELEASE(SwapChain1);
	SAFE_RELEASE(DeviceContext1);
	SAFE_RELEASE(Device1);
	SAFE_RELEASE(dxgiFactory);
	SAFE_RELEASE(dxgiFactory2);
}

void Engine::ResizeWindow(WPARAM wParam)
{
	if (!SwapChain || wParam == SIZE_MINIMIZED)
		return;

	ID3D11Texture2D *pBackBuffer = nullptr;
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DepthStencilView);
	SAFE_RELEASE(DepthStencil);

	//sd.Flags = !vsettings.windowed ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

	SCD.BufferDesc.Width = getWorkAreaSize(hwnd).x;
	SCD.BufferDesc.Height = getWorkAreaSize(hwnd).y;

	if (FAILED(SwapChain->ResizeBuffers(
		SCD.BufferCount, SCD.BufferDesc.Width,
		SCD.BufferDesc.Height, SCD.BufferDesc.Format,
		SCD.Flags)))
	{
		DebugTrace("Engine::ResizeWindow->ResizeBuffers() failed.");
		throw exception("Resize failed!!!");
	}

	DXGI_MODE_DESC md;
	md.Width = SCD.BufferDesc.Width;
	md.Height = SCD.BufferDesc.Height;
	md.RefreshRate = SCD.BufferDesc.RefreshRate;
	md.Format = SCD.BufferDesc.Format;
	md.Scaling = SCD.BufferDesc.Scaling;
	md.ScanlineOrdering = SCD.BufferDesc.ScanlineOrdering;

	if (FAILED(SwapChain->ResizeTarget(&md)))
	{
		DebugTrace("Engine::ResizeWindow->ResizeTarget() failed.");
		throw exception("Resize failed!!!");
	}

	if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer))))
	{
		DebugTrace("Engine::ResizeWindow->GetBuffer() failed.");
		throw exception("Get failed!!!");
	}

	if (FAILED(Device->CreateRenderTargetView(pBackBuffer, 0, &RenderTargetView)))
	{
		DebugTrace("Engine::ResizeWindow->CreateRenderTargetView() failed.");
		throw exception("Create failed!!!");
	}

	descDepth.Width = SCD.BufferDesc.Width;
	descDepth.Height = SCD.BufferDesc.Height;

	if (FAILED(Device->CreateTexture2D(&descDepth, 0, &DepthStencil)))
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

	vp.Width = (FLOAT)getWorkAreaSize(GetHWND()).x;
	vp.Height = (FLOAT)getWorkAreaSize(GetHWND()).y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);
	pBackBuffer->Release();
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
			ResizeWindow(wParam);
			UI::ResizeWnd();
		}
		return false;
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return false;
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
		return false;
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(uMsg, wParam, lParam);
		return false;
		break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return false;
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
