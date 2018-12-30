#include "pch.h"

#include "MainActor.h"

void Engine::MainActor::Update()
{
	if (Health == 0.0f)
		IsDead = true;

	Position = gCamera->GetEyePt();
}

void Engine::MainActor::Render(Matrix View, Matrix Proj)
{
	Update();
}

void Engine::MainActor::Destroy()
{}