#include "pch.h"

#include "Actor.h"

void Engine::Actor::Update(float Time)
{
	if (Health == 0.0f & !IsGod)
		IsDead = true;
	else if (IsGod)
		Health = 999999.9999f;

	Position = gCamera->GetEyePt();

	gCamera->FrameMove(Time);

	gCamera->SetProjParams(gCamera->getFOV(), (DXUTGetDXGIBackBufferSurfaceDesc()->Width / (FLOAT)DXUTGetDXGIBackBufferSurfaceDesc()->Height),
		0.1f, 1000.0f);
}

void Engine::Actor::Render(Matrix View, Matrix Proj, float Time)
{
	Update(Time);
}

HRESULT Engine::Actor::Init(Physics *PhysX)
{
	if (!DLG->IsInit())
		ThrowIfFailed(DLG->Init());
	DLG->LoadFile(&string("For everything.xml"));
	DLG->getMAReplices();

	this->PhysX.reset(PhysX);

	InitClass = true;
	return S_OK;
}

void Engine::Actor::Destroy()
{}