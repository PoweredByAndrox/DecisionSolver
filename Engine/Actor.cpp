#include "pch.h"
#include "Actor.h"
#include "DebugDraw.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Camera.h"
#include "SDKInterface.h"

extern shared_ptr<SDKInterface> SDK;
extern bool DrawCamSphere;
void Actor::Update(float Time)
{
	if (!Application->getCamera()) return;

	if (Health == 0.0f && !IsGod)
		IsDead = true;
	if (IsGod)
		Health = 999999.9999f;

	Position = Application->getCamera()->GetEyePt();

	Application->getCamera()->FrameMove(Time);

	Application->getCamera()->SetProjParams(XM_PI / 3.2f, ((float)Application->getWorkAreaSize(Application->GetHWND()).x /
		(float)Application->getWorkAreaSize(Application->GetHWND()).y), SDK->GetDistNearRender(), SDK->GetDistFarRender());
}

Vector2 Number = { 1.f, 0.f };
void Actor::Render(float Time)
{
	Update(Time);

	auto P = Vector2::SmoothStep(Vector2(Application->getframeTime(), 0.f), Number, 15.f).x;
	Application->getCamera()->SetProjParams(P,
		((float)Application->getWorkAreaSize(Application->GetHWND()).x /
		(float)Application->getWorkAreaSize(Application->GetHWND()).y), SDK->GetDistNearRender(), SDK->GetDistFarRender());

	Number.Clamp(Vector2(0.2f, 0.f), Vector2(1.f, 0.f));
	if (Application->getKeyboard()->GetState().OemPlus)
	{
		Number -= Vector2(Time);
	}
	else if (Application->getKeyboard()->GetState().OemMinus)
	{
		Number += Vector2(Time);
		return;
	}
}

void Actor::ChangePosition(Vector3 Pos)
{
	Application->getCamera()->Teleport(Pos, Application->getCamera()->GetLookAtPt());
}

HRESULT Actor::Init()
{
	//	// Camera Class
	Application->setCamera(make_shared<Camera>());
	if (FAILED(Application->getCamera()->Init((float)Application->getWorkAreaSize(Application->GetHWND()).x,
		(float)Application->getWorkAreaSize(Application->GetHWND()).y)))
	{
		Engine::LogError("Actor::Init::Camera->Init() Failed.",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Camera: Init Failed!");
		return E_FAIL;
	}

	Application->getCamera()->SetCameraControlButtons(false, true);
	Application->getCamera()->SetResetCursorAfterMove(true);
	Application->getCamera()->SetFreeMoveCam(true);
	Application->getCamera()->SetDrag(true);
	Application->getCamera()->SetScalers();
	//Application->getCamera()->Teleport(Vector3(5.5, 1.5, 0), Vector3(6, 0, 0));
	auto PosCCT = Application->getCamera()->GetCCT();
	if (PosCCT.operator bool() && Application->getDebugDraw())
		Application->getDebugDraw()->AddBox(ToExtended(PosCCT->getController()->getPosition()),
			Vector3(PosCCT->getController()->getRadius()),
			(Vector4)Colors::DarkSlateBlue);

	InitClass = true;
	return S_OK;
}
