#include "pch.h"
#include "Dialogs.h"

HRESULT Engine::Dialogs::Init(File_system *FS)
{
	this->FS = FS;

	doc = make_unique<tinyxml2::XMLDocument>();
	if (!doc.operator bool())
	{
		throw exception("Dialogs->Init()::doc == nullptr!!!");
		InitClass = false;
		return E_FAIL;
	}

	InitClass = true;
	return S_OK;
}

HRESULT Engine::Dialogs::LoadFile(string *FileName)
{
	doc->LoadFile(FS->GetFile(*FileName)->PathA.c_str());
	if (doc->ErrorID() > 0)
	{
		StackTrace(doc->ErrorStr());
		throw exception(string(string("Dialogs->LoadFile()::doc->LoadFile:\n") + string(doc->ErrorStr())).c_str());
		return E_FAIL;
	}
	if (doc->Parse(FS->getDataFromFile(FileName, false).c_str()) > 0)
	{
		throw exception(string(string("Dialogs->LoadFile()::doc->Parse:\n") + string(doc->ErrorStr())).c_str());
		return E_FAIL;
	}

	return S_OK;
}

void Engine::Dialogs::getMAReplices()
{
	auto Main_Element = doc->RootElement();
	if (!Main_Element)
	{
		DebugTrace("Dialogs->getMAReplices()::doc->RootElement() == nullptr!!!");
		throw exception("Dialogs->getMAReplices()::doc->RootElement() == nullptr!!!");
		return;
	}
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (Replices.empty())
		{
			Nodes.push_back(Main_Element->FirstChild());
			Replices.push_back(Nodes.back()->FirstChild()->Value());
			continue;
		}

		Nodes.push_back(Nodes.back()->NextSibling());
		Replices.push_back(Nodes.back()->FirstChild()->Value());

		if (Main_Element->LastChild()->FirstChild()->Value() == Nodes.back()->FirstChild()->Value())
			break;
	}
}
