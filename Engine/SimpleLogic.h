#pragma once
#ifndef __SIMPLELOGIC_H__
#define __SIMPLELOGIC_H__
#include "pch.h"

class StepTimer;
class SimpleLogic
{
public:
	enum LogicMode
	{
		Stay = 0,
		// Animation,
		WalkToNewPoint,
		Jump,
		Follow
	};
	struct Point
	{
		Point(Vector3 Pos, Vector3 Rotate, LogicMode TestState): Pos(Pos), Rotate(Rotate), TestState(TestState) {}
		Vector3 Pos = Vector3::Zero, Rotate = Vector3::Zero;
		LogicMode TestState;
	};

	void Init();
	void Update(Vector3 &Pos, Vector3 &Rot);

	void ChangeSec(float Time);
	void AddNewPoint(Vector3 Pos, Vector3 Rotate, LogicMode TestState);
private:
	float Time = 0.5f;
	shared_ptr<StepTimer> timer_1 = make_shared<StepTimer>();
	Vector3 NeededPos = Vector3::Zero;
	int Progress = 0;
	LogicMode CurrentModes;
};
#endif // !__SIMPLELOGIC_H__