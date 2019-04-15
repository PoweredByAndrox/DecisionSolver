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
		io.LogFilename = NULL;

		vector<wstring> FileShaders;
		vector<string> Functions, Version;

		FileShaders.push_back(Application->getFS()->GetFile(string("VertexShader.hlsl"))->PathW);
		FileShaders.push_back(Application->getFS()->GetFile(string("PixelShader.hlsl"))->PathW);

		Functions.push_back(string("Vertex_ui_VS"));
		Functions.push_back(string("Pixel_ui_PS"));

		Version.push_back(string("vs_4_0"));
		Version.push_back(string("ps_4_0"));

		Buf->SetShadersFile(FileShaders, Functions, Version);

		if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
			return false;
		if (!::QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
			return false;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
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
		"Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function?");

	RECT rect;
	::GetClientRect(Application->GetHWND(), &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	INT64 current_time;
	::QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	io.DeltaTime = (float)(current_time - g_Time) / (float)g_TicksPerSecond;
	g_Time = current_time;

	io.KeyCtrl = Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::LeftControl);
	io.KeyShift = Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::LeftShift);
	io.KeyAlt = Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::LeftAlt);
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

void UI::FrameEnd()
{
	ImGui::Render();
	Buf->RenderUI(ImGui::GetDrawData(), Application->IsWireFrame());
}

void UI::Render()
{
	auto Dial = getDialog("Main");
	if ((Dial.operator bool() && !Dial->GetTitle().empty()) && Dial->getVisible())
		Dial->Render();
}

void UI::Destroy()
{
	Buf->Release();
	DestroyContext();
}

HRESULT UI::LoadXmlUI(string File)
{
	doc = make_shared<tinyxml2::XMLDocument>();

	doc->LoadFile(File.c_str());
	if (doc->ErrorID() > 0)
	{
		StackTrace(doc->ErrorStr());
		throw exception("UI->LoatXmlUI()::doc->LoadFile() == 0!!!");
		return E_FAIL;
	}
	if (doc->Parse(Application->getFS()->getDataFromFile(File, true, string("<!--"), string("-->")).c_str()) > 0)
	{
		throw exception(string(string("UI->LoatXmlUI()::doc->Parse:\n") + string(doc->ErrorStr())).c_str());
		return E_FAIL;
	}

	ProcessXML();

	return S_OK;
}

