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
#include "MainActor.h"
#include "GameObjects.h"

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

Vector3 Eye = { 0.f, 2.5f, 0.f }, At = { 0.0f, 2.0f, 1.f };

auto file_system = make_unique<File_system>();
vector<unique_ptr<Models>> model;
auto Sound = make_unique<Audio>();
auto ui = make_unique<UI>();
auto Pick = make_unique<Picking>();
auto terrain = make_unique<Terrain>();
auto frustum = make_unique<Frustum>();
auto buffers = make_unique<Render_Buffer>();
auto mainActor = make_unique<MainActor>();
auto PhysX = make_unique<Physics>();

//auto gameObject = make_unique<GameObjects>();

#ifdef Never_MainMenu
	auto MM = make_unique<MainMenu>();
#endif

HRESULT hr = S_OK;

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
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl *pControl, void* pUserContext);

void InitApp()
{
	if (!ui->IsInitUI())
		ui->Init();
	ui->getHUD()->SetCallback(OnGUIEvent);

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
	ui->AddStatic_Mass(ui->getHUD(), &CountOfStatics, &NameOfStatics, &X, &PositionYStatics, &W, &H);
	ui->AddButton_Mass(ui->getHUD(), &CountOfButtons, &NameOfButtons, &X, &PositionYButtons, &KeysButtons);

	ui->AddCheckBox(ui->getHUD(), ui->getAllComponentsCount() + 1, &wstring(L"Enable/Disable XAxis Movement Mode Camera (Def: X mov is true)"), 0, 0, 125, 22, 1);

#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
	if (!Sound->IsInitSounSystem())
		Sound->Init();
	Sound->AddNewSound();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS

#ifdef Never_MainMenu
	if (!MM->IsInitMainMenu())
		MM->Init(ui.get(), Sound.get());

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

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
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

		if (Obj.size() == 0)
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
		mainActor->getObjCamera()->SetViewParams(Eye, At);
		break;
	case BUTTON_7:
		PhysX->AddNewActor(Vector3(mainActor->getPosition().x, mainActor->getPosition().y + 10.f, mainActor->getPosition().z),
			Vector3(0.5f, 0.5f, 0.5f), rand() % 90 + 1);
		m_shape.push_back(GeometricPrimitive::CreateCube(DXUTGetD3D11DeviceContext(), 1.0f, false));
		break;
	case Check0:
		if (!ui->getObjCheckBox()->empty())
		{
			auto ObjCheck = ui->getHUD()->GetCheckBox(ui->getObjCheckBox()->at(0));
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
#ifdef Never_MainMenu
	MM->setGUIEvent(nEvent, nControlID, pControl, pUserContext);
#endif
}

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device *pd3dDevice, 
	const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc, void* pUserContext)
{
	if (!InitProgram)
		InitApp();

#ifdef Never_MainMenu
	MM->setGameMode(GAME_RUNNING);
#endif

	V_RETURN(ui->getDialogResManager()->OnD3D11CreateDevice(pd3dDevice, DXUTGetD3D11DeviceContext()));

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

#ifndef NEVER_228
	model.push_back(make_unique<Models>(buffers->GetResPathA(&string("nanosuit.obj"))));
	if (model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") + 
			*buffers->GetResPathW(&wstring(L"nanosuit.obj"))).c_str(), L"", MB_OK);

	PhysX->_createTriMesh(model.back().get());

	//Model.back()->Position(Vector3(100.f, 0.f, 100.f));
#endif

#ifndef NEVER_228
	model.push_back(make_unique<Models>(buffers->GetResPathA(&string("planet.obj"))));//, aiProcess_Triangulate, false));
	if (model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") +
			*buffers->GetResPathW(&wstring(L"planet.obj"))).c_str(), L"", MB_OK);

	PhysX->_createTriMesh(model.back().get());

	//Model.back()->Scale(Vector3(2.f, 2.f, 2.f));
	//Model.back()->Position(Vector3(50.f, 50.f, 100.f));
#endif

#ifndef NEVER_228
	model.push_back(make_unique<Models>(buffers->GetResPathA(&string("vue_ready_shasta.obj"))));
	if (model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") +
			*buffers->GetResPathW(&wstring(L"vue_ready_shasta.obj"))).c_str(), L"", MB_OK);

	model.back()->Scale(Vector3(0.05, 0.05, 0.05));

	//PhysX->_createTriMesh(Model.back().get());

	model.back()->Position(Vector3(-22.f, -14.5f, 0.f));
