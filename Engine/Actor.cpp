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

	Application->getCamera()->SetProjParams(XM_PIDIV2, ((float)Application->getWorkAreaSize(Application->GetHWND()).x 
		/ (float)Application->getWorkAreaSize(Application->GetHWND()).y),
		0.01f, 1000.0f);
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
	Application->getCamera()->SetRotateButtons(true, false, false);
	Application->getCamera()->SetEnableYAxisMovement(true);

	InitClass = true;
	return S_OK;
}

void Actor::Destroy()
{}