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
#include "Camera.h"
#include "Picking.h"
#include "Terrain.h"
#include "Render_Buffer.h"

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

Vector3 Eye = { 100.f, 2.5f, 100.f }, At = { 0.0f, 2.5f, 1.0f };

auto file_system = make_unique<File_system>();
vector<unique_ptr<Models>> Model;
auto Sound = make_unique<Audio>();
auto ui = make_unique<UI>();
auto Pick = make_unique<Picking>();
auto g_Camera = make_unique<CFirstPersonCamera>();
auto terrain = make_unique<Terrain>();
auto frustum = make_unique<Frustum>();
auto buffers = make_unique<Render_Buffer>();

#ifdef Never_MainMenu
	auto MM = make_unique<MainMenu>();
#endif

#if defined(Never)
	auto PhysX = make_unique<Physics>();
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
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

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
	
	ui->AddCheckBox(ui->getHUD(), ui->getAllComponentsCount() + 1, &wstring(L"Disable/Enable Movement The Terrain"), 0, 0, 125, 22);

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
	//g_Camera->SetEnableYAxisMovement(false);
	
	g_Camera->SetScalers(0.010f, 6.0f);
	g_Camera->SetRotateButtons(true, false, false);
	//g_Camera->SetChangeFOV(true);

	InitProgram = true;
}

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo,
	UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed,
	void* pUserContext)
{
	return true;
}

bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
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
		auto PosCam = PxVec3(g_Camera->GetEyePt().x, -g_Camera->GetEyePt().y, g_Camera->GetEyePt().z);
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
		break;
	case BUTTON_4:
#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		Sound->doPause();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		break;
	case BUTTON_5:
#ifdef SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		Sound->doStop();
#endif // SOME_ERROR_WITH_AUDIO_AFTER_UPDATE_FCK_DRIVERS
		break;
	case BUTTON_6:
		g_Camera->SetViewParams(Eye, At);
		break;
	case BUTTON_7:
		PhysX->AddNewActor(Vector3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z), Vector3(0.5f, 0.5f, 0.5f));
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

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, 
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
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

	struct ConstantBuffer
	{
		Matrix mWorld;
		Matrix mView;
		Matrix mProjection;
	};

	if (!buffers->isInit())
		 buffers->InitSimpleBuffer(&FileShaders, &Functions, &Version);

#if defined(Never)
	if (!PhysX->IsPhysicsInit())
		PhysX->Init();
#endif

#ifndef NEVER_228
	Model.push_back(make_unique<Models>(buffers->GetResPathA(&string("nanosuit.obj"))));
	if (Model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") + 
			*buffers->GetResPathW(&wstring(L"nanosuit.obj"))).c_str(), L"", MB_OK);

	PhysX->_createTriMesh(Model.back().get());

	//Model.back()->Position(Vector3(100.f, 0.f, 100.f));
#endif

#ifndef NEVER_228
	Model.push_back(make_unique<Models>(buffers->GetResPathA(&string("planet.obj"))));//, aiProcess_Triangulate, false));
	if (Model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") +
			*buffers->GetResPathW(&wstring(L"planet.obj"))).c_str(), L"", MB_OK);

	PhysX->_createTriMesh(Model.back().get());

	//Model.back()->Scale(Vector3(2.f, 2.f, 2.f));
	//Model.back()->Position(Vector3(50.f, 50.f, 100.f));
#endif

#ifdef NEVER_228
	Model.push_back(make_unique<Models>(buffers->GetResPathA(&string("vue_ready_shasta.obj"))));
	if (Model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") +
			*buffers->GetResPathW(&wstring(L"vue_ready_shasta.obj"))).c_str(), L"", MB_OK);

	//Model.back()->Scale(Vector3(0.01, 0.01, 0.01));

	PhysX->_createTriMesh(Model.back().get());

	//Model.back()->Position(Vector3(0.f, -35.f, 0.f));
#endif

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera->SetProjParams(D3DX_PI / 3, fAspectRatio, 0.1f, 1000.0f);
	g_Camera->SetViewParams(Eye, At);

	Pick->SetObjClasses(PhysX.get(), g_Camera.get());

	terrain->Initialize(frustum.get(), file_system->GetResPathA(&string("BitMap_Terrain.bmp"))->c_str(),
		file_system->GetResPathW(&wstring(L"686.jpg"))->c_str());

	PhysX->AddNewActor(Vector3(100.f, 0.f, 100.f), Vector3(0.5f, 0.5f, 0.5f));
	m_shape.push_back(GeometricPrimitive::CreateCube(DXUTGetD3D11DeviceContext(), 1.0f, false));

	return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	V_RETURN(ui->getDialogResManager()->OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height;
	g_Camera->SetProjParams(D3DX_PI / 3, fAspectRatio, 0.1f, 1000.0f);

	int X = pBackBufferSurfaceDesc->Width - 170, Y = 10;

		// *******
	ui->SetLocationButton(ui->getHUD(), 0, X, Y, false);
	ui->SetLocationButton(ui->getHUD(), 1, X, Y += 25, false);
	ui->SetLocationButton(ui->getHUD(), 5, X, Y += 25, false);
	ui->SetLocationButton(ui->getHUD(), 6, X, Y += 25, false);

	if (!ui->getObjCheckBox()->empty())
		ui->SetLocationCheck(ui->getHUD(), ui->getObjCheckBox()->size() - 1, X - 150, Y += 25, false);

	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera->FrameMove(fElapsedTime);
}

