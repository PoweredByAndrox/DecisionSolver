#include "pch.h"
#include "Levels.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "Physics.h"
#include "Camera.h"
#include "Models.h"
#include "SimpleLogic.h"

//vector<shared_ptr<GameObjects::Object>> Levels::Obj_other, Levels::Obj_npc;
//vector<string> Levels::IDModels;

HRESULT Levels::Load(string FileBuff)
{
	deleteWord(FileBuff, string("<!--"), string("-->"));
	if (doc->Parse(FileBuff.c_str()) > 0)
	{
		Application->StackTrace(doc->ErrorStr());
		Engine::LogError("Levels->LoadXML()::doc->LoadFile() == nullptr", "Levels->LoadXML()::doc->LoadFile() == nullptr",
			"Levels: Something is wrong with Load XML File!");
		return E_FAIL;
	}
	else if (doc->ErrorID() != XML_SUCCESS)
	{
		Engine::LogError("Levels->LoadXML()::doc->Parse() returns: " + string(doc->ErrorStr()),
			"Levels->LoadXML()::doc->Parse() returns: " + string(doc->ErrorStr()),
			"Levels: Something is wrong with Load XML File!\nReturned: " + string(doc->ErrorStr()));
		return E_FAIL;
	}

	if (!FindSubStr(FileBuff, "<scene>"))
	{
		Engine::LogError("Levels::Process:This level is corrupted or empty and load aborted!",
			"Levels::Process:The level is corrupted or empty and load aborted!",
			"Levels::Process:This level is corrupted or empty and load aborted!");
		return E_FAIL;
	}
	Process();
	return S_OK;
}

