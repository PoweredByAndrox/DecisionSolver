#include "pch.h"
#include "SimpleLogic.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

//ToDo("Need To Do Another One Capsule Physics And Add "Jump" To Logic")

void SimpleLogic::AddNewPoint(Vector3 Pos, Vector3 Rotate, LogicMode TestState)
{
	Points.push_back(make_shared<Point>(Pos, Rotate, TestState));
}

void SimpleLogic::Update(Vector3 &Pos, Vector3 &Rot)
{
	if (Points.empty())
		return;

	if ((size_t)Progress >= Points.size())
		Progress = 0;

	auto _Point = Points.at(Progress);
	//if (CurrentModes != Follow && (CurrentModes == WalkToNewPoint || CurrentModes == Stay
	//	&& timer_1->GetElapsedSeconds() <= 0.5f)) // Wait for five (0.5f) seconds and go!
	//{
	//	CurrentModes = _Point->GetState();
	//	Progress++;
	//}

	if (CurrentModes != Stay)
	{
		Pos = Vector3::SmoothStep(Pos, _Point->GetPos(), Application->getframeTime() * 0.85f);
		Rot = Vector3::SmoothStep(Rot, _Point->GetRotate(), Application->getframeTime() * 0.85f);
	}
}

void SimpleLogic::Init()
{
	//timer_1->SetFixedTimeStep(true);
}

void SimpleLogic::ChangeSec(float Time)
{
	//timer_1->SetTargetElapsedSeconds(Time);
}