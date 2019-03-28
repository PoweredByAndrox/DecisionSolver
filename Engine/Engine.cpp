#include "pch.h"
#include "UI.h"
#include "Models.h"
#include "Actor.h"
#include "Shaders.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"

ID3D11Device *Engine::Device = nullptr;
ID3D11DeviceContext *Engine::DeviceContext = nullptr;
IDXGISwapChain *Engine::SwapChain = nullptr;
ID3D11RenderTargetView *Engine::RenderTargetView = nullptr;
ID3D11Texture2D *Engine::DepthStencil = nullptr;
ID3D11DepthStencilView *Engine::DepthStencilView = nullptr;
HWND Engine::hwnd = nullptr;

HRESULT Engine::Init(LPCWSTR NameWnd, HINSTANCE hInstance)
{
	try
	{
		WNDCLASSEXW wnd;
		wnd.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wnd.lpfnWndProc = (WNDPROC)Engine::WndProc;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hInstance = hInstance;
		wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wnd.hIconSm = wnd.hIcon;
		wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wnd.lpszMenuName = NULL;
		wnd.lpszClassName = L"WND_ENGINE";
		wnd.cbSize = sizeof(WNDCLASSEXW);

		if (!RegisterClassEx(&wnd))
		{
			DebugTrace("Engine::Init()->RegisterClassEx() is failed");
			throw exception("Init is failed!!!");
			return E_FAIL;
		}

		if (!(hwnd = CreateWindowW(wnd.lpszClassName, NameWnd, WS_CLIPSIBLINGS | WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
			WS_SYSMENU | WS_CAPTION, 0, 0, 1024, 768, NULL, NULL, hInstance, NULL)))
		{
			DebugTrace("Engine::Init()->CreateWindow() is failed");
			throw exception("Init is failed!!!");
			return E_FAIL;
		}

		this->NameWnd = NameWnd;
		ClassWND = wnd.lpszClassName;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
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
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = getWorkAreaSize(hwnd).x;
		sd.BufferDesc.Height = getWorkAreaSize(hwnd).y;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			auto m_driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &SwapChain, &Device, &*featureLevel, &DeviceContext);
			if (SUCCEEDED(hr))
				break;
		}
		if (FAILED(hr))
		{
			DebugTrace("Engine::Init->D3D11CreateDeviceAndSwapChain() failed.");
			throw exception("Create failed!!!");
		}

		ID3D11Texture2D *pBackBuffer = nullptr;
		if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
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

		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = getWorkAreaSize(hwnd).x;
		descDepth.Height = getWorkAreaSize(hwnd).y;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		if (FAILED(Device->CreateTexture2D(&descDepth, NULL, &DepthStencil)))
		{
			DebugTrace("Engine::Init->CreateTexture2D() failed.");
			throw exception("Create failed!!!");
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		if (FAILED(Device->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView)))
		{
			DebugTrace("Engine::Init->CreateDepthStencilView() failed.");
			throw exception("Create failed!!!");
		}

		DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

		D3D11_VIEWPORT vp;
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

	console->Render();

	PhysX->Simulation(false, frameTime, camera->GetViewMatrix(), camera->GetProjMatrix());

	ui->Begin();

	ui->Render();
	ui->getDialogs().front()->getLabels().front()->ChangeText(string((boost::format(
		string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)\nIs WireFrame? : %b\n"))
		% Application->getFPS() % Application->getActor()->getPosition().x % Application->getActor()->getPosition().y
		% Application->getActor()->getPosition().z % Application->IsWireFrame()).str()));

	if (Application->getUI()->getDialogs().front()->getCollapsHeaders().back()->getButtons().at(0)->IsClicked())
		Application->getSound()->doPlay();

	if (Application->getUI()->getDialogs().front()->getCollapsHeaders().back()->getButtons().at(1)->IsClicked())
		Application->getSound()->doStop();

	if (Application->getUI()->getDialogs().front()->getCollapsHeaders().back()->getButtons().at(2)->IsClicked())
		Application->getSound()->doPause();

	ui->End(WireFrame);

	SwapChain->Present(0, 0);

	Sound->Update();

	if (getKeyboard()->IsConnected())
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
	}
	if (getGamepad()->GetState(0).IsConnected())
	{
		auto state = gamepad->GetState(0);
		TrackerGamepad.Update(state);
	}
}

void Engine::Render()
{
	if (!DeviceContext)
		return;

#if defined(UseConsole)
	console->Render(fElapsedTime);

	if (*console->getState() == Console_STATE::Close)
		V(ui->getDialog()->at(0)->OnRender(fElapsedTime));
#endif // UseConsole

	frameCount++;
	if (GetTime() > 1.0f)
	{
		fps = frameCount;
		frameCount = 0;
		StartTimer();
	}

	frameTime = GetFrameTime();

//	model->Render(camera->GetViewMatrix(), camera->GetProjMatrix(), WireFrame);

	/*
	//ui->SetTextStatic(ui->getDialog()->at(0), 0, &string("Cam Pos: "), PosCam);
	//ui->SetLocationStatic(ui->getDialog()->at(0), 0, 0, PosText += 5, false);

	//ui->SetTextStatic(ui->getDialog()->at(0), 1, &string("FPS: "), DXUTGetFPS());
	//ui->SetLocationStatic(ui->getDialog()->at(0), 1, SCREEN_WIDTH / 2, -3, false);

	//auto ObjStatic = PhysX->GetPhysStaticObject();
	//ui->SetTextStatic(ui->getDialog()->at(0), 2, &string("Count Phys Object: "), PhysX->GetPhysDynamicObject().size() + ObjStatic.size());
	//ui->SetLocationStatic(ui->getDialog()->at(0), 2, 0, PosText += 15, false);

	//ui->SetLocationStatic(ui->getDialog()->at(0), 3, 0, PosText += 15, false);

	//ui->SetTextStatic(ui->getDialog()->at(0), 3, &string("Main Actor Health Is: "), mainActor->getHealthActor());
		auto PhysObj = PhysX->GetPhysDynamicObject();
	if (!PhysObj.empty())
	{
		PhysX->Simulation(StopIT, fElapsedTime, mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

		for (int i = 0; i < m_shape.size(); i++)
		{
			vector<PxQuat> aq;
			vector<PxVec3> pos;
			for (int i1 = 0; i1 < PhysObj.size(); i1++)
			{
				aq.push_back(PhysObj.at(i1)->getGlobalPose().q);
				pos.push_back(PhysObj.at(i1)->getGlobalPose().p);

				m_shape.at(i)->Draw(XMMatrixRotationQuaternion(XMVectorSet(aq[i1].x, aq[i1].y, aq[i1].z, aq[i1].w))
					* XMMatrixTranslation(pos[i1].x, pos[i1].y, pos[i1].z), mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix()//,
					//_Color[rand() % 9 + 1]
				);
			}
		}
	}

	*/

#if defined(NEEDED_DEBUG_INFO)
	Device->QueryInterface(IID_ID3D11Debug, (void **) &debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
}

void Engine::Destroy(HINSTANCE hInstance)
{
	DestroyWindow(hwnd);
	UnregisterClass(ClassWND, hInstance);

	SAFE_DELETE(m_desc);

	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);
}

LRESULT Engine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Application->getUI().operator bool())
		if (UI::MsgProc(hWnd, uMsg, wParam, lParam))
			return true;

	switch (uMsg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED && Application->getUI().operator bool())
		{
			ResizeWindow();
			UI::ResizeWnd();
		}
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_ACTIVATEAPP:
	{
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
	}
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
