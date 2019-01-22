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
		HRESULT Init(Audio *sound);

		MainMenu() {}
		~MainMenu() {}

		auto *getDlgMM() { return ui->getDialog()->at(0); }   // For Render
		auto *getDlgVID() { return ui->getDialog()->at(1); } // For Render
		auto *getDlgAUD() { return ui->getDialog()->at(2); } // For Render
		
		void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, Control* pControl, void* pUserContext);

		auto const *getGameMode() { return &gameMode; }
		void setGameMode(GAME_MODE _enum) { gameMode = _enum; }

		bool IsInitMainMenu() { return InitMainMenu; }
		UI *getUI() { if (ui.operator bool()) return ui.get(); return nullptr; }
	protected:
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
		DXUTDeviceSettings g_DeviceSettings;

		void AddD3D11Resolution(DWORD dwWidth, DWORD dwHeight);

		GAME_MODE gameMode = GAME_MODE::GAME_RUNNING;

		unique_ptr<UI> ui = make_unique<UI>();
		unique_ptr<Audio> Sound;

	private:
		Dialog MainMenuDlg, VideoMenuDlg, AudioMenuDlg;
	};
};
#endif // !__MAIN_MENU_H__
