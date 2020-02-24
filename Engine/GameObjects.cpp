#include "pch.h"
#include "GameObjects.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "Console.h"
#include "Physics.h"
#include "Models.h"
#include "SimpleLogic.h"

//void GameObjects::Update()
//{
//}

int GameObjects::Object::ID = 0;

GameObjects::Object::Object(string ID_TEXT, string ModelNameFile, shared_ptr<SimpleLogic> Logic,
	TYPE type, Vector3 PosCoords, Vector3 ScaleCoords, Vector3 RotationCoords)
{
	// Set Up The IDs
	this->ID_TEXT = ID_TEXT;
	ID++;
	// Set Up The Render Model
	SetModel(make_shared<Models>(Application->getFS()->GetFile(ModelNameFile)->PathA));
	if (!model.operator bool())
		Engine::LogError("GameObjects:Object: Create a New Object is failed.",
			"GameObjects:Object: Create Model is failed!!!",
			"GameObjects:Object: Create a New Object is Fail!");
	// Set Up The Logic
	SetLogic(Logic);
	// Set Up The Physic
	//Application->getPhysics()->_createTriMesh(model, false);
	//SetPH(Application->getPhysics()->GetPhysDynamicObject().back());
	this->type = type;

	model->setPosition(PosCoords);
	this->PosCoords = PosCoords;

	const_cast<Vector3 &>(ResetPos) = PosCoords;
	const_cast<Vector3 &>(ResetRot) = RotationCoords;
	const_cast<Vector3 &>(ResetScl) = ScaleCoords;

	model->setRotation(RotationCoords);
	this->RotationCoords = RotationCoords;
	HasRotation = true;

	model->setScale(ScaleCoords);
	this->ScaleCoords = ScaleCoords;
	HasScale = true;
}

void GameObjects::Object::SetLogic(shared_ptr<SimpleLogic> Logic)
{
	this->Logic = Logic;
}

void GameObjects::Object::UpdateLogic(float Time)

{
	if (GetAsyncKeyState(VK_NUMPAD1))
		Test2 += 0.5f;
	if (GetAsyncKeyState(VK_NUMPAD2))
		Test2 -= 0.5f;

	Test1 += Time;
	if (Test1 >= Test2)
	{
		Test1 = 0.0f;
		//if (GetAsyncKeyState(VK_NUMPAD5))
		//	Obj->GetLogic()->follow(Application->getCamera()->GetEyePt());

		Vector3 newPos = ConstrainToBoundary(PosCoords,
			Vector3(-100.f, 0.f, -100.f), Vector3(100.f, 50.f, 100.f)),
			newRot = Vector3::Zero;
		if(Logic.operator bool() && !Logic->GetPoints().empty())
			Logic->Update(PosCoords, RotationCoords);
		//Obj->GetPH()->setGlobalPose(PxTransform(ToPxVec3(newPos)));
	}
}

void GameObjects::Object::Destroy()
{
	if (model.operator bool())
		model->Release();
	//if (PH)
	//	SAFE_release(PH);
}
