#include "pch.h"

#include "Engine.h"
/*
#include "Physics.h"
#include "MainMenu.h"
#include "Picking.h"
#include "Terrain.h"
#include "GameObjects.h"
#include "Levels.h"
*/

#include "File_system.h"
#include "Render_Buffer.h"
#include "Models.h"
#include "Camera.h"
#include "Actor.h"
#include "Audio.h"
#include "Console.h"

shared_ptr<Engine> Application;
#include "UI.h"

shared_ptr<Actor> mActor;
#include "Shaders.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application = make_unique<Engine>();

		// Shader Class!!!
		Application->setShader(make_unique<Shaders>());

		// FS (File System)!!!
		Application->setFS(make_unique<File_system>());

		if (FAILED(Application->Init(L"Engine Programm", hInstance)))
		{
			DebugTrace("wWinMain::engine->Init() is failed.");
			throw exception("wWinMain is failed!!!");
			return E_FAIL;
		}

		if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		{
			DebugTrace("wWinMain::CoInitializeEx() is failed.");
			throw exception("wWinMain is failed!!!");
			return E_FAIL;
		}

		// ***********
		// INITIALIZATION ALL THE CLASSES

		//	// GUI!!!
		Application->setUI(make_unique<UI>());
		Application->getUI()->Init();
		Application->getUI()->addDialog("Main");
		Application->getUI()->addCollapseHead("Sounds", "Main", true);
		Application->getUI()->addComponentToCollapseHead("Sounds", "Main", make_unique<Buttons>("Start!", true));
		Application->getUI()->addComponentToCollapseHead("Sounds", "Main", make_unique<Buttons>("Stop!", true));
		Application->getUI()->addComponentToCollapseHead("Sounds", "Main", make_unique<Buttons>("Pause!", true));
		Application->getUI()->addLabel("", "Main");

		//	// Console Class!!!
		Application->setConsole(make_unique<Console>());
		Application->getConsole()->Init();

		//	// Models Class
		//Application->setModel(make_unique<Models>());

		//	// Camera Class
		Application->setCamera(make_unique<Camera>());
		Application->getCamera()->Init(Application->getWorkAreaSize(Application->GetHWND()).x, Application->getWorkAreaSize(Application->GetHWND()).y);

		//	// Main Actor Class!!!
		Application->setActor(make_unique<Actor>());
		Application->getActor()->Init();

		//	// Audio (Sound) Class!!!
		Application->setSound(make_unique<Audio>());
		Application->getSound()->Init();
		Application->getSound()->AddNewSound();
		Application->getSound()->changeSoundVol(0.03f); // This sound is too loud!!! BBBBEEEE CCCCAAARRREEEFFFUUULLL
		// ***********

		MSG msg = {0};
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				Application->Run();
		}

		Application->getUI()->Destroy();
		//Application->getRender_Buffer()->Release();
		Application->Destroy(hInstance);
	}
	catch (const exception &Catch)
	{
		MessageBoxA(Application->GetHWND(), string(string("The engine was crashed with this error message: ") + string(Catch.what())).c_str(), "Error!!!", MB_OK);
		MessageBoxA(Application->GetHWND(), string(string("You can find a solution using this error code: ") + to_string(0)).c_str(), "Error!!!", MB_OK);
	}

	return 0;
}
