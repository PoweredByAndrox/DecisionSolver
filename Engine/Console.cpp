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
		ui->getDialog()->at(0)->AddListBox(2, X.at(1), Y.at(1), W.at(2), H.at(2));
		ui->getDialog()->at(0)->AddEditBox(1, A2W(Text.at(1)), X.at(2), Y.at(2), W.at(3), H.at(3));

		ui->getDialog()->at(0)->SetSize(W.at(0), H.at(0));
		ui->getDialog()->at(0)->SetBackgroundColors(D3DCOLOR_ARGB(rand() % 255, rand() % 255, rand() % 255, rand() % 255),
			D3DCOLOR_ARGB(rand() % 255, rand() % 255, rand() % 105, rand() % 255),
			D3DCOLOR_ARGB(rand() % 255, rand() % 298, rand() % 255, rand() % 255),
			D3DCOLOR_ARGB(rand() % 255, rand() % 10, rand() % 764, rand() % 179));
		ui->getDialog()->at(0)->SetLocation(W.at(0), H.at(0));
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
	ui->getDialog()->at(0)->SetSize(DXUTGetDXGIBackBufferSurfaceDesc()->Width, DXUTGetDXGIBackBufferSurfaceDesc()->Height -256);
	ui->getDialog()->at(0)->SetLocation(0, DXUTGetDXGIBackBufferSurfaceDesc()->Height -256);
	ui->getDialog()->at(0)->GetListBox(2)->SetSize(ui->getDialog()->at(0)->GetWidth(), ui->getDialog()->at(0)->GetListBox(2)->m_height);
	ui->getDialog()->at(0)->GetButton(0)->SetLocation(ui->getDialog()->at(0)->GetWidth() / 3, ui->getDialog()->at(0)->GetButton(0)->m_y);
	ui->getDialog()->at(0)->GetEditBox(1)->SetLocation(ui->getDialog()->at(0)->GetButton(0)->m_x + 150, ui->getDialog()->at(0)->GetEditBox(1)->m_y);
	ui->getDialog()->at(0)->GetEditBox(1)->SetSize(ui->getDialog()->at(0)->GetWidth() / 4,
		ui->getDialog()->at(0)->GetEditBox(1)->m_height);
}

void Engine::Console::Open()
{
	if (CState == Console_STATE::Close)
	{
		ui->getDialog()->at(0)->SetMinimized(false);
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
		{
			Dialog *All = (Dialog *)pUserContext;
			if (All)
			{
				for (int i = 0; i < All->getAllControls().size(); i++)
				{
					if (All->getAllControls().at(i)->GetType() == CONTROL_EDITBOX)
						Cache_Edit = (EditBox *)All->getAllControls().at(i);
				}
				if (wcsstr(Cache_Edit->GetText(), L"Close Dialog"))
					All->SetMinimized(true);
			}
		}
	}
}