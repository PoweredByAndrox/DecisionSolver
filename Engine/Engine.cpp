#include "pch.h"

#include "UI.h"
#include "Models.h"
#include "Actor.h"
#include "Shaders.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"
#include "CLua.h"
#include "Picking.h"
#include "DebugDraw.h"
#include "Levels.h"
#include "CutScene.h"
#include "Camera.h"
#include "Multiplayer.h"
#include "SDKInterface.h"
#include "File_system.h"

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

shared_ptr<Mouse> Engine::mouse = make_shared<Mouse>();
shared_ptr<Keyboard> Engine::keyboard = make_shared<Keyboard>();
shared_ptr<GamePad> Engine::gamepad = make_shared<GamePad>();

shared_ptr<SDKInterface> SDK = make_shared<SDKInterface>();

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

HRESULT Engine::Init(string NameWnd, HINSTANCE hInstance)
{
	this->hInstance = hInstance;
	ZeroMemory(&wnd, sizeof(WNDCLASSEXA));
	wnd.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_CLASSDC;
	wnd.lpfnWndProc = (WNDPROC)Engine::WndProc;
	wnd.hInstance = hInstance;
	wnd.hIcon = ::LoadIconA(hInstance, (LPCSTR)IDI_ICON1);
	wnd.hIconSm = wnd.hIcon;
	wnd.hCursor = ::LoadCursorA(hInstance, (LPCSTR)IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wnd.lpszClassName = L"WND_ENGINE";
	wnd.cbSize = sizeof(WNDCLASSEXA);

	if (!::RegisterClassExW(&wnd))
	{
		LogError("Engine::Init->RegisterClassEx() Init is failed!",
			"Engine::Init->RegisterClassEx() Init is failed!",
			"Engine: Something is wrong with create the main window class!");
		return E_FAIL;
	}
	if (!(hwnd = ::CreateWindowA("WND_ENGINE", "DecisionSolver", WS_MAXIMIZE | WS_POPUP,
		392, 160, 1024, 768, NULL, NULL, hInstance, NULL)))
	{
		LogError("Engine::Init->CreateWindow() Init is failed!",
			"Engine::Init->CreateWindow() Init is failed!",
			"Engine: Something is wrong with create the main window!");
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
		hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags,
			featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &Device, &*featureLevel,
			&DeviceContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		LogError("Engine::Init->D3D11CreateDeviceAndSwapChain() Init is failed!",
			"Engine::Init->D3D11CreateDeviceAndSwapChain() Init is failed!",
			"Engine: Something is wrong with create Device And Swap Chain Buffer!");
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
		LogError("Engine::Init->DXGI Factory couldn't be obtained!",
			"Engine::Init->DXGI Factory couldn't be obtained!",
			"Engine::Init->DXGI Factory couldn't be obtained!");
		return hr;
	}

	UINT WidthWindow = getWorkAreaSize(hwnd).x, HeightWindow = getWorkAreaSize(hwnd).y;

	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void **>(&Device1));
		if (SUCCEEDED(hr))
			DeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1),
				reinterpret_cast<void **>(&DeviceContext1));

		ZeroMemory(&SCD1, sizeof(SCD1));
		SCD1.Width = WidthWindow;
		SCD1.Height = HeightWindow;
		SCD1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SCD1.SampleDesc.Count = 4;
		SCD1.SampleDesc.Quality = m4xMsaaQuality - 1;
		SCD1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SCD1.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(Device, hwnd, &SCD1, nullptr, nullptr, &SwapChain1);
		if (SUCCEEDED(hr))
			hr = SwapChain1->QueryInterface(__uuidof(IDXGISwapChain),
				reinterpret_cast<void **>(&SwapChain));

		SAFE_RELEASE(dxgiFactory2);
	}
	else
	{
		// DirectX 11.0 systems
		ZeroMemory(&SCD, sizeof(SCD));
		SCD.BufferCount = 1;
		SCD.BufferDesc.Width = WidthWindow;
		SCD.BufferDesc.Height = HeightWindow;
		SCD.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SCD.BufferDesc.RefreshRate.Numerator = 75;
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
		LogError("Engine::Init->GetBuffer() Get is failed!", "Engine::Init->GetBuffer() Get is failed!",
			"Engine: Something is wrong with create Back Buffer!");
		return hr;
	}

	if (FAILED(hr = Device->CreateRenderTargetView(pBackBuffer, nullptr, &RenderTargetView)))
	{
		LogError("Engine::Init->CreateRenderTargetView() Init is failed!",
			"Engine::Init->CreateRenderTargetView() Init is failed!",
			"Engine: Something is wrong with create Render Target View Buffer!");
		return hr;
	}
	SAFE_RELEASE(pBackBuffer);

	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = WidthWindow;
	descDepth.Height = HeightWindow;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 4;
	descDepth.SampleDesc.Quality = m4xMsaaQuality - 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(hr = Device->CreateTexture2D(&descDepth, nullptr, &DepthStencil)))
	{
		LogError("Engine::Init->CreateTexture2D() Get is failed!",
			"Engine::Init->CreateTexture2D() Get is failed!",
			"Engine: Something is wrong with create Deph Stencil Buffer!");
		return hr;
	}

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(hr = Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
	{
		LogError("Engine::Init->CreateDepthStencilState() Get is failed!",
			"Engine::Init->CreateDepthStencilState() Get is failed!",
			"Engine: Something is wrong with create Deph Stencil State Buffer!");
		return hr;
	}
	DeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	if (FAILED(hr = Device->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView)))
	{
		LogError("Engine::Init->CreateDepthStencilView() Init is failed!",
			"Engine::Init->CreateDepthStencilView() Init is failed!",
			"Engine: Something is wrong with create Deph Stencil Buffer!");
		return hr;
	}

	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	auto WF = Render_Buffer::CreateWF();
	RsWF = WF.at(0);
	RsNoWF = WF.at(1);

	vp.Width = (float)WidthWindow;
	vp.Height = (float)HeightWindow;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(1, &vp);

	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);
	return S_OK;
}