void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<Buttons> &btn, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			btn->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			btn->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	btn->ChangeOrder(CountOrder);
	dialog->setComponent(btn);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<Labels> &Label, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			Label->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			Label->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Label->ChangeOrder(CountOrder);
	dialog->setComponent(Label);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<IText> &Itext, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			Itext->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			Itext->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "history") == 0)
		{
			Itext->setHistory(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "hint") == 0)
		{
			Itext->setHint(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "texthint") == 0)
		{
			Itext->ChangeTextHint(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Itext->ChangeOrder(CountOrder);
	dialog->setComponent(Itext);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<ITextMulti> &ItextMul, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			ItextMul->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			ItextMul->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "readonly") == 0)
		{
			ItextMul->setReadOnly(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	ItextMul->ChangeOrder(CountOrder);
	dialog->setComponent(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<TextList> &TList, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			TList->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "label") == 0)
		{
			TList->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	TList->ChangeOrder(CountOrder);
	dialog->setComponent(TList);
}

void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, shared_ptr<child> XMLchild, shared_ptr<Child> &child, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(XMLchild->_Child->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "id") == 0)
		{
			child->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "border") == 0)
		{
			child->setBorder(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "horizontal_scroll") == 0)
		{
			child->setHScroll(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	
	//	Button
	for (int i = 0; i < XMLchild->buttons.size(); i++)
	{
		shared_ptr<Buttons> btn = make_unique<Buttons>();
		WorkOnComponents(child, XMLchild->buttons.at(i)->ToElement(), btn, CountOrder);
	}
	//	InputText
	for (int i = 0; i < XMLchild->texts.size(); i++)
	{
		shared_ptr<IText> Itext = make_unique<IText>();
		WorkOnComponents(child, XMLchild->texts.at(i)->ToElement(), Itext, CountOrder);
	}
	//	InputTextMultiline
	for (int i = 0; i < XMLchild->textmuls.size(); i++)
	{
		shared_ptr<ITextMulti> ItextMul = make_unique<ITextMulti>();
		WorkOnComponents(child, XMLchild->textmuls.at(i)->ToElement(), ItextMul, CountOrder);
	}
	//	Collapse
	//for (int i = 0; i < XMLchild->collpheaders.size(); i++)
	//{
	//	shared_ptr<CollapsingHeaders> CHeader = make_unique<CollapsingHeaders>();
	//	WorkOnComponents(child, XMLchild->collpheaders.at(i), CHeader, CountOrder);
	//}
	//	Label
	for (int i = 0; i < XMLchild->labels.size(); i++)
	{
		shared_ptr<Labels> label = make_unique<Labels>();
		WorkOnComponents(child, XMLchild->labels.at(i)->ToElement(), label, CountOrder);
	}
	//	Separator
	for (int i = 0; i < XMLchild->separators.size(); i++)
	{
		shared_ptr<_Separator> separator = make_unique<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	//for (int i = 0; i < XMLchild->childs.size(); i++)
	//{
	//	shared_ptr<Child> _child = make_unique<Child>();
	//	WorkOnComponents(child, XMLchild, _child, CountOrder);
	//}
	//	UnformatedText
	for (int i = 0; i < XMLchild->utext.size(); i++)
	{
		shared_ptr<UnformatedText> UText = make_unique<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		child->setComponent(UText);
	}

	child->ChangeOrder(CountOrder);
	dialog->setComponent(child);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, shared_ptr<collpheader> XMLCHeader, shared_ptr<CollapsingHeaders> &CHeader, 
	int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(XMLCHeader->CollpsHead->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "seldefault") == 0)
		{
			CHeader->setSelDefault(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			CHeader->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "collapsible") == 0)
		{
			CHeader->setCollapse(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}

	//	Button
	for (int i = 0; i < XMLCHeader->buttons.size(); i++)
	{
		shared_ptr<Buttons> btn = make_unique<Buttons>();
		WorkOnComponents(CHeader, XMLCHeader->buttons.at(i)->ToElement(), btn, CountOrder);
	}
	//	InputText
	for (int i = 0; i < XMLCHeader->texts.size(); i++)
	{
		shared_ptr<IText> Itext = make_unique<IText>();
		WorkOnComponents(CHeader, XMLCHeader->texts.at(i)->ToElement(), Itext, CountOrder);
	}
	//	InputTextMultiline
	for (int i = 0; i < XMLCHeader->textmuls.size(); i++)
	{
		shared_ptr<ITextMulti> ItextMul = make_unique<ITextMulti>();
		WorkOnComponents(CHeader, XMLCHeader->textmuls.at(i)->ToElement(), ItextMul, CountOrder);
	}
	//	Collapse
	//for (int i = 0; i < XMLCHeader->collpheaders.size(); i++)
	//{
	//	shared_ptr<CollapsingHeaders> _CHeader = make_unique<CollapsingHeaders>();
	//	WorkOnComponents(CHeader, XMLCHeader, _CHeader, CountOrder);
	//}
	//	Label
	for (int i = 0; i < XMLCHeader->labels.size(); i++)
	{
		shared_ptr<Labels> label = make_unique<Labels>();
		WorkOnComponents(CHeader, XMLCHeader->labels.at(i)->ToElement(), label, CountOrder);
	}
	//	Separator
	for (int i = 0; i < XMLCHeader->separators.size(); i++)
	{
		shared_ptr<_Separator> separator = make_unique<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	//for (int i = 0; i < XMLCHeader->childs.size(); i++)
	//{
	//	shared_ptr<Child> child = make_unique<Child>();
	//	WorkOnComponents(dialog, XMLCHeader, child, CountOrder);
	//}
		//	UnformatedText
	for (int i = 0; i < XMLCHeader->utext.size(); i++)
	{
		shared_ptr<UnformatedText> UText = make_unique<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		CHeader->setComponent(UText);
	}

	CHeader->ChangeOrder(CountOrder);
	dialog->setComponent(CHeader);
}

void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<Buttons> &btn, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			btn->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			btn->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	btn->ChangeOrder(CountOrder);
	InChild->setComponent(btn);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<Labels> &Label, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			Label->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			Label->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Label->ChangeOrder(CountOrder);
	InChild->setComponent(Label);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<IText> &Itext, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			Itext->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			Itext->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "history") == 0)
		{
			Itext->setHistory(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "hint") == 0)
		{
			Itext->setHint(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "texthint") == 0)
		{
			Itext->ChangeTextHint(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Itext->ChangeOrder(CountOrder);
	InChild->setComponent(Itext);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<ITextMulti> &ItextMul, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			ItextMul->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			ItextMul->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "readonly") == 0)
		{
			ItextMul->setReadOnly(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	ItextMul->ChangeOrder(CountOrder);
	InChild->setComponent(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<TextList> &TList, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			TList->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "label") == 0)
		{
			TList->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	TList->ChangeOrder(CountOrder);
	InChild->setComponent(TList);
}

void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<Buttons> &btn, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			btn->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			btn->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	btn->ChangeOrder(CountOrder);
	InCollaps->setComponent(btn);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<Labels> &Label, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			Label->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			Label->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Label->ChangeOrder(CountOrder);
	InCollaps->setComponent(Label);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<IText> &Itext, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			Itext->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			Itext->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "history") == 0)
		{
			Itext->setHistory(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "hint") == 0)
		{
			Itext->setHint(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "texthint") == 0)
		{
			Itext->ChangeTextHint(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Itext->ChangeOrder(CountOrder);
	InCollaps->setComponent(Itext);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<ITextMulti> &ItextMul, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			ItextMul->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			ItextMul->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "readonly") == 0)
		{
			ItextMul->setReadOnly(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	ItextMul->ChangeOrder(CountOrder);
	InCollaps->setComponent(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<TextList> &TList, int &CountOrder)
{
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(element->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "visible") == 0)
		{
			TList->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			TList->ChangeTitle(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	TList->ChangeOrder(CountOrder);
	InCollaps->setComponent(TList);
}

void UI::ProcessXML()
{
	vector<shared_ptr<dialogs>> dialog;

	for (int i = 0; i < INT16_MAX; i++)
	{
		if (XMLDialogs.empty())
			XMLDialogs.push_back(make_unique<dial>(doc->RootElement()->Parent()->FirstChild()->NextSibling()));

		if (doc->RootElement()->Parent()->LastChild() != XMLDialogs.back()->Dial)
		{
			if (XMLDialogs.back()->Dial && XMLDialogs.back()->Dial->NextSibling())
				XMLDialogs.push_back(make_unique<dial>(XMLDialogs.back()->Dial->NextSibling()));
			else
				break;
		}
		else
			break;
	}

	int i = 0;
	for (auto It = XMLDialogs.begin(); It != XMLDialogs.end(); ++It)
	{
		vector<XMLNode *> TheFirstComponent;
		for (int i = 0; i < INT16_MAX; i++)
		{
			if (TheFirstComponent.empty())
				TheFirstComponent.push_back((*It)->Dial->FirstChild());
			if ((*It)->Dial->LastChild() != TheFirstComponent.back())
			{
				if (TheFirstComponent.back()->NextSibling())
					TheFirstComponent.push_back(TheFirstComponent.back()->NextSibling());
			}
			else
				break;
		}
		for (auto ItComponents = TheFirstComponent.begin(); ItComponents != TheFirstComponent.end(); ++ItComponents)
		{
			int countComponents = 0;

			if (strcmp((*ItComponents)->Value(), "Collapse") == 0)
			{
				int countComponentsForCollapse = 0;

				XMLDialogs.at(i)->collpheaders.push_back(make_unique<collpheader>(*ItComponents, true));
				XMLDialogs.at(i)->IDcollpheaders.push_back(countComponentsForCollapse++);

				vector<XMLNode *> TheSecondComponent;
				for (int i = 0; i < INT16_MAX; i++)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*ItComponents)->ToElement()->FirstChild());
					if ((*ItComponents)->ToElement()->LastChild() != TheSecondComponent.back())
					{
						if (TheSecondComponent.back()->NextSibling())
							TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					}
					else
						break;
				}
				for (auto ItComponents = TheSecondComponent.begin(); ItComponents != TheSecondComponent.end(); ++ItComponents)
				{
					if (strcmp((*ItComponents)->Value(), "Button") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDbuttons.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "InputText") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDtexts.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "InputTextMultiline") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDtextmuls.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "Label") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDlabels.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "Separator") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDseparators.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "ChildDialog") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDchilds.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "UnformatedText") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDutext.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "ListBox") == 0)
					{
						XMLDialogs.at(i)->collpheaders.back()->IDtlist.push_back(countComponentsForCollapse++);
						XMLDialogs.at(i)->collpheaders.back()->setComponentCHeader(*ItComponents);
					}

					XMLDialogs.at(i)->collpheaders.back()->OrderlyRender = countComponentsForCollapse;
				}
			}
			else if (strcmp((*ItComponents)->Value(), "Button") == 0)
			{
				XMLDialogs.at(i)->IDbuttons.push_back(countComponents++);
				XMLDialogs.at(i)->buttons.push_back(*ItComponents);
			}
			else if (strcmp((*ItComponents)->Value(), "InputText") == 0)
			{
				XMLDialogs.at(i)->IDtexts.push_back(countComponents++);
				XMLDialogs.at(i)->texts.push_back(*ItComponents);
			}
			else if (strcmp((*ItComponents)->Value(), "InputTextMultiline") == 0)
			{
				XMLDialogs.at(i)->IDtextmuls.push_back(countComponents++);
				XMLDialogs.at(i)->textmuls.push_back(*ItComponents);
			}
			else if (strcmp((*ItComponents)->Value(), "Label") == 0)
			{
				XMLDialogs.at(i)->IDlabels.push_back(countComponents++);
				XMLDialogs.at(i)->labels.push_back(*ItComponents);
			}
			else if (strcmp((*ItComponents)->Value(), "Separator") == 0)
			{
				XMLDialogs.at(i)->IDseparators.push_back(countComponents++);
				XMLDialogs.at(i)->separators.push_back(*ItComponents);
			}
			else if (strcmp((*ItComponents)->Value(), "ChildDialog") == 0)
			{
				int countComponentsForChild = 0;

				XMLDialogs.at(i)->childs.push_back(make_unique<child>(*ItComponents, true));
				XMLDialogs.at(i)->IDchilds.push_back(countComponents++);

				vector<XMLNode *> TheSecondComponent;
				for (int i = 0; i < INT16_MAX; i++)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*ItComponents)->ToElement()->FirstChild());
					if ((*ItComponents)->ToElement()->LastChild() != TheSecondComponent.back())
					{
						if (TheSecondComponent.back()->NextSibling())
							TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					}
					else
						break;
				}
				for (auto ItComponents = TheSecondComponent.begin(); ItComponents != TheSecondComponent.end(); ++ItComponents)
				{
					if (strcmp((*ItComponents)->Value(), "Button") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDbuttons.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "InputText") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDtexts.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "InputTextMultiline") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDtextmuls.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "Label") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDlabels.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "Separator") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDseparators.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "ChildDialog") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDchilds.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "UnformatedText") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDutext.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}
					else if (strcmp((*ItComponents)->Value(), "ListBox") == 0)
					{
						XMLDialogs.at(i)->childs.back()->IDtlist.push_back(countComponentsForChild++);
						XMLDialogs.at(i)->childs.back()->setComponentChild(*ItComponents);
					}

					XMLDialogs.at(i)->childs.back()->OrderlyRender = countComponentsForChild;
				}
			}
			else if (strcmp((*ItComponents)->Value(), "UnformatedText") == 0)
			{
				XMLDialogs.at(i)->IDutext.push_back(countComponents++);
				XMLDialogs.at(i)->utext.push_back(*ItComponents);
			}
			else if (strcmp((*ItComponents)->Value(), "ListBox") == 0)
			{
				XMLDialogs.at(i)->IDtlist.push_back(countComponents++);
				XMLDialogs.at(i)->tlist.push_back(*ItComponents);
			}

			XMLDialogs.at(i)->OrderlyRender = countComponents;
		}

		i++;
	}

	for (int IDDial = 0; IDDial < XMLDialogs.size(); IDDial++)
	{
		// ********
			// Dialog

		int countComponents = 0;
		dialog.push_back(make_unique<dialogs>());
		
		float W = 0.f, H = 0.f;

		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(XMLDialogs.at(IDDial)->Dial->ToElement()->FirstAttribute());
		for (int i = 1; i < INT16_MAX; i++)
		{
			if (strcmp(FirstAttr->Name(), "id") == 0)
			{
				dialog.at(IDDial)->ChangeTitle(FirstAttr->Value());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "width") == 0)
			{
				W = FirstAttr->FloatValue();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "height") == 0)
			{
				H = FirstAttr->FloatValue();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "open") == 0)
			{
				dialog.at(IDDial)->setVisible(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "resize") == 0)
			{
				dialog.at(IDDial)->setResizeble(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "show_title") == 0)
			{
				dialog.at(IDDial)->SetShowTitle(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "moveble") == 0)
			{
				dialog.at(IDDial)->setMoveble(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "collapsible") == 0)
			{
				dialog.at(IDDial)->setCollapsible(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (strcmp(FirstAttr->Name(), "bring_to_font") == 0)
			{
				dialog.at(IDDial)->setBringToFont(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
		}

		// ********
			// Other

		//	Button
		for (int i = 0; i < XMLDialogs.at(IDDial)->buttons.size(); i++)
		{
			shared_ptr<Buttons> btn = make_unique<Buttons>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->buttons.at(i)->ToElement(), btn, XMLDialogs.at(IDDial)->IDbuttons.at(i));

			countComponents++;
		}
		//	InputText
		for (int i = 0; i < XMLDialogs.at(IDDial)->texts.size(); i++)
		{
			shared_ptr<IText> Itext = make_unique<IText>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->texts.at(i)->ToElement(), Itext, XMLDialogs.at(IDDial)->IDtexts.at(i));
		
			countComponents++;
		}
		//	InputTextMultiline
		for (int i = 0; i < XMLDialogs.at(IDDial)->textmuls.size(); i++)
		{
			shared_ptr<ITextMulti> ItextMul = make_unique<ITextMulti>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->textmuls.at(i)->ToElement(), ItextMul, XMLDialogs.at(IDDial)->IDtextmuls.at(i));

			countComponents++;
		}
		//	Collapse
		for (int i = 0; i < XMLDialogs.at(IDDial)->collpheaders.size(); i++)
		{
			shared_ptr<CollapsingHeaders> CHeader = make_unique<CollapsingHeaders>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->collpheaders.at(i), CHeader, XMLDialogs.at(IDDial)->IDcollpheaders.at(i));
			CHeader->ChangeOrder(XMLDialogs.at(IDDial)->collpheaders.at(i)->getCountOrder());
			
			CHeader->ChangeOrderInDial(countComponents);

			countComponents++;
		}
		//	Label
		for (int i = 0; i < XMLDialogs.at(IDDial)->labels.size(); i++)
		{
			shared_ptr<Labels> label = make_unique<Labels>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->labels.at(i)->ToElement(), label, XMLDialogs.at(IDDial)->IDlabels.at(i));

			countComponents++;
		}
		//	Separator
		for (int i = 0; i < XMLDialogs.at(IDDial)->separators.size(); i++)
		{
			shared_ptr<_Separator> separator = make_unique<_Separator>();
			separator->ChangeOrder(XMLDialogs.at(IDDial)->IDseparators.at(i));

			countComponents++;
		}
		//	ChildDialog
		for (int i = 0; i < XMLDialogs.at(IDDial)->childs.size(); i++)
		{
			shared_ptr<Child> child = make_unique<Child>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->childs.at(i), child, XMLDialogs.at(IDDial)->IDchilds.at(i));
			child->ChangeOrder(XMLDialogs.at(IDDial)->childs.at(i)->getCountOrder());

			child->ChangeOrderInDial(countComponents);

			countComponents++;
		}
		//	UnformatedText
		for (int i = 0; i < XMLDialogs.at(IDDial)->utext.size(); i++)
		{
			shared_ptr<UnformatedText> UText = make_unique<UnformatedText>();
			UText->ChangeOrder(XMLDialogs.at(IDDial)->IDutext.at(i));
			dialog.at(IDDial)->setComponent(UText);

			countComponents++;
		}
		//	TextList
		for (int i = 0; i < XMLDialogs.at(IDDial)->tlist.size(); i++)
		{
			shared_ptr<TextList> TList = make_unique<TextList>();
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->tlist.at(i)->ToElement(), TList, XMLDialogs.at(IDDial)->IDtlist.at(i));

			countComponents++;
		}

		dialog.at(IDDial)->ChangeOrder(countComponents);
		dialog.at(IDDial)->ChangeSize(W, H);
	}
	Dialogs = dialog;
}

