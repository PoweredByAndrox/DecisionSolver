#include "pch.h"
#include "Dialogs.h"

HRESULT Engine::Dialogs::Init(File_system *FS)
{
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
	doc->LoadFile(FileName->c_str());
	if (doc->ErrorID() > 0)
	{
		throw exception(string(string("Dialogs->LoadFile()::doc->LoadFile: \n") + string(doc->ErrorStr())).c_str());
		return E_FAIL;
	}

	return S_OK;
}

void Engine::Dialogs::getAllReplices()
{
	//doc->Parse()
	//Replices
}
