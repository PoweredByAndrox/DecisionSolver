#include "pch.h"

#include <d3d11_1.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Models.h"
#include "Physics.h"
#include "File_system.h"
#include "Audio.h"
#include "UI.h"
#include "MainMenu.h"
#include "Picking.h"
#include "Terrain.h"
#include "Render_Buffer.h"
#include "Actor.h"
#include "GameObjects.h"
#include "Console.h"

#include "Levels.h"

using namespace Engine;

#define Never 1

#ifdef DEBUG
	#include <d3d11sdklayers.h>
#endif

#include "d3dx11effect.h"

#ifdef DEBUG
	#pragma comment(lib, "Effects11d.lib")
#else
	#pragma comment(lib, "Effects11.lib")
#endif

Vector3 Eye = { 0.f, 2.5f, 0.f }, At = { 0.0f, 1.0f, 0.f };

HRESULT hr = S_OK;

auto file_system = make_unique<File_system>();
unique_ptr<Models> model = make_unique<Models>();
auto Sound = make_unique<Audio>();
unique_ptr<UI> ui = make_unique<UI>();
auto Pick = make_unique<Picking>();
auto terrain = make_unique<Terrain>();
auto frustum = make_unique<Frustum>();
auto buffers = make_unique<Render_Buffer>();
auto mainActor = make_unique<Actor>();
auto PhysX = make_unique<Physics>();
auto console = make_unique<Console>();

auto Level = make_unique<Levels>();

#ifndef Never_MainMenu
	auto MM = make_unique<MainMenu>();
#endif

static XMVECTORF32 _ColorBuffer = DirectX::Colors::Black;

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

#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3
#define BUTTON_4 4
#define BUTTON_5 5
#define BUTTON_6 6
#define BUTTON_7 7
#define BUTTON_8 8
#define BUTTON_9 9

#define EDITBOX 10

#define STATIC_TEXT 11
#define STATIC_TEXT_2 12
#define STATIC_TEXT_3 13
#define STATIC_TEXT_4 14
#define STATIC_TEXT_5 15

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define Check0 16

//**************
	// Test
bool StopIT = false, InitProgram = false;
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, vector<void *> pUserContext);

void InitApp()
{
	if (!ui->IsInitUI())
		ui->Init(1, file_system->GetResPathW(&wstring(L"dxutcontrols COPY.dds"))->c_str());
	ui->getDialog()->at(0)->SetCallback(OnGUIEvent);

	vector<int> CountOfButtons =
	{
		1, 2, 3,
		4, 5, 6, 7 
	};
	vector<wstring> NameOfButtons = 
	{
		L"Change Buffer Color",
		L"Do Torque Phys Box",
		L"Play Sound",
		L"Pause Sound",
		L"Stop Sound",
		L"Reset Cam Pos",
		L"Create A New Phys Box"
	};
	auto iY = 10;
	vector<int> PositionYButtons =
	{
		iY,
		iY += 24,
		iY += 24,
		iY += 24,
		iY += 24,
		iY += 24,
		iY += 24
	};
	vector<int> KeysButtons =
	{
		VK_F1,
		VK_F2,
		VK_F3,
		VK_F4,
		VK_F5,
		VK_F6,
		VK_F7,
		VK_F8
	};
	vector<int> CountOfStatics =
	{
		8, 9, 10, 11
	};
	vector<wstring> NameOfStatics =
	{
		L"SomeText#1",
		L"SomeText#2",
		L"SomeText#4",
		L"SomeText#5",
	};
	iY = 10;
	vector<int> PositionYStatics =
	{
		iY,
		iY += 24,
		iY += 24,
		iY += 24,
	};
	vector<int> X = { 35, 35, 35, 35 }, W = { 125, 125, 125, 125 }, H = { 22, 22, 22, 22 };
	ui->AddStatic_Mass(ui->getDialog()->at(0), &CountOfStatics, &NameOfStatics, &X, &PositionYStatics, &W, &H);
	ui->AddButton_Mass(ui->getDialog()->at(0), &CountOfButtons, &NameOfButtons, &X, &PositionYButtons, &KeysButtons);

	ui->AddCheckBox(ui->getDialog()->at(0), ui->getAllComponentsCount() + 1, &wstring(L"Enable/Disable XAxis Movement Mode Camera (Def: X mov is true)"), 0, 0, 125, 22, 1);
	
#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
	if (!Sound->IsInitSoundSystem())
		Sound->Init();
	Sound->AddNewSound();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS

#ifndef Never_MainMenu
	if (!MM->IsInitMainMenu())
		MM->Init(Sound.get());

	MM->getDlgMM()->SetCallback(OnGUIEvent); 
	MM->getDlgAUD()->SetCallback(OnGUIEvent);
	MM->getDlgVID()->SetCallback(OnGUIEvent);
#endif

	//g_Camera.SetClipToBoundary(true, &Vector3(4, 6, 3), &Vector3(1, 2, 5));

	if (!PhysX->IsPhysicsInit())
		PhysX->Init();
	if (!mainActor->IsInit())
	{
		mainActor->Init(PhysX.get());
		mainActor->SetupCamera();
	}

	//g_Camera->SetChangeFOV(true);

	InitProgram = true;
}

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo,
	UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed,
	void* pUserContext)
{
	return true;
}

bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings *pDeviceSettings, void* pUserContext)
{
	return true;
}

vector<unique_ptr<GeometricPrimitive>> m_shape;

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, vector<void *> pUserContext)
{
	switch (nControlID)
	{
	case BUTTON_1:
		_ColorBuffer = _Color[rand() % 9 + 1];
		break;
	case BUTTON_2:
	{
		auto PosCam = PxVec3(mainActor->getPosition().x, mainActor->getPosition().y, mainActor->getPosition().z);
		auto Obj = PhysX->GetPhysDynamicObject();

		if (Obj.empty())
			break;
		PhysX->AddTorque(Obj.at(rand() % Obj.size()), PosCam,
			PxForceMode::Enum::eIMPULSE);
		PhysX->AddTorque(Obj.at(rand() % Obj.size()), PosCam,
			PxForceMode::Enum::eFORCE);
		PhysX->AddTorque(Obj.at(rand() % Obj.size()), PosCam,
			PxForceMode::Enum::eVELOCITY_CHANGE);
		break;
	}
	case BUTTON_3:
#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		Sound->doPlay();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		mainActor->Hit(1.f);
		break;
	case BUTTON_4:
#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		Sound->doPause();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		mainActor->ChangeHealth(4.f, '+');
		break;
	case BUTTON_5:
#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		Sound->doStop();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		mainActor->ChangeHealth(0.f, 'G');
		break;
	case BUTTON_6:
		mainActor->getObjCamera()->setPosCam(Eye);
		break;
	case BUTTON_7:
		PhysX->AddNewActor(Vector3(mainActor->getPosition().x, mainActor->getPosition().y + 10.f, mainActor->getPosition().z),
			Vector3(0.5f, 0.5f, 0.5f), rand() % 90 + 1);
		m_shape.push_back(GeometricPrimitive::CreateCube(DXUTGetD3D11DeviceContext(), 1.0f, false));
		break;
	case Check0:
		if (!ui->getCheckBoxs().empty())
		{
			auto ObjCheck = ui->getDialog()->at(0)->GetCheckBox(ui->getCheckBoxs().at(0));
			if (!ObjCheck)
				break;

			if (ObjCheck->GetChecked())
			{
				ObjCheck->SetChecked(false);
				break;
			}
			else
			{
				ObjCheck->SetChecked(true);
				break;
			}
		}
		break;
	}
#ifndef Never_MainMenu
	MM->OnGUIEvent(nEvent, nControlID, pControl, vector<void *>{pUserContext});
#endif
}

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device *pd3dDevice, const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc, void* pUserContext)
{
	if (!InitProgram)
		InitApp();

#ifndef Never_MainMenu
	MM->setGameMode(GAME_RUNNING);
#endif

	vector<wstring> FileShaders;    
	FileShaders.push_back(*file_system->GetResPathW(&wstring(L"VertexShader.hlsl")));
	FileShaders.push_back(*file_system->GetResPathW(&wstring(L"PixelShader.hlsl")));

	vector<string> Functions, Version;
	Functions.push_back(string("main"));
	Functions.push_back(string("main"));

	Version.push_back(string("vs_4_0"));
	Version.push_back(string("ps_4_0"));

	if (!buffers->isInit())
		 buffers->InitSimpleBuffer(&FileShaders, &Functions, &Version);

	if (!Level->IsInit())
		Level->Init();

	if (!console->IsInit())
		console->Init(PhysX.get(), Level.get());

	Pick->SetObjClasses(PhysX.get(), mainActor->getObjCamera());

#ifdef _NEVER
	terrain->Initialize(frustum.get(), file_system->GetResPathA(&string("BitMap_Terrain.bmp"))->c_str(),
		file_system->GetResPathW(&wstring(L"686.jpg"))->c_str());
#endif // _NEVER
	return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device *pd3dDevice, IDXGISwapChain *pSwapChain,
	const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc, void* pUserContext)
{
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height;
	mainActor->getObjCamera()->SetProjParams(mainActor->getObjCamera()->getFOV(), fAspectRatio, 0.1f, 1000.0f);

	int X = pBackBufferSurfaceDesc->Width - 170, Y = 10;
	
		// *******
	ui->SetLocationButton(ui->getDialog()->at(0), 0, X, Y, true);
	ui->SetLocationButton(ui->getDialog()->at(0), 1, X, Y += 25, true);
	ui->SetLocationButton(ui->getDialog()->at(0), 5, X, Y += 25, true);
	ui->SetLocationButton(ui->getDialog()->at(0), 6, X, Y += 25, true);
	
	if (!ui->getCheckBoxs().empty())
		ui->SetLocationCheck(ui->getDialog()->at(0), ui->getCheckBoxs().front(), X - 150, Y += 25, true);

	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
}