void Levels::Spawn(Vector3 pos, GameObjects::TYPE type)
{
	switch (type)
	{
	//case GameObjects::OBJECTS_Dyn:
		//Obj_other.push_back(make_shared<GameObjects::Object>(ID_TEXT, i, ModelName, Logic, type, Pos, Scale, Rotate));
	//	break;
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
	//Obj_other.clear();
	//Obj_npc.clear();
	// rework it!
	//LoadXML(File);
}

void Levels::Process()
{
	bool IsModels = true, IsSobjs = true; // If do not then abort create new nodes

	XMLNode *scene = doc->FirstChildElement("scene"), // We're now at <scene>
		*models = nullptr,
		*s_objs = nullptr;

	// Does the matter between First or Last?
	if (scene && scene->FirstChildElement("s_objs"))
		s_objs = scene->FirstChildElement("s_objs");
	else if (scene && scene->LastChildElement("s_objs"))
		s_objs = scene->LastChildElement("s_objs");
	else
		IsSobjs = false;

	// Does the matter between First or Last?
	if (scene && scene->FirstChildElement("models"))
		models = scene->FirstChildElement("models");
	else if (scene && scene->LastChildElement("models"))
		models = scene->LastChildElement("models");
	else
		IsModels = false;


	vector<XMLElement *> Models, S_objs;
	if (IsModels)
	{
		for (;;)
		{
			if (models->NoChildren()) break; // <models/>
			if (Models.empty())
				Models.push_back(models->FirstChild()->ToElement());

			if (Models.back() && Models.back()->NextSibling())
				Models.push_back(Models.back()->NextSibling()->ToElement());

			if (!Models.back()->NextSibling())
				break;
		}
	}
	if (IsSobjs)
	{
		for (;;)
		{
			if (s_objs->NoChildren()) break; // <s_objs/>
			if (S_objs.empty())
				S_objs.push_back(s_objs->ToElement());

			if (S_objs.back() && S_objs.back()->NextSibling())
				S_objs.push_back(S_objs.back()->NextSibling()->ToElement());

			if (!S_objs.back()->NextSibling())
				break;
		}
	}

	for (auto It: Models)
	{
		Vector3 Pos = Vector3::Zero, Scale = Vector3::Zero,
			Rotate = Vector3::Zero;
		string ID_TEXT, ModelName;
		auto type = GameObjects::TYPE::Model;

		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(It->FirstAttribute());
		for (;;)
		{
			if (FirstAttr)
			{
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
			}

			if (FirstAttr && ID_TEXT == "id")
			{
				ModelName = FirstAttr->Value();

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			vector<float> Result;
			if (FirstAttr && ID_TEXT == "scale")
			{
				Result.clear();
				getFloat3Text(FirstAttr->Value(), ",", Result);
				Scale = Vector3(Result.data());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (FirstAttr && ID_TEXT == "rotate")
			{
				Result.clear();
				getFloat3Text(FirstAttr->Value(), ",", Result);
				Rotate = Vector3(Result.data());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (FirstAttr && ID_TEXT == "pos")
			{
				Result.clear();
				getFloat3Text(FirstAttr->Value(), ",", Result);
				Pos = Vector3(Result.data());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
		}
		Add(_TypeOfFile::MODELS, make_shared<GameObjects::Object>(ModelName, ModelName, nullptr,
			type, Pos, Scale, Rotate));
	}
}

void Levels::Update()
{
	MainChild->Update();
}

shared_ptr<Levels::Node> Levels::Add(_TypeOfFile T, string PathModel)
{
	shared_ptr<Node> nd = make_shared<Node>();

	nd->ID = path(PathModel).filename().string();
	nd->GM = make_shared<GameObjects::Object>(nd->ID, PathModel,
		nullptr, Model, Vector3::Zero, Vector3::One, Vector3::Zero);
	return MainChild->AddNewNode(nd);
}

shared_ptr<Levels::Node> Levels::Add(_TypeOfFile T, shared_ptr<GameObjects::Object> GM)
{
	shared_ptr<Node> nd = make_shared<Node>();

	nd->ID = GM->GetIdText();
	nd->GM = GM;
	return MainChild->AddNewNode(nd);
}

void Levels::AddTo(string ID, shared_ptr<SimpleLogic> Logic)
{
	auto Obj = MainChild->getNodeByID(ID);
	if (Obj.operator bool() && !Obj->ID.empty())
		Obj->GM->SetLogic(Logic);
}

void Levels::AddTo(shared_ptr<Node> nd, shared_ptr<SimpleLogic> Logic)
{
	if (nd.operator bool() && !nd->ID.empty())
		nd->GM->SetLogic(Logic);
}

void Levels::Remove(string ID)
{
	MainChild->DeleteNode(ID);
}

void Levels::Destroy()
{
	//while (!Obj_other.empty())
	//{
	//	Obj_other.front()->GetModel()->Release();
	//	Obj_other.front()->Destroy();
	//	Obj_other.erase(Obj_other.begin());
	//}
	//while (!Obj_npc.empty())
	//{
	//	Obj_npc.front()->GetModel()->Release();
	//	Obj_npc.front()->Destroy();
	//	Obj_npc.erase(Obj_npc.begin());
	//}
}

ToDo("Add Struct To Save Local Obj To File");
string Levels::Save(shared_ptr<tinyxml2::XMLDocument> Doc, shared_ptr<Node> Node)
{
	/// <scene>
	vector<XMLElement *> Attrib = { Doc->RootElement()->FirstChild()->ToElement() };
	string cache = Doc->RootElement()->FirstChild()->ToElement()->Value();
	to_lower(cache);

	/// <model>
	if (strcmp(cache.c_str(), "model") == 0)
	{
		if (!Attrib.back()->FirstChild())
			return "";

		Attrib.push_back(Attrib.back()->FirstChild()->ToElement());
		for (;;)
		{
			GameObjects::TYPE type;
			if (Attrib.back()->Parent() && strcmp(Attrib.back()->Parent()->Value(), "model") == 0)
			{
				type = GameObjects::TYPE::Model;
			}
			else if (Attrib.back()->Parent() && strcmp(Attrib.back()->Parent()->Value(), "s_obj") == 0)
			{
				type = GameObjects::TYPE::Sound_Obj;
			}
			//else if (Attrib.back()->Parent() && strcmp(Attrib.back()->Parent()->Value(), "npc") == 0)
			//	type = GameObjects::TYPE::NPC;
			else
			{
				type = GameObjects::TYPE::NONE;
			}

			/// <id="ak47"\>
			//XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Attrib.back()->ToElement()->FirstAttribute());
			//for (;;) // Count Of Arguments
			//{
			//	if (FirstAttr && strcmp(FirstAttr->Name(), "id") == 0)
			//	{
			//		ModelName = FirstAttr->Value();
			//		FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			//		if (!FirstAttr)
			//			break;
			//	}
			//}
			/// <id="ak47"\>
			/// <scale>0,0,0</>
			vector<XMLNode *> xmlNode = { Attrib.back()->FirstChild() };
			for (;;) // Count Of Nodes
			{
				vector<float> Pass;
				string Result;
				if (Node->SaveInfo->Scale && xmlNode.back() && strcmp(xmlNode.back()->Value(), "scale") == 0)
				{
					Pass.push_back(Node->GM->GetScaleCord().x);
					Pass.push_back(Node->GM->GetScaleCord().y);
					Pass.push_back(Node->GM->GetScaleCord().z);

					getTextFloat3(Result, ", ", Pass);
					xmlNode.back()->FirstChild()->SetValue(Result.c_str());

					if (!xmlNode.back()->NextSibling())
						break;
					else
						xmlNode.push_back(xmlNode.back()->NextSibling());
					Node->SaveInfo->Scale = false;
				}
				else if (Node->SaveInfo->Rot && xmlNode.back() && strcmp(xmlNode.back()->Value(), "rotate") == 0)
				{
					Pass.push_back(Node->GM->GetRotCord().x);
					Pass.push_back(Node->GM->GetRotCord().y);
					Pass.push_back(Node->GM->GetRotCord().z);

					getTextFloat3(Result, ", ", Pass);
					xmlNode.back()->FirstChild()->SetValue(Result.c_str());

					Node->SaveInfo->Rot = false;
					if (!xmlNode.back()->NextSibling())
						break;
					else
						xmlNode.push_back(xmlNode.back()->NextSibling());
				}
				else if (Node->SaveInfo->Pos && xmlNode.back() && strcmp(xmlNode.back()->Value(), "pos") == 0)
				{
					Pass.clear();
					Result.clear();
					Pass.push_back(Node->GM->GetPositionCord().x);
					Pass.push_back(Node->GM->GetPositionCord().y);
					Pass.push_back(Node->GM->GetPositionCord().z);

					getTextFloat3(Result, ", ", Pass);
					xmlNode.back()->FirstChild()->SetValue(Result.c_str());

					Node->SaveInfo->Pos = false;
					if (!xmlNode.back()->NextSibling())
						break;
					else
						xmlNode.push_back(xmlNode.back()->NextSibling());
				}
				else
					// If everything is same exit from this loop
					if (!xmlNode.back()->NextSibling())
						break;
					else
						xmlNode.push_back(xmlNode.back()->NextSibling());
			}

			if (Attrib.front()->LastChild()->Value() == Attrib.back()->Value())
				break;

			if (Attrib.back()->NextSibling())
				Attrib.push_back(Attrib.back()->NextSibling()->ToElement());
		}
	}

	return "";
}

HRESULT Levels::Init()
{
	//auto MapFiles = Application->getFS()->GetFileByType(_TypeOfFile::LEVELS);
	//for (size_t i = 0; i < MapFiles.size(); i++)
	//{
	//	EngineTrace(LoadXML(MapFiles.at(i)->PathA.c_str()));
	//}

	return S_OK;
}

shared_ptr<Levels::Node> Levels::Child::AddNewNode(shared_ptr<Node> ND)
{
	for (auto It: Nodes)
	{
		if (It->ID == ND->ID)
			ND->ID = "$" + ND->ID;
	}
	Nodes.push_back(ND);
	return Nodes.back();
}

void Levels::Child::DeleteNode(string ID)
{
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (ID == Nodes.at(i)->ID)
		{
			//Nodes.at(i)->GM->Destroy();
			Nodes.erase(Nodes.begin() + i);
		}
	}
}

void Levels::Child::Update()
{
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		auto it = Nodes.at(i)->GM;
		if (!it->RenderIt)
			continue;

		auto Model = it->GetModel();
		if (it->GetScale())
			Model->setScale(it->GetScaleCord());
		if (it->GetRotation())
			Model->setRotation(it->GetRotCord());

		it->UpdateLogic(Application->getframeTime());
		Model->setPosition(it->GetPositionCord());
		Model->Render(Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix());
	}
}

shared_ptr<Levels::Node> Levels::Child::getNodeByID(string ID)
{
	for (auto it: Nodes)
	{
		if (ID == it->ID)
			return it;
	}

	return make_shared<Node>();
}