#endif

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	mainActor->getObjCamera()->SetProjParams(mainActor->getObjCamera()->getFOV(), fAspectRatio, 0.1f, 1000.0f);
	mainActor->getObjCamera()->SetViewParams(Eye, At);

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
	V_RETURN(ui->getDialogResManager()->OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height;
	mainActor->getObjCamera()->SetProjParams(mainActor->getObjCamera()->getFOV(), fAspectRatio, 0.1f, 1000.0f);

	int X = pBackBufferSurfaceDesc->Width - 170, Y = 10;

		// *******
	ui->SetLocationButton(ui->getHUD(), 0, X, Y, true);
	ui->SetLocationButton(ui->getHUD(), 1, X, Y += 25, true);
	ui->SetLocationButton(ui->getHUD(), 5, X, Y += 25, true);
	ui->SetLocationButton(ui->getHUD(), 6, X, Y += 25, true);

	if (!ui->getObjCheckBox()->empty())
		ui->SetLocationCheck(ui->getHUD(), ui->getObjCheckBox()->size() - 1, X - 150, Y += 25, true);

	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	mainActor->getObjCamera()->FrameMove(fElapsedTime);
}

vector<Vector3> Mass;
POINT getPos()
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);
	return ptCursor;
}

void CALLBACK OnD3D11FrameRender(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{
	//g_Camera->SetNumberOfFramesToSmoothMouseData(fElapsedTime);
#ifdef Never_MainMenu
	switch (*MM->getGameMode())
	{
	case GAME_RUNNING:
		break;
	case GAME_MAIN_MENU:
		MM->getDlgMM()->OnRender(fElapsedTime);
		MM->getDlgMM()->SetVisible(true);
		MM->getDlgAUD()->SetVisible(false);
		MM->getDlgVID()->SetVisible(false);
		DXUTSetCursorSettings(true, true);
		g_Camera->SetRotateButtons(true, 0, 0);
		g_Camera->SetResetCursorAfterMove(false);
		break;
	case GAME_AUDIO_MENU:
		MM->getDlgAUD()->OnRender(fElapsedTime);
		MM->getDlgAUD()->SetVisible(true);
		MM->getDlgMM()->SetVisible(false);
		MM->getDlgVID()->SetVisible(false);
		break;
	case GAME_VIDEO_MENU:
		MM->getDlgVID()->OnRender(fElapsedTime);
		MM->getDlgVID()->SetVisible(true);
		MM->getDlgAUD()->SetVisible(false);
		MM->getDlgMM()->SetVisible(false);
		break;
	}
#endif

#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
	 Sound->Update();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS

	float fAspectRatio = DXUTGetDXGIBackBufferSurfaceDesc()->Width / (FLOAT)DXUTGetDXGIBackBufferSurfaceDesc()->Height;
	mainActor->getObjCamera()->SetProjParams(mainActor->getObjCamera()->getFOV(), fAspectRatio, 0.1f, 1000.0f);

#ifdef Never_MainMenu // Need To Move In Thread
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
			vector<Vector4> q;
			vector<PxVec3> pos;
			for (int i1 = 0; i1 < PhysObj.size(); i1++)
			{
				aq.push_back(PhysObj.at(i1)->getGlobalPose().q);
				q.push_back(Vector4(aq.back().x, aq.back().y, aq.back().z, aq.back().w));
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

	auto ObjCheck = ui->getHUD()->GetCheckBox(ui->getObjCheckBox()->at(0));
	if (ObjCheck)
		if (ObjCheck->GetChecked())
			mainActor->getObjCamera()->SetEnableYAxisMovement(true);
		else
			mainActor->getObjCamera()->SetEnableYAxisMovement(false);

#ifndef DEBUG
	ID3D11Debug *debug = nullptr;
	pd3dDevice->QueryInterface(IID_ID3D11Debug, (void **)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

	PhysX->Simulation(StopIT, fElapsedTime);

	int PosText = 0;
	auto *PosCam = &mainActor->getPosition();
	ui->SetTextStatic(ui->getHUD(), 0, &string("Cam Pos: "), PosCam);
	ui->SetLocationStatic(ui->getHUD(), 0, 0, PosText += 5, false);

	ui->SetTextStatic(ui->getHUD(), 1, &string("FPS: "), DXUTGetFPS());
	ui->SetLocationStatic(ui->getHUD(), 1, SCREEN_WIDTH / 2, -3, false);

	auto ObjStatic = PhysX->GetPhysStaticObject();
	ui->SetTextStatic(ui->getHUD(), 2, &string("Count Phys Object: "), PhysObj.size() + ObjStatic.size());
	ui->SetLocationStatic(ui->getHUD(), 2, 0, PosText += 15, false);

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

	ui->SetLocationStatic(ui->getHUD(), 3, 0, PosText += 15, false);

	//Pick->moveCursor(DXUTGetDXGIBackBufferSurfaceDesc()->Width / 2, DXUTGetDXGIBackBufferSurfaceDesc()->Height / 2);
	Pick->tick();

	ui->SetTextStatic(ui->getHUD(), 3, &string("Main Actor Health Is: "), mainActor->getHealthActor());

	if (GetAsyncKeyState(VK_LSHIFT))
		mainActor->getObjCamera()->SetScalers(0.010f, 6.0f * 9.0f);
	else
		mainActor->getObjCamera()->SetScalers(0.010f, 6.0f);

	mainActor->Render(mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

	for (int i = 0; i < model.size(); i++)
		model.at(i)->Render(mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

	buffers->RenderSimpleBuffer(mainActor->getObjCamera()->GetWorldMatrix(), mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());

	V(ui->getHUD()->OnRender(fElapsedTime));

	//auto ActrCamera = PhysX->getActrCamera();
	//if (ActrCamera)
	//{
	//	PxQuat aq;
	//	Vector4 q;
	//	PxVec3 pos;
	//	aq = ActrCamera->getGlobalPose().q;
	//	q = Vector4(aq.x, aq.y, aq.z, aq.w);
	//	pos = ActrCamera->getGlobalPose().p;

	//	mainActor->getObjCamera()->setPosCam(Vector3(pos.x, pos.y, pos.z));
	//	for (int i = 0; i < m_shape.size(); i++)
	//		m_shape.at(i)->Draw(XMMatrixRotationQuaternion(XMVectorSet(aq.x, aq.y, aq.z, aq.w))
	//			* XMMatrixTranslation(pos.x, pos.y-5.f, pos.z),
	//			mainActor->getObjCamera()->GetViewMatrix(), mainActor->getObjCamera()->GetProjMatrix());
	//	//ActrCamera->setGlobalPose(PxTransform(mainActor->getPosition().x, mainActor->getPosition().y - 5.f, mainActor->getPosition().z));
	//}
}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	ui->getDialogResManager()->OnD3D11ReleasingSwapChain();
}

void Destroy_Application()
{
	if (PhysX.operator bool())
		PhysX->Destroy();

	if (terrain.operator bool())
		terrain->Shutdown();

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
}

void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	if (ui.operator bool())
	{
		ui->getDialogResManager()->OnD3D11DestroyDevice();
		ui.release();
	}

	DXUTGetGlobalResourceCache().OnDestroyDevice();
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool *pbNoFurtherProcessing, void* pUserContext)
{
	*pbNoFurtherProcessing = ui->getDialogResManager()->MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	*pbNoFurtherProcessing = ui->getHUD()->MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	auto ObjCamera = mainActor->getObjCamera();
	if (ObjCamera)
		mainActor->getObjCamera()->HandleMessages(hWnd, uMsg, wParam, lParam);

#ifdef Never_MainMenu
	switch (*MM->getGameMode())
	{
	case GAME_RUNNING:
		g_Camera->HandleMessages(hWnd, uMsg, wParam, lParam);
		break;
	case GAME_MAIN_MENU:
		MM->getDlgMM()->MsgProc(hWnd, uMsg, wParam, lParam);
		break;
	case GAME_AUDIO_MENU:
		MM->getDlgAUD()->MsgProc(hWnd, uMsg, wParam, lParam);
		break;
	case GAME_VIDEO_MENU:
		MM->getDlgVID()->MsgProc(hWnd, uMsg, wParam, lParam);
		break;
	}
#endif
	return 0;
}

void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	if(bKeyDown)
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
		}
#ifdef Never_MainMenu
	if (bKeyDown)
	{
		switch (nChar)
		{
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
		break;
			}
		}
	}
#endif Never_MainMenu
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

	V_RETURN(CoInitializeEx(NULL, COINIT_MULTITHREADED));

	DXUTInit(true, true, NULL);
	DXUTSetHotkeyHandling(false, false, false);

	DXUTSetCursorSettings(true, true);
	DXUTCreateWindow(L"EngineProgram");
	DXUTCreateDevice(D3D_FEATURE_LEVEL_9_2, true, SCREEN_WIDTH, SCREEN_HEIGHT);
	V_RETURN(DXUTMainLoop());

		//It is necessary to properly destroy application resources
	Destroy_Application();

	return DXUTGetExitCode();
}