POINT getPos()
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);
	return ptCursor;
}

void CALLBACK OnD3D11FrameRender(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext, double fTime, 
	float fElapsedTime, void* pUserContext)
{
	//g_Camera->SetNumberOfFramesToSmoothMouseData(fElapsedTime);
#ifndef Never_MainMenu
	switch (*MM->getGameMode())
	{
	case GAME_RUNNING:
		break;
	case GAME_MAIN_MENU:
		MM->getDlgMM()->SetLocation(DXUTGetDXGIBackBufferSurfaceDesc()->Width /2, DXUTGetDXGIBackBufferSurfaceDesc()->Height /2);
		if (!MM->getUI()->getDialog()->empty())
			MM->getUI()->getDialog()->at(0)->OnRender(fElapsedTime);

		DXUTSetCursorSettings(true, true);
		mainActor->getObjCamera()->SetRotateButtons(true, 0, 0);
		mainActor->getObjCamera()->SetResetCursorAfterMove(false);

		console->Close();
		break;
	case GAME_AUDIO_MENU:
		MM->getDlgAUD()->SetLocation(DXUTGetDXGIBackBufferSurfaceDesc()->Width /2, DXUTGetDXGIBackBufferSurfaceDesc()->Height /2);
		if (!MM->getUI()->getDialog()->empty())
			MM->getUI()->getDialog()->at(1)->OnRender(fElapsedTime);

		console->Close();
		break;
	case GAME_VIDEO_MENU:
		MM->getDlgVID()->SetLocation(DXUTGetDXGIBackBufferSurfaceDesc()->Width /2, DXUTGetDXGIBackBufferSurfaceDesc()->Height /2);
		if (!MM->getUI()->getDialog()->empty())
			MM->getUI()->getDialog()->at(2)->OnRender(fElapsedTime);

		console->Close();
		break;
	}
#endif

#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
	 Sound->Update();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS

	float fAspectRatio = DXUTGetDXGIBackBufferSurfaceDesc()->Width / (FLOAT)DXUTGetDXGIBackBufferSurfaceDesc()->Height;
	mainActor->getObjCamera()->SetProjParams(mainActor->getObjCamera()->getFOV(), fAspectRatio, 0.1f, 1000.0f);

#ifndef Never_MainMenu // Need To Move In Thread
	if (*MM->getGameMode() != GAME_RUNNING)
		return;
#endif

	ID3D11RenderTargetView *pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, _ColorBuffer);

	ID3D11DepthStencilView *pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	auto PhysObj = PhysX->GetPhysDynamicObject();

	for (int i = 0; i < m_shape.size(); i++)
	{
		if (!PhysObj.empty())
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

#ifdef _NEVER
	terrain->Render(g_Camera->GetWorldMatrix(), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());
	if (!ui->getObjCheckBox()->empty())
	{
		auto ObjCheck = ui->getHUD()->GetCheckBox(ui->getObjCheckBox()->at(0));
		if (ObjCheck)
			if (ObjCheck->GetChecked())
			{
				auto position = g_Camera->GetEyePt();
				float height = 2.0f;

				if (terrain->getQTerrain(position.x, position.z, height))
					g_Camera->setPosCam(Vector3(position.x, height + 2.0f, position.z));
			}
	}
	frustum->ConstructFrustum(1000.f, g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());
#endif // _NEVER

	auto ObjCheck = ui->getDialog()->at(0)->GetCheckBox(ui->getCheckBoxs().at(0));
	if (ObjCheck)
		if (ObjCheck->GetChecked())
			mainActor->getObjCamera()->SetEnableYAxisMovement(true);
		else
			mainActor->getObjCamera()->SetEnableYAxisMovement(false);

#ifdef NDEBUG
	ID3D11Debug *debug = nullptr;
	pd3dDevice->QueryInterface(IID_ID3D11Debug, (void **)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

	PhysX->Simulation(StopIT, fElapsedTime, mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

	int PosText = 0;
	auto *PosCam = &mainActor->getPosition();
	ui->SetTextStatic(ui->getDialog()->at(0), 0, &string("Cam Pos: "), PosCam);
	ui->SetLocationStatic(ui->getDialog()->at(0), 0, 0, PosText += 5, false);

	ui->SetTextStatic(ui->getDialog()->at(0), 1, &string("FPS: "), DXUTGetFPS());
	ui->SetLocationStatic(ui->getDialog()->at(0), 1, SCREEN_WIDTH / 2, -3, false);

	auto ObjStatic = PhysX->GetPhysStaticObject();
	ui->SetTextStatic(ui->getDialog()->at(0), 2, &string("Count Phys Object: "), PhysX->GetPhysDynamicObject().size() + ObjStatic.size());
	ui->SetLocationStatic(ui->getDialog()->at(0), 2, 0, PosText += 15, false);

#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
	auto StatAudio = Sound->getStaticsSound();
	vector<size_t> SoundInformatio =
	{
		StatAudio->playingOneShots,
		StatAudio->playingInstances,
		StatAudio->allocatedInstances,
		StatAudio->allocatedVoices,
		StatAudio->allocatedVoices3d,
		StatAudio->allocatedVoicesOneShot,
		StatAudio->allocatedVoicesIdle
	};

	ui->SetTextStatic(ui->getHUD(), 3, &string("Playing: "), SoundInformatio);
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS

	ui->SetLocationStatic(ui->getDialog()->at(0), 3, 0, PosText += 15, false);

	//Pick->moveCursor(DXUTGetDXGIBackBufferSurfaceDesc()->Width / 2, DXUTGetDXGIBackBufferSurfaceDesc()->Height / 2);
	Pick->tick();

	ui->SetTextStatic(ui->getDialog()->at(0), 3, &string("Main Actor Health Is: "), mainActor->getHealthActor());

	if (GetAsyncKeyState(VK_LSHIFT))
		mainActor->getObjCamera()->SetScalers(0.010f, 6.f * 15.f);
	else
		mainActor->getObjCamera()->SetScalers(0.010f, 6.0f);

	mainActor->Render(mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix(), fElapsedTime);
	
	auto Model = Level->getObjects();
	for (int i = 0; i < Model.size(); i++)
		Model.at(i).model->Render(mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

	buffers->RenderSimpleBuffer(mainActor->getObjCamera()->GetWorldMatrix(), mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

	console->Render(fElapsedTime);

	if (*console->getState() == Console_STATE::Close)
		V(ui->getDialog()->at(0)->OnRender(fElapsedTime));
}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	ui->getDialogResManager()->at(0)->OnD3D11ReleasingSwapChain();
}

void Destroy_Application()
{
	if (PhysX.operator bool())
		PhysX->Destroy();

	//if (terrain.operator bool())
	//	terrain->Shutdown();

	mainActor->Destroy();

	for (int i = 0; i < m_shape.size(); i++)
		 m_shape.at(i).release();

	if (Sound.operator bool())
		Sound.release();

	if (frustum.operator bool())
		frustum.release();

	if (Pick.operator bool())
		Pick.release();

	if (file_system.operator bool())
		file_system.release();

	if (mainActor.operator bool())
		mainActor.release();

	if (buffers.operator bool())
		buffers->Release();

	if (console.operator bool())
		console.release();

	if (ui.operator bool())
		ui.release();
}

void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	ui->getDialogResManager()->at(0)->OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool *pbNoFurtherProcessing, void* pUserContext)
{
	{
		if (!ui->getDialog()->empty())
		{
			*pbNoFurtherProcessing = ui->getDialog()->front()->MsgProc(hWnd, uMsg, wParam, lParam);
			if (*pbNoFurtherProcessing)
				return 0;
		}
		if (!console->getUI()->getDialog()->empty())
		{
			*pbNoFurtherProcessing = console->getUI()->getDialog()->front()->MsgProc(hWnd, uMsg, wParam, lParam);
			if (*pbNoFurtherProcessing)
				return 0;
		}
	}

#ifndef Never_MainMenu
	switch (*MM->getGameMode())
	{
	case GAME_RUNNING:
	{
		Camera *ObjCamera = nullptr;
		ObjCamera = mainActor->getObjCamera();
		if (ObjCamera)
			mainActor->getObjCamera()->HandleMessages(hWnd, uMsg, wParam, lParam);
		break;
	}
	case GAME_MAIN_MENU:
	{
		{
			if (!MM->getUI()->getDialogResManager()->empty())
				MM->getUI()->getDialogResManager()->front()->MsgProc(hWnd, uMsg, wParam, lParam);
			if (!MM->getUI()->getDialog()->empty())
				MM->getUI()->getDialog()->front()->MsgProc(hWnd, uMsg, wParam, lParam);
			break;
		}
	}
	case GAME_AUDIO_MENU:
	{
		{
			if (!MM->getUI()->getDialogResManager()->empty())
				MM->getUI()->getDialogResManager()->at(1)->MsgProc(hWnd, uMsg, wParam, lParam);
			if (!MM->getUI()->getDialog()->empty())
				MM->getUI()->getDialog()->at(1)->MsgProc(hWnd, uMsg, wParam, lParam);
			break;
		}
	}
	case GAME_VIDEO_MENU:
	{
		{
			if (!MM->getUI()->getDialogResManager()->empty())
				MM->getUI()->getDialogResManager()->at(2)->MsgProc(hWnd, uMsg, wParam, lParam);
			if (!MM->getUI()->getDialog()->empty())
				MM->getUI()->getDialog()->at(2)->MsgProc(hWnd, uMsg, wParam, lParam);
			break;
		}
	}
	break;
	}
#endif
	return 0;
}

void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	if (bKeyDown)
		switch (nChar)
		{
		case VK_F8:
			if (!StopIT)
			{
				StopIT = true;
				break;
			}
			else
			{
				StopIT = false;
				break;
			}
			break;
		case VK_F9:
			m_shape.clear();
			PhysX->ClearAllObj();
			break;
		case VK_F10:
			m_shape.push_back(GeometricPrimitive::CreateSphere(DXUTGetD3D11DeviceContext()));
			break;
		case VK_OEM_3:
			if (*console->getState() == Console_STATE::Close)
				console->Open();
			else
				console->Close();
			break;

#ifndef Never_MainMenu
		case VK_ESCAPE:
			switch (*MM->getGameMode())
			{
			case GAME_RUNNING:
				MM->setGameMode(GAME_MAIN_MENU);
				break;
			case GAME_MAIN_MENU:
				MM->setGameMode(GAME_RUNNING);
				break;
			case GAME_AUDIO_MENU:
				MM->setGameMode(GAME_MAIN_MENU);
				break;
			case GAME_VIDEO_MENU:
				MM->setGameMode(GAME_MAIN_MENU);
				break;
			}
#endif Never_MainMenu
		}
}

void CALLBACK OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
	bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
	int xPos, int yPos, void* pUserContext)
{
	mainActor->getObjCamera()->SetEnablePositionMovement(true);

	if (bRightButtonDown)
	{
		Pick->moveCursor(getPos().x, getPos().y);
		Pick->lazyPick(); // Pick Object
		return;
	}

	if (Pick->isPicked())
	{
		Pick->moveCursor(getPos().x, getPos().y);
		Pick->letGo(); // Drop Object. Deleting next frame
		return;
	}
}

