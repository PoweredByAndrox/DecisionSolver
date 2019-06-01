#include "pch.h"

#include "Engine.h"
/*
#include "MainMenu.h"
#include "Picking.h"
#include "Terrain.h"
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

shared_ptr<Engine> Application;
#include "UI.h"

shared_ptr<Actor> mActor;
#include "Shaders.h"

//#include "CLua.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application = make_shared<Engine>();

		// Shader Class!!!
		Application->setShader(make_shared<Shaders>());

		// FS (File System)!!!
		Application->setFS(make_shared<File_system>());

		if (FAILED(Application->Init(wstring(L"DecisionEngine"), hInstance)))
		{
			DebugTrace("wWinMain::engine->Init() is failed.");
			throw exception("wWinMain is failed!!!");
			return 5;
		}

		if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		{
			DebugTrace("wWinMain::CoInitializeEx() is failed.");
			throw exception("wWinMain is failed!!!");
			return 3;
		}

		// ***********
		// INITIALIZATION ALL THE CLASSES

		//Application->setCLua(make_shared<CLua>());

		//	// GUI!!!
		Application->setUI(make_shared<UI>());
		if (FAILED(Application->getUI()->Init()))
		{
			DebugTrace("wWinMain::getUI()->Init() is failed.");
			throw exception("getUI()->Init() is failed!!!");
			return 5;
		}
		Application->getUI()->LoadXmlUI(Application->getFS()->GetFile(string("All.xml"))->PathA.c_str());
		Application->getUI()->getDialog("Main")->ChangePosition(10.f, Application->getWorkAreaSize(Application->GetHWND()).y - 10.f,
			ImVec2(0.f, 1.f));

		//	// Console Class!!!
		Application->setConsole(make_shared<Console>());
		Application->getConsole()->Init();
		Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
		Application->getUI()->getDialog("Console")->ChangeSize(Application->getWorkAreaSize(Application->GetHWND()).x, 
			Application->getWorkAreaSize(Application->GetHWND()).y/3);

		//	// Models Class
		//Application->setModel(make_shared<Models>(Application->getFS()->GetFile(string("cargo transport 3.obj"))->PathA));

		//	// Camera Class
		Application->setCamera(make_shared<Camera>());
		if (FAILED(Application->getCamera()->Init(Application->getWorkAreaSize(Application->GetHWND()).x,
			Application->getWorkAreaSize(Application->GetHWND()).y)))
		{
			DebugTrace("wWinMain::getCamera()->Init() is failed.");
			throw exception("getCamera()->Init() is failed!!!");
			return 5;
		}

		//	// Main Actor Class!!!
		Application->setActor(make_shared<Actor>());
		if (FAILED(Application->getActor()->Init()))
		{
			DebugTrace("wWinMain::getActor()->Init() is failed.");
			throw exception("getActor()->Init() is failed!!!");
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
		//Application->getSound()->AddNewSound();
		//Application->getSound()->changeSoundVol(0.03f); // This sound is too loud!!! BBBBEEEE CCCCAAARRREEEFFFUUULLL
		
		Application->setPhysics(make_shared<Physics>());
		if (FAILED(Application->getPhysics()->Init()))
		{
			DebugTrace("wWinMain::getPhysics()->Init() is failed.");
			throw exception("getPhysics()->Init() is failed!!!");
			return 5;
		}
		Application->getPhysics()->AddNewActor(Vector3::One, Vector3(2.5f, 2.5f, 2.5f), 100.0f);

		//Application->getCLua()->Init();

		//	// Debug Draw!!!
		Application->setDebugDraw(make_shared<DebugDraw>());
		Application->getDebugDraw()->Init();

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

#if defined(DEBUG)
		Application->getUI()->Destroy();
#endif
		Application->Destroy(hInstance);
	}
	catch (const exception &Catch)
	{
		MessageBoxA(Application->GetHWND(), string(string("The engine was crashed with this error message:\n") + string(Catch.what()) + 
			string("\nAnd also error code: ") + to_string(GetLastError())).c_str(), Application->getNameWndA().c_str(), MB_OK | MB_ICONERROR);
	}

	return 0;
}