void Engine::Render()
{
	if (!DeviceContext || !Device)
		return;

	frameTime = float(Timer->GetElapsedSeconds());

	//Timer->SetFixedTimeStep(false);
	//Timer->SetTargetElapsedSeconds(0.001f); // 0.2 == 5 FPS, 0.1 == 10FPS
	fps = float(Timer->GetFramesPerSecond());
	
//	Console::LogInfo((boost::format("\nFramTime: %f\nFPS: %f") % frameTime % fps).str().c_str());

	if (Pick.operator bool())
	{
		if (Pick->isPicked() && mouse->GetState().leftButton)
			Pick->tick();
		else if (!Pick->isPicked() && mouse->GetState().leftButton)
			Pick->UpdatePick();
		else
			Pick->ReleasePick();
	}

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
	else if (gamepad->GetState(0).IsConnected())
	{
		auto state = gamepad->GetState(0);
		TrackerGamepad.Update(state);
	}

	if (lua.operator bool())
		lua->Update();
	
	if (TrackerKeyboard.pressed.F4 && CScene.operator bool())
	{
		CScene->Reset();
		Console::PushCMD("reinit_lua");
		CScene->Restart();
		DrawGrid = true;
	}

	ClearRenderTarget();

	if (ui.operator bool())
		ui->Begin();

	mainActor->Render(frameTime);

	if (CScene.operator bool())
		CScene->Update();

	//if (dDraw.operator bool())
	//{
	//	if (DrawGrid)
	//		dDraw->DrawGrid(Vector3(500.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 500.0f), Vector3::Zero, 300,
	//		(Vector4)Colors::Teal);
	//	if (Sound.operator bool())
	//	{
	//		BoundingSphere sphere;
	//		sphere.Radius = 50.0f;
	//		sphere.Center = Sound->getSoundPosition();
	//		dDraw->Draw(sphere, (Vector4)Colors::Red);
	//		Sound->Update();
	//	}
	//}

	PhysX->Simulation(frameTime);

#if defined(NEEDED_DEBUG_INFO)
	Device->QueryInterface(IID_ID3D11Debug, (void **)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

	if (Level.operator bool())
		Level->Update(camera->GetViewMatrix(), camera->GetProjMatrix(), frameTime);

	if (ui.operator bool())
	{
		//::ShowCursor(false);

		auto Dial1 = ui->getDialog("SDKDIAL");
		if (Dial1.operator bool() && !Dial1->GetTitle().empty())
		{
			float CamPos[] = { mainActor->getPosition().x, mainActor->getPosition().y, mainActor->getPosition().z };
			Dial1->getComponents()->FindComponentLabel("##SDKDIAL_MainLabel")->ChangeText(string((boost::format(
				string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)\nIs WireFrame? : %b\n"
					"Is Simulation PhysX : %b\nResolution Window: W:%f, H:%f\nFrameTime:%f"))
				% fps % CamPos[0] % CamPos[1] % CamPos[2] % WireFrame % !IsSimulation % getWorkAreaSize(hwnd).x %
				getWorkAreaSize(hwnd).y % frameTime).str()));

			auto Comp = Dial1->getComponents()->FindComponentCHeader("##SDKDIAL_ClpSound")->getComponents().front();
			auto Comb = Comp->FindComponentCombo("Select Track!");
			if (Sound && Comp && Comb)
			{
				if (Comp->FindComponentBtn("##Track_Start")->IsClicked() && Comb->GetSelect() != "None")
				{
					Sound->PlayFile(Comb->GetSelect(), true);
					Sound->doPlay();
				}
				if (Comp->FindComponentBtn("##Track_Stop")->IsClicked())
					Sound->doStop();
				if (Comp->FindComponentBtn("##Track_Pause")->IsClicked())
					Sound->doPause();
			}

			if (Comb->IsMouseSelected())
			{
				Comb->ClearItems();
				FS->RescanFilesByType(_TypeOfFile::SOUNDS);
				auto SoundFiles = FS->GetFileByType(_TypeOfFile::SOUNDS);

				for (size_t i = 0; i < SoundFiles.size(); i++)
				{
					string FNameWithoutExt = SoundFiles.at(i)->FileA;
					deleteWord(FNameWithoutExt, SoundFiles.at(i)->ExtA);
					Comb->AddItem(FNameWithoutExt);
				}
			}

			Dial1->Render();
		}

		auto DialCons = ui->getDialog("Console");
		if (DialCons.operator bool() && !DialCons->GetTitle().empty())
			console->Render();

		//auto DialMP = ui->getDialog("MPConnection");
		//if (DialMP.operator bool() && !DialMP->GetTitle().empty() && MPL.operator bool())
		//{
		//	auto Comp = DialMP->getComponents();
		//	if (Comp->FindComponentBtn("##MP_Send")->IsClicked())
		//		MPL->Send("Chat: " + Comp->FindComponentIText("##MP_IText")->GetText(),
		//			MPL->getCurrentUser()->getSocket(),
		//			MPL->getCurrentUser()->getAddresStruct());
		//	if (Comp->FindComponentBtn("##MP_CreateServer")->IsClicked())
		//		MPL->getServer()->Create();
		//	if (Comp->FindComponentBtn("##MP_Connect")->IsClicked())
		//		MPL->Connect();
		//	if (Comp->FindComponentBtn("##MP_Disconnect")->IsClicked())
		//		MPL->getCurrentUser()->Disconnect();
		//	if (Comp->FindComponentBtn("##MP_Clear")->IsClicked())
		//		Comp->FindComponentITextMul("##MP_Chat")->ClearText();
		//	MPL->Update();
		//	//if (!str.empty())
		//	//	Comp->Itextmul.back()->AddCLText(Type::Normal, string("Echo: ") + str);
		//	DialMP->Render();
		//}

		if (SDK)
			SDK->Render();
	}
	
	if (ui.operator bool())
		ui->FrameEnd();

	SwapChain->Present(0, 0);
}

