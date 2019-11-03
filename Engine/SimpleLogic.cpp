#include "pch.h"
#include "SimpleLogic.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "StepTimer.h"

//ToDo("Need To Do Another One Capsule Physics And Add Jump To Logic")
vector<SimpleLogic::Point> Points =
{
	SimpleLogic::Point(Vector3(1.5f, 0.f, 1.5), Vector3::Zero, SimpleLogic::LogicMode::Stay), // 0
	SimpleLogic::Point(Vector3(-1.5f, 0.f, 1.5), Vector3::Zero, SimpleLogic::LogicMode::Stay), // 1
	SimpleLogic::Point(Vector3(-1.5f, 0.f, -1.5), Vector3::Zero, SimpleLogic::LogicMode::Stay), // 2
	SimpleLogic::Point(Vector3(1.5f, 0.f, -1.5), Vector3::Zero, SimpleLogic::LogicMode::Stay), // 3
};

void SimpleLogic::AddNewPoint(Vector3 Pos, Vector3 Rotate, LogicMode TestState)
{
	Points.push_back(Point(Pos, Rotate, TestState));
}

void SimpleLogic::Update(Vector3 &Pos, Vector3 &Rot)
{
	if (!timer_1->GetIsFixedTimeStep())
	{
		throw exception("This Timer hasn't set for Fixed Time!");
		return;
	}

	timer_1->Tick();

	if (GetAsyncKeyState(VK_OEM_PLUS))
		Time += 0.05f;
	if (GetAsyncKeyState(VK_OEM_MINUS))
		Time -= 0.05f;

	timer_1->SetTargetElapsedSeconds(Time);

	//OutputDebugStringA((boost::format("\nTime1: %f") % Time).str().c_str());

	if ((size_t)Progress >= Points.size())
		Progress = 0;

	auto _Point = Points.at(Progress);
	if (CurrentModes != Follow && (CurrentModes == WalkToNewPoint || CurrentModes == Stay
		&& timer_1->GetElapsedSeconds() >= Time)) // Wait for five (0.5f) seconds and go!
	{
		CurrentModes = _Point.TestState;
		Progress++;
	}

	//if (CurrentModes == Follow)
	//{
	//	Pos = Vector3::SmoothStep(Pos, NeededPos, timer_1->GetElapsedSeconds());
	//	Rot = Vector3::SmoothStep(Rot, timer_1->GetElapsedSeconds());
	//}

	if (CurrentModes != Stay)
	{
		Pos = Vector3::SmoothStep(Pos, _Point.Pos, 0.85f);
		Rot = Vector3::SmoothStep(Rot, _Point.Rotate, timer_1->GetElapsedSeconds());
	}
}

void SimpleLogic::Init()
{
	timer_1->SetFixedTimeStep(true);
}

void SimpleLogic::ChangeSec(float Time)
{
	timer_1->SetTargetElapsedSeconds(Time);
}