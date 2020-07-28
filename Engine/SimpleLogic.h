#pragma once
#ifndef __SIMPLELOGIC_H__
#define __SIMPLELOGIC_H__
#include "pch.h"

class SimpleLogic
{
public:
	enum LogicMode
	{
		Stay = 1,
		// Animation,
		WalkToNewPoint,
		// Jump,
		Follow
	};
	struct Point
	{
		Point(Vector3 Pos, Vector3 Rotate, LogicMode TestState): Pos(Pos), Rotate(Rotate), TestState(TestState) {}
		Vector3 GetPos() { return Pos; }
		Vector3 GetRotate() { return Rotate; }
		LogicMode GetState() { return TestState; }

		void SetPos(Vector3 Position) { Pos = Position; }
		void SetRotate(Vector3 Rotation) { Rotate = Rotation; }
		void SetState(LogicMode State) { TestState = State; }
	private:
		Vector3 Pos = Vector3::Zero, Rotate = Vector3::Zero;
		LogicMode TestState;
	};

	SimpleLogic();
	void Update(Vector3 &Pos, Vector3 &Rot, float dTime);
	void Pause(bool p) { isPaused = p; }
	
	void Restart()
	{
		isPaused = false;
		Progress = 0;
	}

	void AddNewPoint(Vector3 Pos, Vector3 Rotate, LogicMode TestState);

	bool isPause() { return isPaused; }
	int getCurrentPoint() { return Progress; }
	vector<shared_ptr<Point>> &GetPoints() { return Points; }
private:
	int Progress = 0;
	bool isPaused = false;
	LogicMode CurrentModes;
	vector<shared_ptr<Point>> Points;
};
#endif // !__SIMPLELOGIC_H__