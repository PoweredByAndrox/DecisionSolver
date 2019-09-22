#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "CutScene.h"
#include "Camera.h"

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

void CutScene::Reset()
{
	curPos = 0;
	Application->getCamera()->Teleport(Points.at(curPos).Pos, Points.at(curPos).Look);
}

void CutScene::Update()
{
	if (IsPause || Points.empty())
		return;

	auto Cam = Application->getCamera();
	auto CurrentPoint = Points.at(curPos),
		EndPoint = Points.back();
	Vector3 CamPos = Cam->GetEyePt(), CamLook = Cam->GetLookAtPt();

	if (!XMVector3NearEqual(CamPos, CurrentPoint.Pos, Vector3(0.001f, 0.001f, 0.001f)))
		ToDo("In Release It's Too Fast!!!")
		Cam->Teleport(Vector3::SmoothStep(CamPos, CurrentPoint.Pos, CurrentPoint.Time),
			Vector3::SmoothStep(CamLook, CurrentPoint.Look, CurrentPoint.Time));
	else
	{
		curPos++;
		if ((size_t)curPos >= Points.size())
			curPos = 0;
	}
}
