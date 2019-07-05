#include "pch.h"

#include "Engine.h"
/*
#include "MainMenu.h"
#include "Picking.h"
#include "GameObjects.h"
#include "Levels.h"
*/

#include "DebugDraw.h"

#include "File_system.h"
#include "Render_Buffer.h"
#include "Models.h"
#include "Camera.h"
#include "Actor.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"
//#include "Picking.h"

#include <iostream>

shared_ptr<Engine> Application;
#include "UI.h"

shared_ptr<Actor> mActor;
#include "Shaders.h"

//#include "CLua.h"
	// winerror.h 
		//	e.g ERROR_FILE_NOT_FOUND

ToDo("Unnecessary: Correct My English, Please)")
ToDo("Use PxPreprocessor.h for Checking We Have x86 and etc")
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application = make_shared<Engine>();

		//	// Shader Class!!!
		Application->setShader(make_shared<Shaders>());

		//	// FS (File System)!!!
		Application->setFS(make_shared<File_system>());

		if (FAILED(Application->Init(wstring(L"DecisionEngine"), hInstance)))
		{
#if defined (_DEBUG)
			DebugTrace("wWinMain::Application->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("wWinMain::Application->Init() is failed!!!");
#endif
			Console::LogError("Application::Init() is Fail!");

			return 5;
		}

		if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
#if defined (_DEBUG)
			DebugTrace("wWinMain::CoInitializeEx() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("wWinMain::CoInitializeEx() is failed!!!");
#endif
			Console::LogError("CoInitializeEx() is Fail!");

			return 3;
		}

		// ***********
		// INITIALIZATION ALL THE CLASSES

		//Application->setCLua(make_shared<CLua>());

		//	// GUI!!!
		Application->setUI(make_shared<UI>());
		if (FAILED(Application->getUI()->Init()))
		{
#if defined (_DEBUG)
			DebugTrace("wWinMain::getUI()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getUI()->Init() is failed!!!");
#endif
			Console::LogError("UI: Init is Fail!");
			return 5;
		}

		//	// Models Class
		//auto Model = Application->getFS()->GetFile(string("Nanosuit.obj"));
		//if (Model.operator bool())
		//	Application->setModel(make_shared<Models>(Model->PathA));

		Application->getUI()->LoadXmlUI(Application->getFS()->GetFile(string("All.xml"))->PathA.c_str());
		Application->getUI()->getDialog("Main")->ChangePosition(10.f, Application->getWorkAreaSize(Application->GetHWND()).y -
			10.f, ImVec2(0.f, 1.f));

		//	// Console Class!!!
		Application->setConsole(make_shared<Console>());
		Application->getConsole()->Init();
		Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
		Application->getUI()->getDialog("Console")->ChangeSize(Application->getWorkAreaSize(Application->GetHWND()).x, 
			Application->getWorkAreaSize(Application->GetHWND()).y/3);
		
		Application->setPhysics(make_shared<Physics>());
		if (FAILED(Application->getPhysics()->Init()))
		{
#if defined (_DEBUG)
			DebugTrace("wWinMain::getPhysics()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getPhysics()->Init() is failed!!!");
#endif
			Console::LogError("PhysX: Init Failed!");
			return 5;
		}

		//	// Camera Class
		Application->setCamera(make_shared<Camera>());
		if (FAILED(Application->getCamera()->Init(Application->getWorkAreaSize(Application->GetHWND()).x,
			Application->getWorkAreaSize(Application->GetHWND()).y)))
		{
#if defined (_DEBUG)
			DebugTrace("wWinMain::getCamera()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getCamera()->Init() is failed!!!");
#endif
			Console::LogError("Camera: Init Failed!");
			return 5;
		}

		//	// Main Actor Class!!!
		Application->setActor(make_shared<Actor>());
		if (FAILED(Application->getActor()->Init()))
		{
#if defined (_DEBUG)
			DebugTrace("wWinMain::getActor()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getActor()->Init() is failed!!!");
#endif
			Console::LogError("Actor: Init Failed!");
			return 5;
		}

		//	// Audio (Sound) Class!!!
		//Application->setSound(make_shared<Audio>());
		//if (FAILED(Application->getSound()->Init()))
		//{
		//	DebugTrace("wWinMain::getSound()->Init() is failed.");
		//	throw exception("getSound()->Init() is failed!!!");
		//	return 5;
		//}
		//Application->getSound()->changeSoundPos(Vector3::One * 5);
		//Application->getSound()->changeSoundVol(0.03f); // This sound is too loud!!! BBBBEEEE CCCCAAARRREEEFFFUUULLL
		
		//Application->getCLua()->Init();

		//	// Debug Draw!!!
#if defined (_DEBUG)
		Application->setDebugDraw(make_shared<DebugDraw>());
		Application->getDebugDraw()->Init();
#endif

		//Application->setPick(make_shared<Picking>());
		// ***********

		MSG msg = {0};
		while (msg.message != WM_QUIT)
		{
			if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				continue;
			}
			else
				Application->Render();
		}

		Application->getPhysics()->Destroy();

		if (Application->getSound().operator bool())
			Application->getSound()->ReleaseAudio();

#if defined(_DEBUG)
		Application->getUI()->Destroy();
#endif
		Application->Destroy();
	}
	catch (const exception &Catch)
	{
#if defined (_DEBUG)
		DebugTrace(string(string("wWinMain::catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("wWinMain::catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
		Console::LogError(string(string(");\nThe engine was crashed!\nReturn Error Text:")
			+ Catch.what()).c_str());
		MessageBoxA(Application->GetHWND(), string(string(");\nThe engine was crashed with this error message:\n") + string(Catch.what()) + 
			string("\nAnd also error code: ") + to_string(GetLastError())).c_str(), Application->getNameWndA().c_str(), MB_OK | MB_ICONERROR);
	}

	return 0;
}