void Engine::LogError(string DebugText, string ExceptionText, string LogText)
{
#if defined (DEBUG)
	if (!DebugText.empty())
		OutputDebugStringA(DebugText.c_str());
#endif
#if defined (ExceptionWhenEachError)
	if (!ExceptionText.empty())
		throw exception(ExceptionText.c_str());
#endif
	if (!LogText.empty())
		Console::LogError(LogText);
}

void Engine::Destroy()
{
	::DestroyWindow(hwnd);
	::UnregisterClassW(ClassWND.c_str(), hInstance);

	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(SwapChain1);

	SAFE_RELEASE(m_depthStencilState);

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
		LogError("Engine::GetDevice() Get is failed", "GetDevice() Get is failed!!!", 
			"Engine: Error with get Main Device Context! Maybe You Forgotten To Create Main Device?");
		return nullptr;
	}
}
ID3D11DeviceContext *Engine::getDeviceContext()
{
	if (DeviceContext)
		return DeviceContext;
	else
	{
		LogError("Engine::GetDeviceContext() Get is failed", "GetDeviceContext() Get is failed!!!",
			"Engine: Error with get Device Context! Maybe You Forgotten To Create Devices?");
		return nullptr;
	}
}

IDXGISwapChain *Engine::getSwapChain()
{
	if (SwapChain)
		return SwapChain;
	else
	{
		LogError("Engine::GetSwapChain() Get is failed", "GetSwapChain() Get is failed!!!",
			"Engine: Error with get Swap Chain Buffer! Maybe You Forgotten To Create Devices?");
		return nullptr;
	}
}

ID3D11RenderTargetView *Engine::getTargetView()
{
	if (RenderTargetView)
		return RenderTargetView;
	else
	{
		LogError("Engine::GetTargetView() Get is failed", "GetTargetView() Get is failed!!!",
			"Engine: Error with get Target View Buffer! Maybe You Forgotten To Create Devices?");
		return nullptr;
	}
}