void UI::ReloadXML(LPCSTR File)
{
	Reload = true;
	Dialogs.clear();
	XMLDialogs.clear();

	Buf->Release();
	LoadXmlUI(File);

	Buf->InitUI();
	Reload = false;
}

HRESULT UI::addDialog(LPCSTR IDName)
{
	Dialogs.push_back(make_unique<dialogs>(IDName, true, true, true, false, true, false, false, 0));
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
		Dialogs.back()->setComponent(make_unique<Buttons>(IDName, true));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
				Dialogs.at(i)->setComponent(make_unique<Buttons>(IDName, true));
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
		Dialogs.back()->setComponent(make_unique<Labels>(IDName, true));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
				Dialogs.at(i)->setComponent(make_unique<Labels>(IDName, true));
		}
	return S_OK;
}
HRESULT UI::addCollapseHead(LPCSTR IDName, LPCSTR IDDialog, bool SelDef, bool Collapse)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	if (!IDDialog)
		Dialogs.back()->setComponent(make_unique<CollapsingHeaders>(IDName, SelDef, Collapse));
	else
		for (int i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
				Dialogs.at(i)->setComponent(make_unique<CollapsingHeaders>(IDName, SelDef));
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
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->setComponent(Component);
		}
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
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
			Dialogs.at(i)->setComponent(Component);
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
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->setComponent(Component);
		}
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
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->setComponent(Component);
		}
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
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->setComponent(Component);
		}
	}
	return S_OK;
}
HRESULT UI::addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<TextList> Component)
{
	if (Dialogs.empty())
	{
		DebugTrace("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		throw exception("UI->addCollapseHead()::Dialogs->empty() == empty!!!");
		return E_FAIL;
	}

	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->setComponent(Component);
		}
	}
	return S_OK;
}

