#pragma once
#if !defined(__PHYSCAMERA_H__)
#define __PHYSCAMERA_H__
#include "pch.h"

#include "Camera_Control.h"

class PhysCamera: public Camera_Control
{
private:
	enum State
	{
		Crouch = 0,
		Standup,
	} state = State::Standup;

	struct Jump
	{
	private:
		bool CanJump = true;
		float JumpTimes = 0.f, ForceJump = 0.f;
	public:
		void Start(PxF32 Force);
		void Stop();
		float getHeight(float elapsedTime = 0.f);
		bool getCanJump() { return CanJump; }
	};
	shared_ptr<Jump> jump = make_shared<Jump>();

public:
	void TryStandup();
	void Update();
	State getState() { return state; }

	shared_ptr<Jump> getJump() { return jump; }
};
#endif
