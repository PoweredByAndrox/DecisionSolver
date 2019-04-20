#include "pch.h"

#include "Actor.h"

void Actor::Update(float Time)
{
	if (Health == 0.0f & !IsGod)
		IsDead = true;
	else if (IsGod)
		Health = 999999.9999f;

	Position = Application->getCamera()->GetEyePt();

	Application->getCamera()->FrameMove(Time);

	Application->getCamera()->SetProjParams(3.5f/2, ((float)Application->getWorkAreaSize(Application->GetHWND()).x 
		/ (float)Application->getWorkAreaSize(Application->GetHWND()).y), 0.000001f, 1000.0f);
}

void Actor::Render(float Time)
{
	Update(Time);
}

HRESULT Actor::Init()
{
	//if (!DLG->IsInit())
	//	V(DLG->Init());
	//DLG->LoadFile(&string("For everything.xml"));
	//DLG->getMAReplices();

	Application->getCamera()->SetScalers(0.010f, 6.0f);
	Application->getCamera()->setCameraControlButtons(false, false, true);
	Application->getCamera()->SetEnableYAxisMovement(true);

	InitClass = true;
	return S_OK;
}

void Actor::Destroy()
{}