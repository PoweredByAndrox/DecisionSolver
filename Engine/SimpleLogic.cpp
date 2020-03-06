#include "pch.h"
#include "SimpleLogic.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

//ToDo("Need To Do Another One Capsule Physics And Add "Jump" To Logic")

SimpleLogic::SimpleLogic()
{
	Points.push_back(make_shared<Point>(Vector3::Zero, -Vector3::Up, WalkToNewPoint));
	Points.push_back(make_shared<Point>(Vector3::One, Vector3::Forward, WalkToNewPoint));
	Points.push_back(make_shared<Point>(Vector3(2, 2, 2), Vector3::Left * 2.f, WalkToNewPoint));
	Points.push_back(make_shared<Point>(Vector3(3, 3, 3), -Vector3::Backward, WalkToNewPoint));
}

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

	if (_Point->GetState() != Stay)
	{
		Pos = Vector3::SmoothStep(Pos, _Point->GetPos(), Application->getframeTime() + 0.85f);
		Rot = Vector3::SmoothStep(Rot, _Point->GetRotate(), Application->getframeTime() + 0.85f);

		if (XMVector3NearEqual(_Point->GetPos(), Pos, Vector3(0.001f, 0.001f, 0.001f)) &&
			XMVector3NearEqual(_Point->GetRotate(), Rot, Vector3(0.001f, 0.001f, 0.001f)))
			Progress++;
	}
	else
		Progress++;
}

void SimpleLogic::ChangeSec(float Time)
{
	//timer_1->SetTargetElapsedSeconds(Time);
}