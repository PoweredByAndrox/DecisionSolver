#include "pch.h"

#include <d3d11_1.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Models.h"
#include "Physics.h"
#include "File_system.h"
#include "Audio.h"
#include "Shaders.h"
#include "UI.h"
#include "MainMenu.h"
#include "Camera.h"
#include "Picking.h"
#include "Terrain.h"

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

ID3D11InputLayout  *g_pLayout = nullptr, *g_pVertexLayout = nullptr;

ID3D11VertexShader *g_pVS = nullptr;
ID3D11PixelShader  *g_pPS = nullptr;

ID3D11SamplerState *TexSamplerState = nullptr;
ID3D11Buffer *pConstantBuffer = nullptr;

Vector3 Eye = { 0.0f, 2.5f, 0.0f }, At = { 0.0f, 2.5f, 1.0f };

auto file_system = make_unique<File_system>();
vector<unique_ptr<Models>> Model;
auto Sound = make_unique<Audio>();
auto Shader = make_unique<Shaders>();
auto ui = make_unique<UI>();
auto Pick = make_unique<Picking>();
auto g_Camera = make_unique<CFirstPersonCamera>();
auto terrain = make_unique<Terrain>();
auto frustum = make_unique<Frustum>();

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

Matrix gWorld;
Matrix gView;
Matrix gProjection;

struct ConstantBuffer 
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
} cb;

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

//**************
	// Test
bool StopIT = false;

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

bool InitProgram = false;

void InitApp()
{
	if (!ui->IsInitUI())
		ui->Init();
	ui->getHUD()->SetCallback(OnGUIEvent);

	vector<int> CountOfButtons =
	{ 1, 2, 3, 4, 5, 6, 7 };
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
	{ 8, 9, 10, 11 };
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
	
	if (!Sound->IsInitSounSystem())
		Sound->Init();

#if defined(Never)
	if (!PhysX->IsPhysicsInit())
		PhysX->Init();
#endif

#ifdef Never_MainMenu
	if (!MM->IsInitMainMenu())
		MM->Init(ui.get(), Sound.get());

	MM->getDlgMM()->SetCallback(OnGUIEvent); 
	MM->getDlgAUD()->SetCallback(OnGUIEvent);
	MM->getDlgVID()->SetCallback(OnGUIEvent);
#endif

	//g_Camera.SetClipToBoundary(true, &D3DXVECTOR3(4, 6, 3), &D3DXVECTOR3(1, 2, 5));
	//g_Camera->SetEnableYAxisMovement(false);
	
	g_Camera->SetScalers(0.010f, 6.0f);
	g_Camera->SetRotateButtons(true, false, false);
	
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
		PhysX->AddTorque(PhysX->GetPhysDynamicObject().at(rand() % PhysX->GetPhysDynamicObject().size()), PosCam,
			PxForceMode::Enum::eIMPULSE);
		PhysX->AddTorque(PhysX->GetPhysDynamicObject().at(rand() % PhysX->GetPhysDynamicObject().size()), PosCam,
			PxForceMode::Enum::eFORCE);
		PhysX->AddTorque(PhysX->GetPhysDynamicObject().at(rand() % PhysX->GetPhysDynamicObject().size()), PosCam,
			PxForceMode::Enum::eVELOCITY_CHANGE);
		break;
	}
	case BUTTON_3:
		Sound->doPlay();
		break;
	case BUTTON_4:
		Sound->doPause();
		break;
	case BUTTON_5:
		Sound->doStop();
		break;
	case BUTTON_6:
		g_Camera->SetViewParams(Eye, At);
		break;
	case BUTTON_7:
		PhysX->AddNewActor(Vector3(1.f, 5.f, -3.f), Vector3(0.5f, 0.5f, 0.5f));
		m_shape.push_back(GeometricPrimitive::CreateCube(DXUTGetD3D11DeviceContext(), 1.0f, false));
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

	Sound->AddNewSound();

#ifdef Never_MainMenu
	MM->setGameMode(GAME_RUNNING);
#endif

	V_RETURN(ui->getDialogResManager()->OnD3D11CreateDevice(pd3dDevice, DXUTGetD3D11DeviceContext()));

	ID3DBlob *VS = nullptr, *PS = nullptr;
	V_RETURN(Shader->CompileShaderFromFile(file_system->GetResPathW(&wstring(L"VertexShader.hlsl")), &string("main"), &string("vs_4_0"),
		&VS));

	V_RETURN(Shader->CompileShaderFromFile(file_system->GetResPathW(&wstring(L"PixelShader.hlsl")), &string("main"), &string("ps_4_0"),
		&PS));

	V_RETURN(pd3dDevice->CreateVertexShader(VS->GetBufferPointer(),
		VS->GetBufferSize(), NULL, &g_pVS));

	V_RETURN(pd3dDevice->CreatePixelShader(PS->GetBufferPointer(),
		PS->GetBufferSize(), NULL, &g_pPS));

