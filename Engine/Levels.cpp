#include "pch.h"
#include "Levels.h"
using namespace Engine;

HRESULT Levels::LoadXML(LPCSTR File)
{
	doc = make_unique<tinyxml2::XMLDocument>();

	doc->LoadFile(File);
	if (doc->ErrorID() > 0)
	{
		StackTrace(doc->ErrorStr());
		throw exception("Levels->LoadXML()::doc->LoadFile() == 0!!!");
		return E_FAIL;
	}
	if (doc->Parse(FS->getDataFromFile(&string(File), true).c_str()) > 0)
		{
			throw exception(string(string("Levels->LoadXML()::doc->Parse: \n") + string(doc->ErrorStr())).c_str());
			return E_FAIL;
		}

	ProcessXML();
	return S_OK;
}

void Levels::ProcessXML()
{
	Vector3 XYZ;

	ToDo("Need To Solve This Bug!!!");
	//if (!doc->RootElement()->FirstChild()->ToComment())
	Attrib = { doc->RootElement()->FirstChild()->ToElement() };
	//else
	//	Attrib = { doc->RootElement()->FirstChild()->ToComment()->NextSibling()->ToElement() };
	//XMLComment* comment
	
	if (!Attrib.back())
	{
		DebugTrace("Levels->ProcessXML()::doc->RootElement() == nullptr!!!");
		throw exception("Levels->ProcessXML()::doc->RootElement() == nullptr!!!");
		return;
	}

	string cache = Attrib.back()->Value();
	to_lower(cache);
	if (strcmp(cache.c_str(), "objects") == 0)
	{
		//if (!Attrib.back()->FirstChild()->ToComment())
		Attrib.push_back(Attrib.back()->FirstChild()->ToElement());
		//else
		//Attrib.push_back(Attrib.back()->FirstChild()->ToComment()->NextSibling()->ToElement());

		for (int i = 1; i < INT16_MAX; i++)
		{
			g_Obj.at(i-1).ID = i;
			XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Attrib.back()->ToElement()->FirstAttribute());
			for (int i = 1; i < INT16_MAX; i++)
			{
				if (Attrib.back()->ToElement()->FirstChild())
					if (strcmp(Attrib.back()->ToElement()->FirstChild()->Value(), "scale") == 0)
					{
						Nods.push_back(Attrib.back()->ToElement()->FirstChild());
						string str = Nods.back()->FirstChild()->Value();
						str = deleteWord(str, ',', ' ');
						sscanf_s(str.c_str(), "%f %f %f", &XYZ.x, &XYZ.y, &XYZ.z);
						g_Obj.at(i-1).HasScale = true;
						g_Obj.at(i-1).model->setScale(XYZ);
						XYZ = Vector3::Zero;
					}

				if (strcmp(FirstAttr->Name(), "id") == 0)
				{
					g_Obj.at(i-1).ID_TEXT = FirstAttr->Value();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}

				if (strcmp(FirstAttr->Name(), "x") == 0)
				{
					XYZ.x = FirstAttr->FloatValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}

				if (strcmp(FirstAttr->Name(), "y") == 0)
				{
					XYZ.y = FirstAttr->FloatValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}

				if (strcmp(FirstAttr->Name(), "z") == 0)
				{
					XYZ.z = FirstAttr->FloatValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					g_Obj.at(i-1).model->setPosition(XYZ);
					if (!FirstAttr)
						break;
				}
			}
			if (/*!Attrib.front()->LastChild()->ToComment() && */Attrib.front()->LastChild()->Value() == Attrib.back()->Value())
				break;

			//if (!Attrib.back()->NextSibling()->ToComment())
				Attrib.push_back(Attrib.back()->NextSibling()->ToElement());
			//else
			//	Attrib.push_back(Attrib.back()->NextSibling()->ToComment()->NextSibling()->ToElement());
		}
	}
}

HRESULT Engine::Levels::Init()
{
	try
	{
		auto Files = FS->getFilesInFolder(&string("models"), ".obj");
		for (int i = 0; i < Files.size(); i++)
			g_Obj.push_back(GameObjects::Object(new Models(&Files.at(i))));

		LoadXML(FS->GetFile(string("first_level.xml"))->PathA.c_str());

		InitClass = true;
		return S_OK;
	}
	catch (const exception &Catch)
	{
		DebugTrace(string(string("Levels: Init is failed. ") + string(Catch.what())).c_str());
		throw exception("Levels:init() is failed!!!");
		InitClass = false;
		return E_FAIL;
	}
}