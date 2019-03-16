#include "pch.h"

#include "UI.h"

#include "Actor.h"
#include "Models.h"
#include "Audio.h"
static shared_ptr<Render_Buffer> Buf = make_unique<Render_Buffer>();

HRESULT UI::Init()
{
	try
	{
		IMGUI_CHECKVERSION();
		CreateContext();

		ImGuiIO &io = GetIO();
		io.IniFilename = NULL;

		vector<wstring> FileShaders;
		vector<string> Functions, Version;

		FileShaders.push_back(Application->getFS()->GetFile(string("VertexShader.hlsl"))->PathW);
		FileShaders.push_back(Application->getFS()->GetFile(string("PixelShader.hlsl"))->PathW);

		Functions.push_back(string("VS"));
		Functions.push_back(string("PS"));

		Version.push_back(string("vs_4_0"));
		Version.push_back(string("ps_4_0"));

		Buf->SetShadersFile(FileShaders, Functions, Version);

		if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
			return false;
		if (!::QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
			return false;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io.BackendPlatformName = "DecisionSolver";
		io.ImeWindowHandle = Application->GetHWND();

		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';

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
	Buf->Release();

	Buf->InitUI();
}

void UI::Begin()
{
	Buf->InitUI();
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() &&
		"Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

	RECT rect;
	::GetClientRect(Application->GetHWND(), &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	INT64 current_time;
	::QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
	g_Time = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;

	UpdateMousePos();

	ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
	if (g_LastMouseCursor != mouse_cursor)
	{
		g_LastMouseCursor = mouse_cursor;
		UpdateMouseCursor();
	}

	Gamepads();
	ImGui::NewFrame();
}

void UI::End(bool WF)
{
	ImGui::Render();
	Buf->RenderUI(ImGui::GetDrawData(), WF);
}

void UI::Render()
{
	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (Dialogs.at(i)->IsVisible)
			Dialogs.at(i)->Render();
	}
}

void UI::Destroy()
{
	Buf->Release();
	//ImGui_ImplWin32_Shutdown();
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

	shared_ptr<dialogs> dialog = make_unique<dialogs>();

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "id") == 0)
		{
			dialog->IDTitle = FirstAttr->Value();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "width") == 0)
		{
			dialog->SizeW = FirstAttr->FloatValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "height") == 0)
		{
			dialog->SizeH = FirstAttr->FloatValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "open") == 0)
		{
			dialog->IsVisible = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "resize") == 0)
		{
			dialog->IsResizeble = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "show_title") == 0)
		{
			dialog->ShowTitle = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "moveble") == 0)
		{
			dialog->IsMoveble = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "collapse") == 0)
		{
			dialog->IsCollapsible = FirstAttr->BoolValue();
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Element.push_back(Element.back()->FirstChild()->ToElement());

	// ********
		// Other

	shared_ptr<Buttons> btn = make_unique<Buttons>();
	shared_ptr<IText> itext = make_unique<IText>();
	shared_ptr<ITextMulti> itextmul = make_unique<ITextMulti>();
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(Element.back()->Name(), "Button") == 0)
		{
			XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
			for (int i = 1; i < INT16_MAX; i++)
			{
				if (strcmp(FirstAttr->Name(), "visible") == 0)
				{
					btn->IsVisible = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "text") == 0)
				{
					btn->IDTitle = FirstAttr->Value();
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
					itext->IsVisible = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "text") == 0)
				{
					itext->Text = FirstAttr->Value();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "history") == 0)
				{
					itext->IsNeedHistory = FirstAttr->BoolValue();
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
					itextmul->IsVisible = FirstAttr->BoolValue();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "text") == 0)
				{
					itextmul->Text = FirstAttr->Value();
					FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
					if (!FirstAttr)
						break;
				}
				if (strcmp(FirstAttr->Name(), "readonly") == 0)
				{
					itextmul->ReadOnly = FirstAttr->BoolValue();
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

	dialog->Btn.push_back(btn);
	dialog->Itext.push_back(itext);
	dialog->Itextmul.push_back(itextmul);
	Dialogs.push_back(dialog);
}

void UI::ReloadXML(LPCSTR File)
{
	Reload = true;
	Dialogs.clear();
	Element.clear();

	Buf->Release();
	LoadXmlUI(File);

	Buf->InitUI();
	Reload = false;
}

HRESULT UI::addDialog(LPCSTR IDName)
{
	Dialogs.push_back(make_unique<dialogs>(IDName, true, true, true, false, true, false, 0));
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
		Dialogs.back()->Btn.push_back(make_unique<Buttons>(IDName, true));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
				Dialogs.at(i)->Btn.push_back(make_unique<Buttons>(IDName, true));
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
		Dialogs.back()->Label.push_back(make_unique<Labels>(IDName, true));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
				Dialogs.at(i)->Label.push_back(make_unique<Labels>(IDName, true));
		}
	return S_OK;
}
HRESULT UI::addCollapseHead(LPCSTR IDName, LPCSTR IDDialog, bool SelDef)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	if (!IDDialog)
		Dialogs.back()->CollpsHeader.push_back(make_unique<CollapsingHeaders>(IDName, SelDef));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
				Dialogs.at(i)->CollpsHeader.push_back(make_unique<CollapsingHeaders>(IDName, SelDef));
		}
	return S_OK;
}
HRESULT UI::addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<Buttons> Component)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->CollpsHeader.back()->Btn.push_back(Component);
	}
	return S_OK;
}
HRESULT UI::addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<CollapsingHeaders> Component)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->CollpsHeader.push_back(Component);
	}
	return S_OK;
}
HRESULT UI::addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<Labels> Component)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->CollpsHeader.back()->Label.push_back(Component);
	}
	return S_OK;
}
HRESULT UI::addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<IText> Component)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->CollpsHeader.back()->Itext.push_back(Component);
	}
	return S_OK;
}
HRESULT UI::addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<ITextMulti> Component)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->CollpsHeader.back()->Itextmul.push_back(Component);
	}
	return S_OK;
}

