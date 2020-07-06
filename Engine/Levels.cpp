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
		Engine::LogError("Levels->LoadXML()::doc->LoadFile() == nullptr",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Levels: Something is wrong with Load XML File!");
		return E_FAIL;
	}
	else if (doc->ErrorID() != XML_SUCCESS)
	{
		Engine::LogError("Levels->LoadXML()::doc->Parse() returns: " + string(doc->ErrorStr()),
			string(__FILE__) + ": " + to_string(__LINE__),
			"Levels: Something is wrong with Load XML File!\nReturned: " + string(doc->ErrorStr()));
		return E_FAIL;
	}

	if (!FindSubStr(FileBuff, "<scene>"))
	{
		Engine::LogError("Levels::Process:This level is corrupted or empty and load aborted!",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Levels::Process:This level is corrupted or empty and load aborted!");
		return E_FAIL;
	}
	Process();
	return S_OK;
}

void Levels::Spawn(Vector3 pos, GameObjects::TYPE type)
{
//	switch (type)
//	{
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

	//default:
	//	break;
//	}
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

	int I = 0;
	for (auto It: Models)
	{
		Vector3 Pos = Vector3::Zero, Scale = Vector3::Zero,
			Rotate = Vector3::Zero;
		string ID_TEXT, ModelID, ModelFileName, NameOfNode;
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
				ModelID = to_string(I);

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
			}
			if (FirstAttr && ID_TEXT == "file_name")
			{
				ModelFileName = FirstAttr->Value();

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
			}
			if (FirstAttr && ID_TEXT == "name")
			{
				NameOfNode = FirstAttr->Value();

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
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
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
			}
			if (FirstAttr && ID_TEXT == "rotate")
			{
				Result.clear();
				getFloat3Text(FirstAttr->Value(), ",", Result);
				Rotate = Vector3(Result.data());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
			}

			if (FirstAttr && ID_TEXT == "pos")
			{
				Result.clear();
				getFloat3Text(FirstAttr->Value(), ",", Result);
				Pos = Vector3(Result.data());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				ID_TEXT = FirstAttr->Name();
				to_lower(ID_TEXT);
			}
		}

		// Check if the model has in resource of engine then add it to level
		if (exists(Application->getFS()->getPathFromType(_TypeOfFile::MODELS) + ModelFileName))
			Add(_TypeOfFile::MODELS, make_shared<GameObjects::Object>(ModelID, ModelFileName, nullptr,
				type, Pos, Scale, Rotate))->RenderName = NameOfNode.empty() ? ModelID : NameOfNode;
		else
			Engine::LogError((boost::format("Model: %s wasn't find in resources Engine and be skiped") % ModelFileName).str(),
				string(__FILE__) + ": " + to_string(__LINE__),
				(boost::format("Model: %s wasn't find in resources Engine and be skiped") % ModelFileName).str());
		I++;
	}
}

void Levels::Update()
{
	if (MainChild)
		MainChild->Update();
}

shared_ptr<Levels::Node> Levels::Add(_TypeOfFile T, string PathModel)
{
	shared_ptr<Node> nd = make_shared<Node>();

	nd->ID = to_string(Objects.size());
	nd->RenderName = path(PathModel).filename().string();
	nd->GM = make_shared<GameObjects::Object>(nd->ID, nd->RenderName,
		nullptr, Model, Vector3::Zero, Vector3::One, Vector3::Zero);
	if (nd->GM->GetIdText().empty())
		return shared_ptr<Levels::Node>();
	
	return MainChild->AddNewNode(nd);
}

