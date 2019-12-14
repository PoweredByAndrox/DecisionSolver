#include "pch.h"
#include "Actor.h"
#include "DebugDraw.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Camera.h"

void Actor::Update(float Time)
{
	if (Health == 0.0f && !IsGod)
		IsDead = true;
	if (IsGod)
		Health = 999999.9999f;

	Position = Application->getCamera()->GetEyePt();

	Application->getCamera()->FrameMove(Time);

	Application->getCamera()->SetProjParams(XM_PI / 3.2f, ((float)Application->getWorkAreaSize(Application->GetHWND()).x /
		(float)Application->getWorkAreaSize(Application->GetHWND()).y), 0.1f, 1000.f);
}

extern bool DrawCamSphere;
Vector2 Number = { 1.f, 0.f };
void Actor::Render(float Time)
{
	Update(Time);

	auto P = Vector2::SmoothStep(Vector2(Application->getframeTime(), 0.f), Number, 15.f).x;
	Application->getCamera()->SetProjParams(P,
		((float)Application->getWorkAreaSize(Application->GetHWND()).x /
		(float)Application->getWorkAreaSize(Application->GetHWND()).y), 0.1f, 1000.f);

	auto PosCCT = Application->getCamera()->GetCCT();
	if (PosCCT.operator bool() && DrawCamSphere && Application->getDebugDraw())
	{
		//BoundingSphere sphere(ToExtended(PosCCT->getController()->getPosition()), PosCCT->getController()->getRadius());
		//Application->getDebugDraw()->Draw(sphere, (Vector4)Colors::DarkSlateBlue);
	}

	Number.Clamp(Vector2(0.2f, 0.f), Vector2(1.f, 0.f));
	if (Application->getKeyboard()->GetState().OemPlus)
	{
		Number.x -= 0.01f;
		return;
	}
	else if (Application->getKeyboard()->GetState().OemMinus)
	{
		Number.x += 0.01f;
		return;
	}
}

HRESULT Actor::Init()
{
	Application->getCamera()->SetCameraControlButtons(false, true, false);
	Application->getCamera()->SetResetCursorAfterMove(true);
	Application->getCamera()->SetFreeMoveCam(true);
	Application->getCamera()->SetDrag(true);
	Application->getCamera()->SetScalers();
	//Application->getCamera()->Teleport(Vector3(5.5, 1.5, 0), Vector3(6, 0, 0));

	InitClass = true;
	return S_OK;
}