void UI::DisableDialog(LPCSTR IDDialog)
{
	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
			Dialogs.at(i)->setVisible(false);
	}
}
void UI::EnableDialog(LPCSTR IDDialog)
{
	for (int i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
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
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
			return Dialogs.at(i);
	}

	return make_unique<dialogs>();
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
	ImGuiIO &io = ImGui::GetIO();

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
	if (!ImGui::GetCurrentContext())
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

		if (!ImGui::IsAnyMouseDown() && !::GetCapture())
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

void CollapsingHeaders::Render()
{
	Flags = 0;
	Flags = ImGuiTreeNodeFlags_CollapsingHeader;

	if (SelDef)
		Flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::CollapsingHeader(IDTitle.c_str(), &IsCollapse, Flags))
	{
		int Count = OrderlyRender, now = 0;

		while (Count != now)
		{
			for (int i = 0; i < Label.size(); i++)
			{
				if (Label.at(i)->GetVisible() && Label.at(i)->getRenderOrder() == now)
					Label.at(i)->Render();
			}

			for (int i = 0; i < Btn.size(); i++)
			{
				if (Btn.at(i)->GetVisible() && Btn.at(i)->getRenderOrder() == now)
					Btn.at(i)->Render();
			}

			for (int i = 0; i < Itextmul.size(); i++)
			{
				if (Itextmul.at(i)->GetVisible() && Itextmul.at(i)->getRenderOrder() == now)
					Itextmul.at(i)->Render();
			}

			for (int i = 0; i < Itext.size(); i++)
			{
				if (Itext.at(i)->GetVisible() && Itext.at(i)->getRenderOrder() == now)
					Itext.at(i)->Render();
			}

			for (int i = 0; i < separators.size(); i++)
			{
				if (separators.at(i)->getRenderOrder() == now)
					separators.at(i)->Render();
			}

			for (int i = 0; i < childs.size(); i++)
			{
				if (childs.at(i)->getCountOrderRenderInDial() == now)
					childs.at(i)->Render();
			}

			for (int i = 0; i < CollpsHeader.size(); i++)
			{
				if (CollpsHeader.at(i)->Collapse() && CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					CollpsHeader.at(i)->Render();
			}

			for (int i = 0; i < UText.size(); i++)
			{
				if (UText.at(i)->getRenderOrder() == now)
					UText.at(i)->Render();
			}

			for (int i = 0; i < TList.size(); i++)
			{
				if (TList.at(i)->GetVisible() && TList.at(i)->getRenderOrder() == now)
					TList.at(i)->Render();
			}
			
			now++;
		}
	}
}

void dialogs::Render()
{
	window_flags = 0;

	if (!ShowTitle)
		window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (!IsMoveble)
		window_flags |= ImGuiWindowFlags_NoMove;
	if (!IsResizeble)
		window_flags |= ImGuiWindowFlags_NoResize;
	if (!IsCollapsible)
		window_flags |= ImGuiWindowFlags_NoCollapse;
		//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	if (IsVisible)
	{
		Begin(IDTitle.c_str(), &IsVisible, window_flags);

		if (IsNeedBringToFont)
			ImGui::BringWindowToFocusFront(GetCurrentWindow());

		if (SizeW != SizeW_Last || SizeH != SizeH_Last)
		{
			ImGui::SetWindowSize(GetCurrentWindow(), ImVec2(SizeW, SizeH), ImGuiCond_::ImGuiCond_Always);
			SizeW_Last = SizeW;
			SizeH_Last = SizeH;
		}

		int Count = this->getOrderCount(), now = 0;

		while (Count != now)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));

			for (int i = 0; i < CollpsHeader.size(); i++)
			{
				if (CollpsHeader.at(i)->Collapse() && CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					CollpsHeader.at(i)->Render();
			}

			for (int i = 0; i < Label.size(); i++)
			{
				if (Label.at(i)->GetVisible() && Label.at(i)->getRenderOrder() == now)
					Label.at(i)->Render();
			}

			for (int i = 0; i < Itext.size(); i++)
			{
				if (Itext.at(i)->GetVisible() && Itext.at(i)->getRenderOrder() == now)
					Itext.at(i)->Render();
			}

			for (int i = 0; i < separator.size(); i++)
			{
				if (separator.at(i)->getRenderOrder() == now)
					separator.at(i)->Render();
			}

			for (int i = 0; i < Itextmul.size(); i++)
			{
				if (Itextmul.at(i)->GetVisible() && Itextmul.at(i)->getRenderOrder() == now)
					Itextmul.at(i)->Render();
			}

			for (int i = 0; i < Btn.size(); i++)
			{
				if (Btn.at(i)->GetVisible() && Btn.at(i)->getRenderOrder() == now)
					Btn.at(i)->Render();
			}

			for (int i = 0; i < child.size(); i++)
			{
				if (child.at(i)->getCountOrderRenderInDial() == now)
					child.at(i)->Render();
			}

			for (int i = 0; i < UText.size(); i++)
			{
				if (UText.at(i)->getRenderOrder() == now)
					UText.at(i)->Render();
			}

			for (int i = 0; i < TList.size(); i++)
			{
				if (TList.at(i)->GetVisible() && TList.at(i)->getRenderOrder() == now)
					TList.at(i)->Render();
			}

			now++;
			ImGui::PopStyleVar();
		}

		End();
	}
}
