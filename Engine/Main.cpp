#include "pch.h"

#include "Engine.h"

#include "DebugDraw.h"

#include "File_system.h"
#include "Render_Buffer.h"
#include "Camera.h"
#include "Actor.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"
#include "Picking.h"
#include "Levels.h"

#include "CutScene.h"

#include "Multiplayer.h"

shared_ptr<Engine> Application;
#include "UI.h"

shared_ptr<Actor> mActor;

#include "CLua.h"
	// winerror.h 
		//	e.g ERROR_FILE_NOT_FOUND

ToDo("Unnecessary: Correct My English, Please)")
ToDo("Use PxPreprocessor.h for Checking We Have x86 or x64")
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Application = make_shared<Engine>();

	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		Engine::LogError("wWinMain::CoInitializeEx() Failed.",
			"wWinMain::CoInitializeEx() Failed!!!",
			"CoInitializeEx() Failed!");
		return 3;
	}

	//	// FS (File System)!!!
	Application->setFS(make_shared<File_system>());

	if (FAILED(Application->Init(wstring(L"DecisionEngine"), hInstance)))
	{
		Engine::LogError("wWinMain::Application->Init() is failed.",
			"wWinMain::Application->Init() is failed!!!",
			"Application::Init() Failed!");
		return 5;
	}

	// ***********
	// INITIALIZATION ALL THE CLASSES

	Application->setCLua(make_shared<CLua>());

	//	// GUI!!!
	Application->setUI(make_shared<UI>());
	if (FAILED(Application->getUI()->Init()))
	{
		Engine::LogError("wWinMain::getUI()->Init() Failed.",
			"getUI()->Init() is failed!!!", "UI: Init Failed!");
		return 5;
	}

	Application->getUI()->LoadXmlUI(Application->getFS()->GetFile(string("All.xml"))->PathA.c_str());

	//	// Console Class!!!
	Application->setConsole(make_shared<Console>());
	Application->getConsole()->Init();
	Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
	Application->getUI()->getDialog("Console")->ChangeSize(
		Application->getWorkAreaSize(Application->GetHWND()).x,
		Application->getWorkAreaSize(Application->GetHWND()).y / 3);

	Application->setPhysics(make_shared<Physics>());
	if (FAILED(Application->getPhysics()->Init()))
	{
		Engine::LogError("wWinMain::getPhysics()->Init() is failed.",
			"getPhysics()->Init() Failed!!!", "PhysX: Init Failed!");
		return 5;
	}

	//	// Camera Class
	Application->setCamera(make_shared<Camera>());
	if (FAILED(Application->getCamera()->Init(Application->getWorkAreaSize(Application->GetHWND()).x,
		Application->getWorkAreaSize(Application->GetHWND()).y)))
	{
		Engine::LogError("wWinMain::getCamera()->Init() Failed.",
			"getCamera()->Init() Failed!!!", "Camera: Init Failed!");
		return 5;
	}

	//	// Main Actor Class!!!
	Application->setActor(make_shared<Actor>());
	if (FAILED(Application->getActor()->Init()))
	{
		Engine::LogError("wWinMain::getActor()->Init() Failed.", "getActor()->Init() Failed!!!",
			"Actor: Init Failed!");
		return 5;
	}

	//	// Cut Scene!!!
	Application->setCScene(make_shared<CutScene>());

	//	// Audio (Sound) Class!!!
	Application->setSound(make_shared<Audio>());
	if (FAILED(Application->getSound()->Init()))
	{
		Engine::LogError("wWinMain::getSound()->Init() Failed.", "getSound()->Init() Failed!!!",
			"Sound: Something is wrong with Init Sound!");
		return 5;
	}

	Application->getCLua()->Init();

	//	// Debug Draw!!!
	//Application->setDebugDraw(make_shared<DebugDraw>());
	//Application->getDebugDraw()->Init();

	//Application->setPick(make_shared<Picking>());
	// ***********

	//	// Level Class
	Application->setLevel(make_shared<Levels>());
	if (FAILED(Application->getLevel()->Init()))
	{
		Engine::LogError("wWinMain::getLevel()->Init() Failed.", "getLevel()->Init() Failed!!!",
			"Levels: Init Failed!");
		return 5;
	}

	//Application->setMultiplayer(make_shared<Multiplayer>());
	//EngineTrace(Application->getMPL()->Init());

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		else
			Application->getTimer()->Tick([]()
		{
			Application->Render();
		});
	}

	Application->getPhysics()->Destroy();

#if defined (DEBUG)
	if (Application->getUI().operator bool())
		Application->getUI()->Destroy();
#endif
	if (Application->getSound().operator bool())
		Application->getSound()->ReleaseAudio();
	Application->Destroy();

	return 0;
}
