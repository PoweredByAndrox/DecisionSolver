#include "pch.h"

#include "UI.h"

#include "Actor.h"
extern shared_ptr<Actor> mActor;
#include "Models.h"

HRESULT UI::Init(int Count, LPCWSTR texture)
{
	try
	{
		IMGUI_CHECKVERSION();
		CreateContext();

		dialogs.push_back(Dialogs("Test", true, true, true, false, true, true, 0));
		dialogs.back().Btn.push_back(Buttons("OneButton!", true));
		dialogs.back().Label.push_back(Labels("", true));

		dialogs.push_back(Dialogs("Test#1", true, true, true, false, true, true, 0));
		dialogs.back().Btn.push_back(Buttons("OneButton#1!", true));
		
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
	for (int i = 0; i < dialogs.size(); i++)
		if (dialogs.at(i).IsVisible)
		{
			dialogs.at(0).Label.at(0).ChangeText(string((boost::format(
				string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)\nIs WireFrame? : %b\n")+
				string("Pos Model Obj: X(%.2f), Y(%.2f), Z(%.2f)"))
				% Application->getFPS() % mActor->getPosition().x % mActor->getPosition().y % mActor->getPosition().z
				% Application->IsWireFrame() % Application->getModel()->getPosition().x % Application->getModel()->getPosition().y
				% Application->getModel()->getPosition().z).str()).data());

			if (dialogs.at(0).Btn.at(0).clicked)
				Application->getModel()->setPosition(mActor->getPosition());

			dialogs.at(i).Render();
		}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI UI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, vector<void*> pUserContext)
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
	if (doc->Parse(Application->getFS()->getDataFromFile(&string(File), true).c_str()) > 0)
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
		// GUI
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "id") == 0)
		{
			ID.push_back(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}

		if (strcmp(FirstAttr->Name(), "width") == 0)
		{
			W.push_back(FirstAttr->IntValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}

		if (strcmp(FirstAttr->Name(), "height") == 0)
		{
			H.push_back(FirstAttr->IntValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Element.push_back(Element.back()->FirstChild()->ToElement());

	// ********
		// Other
	for (int i = 1; i < INT16_MAX; i++)
	{
		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
		for (int i = 1; i < INT16_MAX; i++)
		{
			if (strcmp(FirstAttr->Name(), "id") == 0)
			{
				ID.push_back(FirstAttr->Value());
				//CheckType(Element.back()->ToElement()->Value());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "text") == 0)
			{
				text.push_back(FirstAttr->Value());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "x") == 0)
			{
				X.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "y") == 0)
			{
				Y.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "width") == 0)
			{
				W.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "height") == 0)
			{
				H.push_back(FirstAttr->IntValue());
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

void UI::ReloadXML(LPCSTR File)
{
	W.clear(); 
	H.clear();
	X.clear();
	Y.clear();
	ID.clear();
	text.clear();
	Element.clear();

	LoadXmlUI(File);
}