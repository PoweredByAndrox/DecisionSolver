#include "pch.h"

#include "Console.h"

HRESULT Engine::Console::Init()
{
	USES_CONVERSION;
	try
	{
		if (!ui->IsInitUI())
		{
			ui->Init();
			ui->LoadXmlUI(ui->GetResPathA(&string("All.xml"))->c_str());
		}

		vector<int> W = ui->getW(), H = ui->getH(), X = ui->getX(), Y = ui->getY();
		vector<LPCSTR> Text = ui->getText();

		ui->getDialog()->at(0)->SetCallback(OnGUIEvent);

		ui->getDialog()->at(0)->AddButton(0, A2W(Text.at(0)), X.at(0), Y.at(0), W.at(1), H.at(1));

		ui->getDialog()->at(0)->AddEditBox(1, A2W(Text.at(1)), X.at(1), Y.at(1), W.at(2), H.at(2));

		ui->getDialog()->at(0)->SetSize(W.at(0), H.at(0));
		ui->getDialog()->at(0)->SetBackgroundColors(D3DCOLOR_ARGB(200, 98, 138, 206),
			D3DCOLOR_ARGB(rand() % 900, rand() % 400, rand() % 105, rand() % 463),
			D3DCOLOR_ARGB(rand() % 500, rand() % 298, rand() % 583, rand() % 319),
			D3DCOLOR_ARGB(rand() % 400, rand() % 10, rand() % 764, rand() % 179));
		ui->getDialog()->at(0)->SetLocation(1024 /2, 768 /2);
		ui->getDialog()->at(0)->SetCaptionText(L"Console!");

		InitClass = true;
		return S_OK;
	}
	catch (const std::exception &)
	{
		DebugTrace("Console: Init is failed.\n");
		throw exception("Console is failed!!!");
		InitClass = false;
		return E_FAIL;
	}
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
		ui->getDialog()->at(0)->SetLocation(1024 /2, 768 /2);
		ui->getDialog()->at(0)->FocusDefaultControl();
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
	EditBox *Cache_Edit = nullptr;

	if (pControl->GetType() == CONTROL_TYPE::CONTROL_BUTTON)
	{
		Cache_Button = (Button *)pControl;
		if (Cache_Button != nullptr & Cache_Button->m_bMouseOver)
			MessageBoxA(DXUTGetHWND(), "You're click to this button! Congratulation)", "Smile XD", MB_OK);
	}
	else if (pControl->GetType() == CONTROL_TYPE::CONTROL_EDITBOX)
	{
		Cache_Edit = (EditBox *)pControl;
		if (Cache_Edit != nullptr & Cache_Edit->m_bHasFocus)
		{
			MessageBoxA(DXUTGetHWND(), "You're just enter some text to here! Congratulation)", "Smile XD", MB_OK);
			MessageBoxW(DXUTGetHWND(), wstring(wstring(L"You are enter: ")+wstring(Cache_Edit->GetText())).c_str(), L"Mmm...", MB_OK);
		}
	}
}