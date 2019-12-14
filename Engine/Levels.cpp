#include "pch.h"
#include "Levels.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "Physics.h"
#include "Camera.h"
#include "UI.h"

vector<shared_ptr<GameObjects::Object>> Levels::Obj_other, Levels::Obj_npc;
vector<string> Levels::IDModels;

HRESULT Levels::LoadXML(string File)
{
	doc = make_shared<tinyxml2::XMLDocument>();

	doc->LoadFile(File.c_str());
	if (doc->ErrorID() > 0)
	{
		Application->StackTrace(doc->ErrorStr());
		Engine::LogError("Levels->LoadXML()::doc->LoadFile() == nullptr", "Levels->LoadXML()::doc->LoadFile() == nullptr",
			"Levels: Something is wrong with Load XML File!");
		return E_FAIL;
	}
	else if (doc->Parse(Application->getFS()->getDataFromFile(string(File), true).c_str()) > 0)
	{
		Engine::LogError((boost::format("Levels->LoadXML()::doc->Parse() returns: %s") % string(doc->ErrorStr())).str(),
			(boost::format("Levels->LoadXML()::doc->Parse() returns: %s") % string(doc->ErrorStr())).str(),
			(boost::format("Levels: Something is wrong with Load XML File!\nReturned: %s") % string(doc->ErrorStr())).str());
		return E_FAIL;
	}

	ProcessXML();
	return S_OK;
}

vector<shared_ptr<GameObjects::Object>> Levels::XMLPreparing(vector<XMLElement *> Attrib)
{
	vector<shared_ptr<GameObjects::Object>> g_Obj;
	for (int i = 0;; i++)
	{
		Vector3 Pos = Vector3::Zero, Scale = Vector3::Zero,
			Rotate = Vector3::Zero;
		string ID_TEXT = "", ModelName = "";
		shared_ptr<SimpleLogic> Logic = make_shared<SimpleLogic>();

		GameObjects::TYPE type;
		if (Attrib.back()->Parent() && strcmp(Attrib.back()->Parent()->Value(), "objects") == 0)
			type = GameObjects::TYPE::OBJECTS_Dyn;
		else if (Attrib.back()->Parent() && strcmp(Attrib.back()->Parent()->Value(), "npc") == 0)
			type = GameObjects::TYPE::NPC;
		else
			type = GameObjects::TYPE::NONE;

		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Attrib.back()->ToElement()->FirstAttribute());
		for (;;) // Count Of Arguments
		{
			if (FirstAttr && strcmp(FirstAttr->Name(), "id") == 0)
			{
				ID_TEXT = FirstAttr->Value();
				replaceAll(ID_TEXT, string(" "), string("_"));
				ModelName = FirstAttr->Value();

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
		}

		vector<XMLNode *> Node = { Attrib.back()->FirstChild() };
		for (;;) // Count Of Nodes
		{
			vector<float> Result;
			if (Node.back() && strcmp(Node.back()->Value(), "scale") == 0)
			{
				Result.clear();
				getFloat3Text(Node.back()->FirstChild()->Value(), ",", Result);
				Scale = Vector3(Result.data());

				if (!Node.back()->NextSibling())
					break;
				else
					Node.push_back(Node.back()->NextSibling());
			}
			if (Node.back() && strcmp(Node.back()->Value(), "rotate") == 0)
			{
				Result.clear();
				getFloat3Text(Node.back()->FirstChild()->Value(), ",", Result);
				Rotate = Vector3(Result.data());

				if (!Node.back()->NextSibling())
					break;
				else
					Node.push_back(Node.back()->NextSibling());
			}
		
			if (Node.back() && strcmp(Node.back()->Value(), "pos") == 0)
			{
				Result.clear();
				getFloat3Text(Node.back()->FirstChild()->Value(), ",", Result);
				Pos = Vector3(Result.data());

				if (!Node.back()->NextSibling())
					break;
				else
					Node.push_back(Node.back()->NextSibling());
			}
		}

		g_Obj.push_back(make_shared<GameObjects::Object>(ID_TEXT, ModelName, Logic, type, Pos, Scale, Rotate));
		IDModels.push_back(ID_TEXT);
		if (Attrib.front()->LastChild()->Value() == Attrib.back()->Value())
			break;

		if (Attrib.back()->NextSibling())
			Attrib.push_back(Attrib.back()->NextSibling()->ToElement());
	}

	return g_Obj;
}