#ifdef DEBUG
	DXUT_SetDebugName(g_pVS, "VS");
	DXUT_SetDebugName(g_pPS, "PS");
#endif

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, VS->GetBufferPointer(),
		VS->GetBufferSize(), &g_pLayout));

#ifdef DEBUG
	DXUT_SetDebugName(g_pLayout, "Vertices Shader");
#endif

#ifndef NEVER_228
	Model.push_back(make_unique<Models>(file_system->GetResPathA(&string("nanosuit.obj")), Shader.get()));
	if (Model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") + 
			*file_system->GetResPathW(&wstring(L"nanosuit.obj"))).c_str(), L"", MB_OK);
#endif
#ifdef NEVER_228
	Model.push_back(make_unique<Models>(file_system->GetResPathA(&string("SnowTerrain.obj")), Shader.get()));//, aiProcess_Triangulate, false));
	if (Model.empty())
		MessageBoxW(DXUTGetHWND(), wstring(wstring(L"Model was not loaded along this path: ") +
			*file_system->GetResPathW(&wstring(L"SnowTerrain.obj"))).c_str(), L"", MB_OK);
#endif

	gWorld = Matrix::Identity;
	m_shape.push_back(GeometricPrimitive::CreateCube(DXUTGetD3D11DeviceContext(), 1.0f, false));
	
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera->SetProjParams(D3DX_PI / 3, fAspectRatio, 0.1f, 1000.0f);
	g_Camera->SetViewParams(Eye, At);

	gProjection = Matrix::CreatePerspective(pBackBufferSurfaceDesc->Width, (FLOAT)pBackBufferSurfaceDesc->Height, 0.1f, 1000.f);

	SAFE_RELEASE(VS);
	SAFE_RELEASE(PS);
	
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	V_RETURN(pd3dDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer));

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	V_RETURN(pd3dDevice->CreateSamplerState(&sampDesc, &TexSamplerState));

#if defined(Never)
	// PhysX->ModelPhysics(Model->GetMeshes());
#endif

	Pick->SetObjClasses(PhysX.get(), g_Camera.get());

	terrain->Initialize(Shader.get(), frustum.get(), file_system->GetResPathA(&string("BitMap_Terrain.bmp"))->c_str(),
		file_system->GetResPathW(&wstring(L"686.jpg"))->c_str());

	return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	V_RETURN(ui->getDialogResManager()->OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera->SetProjParams(D3DX_PI / 3, fAspectRatio, 0.1f, 1000.0f);

	int X = pBackBufferSurfaceDesc->Width - 170, Y = 10;

		// *******
	ui->SetLocationButton(ui->getHUD(), 0, X, Y);
	ui->SetLocationButton(ui->getHUD(), 1, X, Y += 25);
	ui->SetLocationButton(ui->getHUD(), 5, X, Y += 25);
	ui->SetLocationButton(ui->getHUD(), 6, X, Y += 25);

	return S_OK;
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera->FrameMove(fElapsedTime);
}

vector<XMVECTOR> Mass;

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

	Sound->Update();

#ifdef Never_MainMenu // Need To Move In Thread
	if (*MM->getGameMode() != GAME_RUNNING)
		return;
