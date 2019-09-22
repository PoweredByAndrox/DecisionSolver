#include "pch.h"
#include "GameObjects.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "Console.h"
#include "Physics.h"

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
	Application->getPhysics()->_createTriMesh(model, false);
	SetPH(Application->getPhysics()->GetPhysDynamicObject().back());
	this->type = type;

	model->setPosition(PosCoords);
	this->PosCoords = PosCoords;

	model->setRotation(RotationCoords);
	this->RotationCoords = RotationCoords;
	HasRotation = true;

	model->setScale(ScaleCoords);
	this->ScaleCoords = ScaleCoords;
	HasScale = true;
}