vector<XMVECTOR> Mass;

ToDo("Need To Move In Picking Class!")
/* 
HRESULT pick()
{
	HRESULT hr;
	Vector3 vPickRayDir;
	Vector3 vPickRayOrig;
	const auto *pd3dsdBackBuffer = DXUTGetDXGIBackBufferSurfaceDesc();

	//g_nNumIntersections = 0L;

	// Get the pick ray from the mouse position
	if (GetCapture())
	{
		const Matrix *pmatProj = &g_Camera->GetProjMatrix();

		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(DXUTGetHWND(), &ptCursor);

		// Compute the vector of the pick ray in screen space
		Vector3 v;
		v.x = (((2.0f * ptCursor.x) / pd3dsdBackBuffer->Width) - 1) / pmatProj->_11;
		v.y = -(((2.0f * ptCursor.y) / pd3dsdBackBuffer->Height) - 1) / pmatProj->_22;
		v.z = 1.0f;

		// Get the inverse view matrix
		const Matrix matView = g_Camera->GetViewMatrix();
		const Matrix matWorld = g_Camera->GetWorldMatrix();
		Matrix mWorldView = matWorld * matView;
		Matrix m;
		m = XMMatrixInverse(NULL, mWorldView);

		// Transform the screen space pick ray into 3D space
		vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
		vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
		vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
		vPickRayOrig.x = m._41;
		vPickRayOrig.y = m._42;
		vPickRayOrig.z = m._43;
	}

	// Get the picked triangle
	if (GetCapture())
	{
	}

	return S_OK;
}
*/

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
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
	g_Camera->SetProjParams(D3DX_PI / 3, fAspectRatio, 0.1f, 1000.0f);

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
					* XMMatrixTranslation(pos[i1].x, pos[i1].y, pos[i1].z), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix()//, 
					//_Color[rand() % 9 + 1]
				);
			}
		}
	}

	//terrain->Render(g_Camera->GetWorldMatrix(), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());

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

#ifndef DEBUG
	ID3D11Debug *debug = nullptr;
	pd3dDevice->QueryInterface(IID_ID3D11Debug, (void **)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

#if defined(Never)
	PhysX->Simulation(StopIT, fElapsedTime);
#endif

	int PosText = 0;
	auto *PosCam = &Vector3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z);
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

	//Pick->tick();

	if (GetAsyncKeyState(VK_LSHIFT))
		g_Camera->SetScalers(0.010f, 6.0f * 9.0f);
	else
		g_Camera->SetScalers(0.010f, 6.0f);

	for (int i = 0; i < Model.size(); i++)
		Model.at(i)->Render(g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());

	buffers->RenderSimpleBuffer(g_Camera->GetWorldMatrix(), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());

	V(ui->getHUD()->OnRender(fElapsedTime));
}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	ui->getDialogResManager()->OnD3D11ReleasingSwapChain();
}

void Destroy_Application()
{
#if defined(Never)
	if (PhysX.operator bool())
		PhysX->Destroy();
#endif
	if (terrain.operator bool())
		terrain->Shutdown();

	for (int i = 0; i < Model.size(); i++)
	{
		 Model.at(i)->Close();
		 Model.at(i)->Release();
		 Model.at(i).release();
	}

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

	if (g_Camera.operator bool())
		g_Camera.release();

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
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	*pbNoFurtherProcessing = ui->getDialogResManager()->MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	*pbNoFurtherProcessing = ui->getHUD()->MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	g_Camera->HandleMessages(hWnd, uMsg, wParam, lParam);

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

float Scale = 0.0f;
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

			// **********
			Scale = 0.0f;

			break;
		case VK_F9:
			m_shape.clear();
			PhysX->ClearAllObj();
			break;
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
	g_Camera->SetEnablePositionMovement(true);
	/*
	if (bRightButtonDown)
	{
		PxRaycastBuffer hit;
		auto T = PhysX->getScene();
		const PxTransform& camPose = PxTransform(PxVec3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z));
		PxVec3 forward = -PxMat33(camPose.q)[1];
		auto PosObjPhys = PhysX->GetPhysDynamicObject();
		if (PosObjPhys.size() == 0)
			return;

		if (T->raycast(camPose.p + forward, forward, Vector3::Distance(
			Vector3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z),
			Vector3(PosObjPhys.at(rand() % PosObjPhys.size())->getGlobalPose().p.x, PosObjPhys.at(rand() % PosObjPhys.size())->getGlobalPose().p.y, 
				PosObjPhys.at(rand() % PosObjPhys.size())->getGlobalPose().p.z)), hit,
			PxHitFlags(PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eDISTANCE | PxHitFlag::eUV)))

			PhysX->AddTorque(PosObjPhys.at(rand() % PosObjPhys.size()), -hit.block.position, PxForceMode::Enum::eACCELERATION);
		Pick->letGo();
	}
	*/
//	Pick->moveCursor(DXUTGetWindowWidth() / 2, DXUTGetWindowHeight() / 2);
//	Pick->lazyPick();
	
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
