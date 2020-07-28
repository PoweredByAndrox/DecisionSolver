#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "CutScene.h"
#include "Camera.h"

vector<CutScene::Point> CutScene::Points;

void CutScene::AddNewPoint(Vector3 Pos, Vector3 Look, float Time)
{
	Points.push_back(Point(Pos, Look, Time));
}

void CutScene::Start()
{
	IsStart = true;
	IsPause = false;
}

void CutScene::Pause()
{
	IsPause = true;
	IsStart = false;
}

void CutScene::Restart()
{
	Start();
	curPos = 0;
}

void CutScene::Reset()
{
	Points.clear();
	curPos = 0;
}

void CutScene::Update()
{
	if (IsPause || Points.empty())
		return;

	if ((size_t)curPos > Points.size() - 1)
		curPos = 0;

	auto Cam = Application->getCamera();
	auto CurrentPoint = Points.at(curPos);
	Vector3 CamPos = Cam->GetEyePt(), CamLook = Cam->GetLookAtPt();

	if (!XMVector3NearEqual(CamPos, CurrentPoint.Pos, Vector3(0.001f, 0.001f, 0.001f)))
		Cam->Teleport(Vector3::Lerp(CamPos, CurrentPoint.Pos, CurrentPoint.Time),
			Vector3::Lerp(CurrentPoint.Look, CamLook, CurrentPoint.Time));

	else
	{
		curPos++;
		if ((size_t)curPos > Points.size()-1)
			curPos = 0;
	}
}