shared_ptr<Levels::Node> Levels::Add(_TypeOfFile T, shared_ptr<GameObjects::Object> GM)
{
	shared_ptr<Node> nd = make_shared<Node>();

	nd->ID = GM->GetIdText();
	nd->RenderName = GM->GetModelNameFile();
	nd->GM = GM;
	if (nd->GM->GetIdText().empty())
		return shared_ptr<Levels::Node>();
	
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

void Levels::RemoveFrom(shared_ptr<Node> nd) // Remove all the logics
{
	if (nd.operator bool() && !nd->ID.empty())
		nd->GM->RemoveLogic();
}

#include "Audio.h"
void Levels::Remove(string ID)
{
	MainChild->DeleteNode(ID);
	if (Application->getSound())
		Application->getSound()->Remove(ID);
}

void Levels::Destroy()
{
	for (auto It: MainChild->GetNodes())
	{
		MainChild->DeleteNode(It->ID);
		if (Application->getSound())
			Application->getSound()->Remove(It->ID);
	}
	if (doc)
		doc->Clear();

	for (auto It: Objects)
	{
		It->Destroy();
	}
	Objects.clear();
}

string Levels::Save(shared_ptr<tinyxml2::XMLDocument> Doc, shared_ptr<Node> Node)
{
	XMLNode *scene = !Doc->FirstChildElement("scene") ? Doc->InsertFirstChild(Doc->NewElement("scene")) :
		Doc->FirstChildElement("scene"), // We're now at <scene>
		*models = nullptr,
		*s_objs = nullptr,
		*model = nullptr,
		*s_obj = nullptr;

	if (Node->SaveInfo->T == GameObjects::TYPE::NONE)
		Node->SaveInfo->T = Node->GM->GetType();

	if (Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj)
	{
		if (scene && (!scene->FirstChildElement("s_objs") || !scene->LastChildElement("s_objs")))
			s_objs = scene->InsertEndChild(Doc->NewElement("s_objs"));
		s_objs = scene->FirstChildElement("s_objs");
	}
	if (Node->SaveInfo->T == GameObjects::TYPE::Model)
	{
		if (scene && (!scene->FirstChildElement("models") || !scene->LastChildElement("models")))
			models = scene->InsertEndChild(Doc->NewElement("models"));
		models = scene->FirstChildElement("models");
	}
	auto Needed = (Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_objs : models)->FirstChildElement();
	if (Needed)
		for (;;)
		{
			string Name = Needed->FirstAttribute()->Value(), id = Node->ID;
			to_lower(Name);
			to_lower(id);
			if (Name == id)
			{
				(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model) = Needed;
				break;
			}

			if (!Needed->NextSibling())
				break;

			Needed = Needed->NextSibling()->ToElement();
			if (!Needed)
				break;
		}

	if (Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj && !s_obj)
	{
		XMLElement *tmp = s_objs->InsertEndChild(Doc->NewElement("s_obj"))->ToElement();
		tmp->SetAttribute("id", Node->ID.c_str());
		tmp->SetAttribute("name", Node->RenderName.c_str());
		tmp->SetAttribute("file_name", path(Node->GM->GetModelNameFile()).filename().string().c_str());
		tmp->SetAttribute("pos", "0.000000, 0.000000, 0.000000");
		tmp->SetAttribute("scale", "0.000000, 0.000000, 0.000000");
		tmp->SetAttribute("rotate", "0.000000, 0.000000, 0.000000");
		s_obj = tmp;
	}

	if (Node->SaveInfo->T == GameObjects::TYPE::Model && !model)
	{
		XMLElement *tmp = models->InsertEndChild(Doc->NewElement("model"))->ToElement();
		tmp->SetAttribute("id", Node->ID.c_str());
		tmp->SetAttribute("name", Node->RenderName.c_str());
		tmp->SetAttribute("file_name", path(Node->GM->GetModelNameFile()).filename().string().c_str());
		tmp->SetAttribute("pos", "0.000000, 0.000000, 0.000000");
		tmp->SetAttribute("scale", "0.000000, 0.000000, 0.000000");
		tmp->SetAttribute("rotate", "0.000000, 0.000000, 0.000000");
		model = tmp;
	}

	if (Node->SaveInfo->IsRemoved)
	{
		if (Node->SaveInfo->T == GameObjects::TYPE::Model && (models && model))
			models->DeleteChild(model);
		else if (Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj && (s_objs && s_obj))
			s_objs->DeleteChild(s_obj);
	}
	else
	{
		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(
			(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)->ToElement()->FirstAttribute());
		for (;;) // Count Of Nodes
		{
			vector<float> Pass;
			string Result,
				nameNode = FirstAttr->Name();
			to_lower(nameNode);

			if (nameNode == "id")
			{
				FirstAttr->SetAttribute(Node->ID.c_str());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				else
					continue;
			}

			if (nameNode == "file_name" || !(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)
				->ToElement()->FindAttribute("file_name"))
			{
				if (!(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)
					->ToElement()->FindAttribute("file_name"))
					(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)
					->ToElement()->SetAttribute("file_name", path(Node->GM->GetModelNameFile()).filename().string().c_str());
				else
					FirstAttr->SetAttribute(path(Node->GM->GetModelNameFile()).filename().string().c_str());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				else
					continue;
			}
			if (nameNode == "name" || !(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)
				->ToElement()->FindAttribute("name"))
			{
				if (!(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)
					->ToElement()->FindAttribute("name"))
					(Node->SaveInfo->T == GameObjects::TYPE::Sound_Obj ? s_obj : model)
					->ToElement()->SetAttribute("name", Node->RenderName.c_str());
				else
					FirstAttr->SetAttribute(Node->RenderName.c_str());

				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				else
					continue;
			}

			if (Node->SaveInfo->Scale && nameNode == "scale")
			{
				Pass.push_back(Node->GM->GetScaleCord().x);
				Pass.push_back(Node->GM->GetScaleCord().y);
				Pass.push_back(Node->GM->GetScaleCord().z);

				getTextFloat3(Result, ", ", Pass);
				FirstAttr->SetAttribute(Result.c_str());

				Node->SaveInfo->Scale = false;
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				else
					continue;
			}
			else if (Node->SaveInfo->Rot && nameNode == "rotate")
			{
				Pass.push_back(Node->GM->GetRotCord().x);
				Pass.push_back(Node->GM->GetRotCord().y);
				Pass.push_back(Node->GM->GetRotCord().z);

				getTextFloat3(Result, ", ", Pass);
				FirstAttr->SetAttribute(Result.c_str());

				Node->SaveInfo->Rot = false;
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				else
					continue;
			}
			else if (Node->SaveInfo->Pos && nameNode == "pos")
			{
				Pass.clear();
				Result.clear();
				Pass.push_back(Node->GM->GetPositionCord().x);
				Pass.push_back(Node->GM->GetPositionCord().y);
				Pass.push_back(Node->GM->GetPositionCord().z);

				getTextFloat3(Result, ", ", Pass);
				FirstAttr->SetAttribute(Result.c_str());

				Node->SaveInfo->Pos = false;
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
				else
					continue;
			}
			else
			{
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
		}
	}
	XMLPrinter Prntr;
	Doc->Print(&Prntr);
	doc = Doc; // Update Our New XML Construction Of File

	return Prntr.CStr();
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
		if (!it->RenderIt || Nodes.at(i)->SaveInfo->IsRemoved)
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
	to_lower(ID);
	for (auto it: Nodes)
	{
		to_lower(it->ID);
		if (ID == it->ID)
			return it;
	}

	return make_shared<Node>();
}
