#include "pch.h"

#include "Console.h"

HRESULT Engine::Console::Init()
{
	if (!ui->IsInitUI())
		ui->Init();

	ui->getDialog()->at(0)->SetCallback(OnGUIEvent);

	ui->getDialog()->at(0)->AddButton(0, L"Send Command", 25, 32, 10, 20);
	
	ui->getDialog()->at(0)->AddEditBox(1, L"Enter Command", 10, 20, 30, 25);

	ui->getDialog()->at(0)->SetSize(250, 300);
	ui->getDialog()->at(0)->SetBackgroundColors(D3DCOLOR_ARGB(200, 98, 138, 206),
		D3DCOLOR_ARGB(rand() % 900, rand() % 400, rand() % 105, rand() % 463),
		D3DCOLOR_ARGB(rand() % 500, rand() % 298, rand() % 583, rand() % 319),
		D3DCOLOR_ARGB(rand() % 400, rand() % 10, rand() % 764, rand() % 179));
	ui->getDialog()->at(0)->SetLocation(1024 / 2, 768 / 2);
	ui->getDialog()->at(0)->SetCaptionText(L"Console!");
	//ui->getDialog()->at(0)->SetVisible(false);

	InitClass = true;
	return S_OK;
}

void Engine::Console::Render(float Time)
{
	ui->Render(Time);
}

void Engine::Console::Open()
{
	if (CState == Console_STATE::Close)
	{
		ui->getDialog()->at(0)->SetMinimized(false);
		ui->getDialog()->at(0)->SetLocation(1024 / 2, 768 / 2);
		CState = Console_STATE::Open;
	}
}

void Engine::Console::Close()
{
	if (CState == Console_STATE::Open)
	{
		ui->getDialog()->at(0)->SetMinimized(true);
		CState = Console_STATE::Close;
	}
}

void CALLBACK Engine::Console::OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, void *pUserContext)
{
	Button *Cache_Button = nullptr;
	EditBox *Cache_Slider = nullptr;

	if (pControl->GetType() == CONTROL_TYPE::CONTROL_BUTTON)
	{
		if (Cache_Button != nullptr & FindSubStr(wstring(Cache_Button->GetText()), wstring(L"Send Command")))
			MessageBoxA(DXUTGetHWND(), "You're click to this button! Congratulation)", "Smile XD", MB_OK);
	}
	else if (pControl->GetType() == CONTROL_TYPE::CONTROL_EDITBOX)
	{
		if (Cache_Slider != nullptr & Cache_Slider->m_bMouseOver)
		{
			MessageBoxA(DXUTGetHWND(), "You're just enter some text to here! Congratulation)", "Smile XD", MB_OK);
			MessageBoxW(DXUTGetHWND(), wstring(wstring(L"You are enter: ")+wstring(Cache_Slider->GetText())).c_str(), L"Mmm...", MB_OK);
		}
	}
}