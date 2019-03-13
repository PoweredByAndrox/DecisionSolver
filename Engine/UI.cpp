#include "pch.h"

#include "UI.h"

#include "Actor.h"
#include "Models.h"
#include "Audio.h"

HRESULT UI::Init(int Count, LPCWSTR texture)
{
	try
	{
		IMGUI_CHECKVERSION();
		CreateContext();

		ImGuiIO &io = GetIO();
		io.IniFilename = NULL;

		Dialogs.push_back(dialogs("Main", true, true, true, false, true, true, 0, 3.0004f, 6.f));
		Dialogs.back().CollpsHeader.push_back(CollapsingHeaders("Sounds", true));
		Dialogs.back().CollpsHeader.back().Btn.push_back(Buttons("Start!", true));
		Dialogs.back().CollpsHeader.back().Btn.push_back(Buttons("Stop!", true));
		Dialogs.back().CollpsHeader.back().Btn.push_back(Buttons("Pause!", true));

		Dialogs.back().Label.push_back(Labels("", true));

		//Dialogs.push_back(dialogs("Test#1", true, true, true, false, true, true, 0));
		//Dialogs.back().Btn.push_back(Buttons("OneButton#1!", true));
		
		ImGui_ImplWin32_Init(Application->GetHWND());
		ImGui_ImplDX11_Init(Application->getDevice(), Application->getDeviceContext());

		InitUI = true;
		return S_OK;
	}
	catch (const exception &Catch)
	{
		DebugTrace(string(string("UI: Init is failed. ") + string(Catch.what())).c_str());
		throw exception("Init is failed!!!");
		InitUI = false;
		return E_FAIL;
	}
	return S_OK;
}

void UI::ResizeWnd()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui_ImplWin32_Init(Application->GetHWND());
	ImGui_ImplDX11_Init(Application->getDevice(), Application->getDeviceContext());
}

void UI::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void UI::End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::Render(float Time, int ID)
{
	for (int i = 0; i < Dialogs.size(); i++)
		if (Dialogs.at(i).IsVisible)
		{
			Dialogs.at(0).Label.at(0).ChangeText(string((boost::format(
				string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)\nIs WireFrame? : %b\n"))
				% Application->getFPS() % Application->getActor()->getPosition().x % Application->getActor()->getPosition().y
				% Application->getActor()->getPosition().z % Application->IsWireFrame()).str()).data());

			if (Dialogs.at(0).CollpsHeader.back().Btn.at(0).clicked)
				Application->getSound()->doPlay();
			if (Dialogs.at(0).CollpsHeader.back().Btn.at(1).clicked)
				Application->getSound()->doStop();
			if (Dialogs.at(0).CollpsHeader.back().Btn.at(2).clicked)
				Application->getSound()->doPause();

			Dialogs.at(i).Render();
		}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI UI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return 0;
}

void UI::Destroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}

HRESULT UI::LoadXmlUI(LPCSTR File)
{
	doc = make_unique<tinyxml2::XMLDocument>();

	doc->LoadFile(File);
	if (doc->ErrorID() > 0)
	{
		StackTrace(doc->ErrorStr());
		throw exception("UI->LoatXmlUI()::doc->LoadFile() == 0!!!");
		return E_FAIL;
	}
	if (doc->Parse(Application->getFS()->getDataFromFile(&string(File), true, string("<!--"), string("-->")).c_str()) > 0)
	{
		throw exception(string(string("UI->LoatXmlUI()::doc->Parse: \n") + string(doc->ErrorStr())).c_str());
		return E_FAIL;
	}
	
	ProcessXML();

	return S_OK;
}

