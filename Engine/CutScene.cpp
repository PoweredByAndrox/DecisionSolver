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
	curPos = 0;
	Application->getCamera()->Teleport(Points.at(curPos).Pos, Points.at(curPos).Look);
}

void CutScene::Reset()
{
	Points.clear();
	curPos = 0;
}

void CutScene::Update()
{
	if (IsPause || Points.empty() || Points.size() <= (size_t)curPos)
		return;

	auto Cam = Application->getCamera();
	auto CurrentPoint = Points.at(curPos),
		EndPoint = Points.back();
	Vector3 CamPos = Cam->GetEyePt(), CamLook = Cam->GetLookAtPt();

	if (!XMVector3NearEqual(CamPos, CurrentPoint.Pos, Vector3(0.001f, 0.001f, 0.001f)))
		Cam->Teleport(Vector3::SmoothStep( CamPos, CurrentPoint.Pos, Application->getframeTime() * CurrentPoint.Time),
			Vector3::SmoothStep(CamLook, CurrentPoint.Look, Application->getframeTime() * CurrentPoint.Time));
	else
	{
		curPos++;
		if ((size_t)curPos >= Points.size())
			curPos = 0;
	}
}
