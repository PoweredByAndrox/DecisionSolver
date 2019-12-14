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
#include "CutScene.h"

state CLua::LuaState;

void CLua::Init()
{
	LuaState.open_libraries(lib::base, lib::package);

	// Log Funtions (Console) !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Console>("Console", "PushCMD",
		&Console::PushCMD, "LogError", &Console::LogError, "LogInfo", &Console::LogInfo,
		"LogNormal", &Console::LogNormal);

	// Audio System !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Audio>("Sound", "PlayFile", &Audio::PlayFile,
		"Stop", &Audio::doStop, "Pause", &Audio::doPause, "ChangeVolume", &Audio::changeSoundVol,
		"Play", &Audio::doPlay);

	// Camera !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Camera>("Camera", "ChangePos", &Camera::Teleport,
		"GetPos", &Camera::GetEyePt, "GetLook", &Camera::GetLookAtPt);

	// File System !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<File_system>("FS", "GetCurPath", &File_system::GetCurPath);

	// CutScene !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<CutScene>("CutScene", constructors<CutScene()>(),
		//CutScene(Vector3, Vector3, float)>(),
		"AddPoint", &CutScene::AddNewPoint, "Start", &CutScene::Start, "Update", &CutScene::Update,
		"DeletePoints", &CutScene::Reset);

	LuaState.new_usertype<Vector3>("vec3", constructors<Vector3(), Vector3(float, float, float),
		void(float, float, float)>());

	// Keyboard !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Keyboard>("Keyboard", "IsKeyDown", &Engine::IsKeyboardDown,
		"IsKeyUp", &Engine::IsKeyboardUp);

	// Mouse !!!
	LuaState["Engine"].get_or_create<table>().new_usertype<Mouse>("Mouse", "IsLeft", &Engine::IsMouseLeft,
		"IsRight", &Engine::IsMouseRight, "SetVisible", &ShowCursor);

	const string package_path = LuaState["package"]["path"];
	LuaState["package"]["path"] = package_path + (!package_path.empty() ? ";" : "")
		+ Application->getFS()->getPathFromType(_TypeOfFile::SCRIPTS) + "?.lua";

	callFunction("main.lua", "initEverything", "");
}

void CLua::Reinit()
{
	callFunction("main.lua", "initEverything", "");
	//Application->setCScene(make_shared<CutScene>(LuaState["CutScene_instance"]));
}

void CLua::Update()
{
	try
	{
		callFunction("main.lua", "main", "");
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
		LuaState.safe_script_file(File);
		LuaState.get<sol::function>(Function.c_str()).template call<void>(params);
	}
	catch (error e)
	{
		Console::LogError(e.what());
	}
}