void UI::ProcessXML()
{
	Element = { doc->RootElement() };
	if (!Element.front())
	{
		DebugTrace("UI->LoadXmlUI()::doc->RootElement() == nullptr!!!");
		throw exception("UI->LoadXmlUI()::doc->RootElement() == nullptr!!!");
		return;
	}

	// ********
		// Dialog

	dialogs dialog;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "id") == 0)
		{
			dialog.IDTitle = FirstAttr->Value();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "width") == 0)
		{
			dialog.SizeW = FirstAttr->FloatValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "height") == 0)
		{
			dialog.SizeH = FirstAttr->FloatValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "open") == 0)
		{
			dialog.IsVisible = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "resize") == 0)
		{
			dialog.IsResizeble = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "show_title") == 0)
		{
			dialog.ShowTitle = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "moveble") == 0)
		{
			dialog.IsMoveble = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "collapse") == 0)
		{
			dialog.IsCollapsible = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Element.push_back(Element.back()->FirstChild()->ToElement());

	// ********
		// Other

	Buttons btn;
	IText itext;
	ITextMulti itextmul;
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(Element.back()->Name(), "Button") == 0)
		{
			XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
			for (int i = 1; i < INT16_MAX; i++)
			{
				if (strcmp(FirstAttr->Name(), "visible") == 0)
				{
					btn.IsVisible = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "text") == 0)
				{
					btn.IDTitle = FirstAttr->Value();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
			}

			if (Element.front()->LastChild()->Value() == Element.back()->Value())
				break;

			Element.push_back(Element.back()->NextSibling()->ToElement());
		}
		if (strcmp(Element.back()->Name(), "InputText") == 0)
		{
			XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
			for (int i = 1; i < INT16_MAX; i++)
			{
				if (strcmp(FirstAttr->Name(), "visible") == 0)
				{
					itext.IsVisible = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "text") == 0)
				{
					itext.Text = FirstAttr->Value();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "history") == 0)
				{
					itext.IsNeedHistory = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
			}

			if (Element.front()->LastChild()->Value() == Element.back()->Value())
				break;

			Element.push_back(Element.back()->NextSibling()->ToElement());
		}
		if (strcmp(Element.back()->Name(), "InputTextMultiline") == 0)
		{
			XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
			for (int i = 1; i < INT16_MAX; i++)
			{
				if (strcmp(FirstAttr->Name(), "visible") == 0)
				{
					itextmul.IsVisible = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "text") == 0)
				{
					itextmul.Text = FirstAttr->Value();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "readonly") == 0)
				{
					itextmul.ReadOnly = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
			}

			if (Element.front()->LastChild()->Value() == Element.back()->Value())
				break;

			Element.push_back(Element.back()->NextSibling()->ToElement());
		}
	}

	dialog.Btn.push_back(btn);
	dialog.Itext.push_back(itext);
	dialog.Itextmul.push_back(itextmul);
	Dialogs.push_back(dialog);
}

void UI::ReloadXML(LPCSTR File)
{
	LoadXmlUI(File);
}

HRESULT UI::addDialog(LPCSTR IDName)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addDialog()::Dialogs->empty() == empty!!!");
		throw exception("UI->addDialog()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	Dialogs.push_back(dialogs(IDName, true, true, true, false, true, false, 0));
	return S_OK;
}

HRESULT UI::addButton(LPCSTR IDName, LPCSTR IDDialog)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addButton()::Dialogs->empty() == empty!!!");
		throw exception("UI->addButton()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	if (!IDDialog)
		Dialogs.back().Btn.push_back(Buttons(IDName, true));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (Dialogs.at(i).IDTitle == IDDialog)
				Dialogs.at(i).Btn.push_back(Buttons(IDName, true));
		}
	return S_OK;
}

HRESULT UI::addLabel(LPCSTR IDName, LPCSTR IDDialog)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addLabel()::Dialogs->empty() == empty!!!");
		throw exception("UI->addLabel()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	if (!IDDialog)
		Dialogs.back().Label.push_back(Labels(IDName, true));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (Dialogs.at(i).IDTitle == IDDialog)
				Dialogs.at(i).Label.push_back(Labels(IDName, true));
		}
	return S_OK;
}

HRESULT UI::addCollapseHead(LPCSTR IDName, LPCSTR IDDialog)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	if (!IDDialog)
		Dialogs.back().CollpsHeader.push_back(CollapsingHeaders(IDName, false));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (Dialogs.at(i).IDTitle == IDDialog)
				Dialogs.at(i).CollpsHeader.push_back(CollapsingHeaders(IDName, false));
		}
	return S_OK;
}