void UI::DisableDialog(LPCSTR IDDialog)
{
	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->setVisible(false);
	}
}
void UI::EnableDialog(LPCSTR IDDialog)
{
	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			Dialogs.at(i)->setVisible(true);
	}
}

shared_ptr<dialogs> UI::getDialog(LPCSTR IDDialog)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addButton()::Dialogs->empty() == empty!!!");
		throw exception("UI->addButton()::Dialogs->empty() == empty!!!");
		return nullptr;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->IDTitle, IDDialog) == 0)
			return Dialogs.at(i);
	}
}

bool UI::UpdateMouseCursor()
{
	ImGuiIO &io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return false;

	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		::SetCursor(NULL);
	else
	{
		LPTSTR win32_cursor = IDC_ARROW;
		switch (imgui_cursor)
		{
		case ImGuiMouseCursor_Arrow:      win32_cursor = IDC_ARROW; break;
		case ImGuiMouseCursor_TextInput:  win32_cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:  win32_cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:   win32_cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:   win32_cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW: win32_cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE: win32_cursor = IDC_SIZENWSE; break;
		case ImGuiMouseCursor_Hand:       win32_cursor = IDC_HAND; break;
		}
		::SetCursor(::LoadCursor(NULL, win32_cursor));
	}
	return true;
}
void UI::UpdateMousePos()
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantSetMousePos)
	{
		POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
		::ClientToScreen(Application->GetHWND(), &pos);
		::SetCursorPos(pos.x, pos.y);
	}

	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	POINT pos;
	if (HWND active_window = ::GetForegroundWindow())
		if (active_window == Application->GetHWND() || ::IsChild(active_window, Application->GetHWND()))
			if (::GetCursorPos(&pos) && ::ScreenToClient(Application->GetHWND(), &pos))
				io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}

void UI::Gamepads()
{
	ImGuiIO &io = ImGui::GetIO();
	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
		return;

	if (g_WantUpdateHasGamepad)
	{
		g_HasGamepad = Application->getGamepad()->GetState(0).IsConnected();
		g_WantUpdateHasGamepad = false;
	}

	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
	if (g_HasGamepad)
	{
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		io.NavInputs[ImGuiNavInput_Activate] = Application->getTracherGamepad().a;
		io.NavInputs[ImGuiNavInput_Cancel] = Application->getTracherGamepad().b;
		io.NavInputs[ImGuiNavInput_Menu] = Application->getTracherGamepad().x;
		io.NavInputs[ImGuiNavInput_Input] = Application->getTracherGamepad().y;

		io.NavInputs[ImGuiNavInput_DpadLeft] = Application->getTracherGamepad().dpadLeft;
		io.NavInputs[ImGuiNavInput_DpadRight] = Application->getTracherGamepad().dpadRight;
		io.NavInputs[ImGuiNavInput_DpadUp] = Application->getTracherGamepad().dpadUp;
		io.NavInputs[ImGuiNavInput_DpadDown] = Application->getTracherGamepad().dpadDown;

		io.NavInputs[ImGuiNavInput_FocusPrev] = Application->getTracherGamepad().leftShoulder;
		io.NavInputs[ImGuiNavInput_FocusNext] = Application->getTracherGamepad().rightShoulder;
		io.NavInputs[ImGuiNavInput_TweakSlow] = Application->getTracherGamepad().leftShoulder;
		io.NavInputs[ImGuiNavInput_TweakFast] = Application->getTracherGamepad().rightShoulder;

		//MAP_ANALOG(ImGuiNavInput_LStickLeft, gamepad.sThumbLX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
		//MAP_ANALOG(ImGuiNavInput_LStickRight, gamepad.sThumbLX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
		//MAP_ANALOG(ImGuiNavInput_LStickUp, gamepad.sThumbLY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
		//MAP_ANALOG(ImGuiNavInput_LStickDown, gamepad.sThumbLY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
	}
}

LRESULT UI::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext() == NULL)
		return 0;

	ImGuiIO &io = ImGui::GetIO();
	switch (uMsg)
	{
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	{
		int button = 0;
		if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK)
			button = Application->getTrackerMouse().leftButton;

		if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK)
			button = Application->getTrackerMouse().rightButton;

		if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK)
			button = Application->getTrackerMouse().middleButton;

		if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONDBLCLK)
			button = Application->getTrackerMouse().xButton1 ? 3 : 4;

		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
			::SetCapture(Application->GetHWND());
		io.MouseDown[button] = true;
		return 0;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int button = 0;
		if (uMsg == WM_LBUTTONUP) 
			button = Application->getTrackerMouse().leftButton;

			if (uMsg == WM_RBUTTONUP) 
				button = Application->getTrackerMouse().rightButton;

			if (uMsg == WM_MBUTTONUP) 
				button = Application->getTrackerMouse().middleButton;

		if (uMsg == WM_XBUTTONUP) 
			button = Application->getTrackerMouse().xButton1 ? 3 : 4;

		io.MouseDown[button] = false;
		return 0;
	}
	case WM_MOUSEWHEEL:
		io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_MOUSEHWHEEL:
		io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return 0;
	case WM_CHAR:
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return 0;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT && UpdateMouseCursor())
			return 1;
		return 0;
	}
	return false;
}