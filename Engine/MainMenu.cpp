#include "pch.h"
#include "MainMenu.h"

using namespace Engine;

HRESULT MainMenu::Init(Audio *sound)
{
	/*
	if (!sound->IsInitSounSystem())
	{
		DebugTrace("MainMenu: Init failed.\n");
		throw exception("Audio hasn't been initialized!!!");
		InitMainMenu = false;
		return E_FAIL;
	}
	Sound = unique_ptr<Audio>(sound);
	*/
	
	if (!ui->IsInitUI())
		ui->Init(3, ui->GetFile(string("Main_texures_UI.dds"))->PathW.c_str());

	MainMenuDlg = *ui->getDialog()->at(0);
	AudioMenuDlg = *ui->getDialog()->at(1);
	VideoMenuDlg = *ui->getDialog()->at(2);

	int iY = ((300 - 30 * 6) / 2), i = ui->getAllComponentsCount();
	vector<int> ID =
	{
		i += 1,
		i += 1,
		i += 1,
		i += 1
	};
	vector<wstring> Text =
	{
		L"Audio",
		L"Video",
		L"Resume",
		L"Quit"
	};
	vector<int> PosButtonX =
	{
		(250 - 125) / 2,
		(250 - 125) / 2,
		(250 - 125) / 2,
		(250 - 125) / 2
	};
	vector<int> PosButtonY =
	{
		iY += 30,
		iY += 30,
		iY += 30,
		iY += 60
	};
	ui->AddButton_Mass(&MainMenuDlg, &ID, &Text, &PosButtonX, &PosButtonY);
	MainMenuDlg.SetBackgroundColors(
		D3DCOLOR_ARGB(rand() % 100, rand() % 100, rand() % 200, rand() % 210),
		D3DCOLOR_ARGB(rand() % 200, rand() % 56, 105, rand() % 200),
		D3DCOLOR_ARGB(rand() % 200, rand() % 57, 105, rand() % 200),
		D3DCOLOR_ARGB(rand() % 200, rand() % 11, rand() % 88, 179));
	MainMenuDlg.SetLocation(1024 / 2, 768 / 2);
	MainMenuDlg.SetSize(250, 300);

	//***********
	Text.clear();

	i = ui->getAllComponentsCount();
	ID =
	{
		i += 1,
		i += 1,
		i += 1,
		i += 1
	};
	vector<int> IDStatic =
	{
		i += 1,
		i += 1
	};
	Text =
	{
		L"Audio",
		L"Video",
		L"Resume",
		L"Quit"
	};
	vector<wstring> TextStatic =
	{
		L"Music Volume",
		L"Sound Effects Volume"
	};
	iY = ((300 - 30 * 6) / 2);
	PosButtonX =
	{
		(250 - 125) / 2,
		(250 - 125) / 2,
	};
	PosButtonY =
	{
		iY += 24,
		iY += 35
	};
	vector<int> W = {125, 125}, H = {22, 22};
	ui->AddStatic_Mass(&AudioMenuDlg, &IDStatic, &TextStatic, &PosButtonX, &PosButtonY, &W, &H);
	
	TextStatic.clear();
	IDStatic.clear();

	iY = ((300 - 30 * 6) / 2);
	i = ui->getAllComponentsCount();
	vector<int> PosStatic = 
	{
		iY += 24,
		iY += 24
	};
	IDStatic =
	{
		i += 1,
		i += 1
	};
	vector<int> X = { (250 - 100) / 2, (250 - 100) / 2 }, Min = { 0, 0 }, Max = { 100, 100 },
		 DefValue = { 100, 100 }, Y;
	W = { 100, 100 }, H = { 22, 22 }; TextStatic.clear(); TextStatic = { L"For Music Volume", L"For Sound Effect" };
	ui->AddSlider_Mass(&AudioMenuDlg, &IDStatic, &TextStatic, &X, &PosStatic, &W, &H, &Min, &Max, &DefValue);
	
	ui->AddButton(&AudioMenuDlg, i += 1, wstring(L"Back"), (250 - 125) / 2, iY += 40, 125, 22);
	
	AudioMenuDlg.SetBackgroundColors(D3DCOLOR_ARGB(200, 98, 138, 206),
		D3DCOLOR_ARGB(200, 54, 105, 192),
		D3DCOLOR_ARGB(200, 54, 105, 192),
		D3DCOLOR_ARGB(200, 10, 73, 179));

	AudioMenuDlg.SetLocation(1024 / 2, 768 / 2);
	AudioMenuDlg.SetSize(250, 300);

	iY = ((300 - 30 * 6) / 2);
	i = ui->getAllComponentsCount();
	X.clear();	X = { 50, 30 };	Y.clear();	Y = { iY += 22, iY }; W.clear();
	W = { 50, 75 }; H.clear(); H = { 22, 22 }; IDStatic.clear(); IDStatic = { i += 1, i += 1 };
	TextStatic.clear(); TextStatic = { L"Aspect:", L"Resolution:" };

	ui->AddStatic_Mass(&VideoMenuDlg, &IDStatic, &TextStatic, &X, &Y, &W, &H);

	i = ui->getAllComponentsCount();
	X.clear();	X = { (250 - 125) /2, (250 - 125) /2 };	Y.clear();	
	Y = { 150, 175 }; IDStatic.clear(); IDStatic = { i += 1, i += 1 };
	TextStatic.clear(); TextStatic = { L"Apply", L"Back" };

	ui->AddButton_Mass(&VideoMenuDlg, &IDStatic, &TextStatic, &X, &Y);

	i = ui->getAllComponentsCount();
	X.clear(); X = { (250 - 200) /2, (250 - 200) /2 };
	Y.clear(); Y = { iY += 30, iY += 26 }; W.clear(); W = { 200, 200 }; 
	H.clear(); H = { 22, 22 }; IDStatic.clear(); IDStatic = { i += 1, i += 1 };
	TextStatic.clear(); TextStatic = { L"Full screen", L"Anti-Aliasing" };

	ui->AddCheckBox_Mass(&VideoMenuDlg, &IDStatic, &TextStatic, &X, &Y, &W, &H);

	i = ui->getAllComponentsCount();
	X.clear(); X = { 100, 100 };
	Y.clear(); Y = { iY += 30, iY += 26 }; W.clear(); W = { 100, 100 };
	H.clear(); H = { 22, 22 }; IDStatic.clear(); IDStatic = { i += 1, i += 1 };
	TextStatic.clear(); TextStatic = { L"For Aspect",L"For Resolution" };
	ui->AddComboBox_Mass(&VideoMenuDlg, &IDStatic, &TextStatic, &X, &Y, &W, &H);

	VideoMenuDlg.SetBackgroundColors(D3DCOLOR_ARGB(200, 98, 138, 206),
		D3DCOLOR_ARGB(rand() % 900, rand() % 400, rand() % 105, rand() % 463),
		D3DCOLOR_ARGB(rand() % 500, rand() % 298, rand() % 583, rand() % 319),
		D3DCOLOR_ARGB(rand() % 400, rand() % 10,  rand() % 764, rand() % 179));
	VideoMenuDlg.SetLocation(1024 /2, 768 /2);
	VideoMenuDlg.SetSize(250, 300);

	ui->getDialog()->clear();
	
	ui->getDialog()->push_back(&MainMenuDlg);
	ui->getDialog()->push_back(&AudioMenuDlg);
	ui->getDialog()->push_back(&VideoMenuDlg);

	InitMainMenu = true;
	return S_OK;
}