bool CALLBACK OnDeviceRemoved(void* pUserContext)
{
	return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		DXUTSetCallbackFrameMove(OnFrameMove);
		DXUTSetCallbackKeyboard(OnKeyboard);
		DXUTSetCallbackMouse(OnMouse);
		DXUTSetCallbackMsgProc(MsgProc);
		DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
		DXUTSetCallbackDeviceRemoved(OnDeviceRemoved);

		DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
		DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
		DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
		DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
		DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
		DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);

		V(CoInitializeEx(NULL, COINIT_MULTITHREADED));

		DXUTInit(true, true, NULL);
		DXUTSetHotkeyHandling(false, false, false);

		DXUTSetCursorSettings(true, true);
		DXUTCreateWindow(L"EngineProgram");
		DXUTCreateDevice(D3D_FEATURE_LEVEL_9_2, true, SCREEN_WIDTH, SCREEN_HEIGHT);
		V(DXUTMainLoop());

			//It's necessary to properly destroy application resources
		Destroy_Application();
	}
	catch (const exception &Catch)
	{
		MessageBoxA(DXUTGetHWND(), string(string("The engine was crashed with this error message: ") + string(Catch.what())).c_str(), "Error!!!", MB_OK);
		MessageBoxA(DXUTGetHWND(), string(string("You can find a solution using this error code: ") + to_string(DXUTGetExitCode())).c_str(), "Error!!!", MB_OK);
	}

	return DXUTGetExitCode();
}
