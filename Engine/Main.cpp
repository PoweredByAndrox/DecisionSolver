#include "pch.h"

#include "Engine.h"

#include "DebugDraw.h"

#include "File_system.h"
#include "Render_Buffer.h"
#include "Models.h"
#include "Camera.h"
#include "Actor.h"
#include "Audio.h"
#include "Console.h"
#include "Physics.h"
#include "Picking.h"
#include "Levels.h"

shared_ptr<Engine> Application;
#include "UI.h"

shared_ptr<Actor> mActor;
#include "Shaders.h"

#include "CLua.h"
	// winerror.h 
		//	e.g ERROR_FILE_NOT_FOUND

ToDo("Unnecessary: Correct My English, Please)")
ToDo("Use PxPreprocessor.h for Checking We Have x86 or x64")
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application = make_shared<Engine>();

		if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
#if defined (DEBUG)
			DebugTrace("wWinMain::CoInitializeEx() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("wWinMain::CoInitializeEx() is failed!!!");
#endif
			Console::LogError("CoInitializeEx() is Fail!");
			return 3;
		}

		//	// Shader Class!!!
		Application->setShader(make_shared<Shaders>());

		//	// FS (File System)!!!
		Application->setFS(make_shared<File_system>());

		if (FAILED(Application->Init(wstring(L"DecisionEngine"), hInstance)))
		{
#if defined (DEBUG)
			DebugTrace("wWinMain::Application->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("wWinMain::Application->Init() is failed!!!");
#endif
			Console::LogError("Application::Init() is Fail!");
			return 5;
		}

		// ***********
		// INITIALIZATION ALL THE CLASSES

		Application->setCLua(make_shared<CLua>());

#if defined (DEBUG)
		//	// GUI!!!
		Application->setUI(make_shared<UI>());
		if (FAILED(Application->getUI()->Init()))
		{
#if defined (DEBUG)
			DebugTrace("wWinMain::getUI()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getUI()->Init() is failed!!!");
#endif
			Console::LogError("UI: Init is Fail!");
			return 5;
		}

		Application->getUI()->LoadXmlUI(Application->getFS()->GetFile(string("All.xml"))->PathA.c_str());
		Application->getUI()->getDialog("Main")->ChangePosition(10.f,
			Application->getWorkAreaSize(Application->GetHWND()).y - 10.f, ImVec2(0.f, 1.f));

		//	// Console Class!!!
		Application->setConsole(make_shared<Console>());
		Application->getConsole()->Init();
		Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
		Application->getUI()->getDialog("Console")->ChangeSize(
			Application->getWorkAreaSize(Application->GetHWND()).x,
			Application->getWorkAreaSize(Application->GetHWND()).y / 3);
#endif

		Application->setPhysics(make_shared<Physics>());
		if (FAILED(Application->getPhysics()->Init()))
		{
#if defined (DEBUG)
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
#if defined (DEBUG)
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
#if defined (DEBUG)
			DebugTrace("wWinMain::getActor()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getActor()->Init() is failed!!!");
#endif
			Console::LogError("Actor: Init Failed!");
			return 5;
		}

		//	// Audio (Sound) Class!!!
		Application->setSound(make_shared<Audio>());
		if (FAILED(Application->getSound()->Init()))
		{
#if defined (DEBUG)
			DebugTrace("wWinMain::getSound()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getSound()->Init() is failed!!!");
#endif

			Console::LogError("Sound: Something is wrong with Init Sound!");
			return 5;
		}
		//Application->getSound()->changeSoundPos(Vector3::One * 5);

		Application->getCLua()->Init();

		//	// Debug Draw!!!

		Application->setDebugDraw(make_shared<DebugDraw>());
		Application->getDebugDraw()->Init();

		//Application->setPick(make_shared<Picking>());
		// ***********

		//	// Level Class
		Application->setLevel(make_shared<Levels>());
		if (FAILED(Application->getLevel()->Init()))
		{
#if defined (DEBUG)
			DebugTrace("wWinMain::getLevel()->Init() is failed.");
#endif
#if defined (ExceptionWhenEachError)
			throw exception("getLevel()->Init() is failed!!!");
#endif
			Console::LogError("Levels: Init Failed!");
			return 5;
		}

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
		Application->getUI()->Destroy();
#endif
		Application->getSound()->ReleaseAudio();
		Application->Destroy();
	}
	catch (const exception &Catch)
	{
#if defined (DEBUG)
		DebugTrace(string(string("wWinMain::catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("wWinMain::catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
		Console::LogError(string(string(");\nThe engine was crashed!\nReturn Error Text:")
			+ Catch.what()).c_str());
		MessageBoxA(Application->GetHWND(), string(string(");\nThe engine was crashed with this error message:\n")
			+ string(Catch.what()) + string("\nAnd also error code: ") + to_string(GetLastError())).c_str(),
			Application->getNameWndA().c_str(), MB_OK | MB_ICONERROR);
	}

	return 0;
}