#endif
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, _ColorBuffer);

	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	pd3dImmediateContext->IASetInputLayout(g_pLayout);

	frustum->ConstructFrustum(1000.f, cb.mWorld, cb.mProjection);

	for (int i = 0; i < m_shape.size(); i++)
	{
		auto Obj = PhysX->GetPhysDynamicObject();
		if (!Obj.empty())
		{
			vector<PxQuat> aq;
			vector<Vector4> q;
			vector<PxVec3> pos;
			for (int i1 = 0; i1 < PhysX->GetPhysDynamicObject().size(); i1++)
			{
				aq.push_back(Obj.at(i1)->getGlobalPose().q);
				q.push_back(Vector4(aq.back().x, aq.back().y, aq.back().z, aq.back().w));
				pos.push_back(Obj.at(i1)->getGlobalPose().p);

				m_shape.at(i)->Draw(XMMatrixRotationQuaternion(XMVectorSet(aq[i1].x, aq[i1].y, aq[i1].z, aq[i1].w))
					* XMMatrixTranslation(pos[i1].x, pos[i1].y, pos[i1].z), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix()//, 
					//_Color[rand() % 9 + 1]
				);
			}
		}
	}

	cb.mWorld = XMMatrixTranspose(g_Camera->GetWorldMatrix());
	cb.mView = XMMatrixTranspose(g_Camera->GetViewMatrix());
	cb.mProjection = XMMatrixTranspose(g_Camera->GetProjMatrix());

	for (int i = 0; i < Model.size(); i++)
		Model.at(i)->Render(g_Camera->GetWorldMatrix(), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());

	terrain->Render(g_Camera->GetWorldMatrix(), g_Camera->GetViewMatrix(), g_Camera->GetProjMatrix());

		// Get the current position of the camera.
	auto position = g_Camera->GetEyePt();
	float height = 2.0f;

		// Get the height of the triangle that is directly underneath the given camera position.
	if (terrain->getQTerrain(position.x, position.z, height))
		g_Camera->setPosCam(Vector3(position.x, height + 2.0f, position.z));

	pd3dImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);

	pd3dImmediateContext->VSSetShader(g_pVS, 0, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	pd3dImmediateContext->PSSetShader(g_pPS, 0, 0);
	pd3dImmediateContext->PSSetSamplers(0, 1, &TexSamplerState);

	V(ui->getHUD()->OnRender(fElapsedTime));

#ifndef DEBUG
	ID3D11Debug* debug = 0;
	pd3dDevice->QueryInterface(IID_ID3D11Debug, (void**)&debug);
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

#if defined(Never)
	PhysX->Simulation(StopIT, fElapsedTime);
#endif

	int PosText = 0;
	auto *PosCam = &Vector3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z);
	ui->SetTextStatic(ui->getHUD(), 0, &string("Cam Pos: "), PosCam);
	ui->SetLocationStatic(ui->getHUD(), 0, 0, PosText += 5);

	ui->SetTextStatic(ui->getHUD(), 1, &string("FPS: "), DXUTGetFPS());
	ui->SetLocationStatic(ui->getHUD(), 1, SCREEN_WIDTH / 2, -3);

	ui->SetTextStatic(ui->getHUD(), 2, &string("Count Phys Object: "), PhysX->GetPhysDynamicObject().size());
	ui->SetLocationStatic(ui->getHUD(), 2, 0, PosText += 15);

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
	ui->SetLocationStatic(ui->getHUD(), 3, 0, PosText += 15);

	Pick->tick();
}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	ui->getDialogResManager()->OnD3D11ReleasingSwapChain();
}

void Destroy_Application()
{
#if defined(Never)
	PhysX->Destroy();
#endif
	terrain->Shutdown();
	for (int i = 0; i < Model.size(); i++)
		 Model.at(i)->Close();
	for (int i = 0; i < m_shape.size(); i++)
		 m_shape[i].release();
}

void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	SAFE_RELEASE(g_pLayout);
	SAFE_RELEASE(g_pVS);
	SAFE_RELEASE(g_pPS);
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(TexSamplerState);
	SAFE_RELEASE(pConstantBuffer);
	ui->getDialogResManager()->OnD3D11DestroyDevice();
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
			g_Camera->setPosCam(Vector3(4, 6, 1));
			//PhysX->CreateJoint(PhysX->GetPhysDynamicObject().at(rand() % PhysX->GetPhysDynamicObject().size()),
			//PhysX->GetPhysDynamicObject().at(rand() % PhysX->GetPhysDynamicObject().size()), PxVec3(1.1f, 0.1f, 3.5f));
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

	if (bRightButtonDown)
	{
		PxRaycastBuffer hit;
		auto T = PhysX->getScene();
		const PxTransform& camPose = PxTransform(PxVec3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z));
		PxVec3 forward = -PxMat33(camPose.q)[1];
		auto PosObjPhys = PhysX->GetPhysDynamicObject().at(rand() % PhysX->GetPhysDynamicObject().size());
		if (T->raycast(camPose.p + forward, forward, Vector3::Distance(
			Vector3(g_Camera->GetEyePt().x, g_Camera->GetEyePt().y, g_Camera->GetEyePt().z),
			Vector3(PosObjPhys->getGlobalPose().p.x, PosObjPhys->getGlobalPose().p.y, PosObjPhys->getGlobalPose().p.z)), hit, PxHitFlags(PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eDISTANCE | PxHitFlag::eUV)))
			PhysX->AddTorque(PosObjPhys, -hit.block.position, PxForceMode::Enum::eACCELERATION);
		Pick->letGo();
	}
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
