#include "pch.h"
#define SOL_CHECK_ARGUMENTS 1

#include "CLua.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "Console.h"
#include "Audio.h"
#include "Camera.h"

void CLua::Init()
{
	LuaState.open_libraries();

	// Log Funtions (Console) !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Console>("Console", "AddCmd",
		&Console::AddCmd, "LogError", &Console::LogError, "LogInfo", &Console::LogInfo,
		"LogNormal", &Console::LogNormal);

	// Audio System !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Audio>("Sound", "PlayFile", &Audio::PlayFile,
		"Stop", &Audio::doStop, "Pause", &Audio::doPause, "ChangeVolume", &Audio::changeSoundVol,
		"Play", &Audio::doPlay);

	// Camera !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Camera>("Camera", "ChangePos", &Camera::Teleport,
		"GetPos", &Camera::GetEyePt, "GetDir", &Camera::GetLookAtPt);

	// File System !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<File_system>("FS", "GetCurPath", &File_system::GetCurPath);

	//// Keyboard !!!
	//LuaState["Engine"].get_or_create<table>().new_usertype<Keyboard>("Keyboard", "IsKeyDown", &Engine::IsKeyboardDown,
	//	"IsKeyUp", &Engine::IsKeyboardUp);

	//// Mouse !!!
	//Table_Engine.new_usertype<Mouse::>("Mouse", "IsKeyDown", &Engine::IsKeyDown,
	//	"IsKeyUp", &Engine::IsKeyUp, "SetVisible", &Engine::SetVisibleMouse);
}

void CLua::Update()
{
	try
	{
		if (Application->getTrackerKeyboard().IsKeyPressed(Keyboard::Keys::D5))
			callFunction("test.lua", "main", "");
	}
	catch (const exception &SomeError)
	{
		if (MessageBoxA(Application->GetHWND(), string(string("Some errors with LUA:\n") + SomeError.what() +
			string("\nDo you want to continue?")).c_str(), Application->getNameWndA().c_str(), MB_YESNO) == IDYES)
		{
			Console::LogError(string("Some errors with LUA:\n") + SomeError.what());
			return;
		}
		else
			Engine::Quit();
	}
}

void CLua::callFunction(string FileName, string Function, string params)
{
	try
	{
		auto File = Application->getFS()->GetFile(FileName)->PathA;
		if (File.empty())
		{
			Console::LogError(string("Lua error: File: \"") + FileName + string("\" Doesn't Exist!"));
			return;
		}
		LuaState.script_file(File);
		LuaState.get<sol::function>(Function.c_str()).template call<void>(params);
	}
	catch (error e)
	{
		Console::LogError(string("Lua error:\n") + string(e.what()));
	}
}