void CALLBACK MainMenu::OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, vector<void *> pUserContext)
{
	Button *Cache_Button = nullptr;
	Slider *Cache_Slider = nullptr;

	if (pControl->GetType() == CONTROL_TYPE::CONTROL_BUTTON)
	{
		Cache_Button = (Button *)pControl;
		if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Audio")))
		{
			gameMode = GAME_AUDIO_MENU;
			return;
		}
		else if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Video")))
		{
			gameMode = GAME_VIDEO_MENU;
			VideoMenuDlg.EnableNonUserEvents(true);
			//VideoMenuDlg.GetCheckBox(IDC_FULLSCREEN)->SetChecked(!DXUTIsWindowed());
			//VideoMenuDlg.EnableNonUserEvents(false);
			//VideoMenuDlg.GetCheckBox(IDC_ANTI_ALIASING)->SetChecked(DXUTGetDeviceSettings().d3d9.pp.MultiSampleType != D3DMULTISAMPLE_NONE);
			//VideoMenuDlg.GetComboBox(IDC_RESOLUTION)->SetSelectedByData(
			//	UintToPtr(MAKELONG(DXUTGetDXGIBackBufferSurfaceDesc()->Height,
			//		DXUTGetDXGIBackBufferSurfaceDesc()->Height)));
			return;
		}
		else if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Resume")))
		{
			gameMode = GAME_RUNNING;
			DXUTSetCursorSettings(false, true);
			return;
		}
		else if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Quit")))
		{
			DXUTShutdown(0);
			return;
		}
		else if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Apply")))
		{
			//bool bAA = VideoMenuDlg.GetCheckBox(IDC_ANTI_ALIASING)->GetChecked();
			UINT nRes;
			for (int i = 0; i < VideoMenuDlg.getAllControls().size(); i++)
				if (VideoMenuDlg.getAllControls().at(i)->GetType() == CONTROL_TYPE::CONTROL_COMBOBOX)
				{
					ComboBox *Stat = (ComboBox *)VideoMenuDlg.getAllControls().at(i);
					if (FindSubStr(wstring(Stat->GetText()), wstring(L"For Aspect")))
						nRes = (UINT)Stat->GetSelectedData();
				}

			int nWidth;
			nWidth = LOWORD(nRes);
			int nHeight;
			nHeight = HIWORD(nRes);
			bool bFullscreen;
			//	bFullscreen = VideoMenuDlg.GetCheckBox(IDC_FULLSCREEN)->GetChecked();

			DXUTDeviceSettings ds;
			ds = DXUTGetDeviceSettings();
			//ds.d3d11.MultiSampleType = (bAA) ? D3DMULTISAMPLE_4_SAMPLES : D3DMULTISAMPLE_NONE;
		//	ds.d3d11.sd.Windowed = (BOOL)!bFullscreen;
			ds.d3d11.sd.BufferDesc.Width = nWidth;
			ds.d3d11.sd.BufferDesc.Height = nHeight;

			// Change the device settings
			//g_Render.bDetectOptimalSettings = false;
			DXUTCreateDeviceFromSettings(&ds);
			//g_Render.bDetectOptimalSettings = true;
			return;
		}
		else if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Back")))
		{
			gameMode = GAME_MAIN_MENU;
			return;
		}
	}
	else if (pControl->GetType() == CONTROL_TYPE::CONTROL_SLIDER)
		Cache_Slider = (Slider *)pControl;
}
/*		
		//case IDC_SOUNDFX_SCALE:
			//g_audioState.fSoundFXVolume = (float)(AudioMenuDlg.GetSlider(IDC_SOUNDFX_SCALE)->GetValue() /
			//	100.0f);
			//SetSoundFxVolume(g_audioState.fSoundFXVolume);
		//break;
		else if (getComponentName_By_ID(ui.get(), getComponentBy_ID(ui.get(), nControlID)) == getComponentName_By_ID(ui.get(), i)) /// Audio Change Global Volume
		{
			Sound->changeSoundVol((float)(AudioMenuDlg.GetSlider(ui->getComponentID_By_Name(ui.get(),
				ui->getObjSlider(), &wstring(L"For Music Volume")))->GetValue() / 100.0f));
			break;
		}
		//case 19:
			// Update the list of aspect ratios when the user (un)checks the fullscreen box
			//DXUTDeviceSettings ds = DXUTGetDeviceSettings();
		//	UpdateAspectRatioList();
		//	break;
		else if (getComponentName_By_ID(ui.get(), getComponentBy_ID(ui.get(), nControlID)) == getComponentName_By_ID(ui.get(), i))
		{
			UpdateD3D11Resolutions();
			break;
		}
	}
}
/*
void MainMenu::UpdateAspectRatioList(DXUTDeviceSettings* pDS)
{
	bool bFullScreenChecked = VideoMenuDlg.GetCheckBox(IDC_FULLSCREEN)->GetChecked();

	// If windowed, then display a predefined list.  If fullscreen, then 
	// use the list of mode the HW device supports 
	CD3D9EnumDeviceSettingsCombo* pDeviceSettingsCombo = DXUTGetD3D9Enumeration()->GetDeviceSettingsCombo
	(&pDS->d3d11);
	if (pDeviceSettingsCombo == NULL)
		return;
	CGrowableArray <D3DDISPLAYMODE>* pDMList;
	if (!bFullScreenChecked)
		pDMList = &g_Render.aWindowedDMList;
	else
		pDMList = &pDeviceSettingsCombo->pAdapterInfo->displayModeList;

	UINT nAdapterMonitorWidth = g_Render.rcAdapterWork[pDS->d3d11.AdapterOrdinal].right -
		g_Render.rcAdapterWork[pDS->d3d11.AdapterOrdinal].left;
	UINT nAdapterMonitorHeight = g_Render.rcAdapterWork[pDS->d3d11.AdapterOrdinal].bottom -
		g_Render.rcAdapterWork[pDS->d3d11.AdapterOrdinal].top;

	CGrowableArray <float> aspectRatioList;
	for (int i = 0; i < pDMList->GetSize(); i++)
	{
		D3DDISPLAYMODE dm = pDMList->GetAt(i);

		// Use this display mode only if it meets certain min requirements
		if ((bFullScreenChecked && dm.Height >= 600) ||
			(!bFullScreenChecked && dm.Width <= nAdapterMonitorWidth && dm.Height <= nAdapterMonitorHeight))
		{
			// Calc the aspect ratio of this mode, and create a list of aspect ratios 
			float fAspect = (float)dm.Width / (float)dm.Height;
			bool bFound = false;
			for (int iAspect = 0; iAspect < aspectRatioList.GetSize(); iAspect++)
			{
				if (fabsf(aspectRatioList.GetAt(iAspect) - fAspect) < 0.05f)
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
				aspectRatioList.Add(fAspect);
		}
	}

	// Sort aspect ratio list
	if (aspectRatioList.GetData())
		qsort(aspectRatioList.GetData(), aspectRatioList.GetSize(), sizeof(float), SortAspectRatios);

	// Store the currently selected aspect ratio so it can be restored later
	float fCurrentlySelectedAspect = 0.0f;
	if (g_Render.VideoMenuDlg.GetComboBox(IDC_ASPECT)->GetNumItems() > 0)
	{
		void* pD = g_Render.VideoMenuDlg.GetComboBox(IDC_ASPECT)->GetSelectedData();
		fCurrentlySelectedAspect = *(float*)(void*)&pD;
	}

	// Build the UI list of aspect ratios
	g_Render.VideoMenuDlg.GetComboBox(IDC_ASPECT)->RemoveAllItems();
	for (int i = 0; i < aspectRatioList.GetSize(); i++)
	{
		WCHAR sz[256];
		float fAspect = aspectRatioList.GetAt(i);

		// Make user friendly strings for common aspect ratios
		if (fabsf(fAspect - 1.3333f) < 0.05f) swprintf_s(sz, 256, L"4:3");
		else if (fabsf(fAspect - 1.25f) < 0.05f) swprintf_s(sz, 256, L"5:4");
		else if (fabsf(fAspect - 1.77f) < 0.05f) swprintf_s(sz, 256, L"16:9");
		else if (fabsf(fAspect - 1.6f) < 0.05f) swprintf_s(sz, 256, L"16:10");
		else if (fabsf(fAspect - 1.5f) < 0.05f) swprintf_s(sz, 256, L"3:2");
		else if (fabsf(fAspect - 0.8f) < 0.05f) swprintf_s(sz, 256, L"4:5");
		else if (fabsf(fAspect - 1.66f) < 0.05f) swprintf_s(sz, 256, L"5:3");
		else if (fabsf(fAspect - 0.75f) < 0.05f) swprintf_s(sz, 256, L"3:4");
		else if (fabsf(fAspect - 0.5625f) < 0.05f) swprintf_s(sz, 256, L"9:16");
		else
			swprintf_s(sz, 256, L"%0.2f:1", fAspect);

		g_Render.VideoMenuDlg.GetComboBox(IDC_ASPECT)->AddItem(sz, UintToPtr(*(DWORD*)&fAspect));
	}

	// Important: if nothing was selected before, then default to the desktop resolution
	// Defaulting to the aspect ratio of the desktop is the best default aspect ratio
	// since the desktop resolution can be assumed to be setup correctly for the display device
	if (fCurrentlySelectedAspect == 0.0f)
	{
		D3DDISPLAYMODE dmDesktop;
		DXUTGetDesktopResolution(pDS->d3d11.AdapterOrdinal, &dmDesktop.Width, &dmDesktop.Height);
		g_Render.fDesktopAspectRatio = dmDesktop.Width / (FLOAT)dmDesktop.Height;
		fCurrentlySelectedAspect = g_Render.fDesktopAspectRatio;
	}
	g_Render.VideoMenuDlg.GetComboBox(IDC_ASPECT)->SetSelectedByData(UintToPtr
	(*(DWORD*)&fCurrentlySelectedAspect));

	// Now update resolution list
	UpdateResolutionList(pDS);
}
*/
//--------------------------------------------------------------------------------------
// Updates the resolution list for D3D11
//--------------------------------------------------------------------------------------
HRESULT MainMenu::UpdateD3D11Resolutions()
{
	const DWORD dwWidth = g_DeviceSettings.d3d11.sd.BufferDesc.Width;
	const DWORD dwHeight = g_DeviceSettings.d3d11.sd.BufferDesc.Height;

	// DXUTSETTINGSDLG_D3D11_RESOLUTION
	//auto pResolutionComboBox = VideoMenuDlg.GetComboBox(ui->getComponentID_By_Name(ui.get(),
	//	ui->getObjComboBox(), &wstring(L"For Resolution")));
	//pResolutionComboBox->RemoveAllItems();

	auto pD3DEnum = DXUTGetD3D11Enumeration();
	auto OutputInfo = pD3DEnum->GetOutputInfo(g_DeviceSettings.d3d11.AdapterOrdinal,
		g_DeviceSettings.d3d11.Output);

	auto pOutputInfo = OutputInfo;
	if (!pOutputInfo)
		return E_FAIL;

	bool bWindowed = DXUTIsWindowed();
	if (!bWindowed)
	{
		//auto pShowAllCB = m_Dialog.GetCheckBox(DXUTSETTINGSDLG_RESOLUTION_SHOW_ALL);
		//bool bShowAll = pShowAllCB->GetChecked();

		// Get the desktop aspect ratio
		DXGI_MODE_DESC dmDesktop;
		DXUTGetDesktopResolution(g_DeviceSettings.d3d11.AdapterOrdinal, &dmDesktop.Width, &dmDesktop.Height);
		float fDesktopAspectRatio = dmDesktop.Width / (float)dmDesktop.Height;

		if (//!bShowAll
			//&& 
			!DXUTIsWindowed())
		{
			float fAspect = (float)dwWidth / (float)dwHeight;
			if (fabsf(fDesktopAspectRatio - fAspect) >= 0.05f)
			{
				// Our current fullscren resolution should be listed in the combo box despite the aspect ratio
			//	pShowAllCB->SetChecked(true);
			//	bShowAll = true;
			}
		}

		for (size_t idm = 0; idm < pOutputInfo->displayModeList.size(); idm++)
		{
			auto DisplayMode = pOutputInfo->displayModeList[idm];
			float fAspect = (float)DisplayMode.Width / (float)DisplayMode.Height;

			if (DisplayMode.Format == g_DeviceSettings.d3d11.sd.BufferDesc.Format)
				// If "Show All" is not checked, then hide all resolutions
				// that don't match the aspect ratio of the desktop resolution
				if (//bShowAll || 
					(//!bShowAll && 
						fabsf(fDesktopAspectRatio - fAspect) < 0.05f))
					AddD3D11Resolution(DisplayMode.Width, DisplayMode.Height);
		}
	}
	else
	{
		//pResolutionComboBox->RemoveAllItems();
		AddD3D11Resolution(dwWidth, dwHeight);
	}

	//pResolutionComboBox->SetSelectedByData(ULongToPtr(MAKELONG(dwWidth, dwHeight)));
	OnD3D11ResolutionChanged();

	return S_OK;
}

