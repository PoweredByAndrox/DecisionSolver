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

		void SetPos(Vector3 Pos) { this->Pos = Pos; }
		void SetRotate(Vector3 Rotate) { this->Rotate = Rotate; }
		void SetState(LogicMode State) { TestState = State; }
	private:
		Vector3 Pos = Vector3::Zero, Rotate = Vector3::Zero;
		LogicMode TestState;
	};

	void Init();
	void Update(Vector3 &Pos, Vector3 &Rot);

	void Restart() { Progress = 0; }

	void ChangeSec(float Time);
	void AddNewPoint(Vector3 Pos, Vector3 Rotate, LogicMode TestState);

	int getCurrentPoint() { return Progress; }
	vector<shared_ptr<Point>> GetPoints() { return Points; }
private:
	float Time = 0.5f;
	Vector3 NeededPos = Vector3::Zero;
	int Progress = 0;
	LogicMode CurrentModes;
	vector<shared_ptr<Point>> Points;
};
#endif // !__SIMPLELOGIC_H__