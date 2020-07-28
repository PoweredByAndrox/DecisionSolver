/**
 * \file	main.cpp.
 *
 * \brief	Implements Init All The Classes And Have "Application" Variable For Engine
 */

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
#include "UI.h"
#include "CLua.h"
#include "SDKInterface.h"

#include "Timer.h"

/** \brief	The application */
shared_ptr<Engine> Application;

	// winerror.h 
		//	e.g ERROR_FILE_NOT_FOUND

///< .
ToDo("Unnecessary: Correct My English, Please)")
ToDo("Use PxPreprocessor.h for Check If We Have x86 or x64")

/**
 * \fn	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
 *
 * \brief \mainpage Everything Has Started Here. The Following Function Inits Each Class.
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	hInstance	 	The instance.
 * \param 	hPrevInstance	The previous instance.
 * \param 	lpCmdLine	 	The command line.
 * \param 	nCmdShow	 	The command show.
 *
 * \returns	A WINAPI.
 * \see Engine
 */
shared_ptr<SDKInterface> SDK = make_shared<SDKInterface>();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	using namespace std::chrono_literals;
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Application = make_shared<Engine>();

	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		Engine::LogError("wWinMain::CoInitializeEx() Failed.",
			string(__FILE__) + ": " + to_string(__LINE__),
			"CoInitializeEx() Failed!");
		return 3;
	}

	//	// FS (File System)!!!
	Application->setFS(make_shared<File_system>());

	if (FAILED(Application->Init("DecisionEngine", hInstance)))
	{
		Engine::LogError("wWinMain::Application->Init() is failed.",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Application::Init() Failed!");
		return 5;
	}

	// ***********
	// INITIALIZATION ALL THE CLASSES

	Application->setCLua(make_shared<CLua>());
	if (Application->getCLua().operator bool())
		Application->getCLua()->Init();

	Application->setPhysics(make_shared<Physics>());
	if (Application->getPhysics().operator bool())
		if (FAILED(Application->getPhysics()->Init()))
		{
			Engine::LogError("wWinMain::getPhysics()->Init() is failed.",
				string(__FILE__) + ": " + to_string(__LINE__),
				"PhysX: Init Failed!");
			return 5;
		}

	//	// Audio (Sound) Class!!!
	Application->setSound(make_shared<Audio>());
	if (Application->getSound().operator bool())
		if (FAILED(Application->getSound()->Init()))
		{
			Engine::LogError("wWinMain::getSound()->Init() Failed.",
				string(__FILE__) + ": " + to_string(__LINE__),
				"Sound: Something is wrong with Init Sound!");
			return 5;
		}

	//	// Debug Draw!!!
	//Application->setDebugDraw(make_shared<DebugDraw>());

	//	// Main Actor Class!!!
	Application->setActor(make_shared<Actor>());
	if (Application->getActor().operator bool())
		if (FAILED(Application->getActor()->Init()))
		{
			Engine::LogError("wWinMain::getActor()->Init() Failed.",
				string(__FILE__) + ": " + to_string(__LINE__),
				"Actor: Init Failed!");
			return 5;
		}

	//	// GUI!!!
	if (Application->getDevice() && Application->getDeviceContext())
		Application->setUI(make_shared<UI>());
	if (Application->getUI())
	{
		if (FAILED(Application->getUI()->Init()))
		{
			Engine::LogError("wWinMain::getUI()->Init() Failed.",
				string(__FILE__) + ": " + to_string(__LINE__),
				"UI: Init Failed!");
			return 5;
		}
		Application->getUI()->LoadFileUI(Application->getFS()->GetFile("All.xml")->PathA.c_str());

		//	// Console Class!!!
		Application->setConsole(make_shared<Console>());
		if (Application->getConsole().operator bool())
		{
			Application->getConsole()->Init();
			Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
			Application->getUI()->getDialog("Console")->ChangeSize(
				(float)Application->getWorkAreaSize(Application->GetHWND()).x,
				(float)Application->getWorkAreaSize(Application->GetHWND()).y / 3.0f);
		}
	}

	if (SDK && Application->getFS())
		SDK->LoadSettings(Application->getFS()->LoadSettingsFile());

	//Application->setPick(make_shared<Picking>());
	// ***********

	//	// Level Class
	if (Application->getDevice() && Application->getDeviceContext())
		Application->setLevel(make_shared<Levels>());
	if (Application->getLevel().operator bool())
	{
		if (FAILED(Application->getLevel()->Init()))
		{
			Engine::LogError("wWinMain::getLevel()->Init() Failed.",
				string(__FILE__) + ": " + to_string(__LINE__),
				"Levels: Init Failed!");
			return 5;
		}
		//auto Obj = Application->getFS()->GetFile(string("New File"));
		//if (Obj)
		//{
		//	Application->getFS()->GetProject()->SetCurProject(path(Obj->PathA));
		//	Application->getFS()->GetProject()->OpenFile(Obj->PathA);
		//}
	}
	//Application->setMultiplayer(make_shared<Multiplayer>());
	//EngineTrace(Application->getMPL()->Init());
	
	// Start The Main Thread And Wait For While Message From System (WM_QUIT) Will Be
	Application->Render();
	MSG msg = { 0 };
	while (!Application->IsQuit())
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if (Application->getThreadState() == Engine::ThreadStatus::_Quit)
			Application->Destroy();
		else
			this_thread::sleep_for(10ms);
	}

	return 0;
}

/**
// End of main.cpp
 */