void Levels::Spawn(Vector3 pos, GameObjects::TYPE type)
{
	switch (type)
	{
	case GameObjects::OBJECTS_Dyn:
		//Obj_other.push_back(make_shared<GameObjects::Object>(ID_TEXT, i, ModelName, Logic, type, Pos, Scale, Rotate));
		break;
//	case GameObjects::NPC:
//		break;
//	case GameObjects::ACTOR:
//		break;
//	case GameObjects::OBJECTS_Stat:
//		break;
//	case GameObjects::ETC:
//		break;
//	case GameObjects::NONE:
//		break;

	default:
		break;
	}
}

void Levels::Reload_Level(string File)
{
	Obj_other.clear();
	Obj_npc.clear();
	LoadXML(File);
}

void Levels::ProcessXML()
{
	vector<XMLElement *> Attrib = { doc->RootElement()->FirstChild()->ToElement() };

	if (!Attrib.back())
	{
		Engine::LogError("Levels->LoadXML()::doc->RootElement() == nullptr",
			"Levels->LoadXML()::doc->RootElement() == nullptr",
			"Levels: Something is wrong with Process XML File!");
		return;
	}

	string cache = doc->RootElement()->FirstChild()->ToElement()->Value();
	to_lower(cache);

	if (strcmp(cache.c_str(), "objects") == 0)
	{
		if (!Attrib.back()->FirstChild())
			return;

		Attrib.push_back(Attrib.back()->FirstChild()->ToElement());
		Obj_other = XMLPreparing(Attrib);
	}
	else if (strcmp(cache.c_str(), "npc") == 0)
	{
		if (!Attrib.back()->FirstChild())
			return;

		Attrib.push_back(Attrib.back()->FirstChild()->ToElement());
		Obj_npc = XMLPreparing(Attrib);
	}
}

#include "DebugDraw.h"
void Levels::Update(Matrix View, Matrix Proj, float Time)
{
	for (auto it : Obj_other)
	{
		if (!it->RenderIt)
			continue;

		auto Model = it->GetModel();
		if (it->GetScale())
			Model->setScale(it->GetScaleCord());
		if (it->GetRotation())
			Model->setRotation(it->GetRotCord());

		UpdateLogic(Time, it);
		Model->setPosition(it->GetPositionCord());
		Model->Render(View, Proj);
	}

	// NPC
	for (auto it : Obj_npc)
	{
		if (!it->RenderIt)
			continue;

		auto Model = it->GetModel();
		if (it->GetScale())
			Model->setScale(it->GetScaleCord());
		if (it->GetRotation())
			Model->setRotation(it->GetRotCord());

		//it->SetPositionCoords(UpdateLogic(Time, it));
		Model->setPosition(it->GetPositionCord());
		Model->Render(View, Proj);
	}

	//if (LOGO.operator bool() && !LOGO->GetTitle().empty())
	//	LOGO->Render();
}

void Levels::Destroy()
{
	while (!Obj_other.empty())
	{
		Obj_other.front()->GetModel()->Release();
		Obj_other.front()->Destroy();
		Obj_other.erase(Obj_other.begin());
	}
	while (!Obj_npc.empty())
	{
		Obj_npc.front()->GetModel()->Release();
		Obj_npc.front()->Destroy();
		Obj_npc.erase(Obj_npc.begin());
	}
}

float Test1 = 1.0f, Test2 = 3.0f;
void Levels::UpdateLogic(float Time, shared_ptr<GameObjects::Object> &Obj)
{
	if (GetAsyncKeyState(VK_NUMPAD1))
		Test2 += 0.05f;
	if (GetAsyncKeyState(VK_NUMPAD2))
		Test2 -= 0.05f;

	Test1 += Time;
	if (Test1 >= Test2)
	{
		Test1 = 0.0f;
		//if (GetAsyncKeyState(VK_NUMPAD5))
		//	Obj->GetLogic()->follow(Application->getCamera()->GetEyePt());
		
		//Vector3 newPos = ConstrainToBoundary(Obj->GetPositionCord(),
		//	Vector3(-100.f, 0.f, -100.f), Vector3(100.f, 50.f, 100.f)),
		//	newRot = Vector3::Zero;
		Vector3 newPos = Obj->GetPositionCord(), newRot = Obj->GetRotCord();
		Obj->GetLogic()->Update(newPos, newRot);
		Obj->SetRotationCoords(newRot);
		//Obj->GetPH()->setGlobalPose(PxTransform(ToPxVec3(newPos)));
		Obj->SetPositionCoords(newPos);
	}
}

HRESULT Levels::Init()
{
	auto MapFiles = Application->getFS()->GetFileByType(_TypeOfFile::LEVELS);
	for (size_t i = 0; i < MapFiles.size(); i++)
	{
		EngineTrace(LoadXML(MapFiles.at(i)->PathA.c_str()));
	}

	return S_OK;
}