HRESULT MainMenu::OnD3D11ResolutionChanged()
{
	if (g_DeviceSettings.d3d11.sd.Windowed)
		return S_OK;

	DWORD dwWidth = 0, dwHeight = 0;

	//auto pComboBox = VideoMenuDlg.GetComboBox(ui->getComponentID_By_Name(ui.get(),
	//	ui->getObjComboBox(), &wstring(L"For Resolution")));
//	DWORD dwResolution = PtrToUlong(pComboBox->GetSelectedData());
//	dwWidth = LOWORD(dwResolution);
	//dwHeight = HIWORD(dwResolution);

	g_DeviceSettings.d3d11.sd.BufferDesc.Width = dwWidth;
	g_DeviceSettings.d3d11.sd.BufferDesc.Height = dwHeight;

	// DXUTSETTINGSDLG_D3D11_REFRESH_RATE
	HRESULT hr = UpdateD3D11RefreshRates();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void MainMenu::AddD3D11Resolution(DWORD dwWidth, DWORD dwHeight)
{
//	auto pComboBox = VideoMenuDlg.GetComboBox(ui->getComponentID_By_Name(ui.get(),
//		ui->getObjComboBox(), &wstring(L"For Resolution")));

	DWORD dwResolutionData;
	WCHAR strResolution[50];
	dwResolutionData = MAKELONG(dwWidth, dwHeight);
	swprintf_s(strResolution, 50, L"%u by %u", dwWidth, dwHeight);

	//if (!pComboBox->ContainsItem(strResolution))
	//	pComboBox->AddItem(strResolution, ULongToPtr(dwResolutionData));
}

//--------------------------------------------------------------------------------------
// Updates the refresh list for D3D11
//--------------------------------------------------------------------------------------
HRESULT MainMenu::UpdateD3D11RefreshRates()
{
	const DWORD dwWidth = g_DeviceSettings.d3d11.sd.BufferDesc.Width;
	const DWORD dwHeight = g_DeviceSettings.d3d11.sd.BufferDesc.Height;
	DXGI_FORMAT backBuffer = g_DeviceSettings.d3d11.sd.BufferDesc.Format;
	const DXGI_RATIONAL RefreshRate = g_DeviceSettings.d3d11.sd.BufferDesc.RefreshRate;

	//auto pRefreshRateComboBox = VideoMenuDlg.GetComboBox(ui->getComponentID_By_Name(ui.get(),
	//	ui->getObjComboBox(), &wstring(L"For Aspect")));//IDC_REFRESH_RATE);
	//for (UINT i = 0; i < pRefreshRateComboBox->GetNumItems(); ++i)
	//{
	//	auto pRefreshRate = reinterpret_cast<DXGI_RATIONAL*>(pRefreshRateComboBox->GetItemData(i));
	//	delete pRefreshRate;
	//}
	//pRefreshRateComboBox->RemoveAllItems();

	bool bWindowed = DXUTIsWindowed();
	if (bWindowed)
	{
		DXGI_RATIONAL def;
		def.Denominator = def.Numerator = 0;
		AddD3D11RefreshRate(def);
	}
	else
	{
		auto pD3DEnum = DXUTGetD3D11Enumeration();
		if (!pD3DEnum)
			return E_POINTER;

		auto pOutputInfo = pD3DEnum->GetOutputInfo(g_DeviceSettings.d3d11.AdapterOrdinal, g_DeviceSettings.d3d11.Output);
		if (!pOutputInfo)
			return E_POINTER;

		for (auto it = pOutputInfo->displayModeList.cbegin(); it != pOutputInfo->displayModeList.cend(); ++it)
		{
			if (it->Width == dwWidth && it->Height == dwHeight && it->Format == backBuffer)
				AddD3D11RefreshRate(it->RefreshRate);
		}

	//	auto pRefreshRateComboBox = VideoMenuDlg.GetComboBox(ui->getComponentID_By_Name(ui.get(),
	//		ui->getObjComboBox(), &wstring(L"For Aspect")));//DXUTSETTINGSDLG_D3D11_REFRESH_RATE);

	//	for (UINT i = 0; i < pRefreshRateComboBox->GetNumItems(); ++i)
	//	{
	//		auto pRate = reinterpret_cast<DXGI_RATIONAL*>(pRefreshRateComboBox->GetItemData(i));
//
	//		if (pRate && pRate->Numerator == RefreshRate.Numerator && pRate->Denominator == RefreshRate.Denominator)
	//		{
	//			pRefreshRateComboBox->SetSelectedByIndex(i);
	//			return 0;
	//		}
	//	}
	}
	return S_OK;
}

void MainMenu::AddD3D11RefreshRate(_In_ DXGI_RATIONAL RefreshRate)
{
	//auto pComboBox = VideoMenuDlg.GetComboBox(ui->getComponentID_By_Name(ui.get(),
	//	ui->getObjComboBox(), &wstring(L"For Aspect")));//DXUTSETTINGSDLG_D3D11_REFRESH_RATE);

	WCHAR strRefreshRate[50];

	if (RefreshRate.Numerator == 0 && RefreshRate.Denominator == 0)
		wcscpy_s(strRefreshRate, 50, L"Default Rate");
	else
		swprintf_s(strRefreshRate, 50, L"%u Hz", RefreshRate.Numerator / RefreshRate.Denominator);

	//if (!pComboBox->ContainsItem(strRefreshRate))
	//{
	//	auto pNewRate = new (nothrow) DXGI_RATIONAL;
	//	if (pNewRate)
	//	{
	//		*pNewRate = RefreshRate;
	//		pComboBox->AddItem(strRefreshRate, pNewRate);
	//	}
	//}
}
