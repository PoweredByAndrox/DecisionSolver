#include "pch.h"
#include "Actor.h"
#include "DebugDraw.h"

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
void Actor::Render(float Time)
{
	Update(Time);

	auto PosCCT = Application->getCamera()->GetCCT();
	if (PosCCT.operator bool() && DrawCamSphere)
	{
		BoundingSphere sphere(ToExtended(PosCCT->getController()->getPosition()), PosCCT->getController()->getRadius());
		Application->getDebugDraw()->Draw(sphere, (Vector4)Colors::DarkSlateBlue);
	}
}

HRESULT Actor::Init()
{
	Application->getCamera()->SetCameraControlButtons(false, true, false);
	Application->getCamera()->SetResetCursorAfterMove(false);
	Application->getCamera()->SetFreeMoveCam(true);

	InitClass = true;
	return S_OK;
}