void Engine::ClearRenderTarget()
{
	DeviceContext->ClearRenderTargetView(RenderTargetView, _ColorBuffer);
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
		LogError((boost::format("ResizeWindow()->ResizeBuffers() Is Failed!\nReturn Error Text: %s")
			% to_string(hr)).str(),
			(boost::format("ResizeWindow()->ResizeBuffers() Is Failed!\nReturn Error Text: %s")
				% to_string(hr)).str(),
				(boost::format("ResizeWindow: Something is wrong with Resize Buffers Swap Chain!\n"\
					"Return Error Text: %s")
					% to_string(hr)).str());
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
		LogError((boost::format("ResizeWindow()->ResizeTarget() Is Failed!\nReturn Error Text: %s")
			% to_string(hr)).str(),
			(boost::format("ResizeWindow()->ResizeTarget() Is Failed!\nReturn Error Text: %s")
				% to_string(hr)).str(),
				(boost::format("ResizeWindow: Something is wrong with Resize Target Swap Chain!\n"\
					"Return Error Text: %s")
					% to_string(hr)).str());
		return hr;
	}

	if (FAILED(hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer))))
	{
		LogError((boost::format("ResizeWindow()->GetBuffer() Is Failed!\nReturn Error Text: %s")
			% to_string(hr)).str(),
			(boost::format("ResizeWindow()->GetBuffer() Is Failed!\nReturn Error Text: %s")
				% to_string(hr)).str(),
				(boost::format("ResizeWindow: Something is wrong with create Target Swap Chain!\n"\
					"Return Error Text: %s")
					% to_string(hr)).str());
		return hr;
	}

	if (FAILED(hr = Device->CreateRenderTargetView(pBackBuffer, 0, &RenderTargetView)))
	{
		LogError((boost::format("ResizeWindow()->CreateRenderTargetView() Is Failed!\nReturn Error Text: %s")
			% to_string(hr)).str(),
			(boost::format("ResizeWindow()->CreateRenderTargetView() Is Failed!\nReturn Error Text: %s")
				% to_string(hr)).str(),
				(boost::format("ResizeWindow: Something is wrong with create Render Target Buffer!\n"\
					"Return Error Text: %s")
					% to_string(hr)).str());
		return hr;
	}

	descDepth.Width = SCD.BufferDesc.Width;
	descDepth.Height = SCD.BufferDesc.Height;

	if (FAILED(hr = Device->CreateTexture2D(&descDepth, 0, &DepthStencil)))
	{
		LogError((boost::format("ResizeWindow()->CreateTexture2D() Is Failed!\nReturn Error Text: %s")
			% to_string(hr)).str(),
			(boost::format("ResizeWindow()->CreateTexture2D() Is Failed!\nReturn Error Text: %s")
				% to_string(hr)).str(),
				(boost::format("ResizeWindow: Something is wrong with create Depth Stencil Buffer!\n"\
					"Return Error Text: %s")
					% to_string(hr)).str());
		return hr;
	}

	if (FAILED(hr = Device->CreateDepthStencilView(DepthStencil, 0, &DepthStencilView)))
	{
		LogError((boost::format("ResizeWindow()->CreateDepthStencilView() Is Failed!\nReturn Error Text: %s")
			% to_string(hr)).str(),
			(boost::format("ResizeWindow()->CreateDepthStencilView() Is Failed!\nReturn Error Text: %s")
				% to_string(hr)).str(),
				(boost::format("ResizeWindow: Something is wrong with create View Depth Stencil Buffer!\n"\
					"Return Error Text: %s")
					% to_string(hr)).str());
		return hr;
	}

	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	vp.Width = (float)getWorkAreaSize(hwnd).x;
	vp.Height = (float)getWorkAreaSize(hwnd).y;
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
	return frameTime;
}

void Engine::StackTrace(LPCSTR Error)
{
#if defined (DEBUG)
	OutputDebugStringA("\n***********ERROR IN XML FILE***********\n");
	OutputDebugStringA(string(Error).c_str());
	OutputDebugStringA("\n***********ERROR IN XML FILE***********\n");
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
			ResizeWindow(wParam);
			UI::ResizeWnd();
		}
	break;

	case WM_DESTROY:
		Quit();
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
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return true;
}

//	Work with LUA Scripts
bool Engine::IsKeyboardDown(int Key)
{
	if (!keyboard.operator bool() && !keyboard->IsConnected())
		return false;

	return keyboard->GetState().IsKeyDown((Keyboard::Keys)Key);
}

bool Engine::IsKeyboardUp(int Key)
{
	if (!keyboard.operator bool() && !keyboard->IsConnected())
		return false;

	return keyboard->GetState().IsKeyUp((Keyboard::Keys)Key);
}

bool Engine::IsMouseLeft()
{
	if (!mouse.operator bool() && !mouse->IsConnected())
		return false;

	return mouse->GetState().leftButton;
}

bool Engine::IsMouseRight()
{
	if (!mouse.operator bool() && !mouse->IsConnected())
		return false;

	return mouse->GetState().rightButton;
}
