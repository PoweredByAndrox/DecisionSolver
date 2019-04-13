#include "pch.h"
#define SOL_CHECK_ARGUMENTS 1

#include "CLua.h"

ToDo("It's Necessary To Add Catch Some Errors!");

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "Console.h"

void CLua::GetDate(string Text)
{
	MessageBoxA(Application->GetHWND(), Text.c_str(), "DecisionSolver", MB_OK);
}

void CLua::Init()
{
	LuaState.open_libraries();

	auto bark = LuaState["Engine"].get_or_create<table>();
	bark.new_usertype<Console>("Console", "AddCmd", &Console::AddCmd);
}

void CLua::Update()
{
	try
	{
		LuaState.script_file(Application->getFS()->GetFile("test.lua")->PathA);
	}
	catch (const exception &SomeError)
	{
		if (MessageBoxA(Application->GetHWND(), string(string("Some errors with LUA:\n") + SomeError.what() + 
			string("\nDo you want to continue this application?")).c_str(), "DecisionSolver", MB_YESNO) == IDYES)
		{
			Application->getUI()->getDialog("Console")->getChilds().back()->getUTexts().back()->AddCLText(
				UnformatedText::Type::Error, string("Some errors with LUA:\n") + SomeError.what());

			return;
		}
	}
}
