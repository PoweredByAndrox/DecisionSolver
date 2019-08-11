#include "pch.h"
#include "SimpleLogic.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "StepTimer.h"

//ToDo("Need To Do Another One Capsule Physics And Add Jump To Logic")

vector<SimpleLogic::LogicMode> ProgressMass = { SimpleLogic::LogicMode::Forward,
SimpleLogic::LogicMode::Forward, SimpleLogic::LogicMode::Forward, SimpleLogic::LogicMode::Right };

void SimpleLogic::follow(Vector3 Where)
{
	Modes = LogicMode::Follow;
	NeededPos = Where;
}

void SimpleLogic::Update(Vector3 &Pos)
{
	if (!timer_1->GetIsFixedTimeStep())
	{
		exception("This Timer hasn't set for Fixed Time!");
		return;
	}

	if (Progress >= 4)
		Progress = 0;

	timer_1->Tick();

	if (GetAsyncKeyState(VK_OEM_PLUS))
		Time += 0.05f;
	if (GetAsyncKeyState(VK_OEM_MINUS))
		Time -= 0.05f;

	timer_1->SetTargetElapsedSeconds(Time);

	OutputDebugStringA((boost::format("\nTime1: %f") % Time).str().c_str());

//	if (Modes != Follow)
	Modes = ProgressMass.at(Progress);

	Progress++;
	if (Modes == Forward) // Up
	{
		Pos.z -= (float)timer_1->GetElapsedSeconds(); // Forward
		return;
	}
	if (Modes == Backward) // Back
	{
		Pos.z += (float)timer_1->GetElapsedSeconds(); // Backward
		return;
	}
	if (Modes == Left) // Left
	{
		Pos.x -= (float)timer_1->GetElapsedSeconds(); // Left
		return;
	}
	if (Modes == Right) // Right
	{
		Pos.x += (float)timer_1->GetElapsedSeconds(); // Right
		return;
	}

	return;
}

void SimpleLogic::Init()
{
	timer_1->SetFixedTimeStep(true);
}

void SimpleLogic::ChangeSec(float Time)
{
	timer_1->SetTargetElapsedSeconds(Time);
}