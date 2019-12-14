#pragma once
#if !defined(__CUTSCENE_H__)
#define __CUTSCENE_H__
#include "pch.h"

class CutScene
{
public:
	struct Point
	{
		Point() {}
		Point(Vector3 Pos, Vector3 Look, float Time = 0.2f): Pos(Pos), Look(Look), Time(Time) {}
		Vector3 Pos = Vector3::Zero, Look = Vector3::Zero;
		float Time = 0.2f;
	};

	void AddNewPoint(Vector3 Pos, Vector3 Look, float Time);
	void Start();
	void Pause();
	void Reset();
	void Update();

	void Restart();

	bool getIsStart() { return IsStart; }
	bool getIsPause() { return IsPause; }
	int getCurPos() { return curPos; }
	static vector<Point> Points;
private:
	bool IsStart = false, IsPause = false;
	int curPos = 0;
};
#endif // !__CUTSCENE_H__
