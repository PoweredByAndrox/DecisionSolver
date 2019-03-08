#include "pch.h"

#include "Engine.h"
/*
#include "Physics.h"
#include "Audio.h"
#include "MainMenu.h"
#include "Picking.h"
#include "Terrain.h"
#include "GameObjects.h"
#include "Console.h"
#include "Levels.h"
*/

#include "File_system.h"
#include "Render_Buffer.h"
#include "Models.h"
#include "Camera.h"
#include "Actor.h"

shared_ptr<Engine> Application;
#include "UI.h"

bool Resize = false;
shared_ptr<Actor> mActor;

#include "Shaders.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Application = make_unique<Engine>();
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
			// GUI!!!
		Application->setUI(make_unique<UI>());
		Application->getUI()->Init();

			// Render Buffers!!!
		Application->setRender_Buffer(make_unique<Render_Buffer>());

			// Models Class
		Application->setModel(make_unique<Models>());
		Application->getModel()->LoadFromFile(&string("test.obj"));

			// Camera Class
		Application->setCamera(make_unique<Camera>());
		Application->getCamera()->Init(Application->getWorkAreaSize().x, Application->getWorkAreaSize().y);

			// Main Actor Class!!!
		Application->setActor(mActor = make_unique<Actor>());
		Application->getActor()->Init();
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

			if (Resize)
			{
//				Application->ResizibleWnd();
				Resize = false;
			}
		}

		Application->getUI()->Destroy();
		Application->getRender_Buffer()->Release();
		Application->Destroy();
	}
	catch (const exception &Catch)
	{
		MessageBoxA(Application->GetHWND(), string(string("The engine was crashed with this error message: ") + string(Catch.what())).c_str(), "Error!!!", MB_OK);
		MessageBoxA(Application->GetHWND(), string(string("You can find a solution using this error code: ") + to_string(0)).c_str(), "Error!!!", MB_OK);
	}

	return 0;
}
