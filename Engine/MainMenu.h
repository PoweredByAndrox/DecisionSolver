#pragma once
#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__
#include "pch.h"

#include "File_system.h"
#include "UI.h"
#include "Audio.h"

namespace Engine
{
	enum GAME_MODE
	{
		GAME_RUNNING = 1,
		GAME_MAIN_MENU,
		GAME_AUDIO_MENU,
		GAME_VIDEO_MENU
	};

	class MainMenu: public UI, public Audio
	{
	public:
		HRESULT Init(UI *ui, Audio *sound);

		void setGUIEvent(UINT nEvent, int nControlID, Control* pControl, void* pUserContext);

		MainMenu() {}
		~MainMenu() {}

		auto *getDlgMM()  { return &MainMenuDlg; }   // For Render
		auto *getDlgVID() { return &VideoMenuDlg; } // For Render
		auto *getDlgAUD() { return &AudioMenuDlg; } // For Render

		auto const *getGameMode() { return &gameMode; }
		void setGameMode(GAME_MODE _enum) { gameMode = _enum; }

		bool IsInitMainMenu() { return InitMainMenu; }

	private:
		bool InitMainMenu = false;

#define IDC_STATIC              11
#define IDC_AUDIO               12
#define IDC_VIDEO               13
#define IDC_RESUME              14
#define IDC_QUIT                15
#define IDC_BACK                16
#define IDC_SOUNDFX_SCALE       17
#define IDC_MUSIC_SCALE         18
#define IDC_RESOLUTION          19
		//#define IDC_ANTI_ALIASING       10
#define IDC_APPLY               20
#define IDC_FULLSCREEN          22
#define IDC_ASPECT              23


		HRESULT UpdateD3D11RefreshRates();
		HRESULT UpdateD3D11Resolutions();
		HRESULT OnD3D11ResolutionChanged();
		void AddD3D11RefreshRate(_In_ DXGI_RATIONAL RefreshRate);
		DXUTDeviceSettings  g_DeviceSettings;

		void AddD3D11Resolution(DWORD dwWidth, DWORD dwHeight);

		GAME_MODE gameMode = GAME_MODE::GAME_RUNNING;

		unique_ptr<UI> ui;
		unique_ptr<Audio> Sound;

		Dialog MainMenuDlg;   // dialog for main menu
		Dialog VideoMenuDlg;  // dialog for video menu
		Dialog AudioMenuDlg;  // dialog for audio menu
	};
};
#endif // !__MAIN_MENU_H__
