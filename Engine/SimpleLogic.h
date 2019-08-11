#pragma once
#ifndef __SIMPLELOGIC_H__
#define __SIMPLELOGIC_H__
#include "pch.h"

class StepTimer;
class SimpleLogic
{
private:
	float Time = 0.5f;
	shared_ptr<StepTimer> timer_1 = make_shared<StepTimer>();
	Vector3 NeededPos = Vector3::Zero;
	int Progress;

public:
	void Init();
	void Update(Vector3 &Pos);

	void ChangeSec(float Time);
	void follow(Vector3 Where);

	enum LogicMode
	{
		Forward = 0,
		Backward,
		Left,
		Right,
		//Jump,
		Follow
	} Modes;
};
#endif // !__SIMPLELOGIC_H__