#include "pch.h"

#include "UI.h"

#include "Actor.h"
#include "Models.h"
#include "Audio.h"
static shared_ptr<Render_Buffer> Buf = make_shared<Render_Buffer>();

HRESULT UI::Init()
{
	try
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO &io = Get_IO();
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

	ToDo("Change This!!!");
	if (!Application->getUI()->getDialogs().empty())
	{
		Application->getUI()->getDialog("Main")->ChangePosition(10.f, Application->getWorkAreaSize(Application->GetHWND()).y - 10.f,
			ImVec2(0.f, 1.f));
		Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
		Application->getUI()->getDialog("Console")->ChangeSize(Application->getWorkAreaSize(Application->GetHWND()).x,
			Application->getWorkAreaSize(Application->GetHWND()).y / 3);
	}
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

void UI::Destroy()
{
	Buf->Release();
	ImGui::DestroyContext();
}

HRESULT UI::LoadXmlUI(string File)
{
	doc = make_shared<tinyxml2::XMLDocument>();

	doc->LoadFile(File.c_str());
	if (doc->ErrorID() > 0)
	{
		Engine::StackTrace(doc->ErrorStr());
		throw exception((boost::format(("UI->LoatXmlUI()::doc->LoadFile() == %d!!!")) % doc->ErrorID()).str().c_str());
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

void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<Buttons> &btn,
	int &CountOrder)
{
	GetParam(element, btn);
	btn->ChangeOrder(CountOrder);
	dialog->setComponent(btn);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<Labels> &Label,
	int &CountOrder)
{
	GetParam(element, Label);
	Label->ChangeOrder(CountOrder);
	dialog->setComponent(Label);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<IText> &Itext,
	int &CountOrder)
{
	GetParam(element, Itext);
	Itext->ChangeOrder(CountOrder);
	dialog->setComponent(Itext);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<ITextMulti> &ItextMul,
	int &CountOrder)
{
	GetParam(element, ItextMul);
	ItextMul->ChangeOrder(CountOrder);
	dialog->setComponent(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<TextList> &TList,
	int &CountOrder)
{
	GetParam(element, TList);
	TList->ChangeOrder(CountOrder);
	dialog->setComponent(TList);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLNode *Nods, shared_ptr<XMLComponents> XMLTab,
	shared_ptr<Tab> &tab, int &CountOrder)
{
	GetParam(Nods, tab);

	//	Button
	for (size_t i = 0; i < XMLTab->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTab->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTab->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTab->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLCHead.at(i), XMLTab, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTab->labels.size(); i++)
	{
		CountOrder++;
		shared_ptr<Labels> label = make_shared<Labels>();
		WorkOnComponents(tab, XMLTab->labels.at(i)->ToElement(), label, CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTab->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTab->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLDChild.at(i), XMLTab, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTab->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		tab->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTab->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_Tab.at(i), XMLTab, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTab->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_TreeNode.at(i), XMLTab, make_shared<TreeNode>(), CountOrder);
	}

	tab->ChangeOrder(CountOrder);
	dialog->setComponent(tab);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLNode *Nods, shared_ptr<XMLComponents> XMLTNode,
	shared_ptr<TreeNode> &TNode, int &CountOrder)
{
	GetParam(Nods, TNode);

	//	Button
	for (size_t i = 0; i < XMLTNode->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTNode->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTNode->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTNode->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLCHead.at(i), XMLTNode, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTNode->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTNode->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTNode->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLDChild.at(i), XMLTNode, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTNode->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		TNode->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTNode->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_Tab.at(i), XMLTNode, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTNode->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_TreeNode.at(i), XMLTNode, make_shared<TreeNode>(), CountOrder);
	}

	TNode->ChangeOrder(CountOrder);
	dialog->setComponent(TNode);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLNode *Nods, shared_ptr<XMLComponents> XMLchild,
	shared_ptr<Child> &child, int &CountOrder)
{
	GetParam(Nods, child);

	//	Button
	for (size_t i = 0; i < XMLchild->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLchild->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLchild->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLchild->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLCHead.at(i), XMLchild, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLchild->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLchild->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLchild->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLDChild.at(i), XMLchild, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLchild->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		child->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLchild->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_Tab.at(i), XMLchild, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLchild->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_TreeNode.at(i), XMLchild, make_shared<TreeNode>(), CountOrder);
	}

	child->ChangeOrder(CountOrder);
	dialog->setComponent(child);
}
void UI::WorkOnComponents(shared_ptr<dialogs> &dialog, XMLNode *Nods, shared_ptr<XMLComponents> XMLCHeader,
	shared_ptr<CollapsingHeaders> &CHeader, int &CountOrder)
{
	GetParam(Nods, CHeader);

	//	Button
	for (size_t i = 0; i < XMLCHeader->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLCHeader->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLCHeader->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLCHeader->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLCHead.at(i), XMLCHeader, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLCHeader->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLCHeader->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLCHeader->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLDChild.at(i), XMLCHeader, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLCHeader->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		CHeader->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLCHeader->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_Tab.at(i), XMLCHeader, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLCHeader->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_TreeNode.at(i), XMLCHeader, make_shared<TreeNode>(), CountOrder);
	}

	CHeader->ChangeOrder(CountOrder);
	dialog->setComponent(CHeader);
}

void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<Buttons> &btn,
	int &CountOrder)
{
	GetParam(element, btn);
	btn->ChangeOrder(CountOrder);
	InChild->getComponent()->Btn.push_back(btn);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<Labels> &Label,
	int &CountOrder)
{
	GetParam(element, Label);
	Label->ChangeOrder(CountOrder);
	InChild->getComponent()->Label.push_back(Label);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<IText> &Itext,
	int &CountOrder)
{
	GetParam(element, Itext);
	Itext->ChangeOrder(CountOrder);
	InChild->getComponent()->Itext.push_back(Itext);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<ITextMulti> &ItextMul,
	int &CountOrder)
{
	GetParam(element, ItextMul);
	ItextMul->ChangeOrder(CountOrder);
	InChild->getComponent()->Itextmul.push_back(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<TextList> &TList,
	int &CountOrder)
{
	GetParam(element, TList);
	TList->ChangeOrder(CountOrder);
	InChild->getComponent()->TList.push_back(TList);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLNode *Nods, shared_ptr<XMLComponents> XMLTab,
	shared_ptr<Tab> &tab, int &CountOrder)
{
	GetParam(Nods, tab);

	//	Button
	for (size_t i = 0; i < XMLTab->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTab->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTab->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTab->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLCHead.at(i), XMLTab, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTab->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTab->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTab->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLDChild.at(i), XMLTab, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTab->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		tab->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTab->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_Tab.at(i), XMLTab, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTab->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_TreeNode.at(i), XMLTab, make_shared<TreeNode>(), CountOrder);
	}

	tab->ChangeOrder(CountOrder);
	InChild->getComponent()->Tabs.push_back(tab);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLNode *Nods, shared_ptr<XMLComponents> XMLTNode,
	shared_ptr<TreeNode> &TNode, int &CountOrder)
{
	GetParam(Nods, TNode);

	//	Button
	for (size_t i = 0; i < XMLTNode->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTNode->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTNode->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTNode->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLCHead.at(i), XMLTNode, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTNode->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTNode->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTNode->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLDChild.at(i), XMLTNode, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTNode->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		TNode->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTNode->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_Tab.at(i), XMLTNode, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTNode->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_TreeNode.at(i), XMLTNode, make_shared<TreeNode>(), CountOrder);
	}

	TNode->ChangeOrder(CountOrder);
	InChild->getComponent()->TNode.push_back(TNode);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLNode *Nods, shared_ptr<XMLComponents> XMLchild,
	shared_ptr<Child> &child, int &CountOrder)
{
	GetParam(Nods, child);

	//	Button
	for (size_t i = 0; i < XMLchild->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLchild->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLchild->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLchild->XMLCHead.size(); i++)
	{
		WorkOnComponents(child, XMLchild->XMLCHead.at(i), XMLchild, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLchild->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLchild->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLchild->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLDChild.at(i), XMLchild, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLchild->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		child->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLchild->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_Tab.at(i), XMLchild, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLchild->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_TreeNode.at(i), XMLchild, make_shared<TreeNode>(), CountOrder);
	}

	child->ChangeOrder(CountOrder);
	InChild->getComponent()->childs.push_back(child);
}
void UI::WorkOnComponents(shared_ptr<Child> &InChild, XMLNode *Nods, shared_ptr<XMLComponents> XMLCHeader,
	shared_ptr<CollapsingHeaders> &CHeader, int &CountOrder)
{
	GetParam(Nods, CHeader);

	//	Button
	for (size_t i = 0; i < XMLCHeader->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLCHeader->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLCHeader->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLCHeader->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLCHead.at(i), XMLCHeader, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLCHeader->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLCHeader->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLCHeader->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLDChild.at(i), XMLCHeader, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLCHeader->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		CHeader->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLCHeader->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_Tab.at(i), XMLCHeader, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLCHeader->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_TreeNode.at(i), XMLCHeader, make_shared<TreeNode>(), CountOrder);
	}

	CHeader->ChangeOrder(CountOrder);
	InChild->getComponent()->CollpsHeader.push_back(CHeader);
}

void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<Buttons> &btn,
	int &CountOrder)
{
	GetParam(element, btn);
	btn->ChangeOrder(CountOrder);
	InCollaps->getComponent()->Btn.push_back(btn);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<Labels> &Label,
	int &CountOrder)
{
	GetParam(element, Label);
	Label->ChangeOrder(CountOrder);
	InCollaps->getComponent()->Label.push_back(Label);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<IText> &Itext,
	int &CountOrder)
{
	GetParam(element, Itext);
	Itext->ChangeOrder(CountOrder);
	InCollaps->getComponent()->Itext.push_back(Itext);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element,
	shared_ptr<ITextMulti> &ItextMul, int &CountOrder)
{
	GetParam(element, ItextMul);
	ItextMul->ChangeOrder(CountOrder);
	InCollaps->getComponent()->Itextmul.push_back(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<TextList> &TList,
	int &CountOrder)
{
	GetParam(element, TList);
	TList->ChangeOrder(CountOrder);
	InCollaps->getComponent()->TList.push_back(TList);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLNode *Nods, shared_ptr<XMLComponents> XMLTab,
	shared_ptr<Tab> &tab, int &CountOrder)
{
	GetParam(Nods, tab);

	//	Button
	for (size_t i = 0; i < XMLTab->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTab->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTab->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTab->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLCHead.at(i), XMLTab, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTab->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTab->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTab->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLDChild.at(i), XMLTab, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTab->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		tab->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTab->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_Tab.at(i), XMLTab, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTab->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_TreeNode.at(i), XMLTab, make_shared<TreeNode>(), CountOrder);
	}

	tab->ChangeOrder(CountOrder);
	InCollaps->getComponent()->Tabs.push_back(tab);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLNode *Nods, shared_ptr<XMLComponents> XMLTNode,
	shared_ptr<TreeNode> &TNode, int &CountOrder)
{
	GetParam(Nods, TNode);

	//	Button
	for (size_t i = 0; i < XMLTNode->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTNode->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTNode->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTNode->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLCHead.at(i), XMLTNode, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTNode->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTNode->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTNode->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLCHead.at(i), XMLTNode, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTNode->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		TNode->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTNode->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_Tab.at(i), XMLTNode, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTNode->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_TreeNode.at(i), XMLTNode, make_shared<TreeNode>(), CountOrder);
	}

	TNode->ChangeOrder(CountOrder);
	InCollaps->getComponent()->TNode.push_back(TNode);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLNode *Nods, shared_ptr<XMLComponents> XMLchild,
	shared_ptr<Child> &child, int &CountOrder)
{
	GetParam(Nods, child);

	//	Button
	for (size_t i = 0; i < XMLchild->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLchild->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLchild->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLchild->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLCHead.at(i), XMLchild, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLchild->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLchild->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLchild->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLDChild.at(i), XMLchild, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLchild->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		child->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLchild->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_Tab.at(i), XMLchild, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLchild->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_TreeNode.at(i), XMLchild, make_shared<TreeNode>(), CountOrder);
	}

	child->ChangeOrder(CountOrder);
	InCollaps->getComponent()->childs.push_back(child);
}
void UI::WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLNode *Nods, shared_ptr<XMLComponents> XMLCHeader,
	shared_ptr<CollapsingHeaders> &CHeader, int &CountOrder)
{
	GetParam(Nods, CHeader);

	//	Button
	for (size_t i = 0; i < XMLCHeader->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLCHeader->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLCHeader->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLCHeader->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLCHead.at(i), XMLCHeader->CpsHead.back(),
			make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLCHeader->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLCHeader->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLCHeader->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLCHead.at(i), XMLCHeader->DialChild.back(), make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLCHeader->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		CHeader->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLCHeader->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_Tab.at(i), XMLCHeader->_Tab.back(), make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLCHeader->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_TreeNode.at(i), XMLCHeader->_TreeNode.back(),
			make_shared<TreeNode>(), CountOrder);
	}

	CHeader->ChangeOrder(CountOrder);
	InCollaps->getComponent()->CollpsHeader.push_back(CHeader);
}

void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLElement *element, shared_ptr<Buttons> &btn,
	int &CountOrder)
{
	GetParam(element, btn);
	btn->ChangeOrder(CountOrder);
	InTab->getComponent()->Btn.push_back(btn);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLElement *element, shared_ptr<Labels> &Label,
	int &CountOrder)
{
	GetParam(element, Label);
	Label->ChangeOrder(CountOrder);
	InTab->getComponent()->Label.push_back(Label);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLElement *element, shared_ptr<IText> &Itext,
	int &CountOrder)
{
	GetParam(element, Itext);
	Itext->ChangeOrder(CountOrder);
	InTab->getComponent()->Itext.push_back(Itext);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLElement *element, shared_ptr<ITextMulti> &ItextMul,
	int &CountOrder)
{
	GetParam(element, ItextMul);
	ItextMul->ChangeOrder(CountOrder);
	InTab->getComponent()->Itextmul.push_back(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLElement *element, shared_ptr<TextList> &TList,
	int &CountOrder)
{
	GetParam(element, TList);
	TList->ChangeOrder(CountOrder);
	InTab->getComponent()->TList.push_back(TList);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLNode *Nods, shared_ptr<XMLComponents> XMLTab,
	shared_ptr<Tab> &tab, int &CountOrder)
{
	GetParam(Nods, tab);

	//	Button
	for (size_t i = 0; i < XMLTab->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTab->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTab->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTab->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLCHead.at(i), XMLTab, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTab->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTab->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTab->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLDChild.at(i), XMLTab, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTab->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		tab->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTab->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_Tab.at(i), XMLTab, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTab->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_TreeNode.at(i), XMLTab, make_shared<TreeNode>(), CountOrder);
	}

	tab->ChangeOrder(CountOrder);
	InTab->getComponent()->Tabs.push_back(tab);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLNode *Nods, shared_ptr<XMLComponents> XMLTNode,
	shared_ptr<TreeNode> &TNode, int &CountOrder)
{
	GetParam(Nods, TNode);

	//	Button
	for (size_t i = 0; i < XMLTNode->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTNode->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTNode->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTNode->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLCHead.at(i), XMLTNode, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTNode->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTNode->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTNode->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLDChild.at(i), XMLTNode, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTNode->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		TNode->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTNode->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_Tab.at(i), XMLTNode, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTNode->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_TreeNode.at(i), XMLTNode, make_shared<TreeNode>(), CountOrder);
	}

	TNode->ChangeOrder(CountOrder);
	InTab->getComponent()->TNode.push_back(TNode);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLNode *Nods, shared_ptr<XMLComponents> XMLchild,
	shared_ptr<Child> &child, int &CountOrder)
{
	GetParam(Nods, child);

	//	Button
	for (size_t i = 0; i < XMLchild->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLchild->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLchild->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLchild->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLCHead.at(i), XMLchild, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLchild->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLchild->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLchild->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLDChild.at(i), XMLchild, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLchild->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		child->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLchild->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_Tab.at(i), XMLchild, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLchild->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_TreeNode.at(i), XMLchild, make_shared<TreeNode>(), CountOrder);
	}

	child->ChangeOrder(CountOrder);
	InTab->getComponent()->childs.push_back(child);
}
void UI::WorkOnComponents(shared_ptr<Tab> &InTab, XMLNode *Nods, shared_ptr<XMLComponents> XMLCHeader,
	shared_ptr<CollapsingHeaders> &CHeader, int &CountOrder)
{
	GetParam(Nods, CHeader);

	//	Button
	for (size_t i = 0; i < XMLCHeader->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLCHeader->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLCHeader->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLCHeader->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLCHead.at(i), XMLCHeader, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLCHeader->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLCHeader->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLCHeader->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLDChild.at(i), XMLCHeader, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLCHeader->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		CHeader->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLCHeader->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_Tab.at(i), XMLCHeader, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLCHeader->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_TreeNode.at(i), XMLCHeader, make_shared<TreeNode>(), CountOrder);
	}

	CHeader->ChangeOrder(CountOrder);
	InTab->getComponent()->CollpsHeader.push_back(CHeader);
}

void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLElement *element, shared_ptr<Buttons> &btn,
	int &CountOrder)
{
	GetParam(element, btn);
	btn->ChangeOrder(CountOrder);
	InTNode->getComponent()->Btn.push_back(btn);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLElement *element, shared_ptr<Labels> &Label,
	int &CountOrder)
{
	GetParam(element, Label);
	Label->ChangeOrder(CountOrder);
	InTNode->getComponent()->Label.push_back(Label);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLElement *element, shared_ptr<IText> &Itext,
	int &CountOrder)
{
	GetParam(element, Itext);
	Itext->ChangeOrder(CountOrder);
	InTNode->getComponent()->Itext.push_back(Itext);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLElement *element, shared_ptr<ITextMulti> &ItextMul,
	int &CountOrder)
{
	GetParam(element, ItextMul);
	ItextMul->ChangeOrder(CountOrder);
	InTNode->getComponent()->Itextmul.push_back(ItextMul);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLElement *element, shared_ptr<TextList> &TList,
	int &CountOrder)
{
	GetParam(element, TList);
	TList->ChangeOrder(CountOrder);
	InTNode->getComponent()->TList.push_back(TList);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLNode *Nods, shared_ptr<XMLComponents> XMLTab,
	shared_ptr<Tab> &tab, int &CountOrder)
{
	GetParam(Nods, tab);

	//	Button
	for (size_t i = 0; i < XMLTab->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTab->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTab->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTab->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLCHead.at(i), XMLTab, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTab->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTab->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTab->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XMLDChild.at(i), XMLTab, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTab->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		tab->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTab->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_Tab.at(i), XMLTab, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTab->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(tab, XMLTab->XML_TreeNode.at(i), XMLTab, make_shared<TreeNode>(), CountOrder);
	}

	tab->ChangeOrder(CountOrder);
	InTNode->getComponent()->Tabs.push_back(tab);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLNode *Nods, shared_ptr<XMLComponents> XMLTNode,
	shared_ptr<TreeNode> &TNode, int &CountOrder)
{
	GetParam(Nods, TNode);

	//	Button
	for (size_t i = 0; i < XMLTNode->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLTNode->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLTNode->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLTNode->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLCHead.at(i), XMLTNode, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLTNode->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLTNode->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLTNode->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XMLDChild.at(i), XMLTNode, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLTNode->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		TNode->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLTNode->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_Tab.at(i), XMLTNode, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLTNode->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(TNode, XMLTNode->XML_TreeNode.at(i), XMLTNode, make_shared<TreeNode>(), CountOrder);
	}

	TNode->ChangeOrder(CountOrder);
	InTNode->getComponent()->TNode.push_back(TNode);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLNode *Nods, shared_ptr<XMLComponents> XMLchild,
	shared_ptr<Child> &child, int &CountOrder)
{
	GetParam(Nods, child);

	//	Button
	for (size_t i = 0; i < XMLchild->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLchild->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLchild->textmuls.size(); i++)
	{
		WorkOnComponents(child, XMLchild->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLchild->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLCHead.at(i), XMLchild, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLchild->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLchild->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLchild->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XMLDChild.at(i), XMLchild, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLchild->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		child->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLchild->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_Tab.at(i), XMLchild, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLchild->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(child, XMLchild->XML_TreeNode.at(i), XMLchild, make_shared<TreeNode>(), CountOrder);
	}

	child->ChangeOrder(CountOrder);
	InTNode->getComponent()->childs.push_back(child);
}
void UI::WorkOnComponents(shared_ptr<TreeNode> &InTNode, XMLNode *Nods, shared_ptr<XMLComponents> XMLCHeader,
	shared_ptr<CollapsingHeaders> &CHeader, int &CountOrder)
{
	GetParam(Nods, CHeader);

	//	Button
	for (size_t i = 0; i < XMLCHeader->buttons.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->buttons.at(i)->ToElement(), make_shared<Buttons>(), CountOrder);
	}
	//	InputText
	for (size_t i = 0; i < XMLCHeader->texts.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->texts.at(i)->ToElement(), make_shared<IText>(), CountOrder);
	}
	//	InputTextMultiline
	for (size_t i = 0; i < XMLCHeader->textmuls.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->textmuls.at(i)->ToElement(), make_shared<ITextMulti>(), CountOrder);
	}
	//	Collapse
	for (size_t i = 0; i < XMLCHeader->XMLCHead.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLCHead.at(i), XMLCHeader, make_shared<CollapsingHeaders>(), CountOrder);
	}
	//	Label
	for (size_t i = 0; i < XMLCHeader->labels.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->labels.at(i)->ToElement(), make_shared<Labels>(), CountOrder);
	}
	//	Separator
	for (size_t i = 0; i < XMLCHeader->separators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(CountOrder);
	}
	//	ChildDialog
	for (size_t i = 0; i < XMLCHeader->XMLDChild.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XMLDChild.at(i), XMLCHeader, make_shared<Child>(), CountOrder);
	}
	//	UnformatedText
	for (size_t i = 0; i < XMLCHeader->utext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		UText->ChangeOrder(CountOrder);
		CHeader->getComponent()->UText.push_back(UText);
	}
	//	Tab
	for (size_t i = 0; i < XMLCHeader->XML_Tab.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_Tab.at(i), XMLCHeader, make_shared<Tab>(), CountOrder);
	}
	//	TreeNode
	for (size_t i = 0; i < XMLCHeader->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		WorkOnComponents(CHeader, XMLCHeader->XML_TreeNode.at(i), XMLCHeader, make_shared<TreeNode>(), CountOrder);
	}

	CHeader->ChangeOrder(CountOrder);
	InTNode->getComponent()->CollpsHeader.push_back(CHeader);
}

void UI::XMLPreparing(shared_ptr<dial> &InDial, vector<XMLNode *>::iterator everything, int &countComp)
{
	countComp++;

	if (strcmp((*everything)->Value(), "Button") == 0)
	{
		InDial->Components->IDbuttons.push_back(countComp);
		InDial->Components->buttons.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputText") == 0)
	{
		InDial->Components->IDtexts.push_back(countComp);
		InDial->Components->texts.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputTextMultiline") == 0)
	{
		InDial->Components->IDtextmuls.push_back(countComp);
		InDial->Components->textmuls.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Label") == 0)
	{
		InDial->Components->IDlabels.push_back(countComp);
		InDial->Components->labels.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Separator") == 0)
	{
		InDial->Components->IDseparators.push_back(countComp);
		InDial->Components->separators.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "UnformatedText") == 0)
	{
		InDial->Components->IDutext.push_back(countComp);
		InDial->Components->utext.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "TreeNode") == 0)
	{
		InDial->Components->XML_TreeNode.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Tab") == 0)
	{
		InDial->Components->XML_Tab.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ChildDialog") == 0)
	{
		InDial->Components->XMLDChild.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Collapse") == 0)
	{
		InDial->Components->XMLCHead.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ListBox") == 0)
	{
		InDial->Components->IDtlist.push_back(countComp);
		InDial->Components->tlist.push_back(*everything);
		return;
	}
}
void UI::XMLPreparingCollps(shared_ptr<dial> &InCHead, vector<XMLNode *>::iterator everything, int &countComp)
{
	if (InCHead->Components->CpsHead.empty())
		InCHead->Components->CpsHead.push_back(make_shared<XMLComponents>());

	countComp++;

	if (strcmp((*everything)->Value(), "Button") == 0)
	{
		InCHead->Components->CpsHead.back()->IDbuttons.push_back(countComp);
		InCHead->Components->CpsHead.back()->buttons.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputText") == 0)
	{
		InCHead->Components->CpsHead.back()->IDtexts.push_back(countComp);
		InCHead->Components->CpsHead.back()->texts.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputTextMultiline") == 0)
	{
		InCHead->Components->CpsHead.back()->IDtextmuls.push_back(countComp);
		InCHead->Components->CpsHead.back()->textmuls.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Label") == 0)
	{
		InCHead->Components->CpsHead.back()->IDlabels.push_back(countComp);
		InCHead->Components->CpsHead.back()->labels.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Separator") == 0)
	{
		InCHead->Components->CpsHead.back()->IDseparators.push_back(countComp);
		InCHead->Components->CpsHead.back()->separators.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "UnformatedText") == 0)
	{
		InCHead->Components->CpsHead.back()->IDutext.push_back(countComp);
		InCHead->Components->CpsHead.back()->utext.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "TreeNode") == 0)
	{
		InCHead->Components->CpsHead.back()->XML_TreeNode.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Tab") == 0)
	{
		InCHead->Components->CpsHead.back()->XML_Tab.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ChildDialog") == 0)
	{
		InCHead->Components->CpsHead.back()->XMLDChild.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Collapse") == 0)
	{
		InCHead->Components->CpsHead.back()->XMLCHead.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ListBox") == 0)
	{
		InCHead->Components->CpsHead.back()->IDtlist.push_back(countComp);
		InCHead->Components->CpsHead.back()->tlist.push_back(*everything);
		return;
	}
}
void UI::XMLPreparingChild(shared_ptr<dial> &InChild, vector<XMLNode *>::iterator everything, int &countComp)
{
	if (InChild->Components->DialChild.empty())
		InChild->Components->DialChild.push_back(make_shared<XMLComponents>());

	countComp++;

	if (strcmp((*everything)->Value(), "Button") == 0)
	{
		InChild->Components->DialChild.back()->IDbuttons.push_back(countComp);
		InChild->Components->DialChild.back()->buttons.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputText") == 0)
	{
		InChild->Components->DialChild.back()->IDtexts.push_back(countComp);
		InChild->Components->DialChild.back()->texts.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputTextMultiline") == 0)
	{
		InChild->Components->DialChild.back()->IDtextmuls.push_back(countComp);
		InChild->Components->DialChild.back()->textmuls.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Label") == 0)
	{
		InChild->Components->DialChild.back()->IDlabels.push_back(countComp);
		InChild->Components->DialChild.back()->labels.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Separator") == 0)
	{
		InChild->Components->DialChild.back()->IDseparators.push_back(countComp);
		InChild->Components->DialChild.back()->separators.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "UnformatedText") == 0)
	{
		InChild->Components->DialChild.back()->IDutext.push_back(countComp);
		InChild->Components->DialChild.back()->utext.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ListBox") == 0)
	{
		InChild->Components->DialChild.back()->IDtlist.push_back(countComp);
		InChild->Components->DialChild.back()->tlist.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "TreeNode") == 0)
	{
		InChild->Components->DialChild.back()->XML_TreeNode.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Tab") == 0)
	{
		InChild->Components->DialChild.back()->XML_Tab.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ChildDialog") == 0)
	{
		InChild->Components->DialChild.back()->XMLDChild.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Collapse") == 0)
	{
		InChild->Components->DialChild.back()->XMLCHead.push_back(*everything);
		return;
	}
}
void UI::XMLPreparingTNode(shared_ptr<dial> &InTNode, vector<XMLNode *>::iterator everything, int &countComp)
{
	if (InTNode->Components->_TreeNode.empty())
		InTNode->Components->_TreeNode.push_back(make_shared<XMLComponents>());

	countComp++;

	if (strcmp((*everything)->Value(), "Button") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDbuttons.push_back(countComp);
		InTNode->Components->_TreeNode.back()->buttons.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputText") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDtexts.push_back(countComp);
		InTNode->Components->_TreeNode.back()->texts.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputTextMultiline") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDtextmuls.push_back(countComp);
		InTNode->Components->_TreeNode.back()->textmuls.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Label") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDlabels.push_back(countComp);
		InTNode->Components->_TreeNode.back()->labels.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Separator") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDseparators.push_back(countComp);
		InTNode->Components->_TreeNode.back()->separators.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "UnformatedText") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDutext.push_back(countComp);
		InTNode->Components->_TreeNode.back()->utext.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "TreeNode") == 0)
	{
		InTNode->Components->_TreeNode.back()->XML_TreeNode.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Tab") == 0)
	{
		InTNode->Components->_TreeNode.back()->XML_Tab.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ChildDialog") == 0)
	{
		InTNode->Components->_TreeNode.back()->XMLDChild.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Collapse") == 0)
	{
		InTNode->Components->_TreeNode.back()->XMLCHead.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ListBox") == 0)
	{
		InTNode->Components->_TreeNode.back()->IDtlist.push_back(countComp);
		InTNode->Components->_TreeNode.back()->tlist.push_back(*everything);
		return;
	}
}
void UI::XMLPreparingTab(shared_ptr<dial> &InTab, vector<XMLNode *>::iterator everything, int &countComp)
{
	if (InTab->Components->_Tab.empty())
		InTab->Components->_Tab.push_back(make_shared<XMLComponents>());

	countComp++;

	if (strcmp((*everything)->Value(), "Button") == 0)
	{
		InTab->Components->_Tab.back()->IDbuttons.push_back(countComp);
		InTab->Components->_Tab.back()->buttons.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputText") == 0)
	{
		InTab->Components->_Tab.back()->IDtexts.push_back(countComp);
		InTab->Components->_Tab.back()->texts.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputTextMultiline") == 0)
	{
		InTab->Components->_Tab.back()->IDtextmuls.push_back(countComp);
		InTab->Components->_Tab.back()->textmuls.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Label") == 0)
	{
		InTab->Components->_Tab.back()->IDlabels.push_back(countComp);
		InTab->Components->_Tab.back()->labels.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Separator") == 0)
	{
		InTab->Components->_Tab.back()->IDseparators.push_back(countComp);
		InTab->Components->_Tab.back()->separators.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "UnformatedText") == 0)
	{
		InTab->Components->_Tab.back()->IDutext.push_back(countComp);
		InTab->Components->_Tab.back()->utext.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "TreeNode") == 0)
	{
		InTab->Components->_Tab.back()->XML_TreeNode.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Tab") == 0)
	{
		InTab->Components->_Tab.back()->XML_Tab.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ChildDialog") == 0)
	{
		InTab->Components->_Tab.back()->XMLDChild.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Collapse") == 0)
	{
		InTab->Components->_Tab.back()->XMLCHead.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ListBox") == 0)
	{
		InTab->Components->_Tab.back()->IDtlist.push_back(countComp);
		InTab->Components->_Tab.back()->tlist.push_back(*everything);
		return;
	}
}

void UI::XMLPreparingRecursion(shared_ptr<XMLComponents> &InCHead, vector<XMLNode *>::iterator everything,
	int &countComp)
{
	countComp++;

	if (strcmp((*everything)->Value(), "Button") == 0)
	{
		InCHead->IDbuttons.push_back(countComp);
		InCHead->buttons.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputText") == 0)
	{
		InCHead->IDtexts.push_back(countComp);
		InCHead->texts.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "InputTextMultiline") == 0)
	{
		InCHead->IDtextmuls.push_back(countComp);
		InCHead->textmuls.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Label") == 0)
	{
		InCHead->IDlabels.push_back(countComp);
		InCHead->labels.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Separator") == 0)
	{
		InCHead->IDseparators.push_back(countComp);
		InCHead->separators.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "UnformatedText") == 0)
	{
		InCHead->IDutext.push_back(countComp);
		InCHead->utext.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "TreeNode") == 0)
	{
		InCHead->XML_TreeNode.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Tab") == 0)
	{
		InCHead->XML_Tab.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ChildDialog") == 0)
	{
		InCHead->XMLDChild.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "Collapse") == 0)
	{
		InCHead->XMLCHead.push_back(*everything);
		return;
	}
	if (strcmp((*everything)->Value(), "ListBox") == 0)
	{
		InCHead->IDtlist.push_back(countComp);
		InCHead->tlist.push_back(*everything);
		return;
	}
}

void UI::GetParam(XMLNode *Nods, shared_ptr<Child>& InChild)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		if (strcmp(FirstAttr->Name(), "id") == 0)
		{
			InChild->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "border") == 0)
		{
			InChild->setBorder(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "horizontal_scroll") == 0)
		{
			InChild->setHScroll(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<Tab> &InTab)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		if (strcmp(FirstAttr->Name(), "dragging_tabs") == 0)
		{
			InTab->setDragTabs(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			InTab->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "auto_select_new_tab") == 0)
		{
			InTab->setASelectNewTab(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "no_close_midle_mouse") == 0)
		{
			InTab->setCloseMidMouse(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<CollapsingHeaders> &InCollaps)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		if (strcmp(FirstAttr->Name(), "seldefault") == 0)
		{
			InCollaps->setSelDefault(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			InCollaps->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (strcmp(FirstAttr->Name(), "collapsible") == 0)
		{
			InCollaps->setCollapse(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<TreeNode> &InTNode)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		if (strcmp(FirstAttr->Name(), "text") == 0)
		{
			InTNode->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}

void UI::GetParam(XMLElement *Nods, shared_ptr<Buttons>& btn)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
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
}
void UI::GetParam(XMLElement *Nods, shared_ptr<TextList> &TList)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
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
}
void UI::GetParam(XMLElement *Nods, shared_ptr<Labels> &Label)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
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
}
void UI::GetParam(XMLElement *Nods, shared_ptr<IText> &Itext)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
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
}
void UI::GetParam(XMLElement *Nods, shared_ptr<ITextMulti> &ItextMul)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
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
}

void UI::GetForRecursion(vector<XMLNode *> SomeComponents, int &countComponents,
	shared_ptr<XMLComponents> &SomeComponent)
{
	for (auto Two = SomeComponents.begin(); Two != SomeComponents.end(); ++Two)
	{
		int i = 0;
		while (!(*Two)->NoChildren())
		{
			if (strcmp((*Two)->Value(), "Tab") == 0)
			{
				SomeComponent->_Tab.push_back(make_shared<XMLComponents>());
				SomeComponent->XML_Tab.push_back(*Two);

				vector<XMLNode *> TheSecondComponent; // Look for The Components From Under-Component
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*Two)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three = TheSecondComponent.begin(); Three != TheSecondComponent.end(); ++Three)
				{
					if (!(*Three)->NoChildren())
						GetForRecursion(TheSecondComponent, SomeComponent->_Tab.back()->OrderlyRender,
							SomeComponent->_Tab.back());
					else
						XMLPreparingRecursion(SomeComponent->_Tab.back(), Three, SomeComponent->OrderlyRender);
				}
			}
			else if (strcmp((*Two)->Value(), "TreeNode") == 0)
			{
				SomeComponent->_TreeNode.push_back(make_shared<XMLComponents>());
				SomeComponent->XML_TreeNode.push_back(*Two);

				vector<XMLNode *> TheSecondComponent; // Look for The Components From Under-Component
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*Two)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three = TheSecondComponent.begin(); Three != TheSecondComponent.end(); ++Three)
				{
					if (!(*Three)->NoChildren())
						GetForRecursion(TheSecondComponent, SomeComponent->_TreeNode.back()->OrderlyRender,
							SomeComponent->_TreeNode.back());
					else
						XMLPreparingRecursion(SomeComponent->_TreeNode.back(), Three,
							SomeComponent->OrderlyRender);
				}
			}
			else if (strcmp((*Two)->Value(), "Collapse") == 0)
			{
				SomeComponent->CpsHead.push_back(make_shared<XMLComponents>());
				SomeComponent->XMLCHead.push_back(*Two);

				vector<XMLNode *> TheSecondComponent; // Look for The Components From Under-Component
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*Two)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three = TheSecondComponent.begin(); Three != TheSecondComponent.end(); ++Three)
				{
					if (!(*Three)->NoChildren())
						GetForRecursion(TheSecondComponent, SomeComponent->CpsHead.back()->OrderlyRender,
							SomeComponent->CpsHead.back());
					else
						XMLPreparingRecursion(SomeComponent->CpsHead.back(), Three, SomeComponent->OrderlyRender);
				}
			}
			else if (strcmp((*Two)->Value(), "ChildDialog") == 0)
			{
				SomeComponent->DialChild.push_back(make_shared<XMLComponents>());
				SomeComponent->XMLDChild.push_back(*Two);

				vector<XMLNode *> TheSecondComponent; // Look for The Components From Under-Component
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*Two)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three = TheSecondComponent.begin(); Three != TheSecondComponent.end(); ++Three)
				{
					if (!(*Three)->NoChildren())
						GetForRecursion(TheSecondComponent, SomeComponent->DialChild.back()->OrderlyRender,
							SomeComponent->DialChild.back());
					else
						XMLPreparingRecursion(SomeComponent->DialChild.back(), Three, SomeComponent->OrderlyRender);
				}
			}

			i++;
			break;
		}
	}
}
void UI::GetForRecursionForAddComponents(shared_ptr<dialogs> &RequiredComponent,
	shared_ptr<XMLComponents> &SomeComponent)
{
	//	Collapse
	for (size_t i = 0; i < SomeComponent->XMLCHead.size(); i++)
	{
		shared_ptr<CollapsingHeaders> CHeader = make_shared<CollapsingHeaders>();
		// Get Params In New Component!
		GetParam(SomeComponent->XMLCHead.at(i), CHeader);
		CHeader->ChangeOrder(SomeComponent->CpsHead.at(i)->OrderlyRender);
		CHeader->ChangeOrderInDial(SomeComponent->OrderlyRender);

		if (!SomeComponent->CpsHead.back()->XMLDChild.empty())
		{
			int Count = 0;
			shared_ptr<Child> child = make_shared<Child>();

			WorkOnComponents(CHeader, SomeComponent->CpsHead.at(i)->XMLDChild.at(i),
				SomeComponent->CpsHead.at(i)->DialChild.at(i), child, Count);

			child->ChangeOrder(Count);
			child->ChangeOrderInDial(SomeComponent->CpsHead.at(i)->DialChild.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->CpsHead.back()->XML_Tab.empty())
		{
			int Count = 0;
			shared_ptr<Tab> tab = make_shared<Tab>();

			WorkOnComponents(CHeader, SomeComponent->CpsHead.at(i)->XML_Tab.at(i),
				SomeComponent->CpsHead.at(i)->_Tab.at(i), tab, Count);

			tab->ChangeOrder(Count);
			tab->ChangeOrderInDial(SomeComponent->CpsHead.at(i)->_Tab.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->CpsHead.back()->XML_TreeNode.empty())
		{
			int Count = 0;
			shared_ptr<TreeNode> treenode = make_shared<TreeNode>();

			WorkOnComponents(CHeader, SomeComponent->CpsHead.at(i)->XML_TreeNode.at(i),
				SomeComponent->CpsHead.at(i)->_TreeNode.at(i), treenode, Count);

			treenode->ChangeOrder(Count);
			treenode->ChangeOrderInDial(SomeComponent->CpsHead.at(i)->_TreeNode.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->CpsHead.back()->XMLCHead.empty())
		{
			int Count = 0;
			shared_ptr<CollapsingHeaders> collps = make_shared<CollapsingHeaders>();

			WorkOnComponents(CHeader, SomeComponent->CpsHead.at(i)->XMLCHead.at(i),
				SomeComponent->CpsHead.at(i)->CpsHead.at(i), collps, Count);

			collps->ChangeOrder(Count);
			collps->ChangeOrderInDial(SomeComponent->CpsHead.at(i)->CpsHead.at(i)->OrderlyRender);
		}
		else
		{
			int Cache = 0;
			WorkOnComponents(RequiredComponent, SomeComponent->XMLCHead.at(i), SomeComponent->CpsHead.at(i),
				CHeader, Cache);
			continue;
		}
		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->setComponent(CHeader);
	}
	//	ChildDialog
	for (size_t i = 0; i < SomeComponent->XMLDChild.size(); i++)
	{
		shared_ptr<Child> child = make_shared<Child>();
		GetParam(SomeComponent->XMLDChild.at(i), child);
		child->ChangeOrder(SomeComponent->DialChild.at(i)->OrderlyRender);
		child->ChangeOrderInDial(SomeComponent->OrderlyRender);

		if (!SomeComponent->DialChild.back()->XMLDChild.empty())
		{
			int Count = 0;
			shared_ptr<Child> _Ñhild = make_shared<Child>();

			WorkOnComponents(child, SomeComponent->DialChild.at(i)->XMLDChild.at(i),
				SomeComponent->DialChild.at(i)->DialChild.at(i), _Ñhild, Count);

			_Ñhild->ChangeOrder(Count);
			_Ñhild->ChangeOrderInDial(SomeComponent->DialChild.at(i)->DialChild.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->DialChild.back()->XML_Tab.empty())
		{
			int Count = 0;
			shared_ptr<Tab> tab = make_shared<Tab>();

			WorkOnComponents(child, SomeComponent->DialChild.at(i)->XML_Tab.at(i),
				SomeComponent->DialChild.at(i)->_Tab.at(i), tab, Count);

			tab->ChangeOrder(Count);
			tab->ChangeOrderInDial(SomeComponent->DialChild.at(i)->_Tab.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->DialChild.back()->XML_TreeNode.empty())
		{
			int Count = 0;
			shared_ptr<TreeNode> treenode = make_shared<TreeNode>();

			WorkOnComponents(child, SomeComponent->DialChild.at(i)->XML_TreeNode.at(i),
				SomeComponent->DialChild.at(i)->_TreeNode.at(i), treenode, Count);

			treenode->ChangeOrder(Count);
			treenode->ChangeOrderInDial(SomeComponent->DialChild.at(i)->_TreeNode.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->DialChild.back()->XMLCHead.empty())
		{
			int Count = 0;
			shared_ptr<CollapsingHeaders> collps = make_shared<CollapsingHeaders>();

			WorkOnComponents(child, SomeComponent->DialChild.at(i)->XMLCHead.at(i),
				SomeComponent->DialChild.at(i)->CpsHead.at(i), collps, Count);

			collps->ChangeOrder(Count);
			collps->ChangeOrderInDial(SomeComponent->DialChild.at(i)->CpsHead.at(i)->OrderlyRender);
		}
		else
		{
			int Cache = 0;
			WorkOnComponents(RequiredComponent, SomeComponent->XMLDChild.at(i), SomeComponent->DialChild.at(i),
				child, Cache);
			continue;
		}
		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->setComponent(child);
	}
	// Tab
	for (size_t i = 0; i < SomeComponent->XML_Tab.size(); i++)
	{
		shared_ptr<Tab> tab = make_shared<Tab>();
		GetParam(SomeComponent->XML_Tab.at(i), tab);
		tab->ChangeOrder(SomeComponent->_Tab.at(i)->OrderlyRender);
		tab->ChangeOrderInDial(SomeComponent->OrderlyRender);

		if (!SomeComponent->_Tab.back()->XMLDChild.empty())
		{
			int Count = 0;
			shared_ptr<Child> child = make_shared<Child>();

			WorkOnComponents(tab, SomeComponent->_Tab.at(i)->XMLDChild.at(i),
				SomeComponent->_Tab.at(i)->DialChild.at(i), child, Count);

			child->ChangeOrder(Count);
			child->ChangeOrderInDial(SomeComponent->_Tab.at(i)->DialChild.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->_Tab.back()->XML_Tab.empty())
		{
			int Count = 0;
			// C2* mean = Component To
			shared_ptr<Tab> C2Tab = make_shared<Tab>();

			WorkOnComponents(tab, SomeComponent->_Tab.at(i)->XML_Tab.at(i),
				SomeComponent->_Tab.at(i)->_Tab.at(i), C2Tab, Count);

			C2Tab->ChangeOrder(Count);
			C2Tab->ChangeOrderInDial(SomeComponent->_Tab.at(i)->_Tab.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->_Tab.back()->XML_TreeNode.empty())
		{
			int Count = 0;
			shared_ptr<TreeNode> treenode = make_shared<TreeNode>();

			WorkOnComponents(tab, SomeComponent->DialChild.at(i)->XML_TreeNode.at(i),
				SomeComponent->DialChild.at(i)->_TreeNode.at(i), treenode, Count);

			treenode->ChangeOrder(Count);
			treenode->ChangeOrderInDial(SomeComponent->DialChild.at(i)->_TreeNode.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->_Tab.back()->XMLCHead.empty())
		{
			int Count = 0;
			shared_ptr<CollapsingHeaders> collps = make_shared<CollapsingHeaders>();

			WorkOnComponents(tab, SomeComponent->_Tab.at(i)->XMLCHead.at(i),
				SomeComponent->_Tab.at(i)->CpsHead.at(i), collps, Count);

			collps->ChangeOrder(Count);
			collps->ChangeOrderInDial(SomeComponent->_Tab.at(i)->CpsHead.at(i)->OrderlyRender);
		}
		else
		{
			int Cache = 0;
			WorkOnComponents(RequiredComponent, SomeComponent->XML_Tab.at(i), SomeComponent->_Tab.at(i),
				tab, Cache);
			continue;
		}
		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->setComponent(tab);
	}
	// TreeNode
	for (size_t i = 0; i < SomeComponent->XML_TreeNode.size(); i++)
	{
		shared_ptr<TreeNode> TNode = make_shared<TreeNode>();
		GetParam(SomeComponent->XML_TreeNode.at(i), TNode);
		TNode->ChangeOrder(SomeComponent->_TreeNode.at(i)->OrderlyRender);
		TNode->ChangeOrderInDial(SomeComponent->OrderlyRender);

		if (!SomeComponent->_TreeNode.back()->XMLDChild.empty())
		{
			int Count = 0;
			shared_ptr<Child> child = make_shared<Child>();

			WorkOnComponents(TNode, SomeComponent->_Tab.at(i)->XMLDChild.at(i),
				SomeComponent->_TreeNode.at(i)->DialChild.at(i), child, Count);

			child->ChangeOrder(Count);
			child->ChangeOrderInDial(SomeComponent->_TreeNode.at(i)->DialChild.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->_TreeNode.back()->XML_Tab.empty())
		{
			int Count = 0;
			// C2* mean = Component To
			shared_ptr<Tab> tab = make_shared<Tab>();

			WorkOnComponents(TNode, SomeComponent->_TreeNode.at(i)->XML_Tab.at(i),
				SomeComponent->_TreeNode.at(i)->_Tab.at(i), tab, Count);

			tab->ChangeOrder(Count);
			tab->ChangeOrderInDial(SomeComponent->_TreeNode.at(i)->_Tab.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->_TreeNode.back()->XML_TreeNode.empty())
		{
			int Count = 0;
			shared_ptr<TreeNode> treenode = make_shared<TreeNode>();

			WorkOnComponents(TNode, SomeComponent->_TreeNode.at(i)->XML_TreeNode.at(i),
				SomeComponent->_TreeNode.at(i)->_TreeNode.at(i), treenode, Count);

			treenode->ChangeOrder(Count);
			treenode->ChangeOrderInDial(SomeComponent->_TreeNode.at(i)->_TreeNode.at(i)->OrderlyRender);
		}
		else if (!SomeComponent->_TreeNode.back()->XMLCHead.empty())
		{
			int Count = 0;
			shared_ptr<CollapsingHeaders> collps = make_shared<CollapsingHeaders>();

			WorkOnComponents(TNode, SomeComponent->_TreeNode.at(i)->XMLCHead.at(i),
				SomeComponent->_TreeNode.at(i)->CpsHead.at(i), collps, Count);

			collps->ChangeOrder(Count);
			collps->ChangeOrderInDial(SomeComponent->_TreeNode.at(i)->CpsHead.at(i)->OrderlyRender);
		}
		else
		{
			int Cache = 0;
			WorkOnComponents(RequiredComponent, SomeComponent->XML_TreeNode.at(i), SomeComponent->_TreeNode.at(i),
				TNode, Cache);
			continue;
		}
		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->setComponent(TNode);
	}
}

void UI::ProcessXML()
{
	vector<shared_ptr<dialogs>> dialog;

		// Get All The Dialogs
	for (;;)
	{
		if (XMLDialogs.empty())
			XMLDialogs.push_back(make_shared<dial>(doc->RootElement()->Parent()->FirstChild()->NextSibling()));

		if (doc->RootElement()->Parent()->LastChild() != XMLDialogs.back()->Dial)
		{
			if (XMLDialogs.back()->Dial && XMLDialogs.back()->Dial->NextSibling())
				XMLDialogs.push_back(make_shared<dial>(XMLDialogs.back()->Dial->NextSibling()));
			else
				break;
		}
		else
			break;
	}
		// Starting Work On Dialogs
	int i = 0;
	for (auto It = XMLDialogs.begin(); It != XMLDialogs.end(); ++It)
	{
			// Get The Last Element In This Dialog!
		vector<XMLNode *> TheFirstComponent;
		for (;;)
		{
			if (TheFirstComponent.empty())
				TheFirstComponent.push_back((*It)->Dial->FirstChild());
			else if (TheFirstComponent.back()->NextSibling())
				TheFirstComponent.push_back(TheFirstComponent.back()->NextSibling());
			else
				break;
		}
			// Work With All The XML Nodes!
		for (auto First = TheFirstComponent.begin(); First != TheFirstComponent.end(); ++First)
		{
			int InCps = 0, InCld = 0, InTab = 0, InTNode = 0;

			if (strcmp((*First)->Value(), "Collapse") == 0)
			{
				InCps++;
				XMLDialogs.at(i)->Components->CpsHead.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XMLCHead.push_back(*First);

				vector<XMLNode *> TheSecondComponent;
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*First)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two = TheSecondComponent.begin(); Two != TheSecondComponent.end(); ++Two)
				{
					if (!(*Two)->NoChildren()) // If We Need Recursion!
						GetForRecursion(TheSecondComponent, XMLDialogs.at(i)->Components->CpsHead.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->CpsHead.back());
					else
						XMLPreparingCollps(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->CpsHead.back()->OrderlyRender);
				}
			}
			else if (strcmp((*First)->Value(), "ChildDialog") == 0)
			{
				InCld++;
				XMLDialogs.at(i)->Components->DialChild.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XMLDChild.push_back(*First);

				vector<XMLNode *> TheSecondComponent;
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*First)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two = TheSecondComponent.begin(); Two != TheSecondComponent.end(); ++Two)
				{
					if (!(*Two)->NoChildren())
						GetForRecursion(TheFirstComponent, XMLDialogs.at(i)->Components->DialChild.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->DialChild.back());
					else
						XMLPreparingChild(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->DialChild.back()->OrderlyRender);
				}
			}
			else if (strcmp((*First)->Value(), "TreeNode") == 0)
			{
				InTNode++;
				XMLDialogs.at(i)->Components->_TreeNode.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XML_TreeNode.push_back(*First);

				vector<XMLNode *> TheSecondComponent;
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*First)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two = TheSecondComponent.begin(); Two != TheSecondComponent.end(); ++Two)
				{
					if (!(*Two)->NoChildren()) // If We Need Recursion!
						GetForRecursion(TheFirstComponent, XMLDialogs.at(i)->Components->_TreeNode.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->_TreeNode.back());
					else
						XMLPreparingTNode(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->_TreeNode.back()->OrderlyRender);
				}
			}
			else if (strcmp((*First)->Value(), "Tab") == 0)
			{
				InTNode++;
				XMLDialogs.at(i)->Components->_Tab.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XML_Tab.push_back(*First);

				vector<XMLNode *> TheSecondComponent;
				for (;;)
				{
					if (TheSecondComponent.empty())
						TheSecondComponent.push_back((*First)->ToElement()->FirstChild());
					else if (TheSecondComponent.back()->NextSibling())
						TheSecondComponent.push_back(TheSecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two = TheSecondComponent.begin(); Two != TheSecondComponent.end(); ++Two)
				{
					if (!(*Two)->NoChildren())
						GetForRecursion(TheFirstComponent, XMLDialogs.at(i)->Components->_Tab.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->_Tab.back());
					else
						XMLPreparingTab(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->_Tab.back()->OrderlyRender);
				}
			}
				// *******
			else
				XMLPreparing(XMLDialogs.at(i), First, XMLDialogs.at(i)->Components->OrderlyRender);

			XMLDialogs.at(i)->Components->OrderlyRender += (InCps + InCld + InTab + InTNode);
		}
			// ID (Count) of XMLDialogs
		i++;
	}

	for (size_t IDDial = 0; IDDial < XMLDialogs.size(); IDDial++)
	{
		// ********
			// Dialog
		dialog.push_back(make_shared<dialogs>());
		float W = 0.f, H = 0.f;
		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(XMLDialogs.at(IDDial)->Dial->ToElement()->FirstAttribute());
		for (;;)
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

		// Collapse, Child, Tab, TreeNode, etc
		GetForRecursionForAddComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components);

		//	Button
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDbuttons.size(); i++)
		{
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components->buttons.at(i)->ToElement(),
				make_shared<Buttons>(), XMLDialogs.at(IDDial)->Components->IDbuttons.at(i));
		}
		//	InputText
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDtexts.size(); i++)
		{
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components->texts.at(i)->ToElement(),
				make_shared<IText>(), XMLDialogs.at(IDDial)->Components->IDtexts.at(i));
		}
		//	InputTextMultiline
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDtextmuls.size(); i++)
		{
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components->textmuls.at(i)->ToElement(),
				make_shared<ITextMulti>(), XMLDialogs.at(IDDial)->Components->IDtextmuls.at(i));
		}
		//	Label
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDlabels.size(); i++)
		{
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components->labels.at(i)->ToElement(),
				make_shared<Labels>(), XMLDialogs.at(IDDial)->Components->IDlabels.at(i));
		}
		//	Separator
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDseparators.size(); i++)
		{
			shared_ptr<_Separator> separator = make_shared<_Separator>();
			separator->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDseparators.at(i));
		}
		//	UnformatedText
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDutext.size(); i++)
		{
			shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
			UText->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDutext.at(i));
			dialog.at(IDDial)->setComponent(UText);
		}
		//	TextList
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDtlist.size(); i++)
		{
			WorkOnComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components->tlist.at(i)->ToElement(),
				make_shared<TextList>(), XMLDialogs.at(IDDial)->Components->IDtlist.at(i));
		}
		dialog.at(IDDial)->ChangeOrder(XMLDialogs.at(IDDial)->Components->OrderlyRender); // Count of all the components only in dialog!
		dialog.at(IDDial)->ChangeSize(W, H);
	}
	Dialogs = dialog;
}

void UI::ReloadXML(LPCSTR File)
{
	Dialogs.clear();
	XMLDialogs.clear();

	Buf->Release();
	LoadXmlUI(File);

	Buf->InitUI();
}

HRESULT UI::addDialog(LPCSTR IDName)
{
	Dialogs.push_back(make_shared<dialogs>(IDName, true, true, true, false, true, false, false, 0));
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
		Dialogs.back()->setComponent(make_shared<Buttons>(IDName, true));
	else
		for (size_t i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
				Dialogs.at(i)->setComponent(make_shared<Buttons>(IDName, true));
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
		Dialogs.back()->setComponent(make_shared<Labels>(IDName, true));
	else
		for (size_t i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
				Dialogs.at(i)->setComponent(make_shared<Labels>(IDName, true));
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
		Dialogs.back()->setComponent(make_shared<CollapsingHeaders>(IDName, SelDef, Collapse));
	else
		for (size_t i = 0; i < Dialogs.size(); i++)
		{
			if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
				Dialogs.at(i)->setComponent(make_shared<CollapsingHeaders>(IDName, SelDef));
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

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->getComponent()->Btn.push_back(Component);
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

	for (size_t i = 0; i < Dialogs.size(); i++)
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

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->getComponent()->Label.push_back(Component);
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

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->getComponent()->Itext.push_back(Component);
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

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->getComponent()->Itextmul.push_back(Component);
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

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
		{
			if (Dialogs.at(i)->getCollapsHeaders().empty())
				addCollapseHead(IDColpsHead);
			Dialogs.at(i)->getCollapsHeaders().back()->getComponent()->TList.push_back(Component);
		}
	}
	return S_OK;
}

void UI::DisableDialog(LPCSTR IDDialog)
{
	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
			Dialogs.at(i)->setVisible(false);
	}
}
void UI::EnableDialog(LPCSTR IDDialog)
{
	for (size_t i = 0; i < Dialogs.size(); i++)
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

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (strcmp(Dialogs.at(i)->GetTitle().c_str(), IDDialog) == 0)
			return Dialogs.at(i);
	}

	return make_shared<dialogs>();
}

// Update Mouse and Keyboard, Gamepad
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
		io.NavInputs[ImGuiNavInput_Activate] = (float)Application->getTracherGamepad().a;
		io.NavInputs[ImGuiNavInput_Cancel] = (float)Application->getTracherGamepad().b;
		io.NavInputs[ImGuiNavInput_Menu] = (float)Application->getTracherGamepad().x;
		io.NavInputs[ImGuiNavInput_Input] = (float)Application->getTracherGamepad().y;

		io.NavInputs[ImGuiNavInput_DpadLeft] = (float)Application->getTracherGamepad().dpadLeft;
		io.NavInputs[ImGuiNavInput_DpadRight] = (float)Application->getTracherGamepad().dpadRight;
		io.NavInputs[ImGuiNavInput_DpadUp] = (float)Application->getTracherGamepad().dpadUp;
		io.NavInputs[ImGuiNavInput_DpadDown] = (float)Application->getTracherGamepad().dpadDown;

		io.NavInputs[ImGuiNavInput_FocusPrev] = (float)Application->getTracherGamepad().leftShoulder;
		io.NavInputs[ImGuiNavInput_FocusNext] = (float)Application->getTracherGamepad().rightShoulder;
		io.NavInputs[ImGuiNavInput_TweakSlow] = (float)Application->getTracherGamepad().leftShoulder;
		io.NavInputs[ImGuiNavInput_TweakFast] = (float)Application->getTracherGamepad().rightShoulder;

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

		while (Count >= now)
		{
			for (size_t i = 0; i < Component->Label.size(); i++)
			{
				if (Component->Label.at(i)->GetVisible() && Component->Label.at(i)->getRenderOrder() == now)
					Component->Label.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Btn.size(); i++)
			{
				if (Component->Btn.at(i)->GetVisible() && Component->Btn.at(i)->getRenderOrder() == now)
					Component->Btn.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itextmul.size(); i++)
			{
				if (Component->Itextmul.at(i)->GetVisible() && Component->Itextmul.at(i)->getRenderOrder() == now)
					Component->Itextmul.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itext.size(); i++)
			{
				if (Component->Itext.at(i)->GetVisible() && Component->Itext.at(i)->getRenderOrder() == now)
					Component->Itext.at(i)->Render();
			}
			for (size_t i = 0; i < Component->separators.size(); i++)
			{
				if (Component->separators.at(i)->getRenderOrder() == now)
					Component->separators.at(i)->Render();
			}
			for (size_t i = 0; i < Component->childs.size(); i++)
			{
				if (Component->childs.at(i)->getCountOrderRenderInDial() == now)
					Component->childs.at(i)->Render();
			}
			for (size_t i = 0; i < Component->CollpsHeader.size(); i++)
			{
				if (Component->CollpsHeader.at(i)->Collapse() &&
					Component->CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					Component->CollpsHeader.at(i)->Render();
			}
			for (size_t i = 0; i < Component->UText.size(); i++)
			{
				if (Component->UText.at(i)->getRenderOrder() == now)
					Component->UText.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TList.size(); i++)
			{
				if (Component->TList.at(i)->GetVisible() && Component->TList.at(i)->getRenderOrder() == now)
					Component->TList.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Tabs.size(); i++)
			{
				if (Component->Tabs.at(i)->getCountOrderRenderInDial() == now)
					Component->Tabs.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TNode.size(); i++)
			{
				if (Component->TNode.at(i)->getCountOrderRenderInDial() == now)
					Component->TNode.at(i)->Render();
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

	if (IsVisible)
	{
		if (PosX != PosX_Last || PosY != PosY_Last)
		{
			ImGui::SetNextWindowPos(ImVec2(PosX, PosY), ImGuiCond_::ImGuiCond_Always, Pivot);
			PosX_Last = PosX;
			PosY_Last = PosY;
		}

		ImGui::Begin(IDTitle.c_str(), &IsVisible, window_flags);

		if (IsNeedBringToFont)
		{
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			ImGui::BringWindowToFocusFront(GetCurrentWindow());
		}
		if (SizeW != SizeW_Last || SizeH != SizeH_Last)
		{
			ImGui::SetWindowSize(GetCurrentWindow(), ImVec2(SizeW, SizeH), ImGuiCond_::ImGuiCond_Always);
			SizeW_Last = SizeW;
			SizeH_Last = SizeH;
		}

		int Count = this->getOrderCount(), now = 0;

		while (Count >= now)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));

			for (size_t i = 0; i < CollpsHeader.size(); i++)
			{
				if (CollpsHeader.at(i)->Collapse() &&
					CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					CollpsHeader.at(i)->Render();
			}
			for (size_t i = 0; i < Label.size(); i++)
			{
				if (Label.at(i)->GetVisible() && Label.at(i)->getRenderOrder() == now)
					Label.at(i)->Render();
			}
			for (size_t i = 0; i < Itext.size(); i++)
			{
				if (Itext.at(i)->GetVisible() && Itext.at(i)->getRenderOrder() == now)
					Itext.at(i)->Render();
			}
			for (size_t i = 0; i < separator.size(); i++)
			{
				if (separator.at(i)->getRenderOrder() == now)
					separator.at(i)->Render();
			}
			for (size_t i = 0; i < Itextmul.size(); i++)
			{
				if (Itextmul.at(i)->GetVisible() && Itextmul.at(i)->getRenderOrder() == now)
					Itextmul.at(i)->Render();
			}
			for (size_t i = 0; i < Btn.size(); i++)
			{
				if (Btn.at(i)->GetVisible() && Btn.at(i)->getRenderOrder() == now)
					Btn.at(i)->Render();
			}
			for (size_t i = 0; i < child.size(); i++)
			{
				if (child.at(i)->getCountOrderRenderInDial() == now)
					child.at(i)->Render();
			}
			for (size_t i = 0; i < UText.size(); i++)
			{
				if (UText.at(i)->getRenderOrder() == now)
					UText.at(i)->Render();
			}
			for (size_t i = 0; i < TList.size(); i++)
			{
				if (TList.at(i)->GetVisible() && TList.at(i)->getRenderOrder() == now)
					TList.at(i)->Render();
			}
			for (size_t i = 0; i < Tabs.size(); i++)
			{
				if (Tabs.at(i)->getCountOrderRenderInDial() == now)
					Tabs.at(i)->Render();
			}
			for (size_t i = 0; i < TNode.size(); i++)
			{
				if (TNode.at(i)->getCountOrderRenderInDial() == now)
					TNode.at(i)->Render();
			}

			now++;
			ImGui::PopStyleVar();
		}

		ImGui::End();
	}
}

void Tab::Render()
{
	Flags = 0;

	if (DragTabs)
		Flags = ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable;

	if (ImGui::BeginTabBar(IDTitle.c_str(), Flags))
	{
		int Count = OrderlyRender, now = 0;

		while (Count >= now)
		{
			if (ImGui::BeginTabItem("Sounds"))
				ImGui::EndTabItem();

			for (size_t i = 0; i < Component->Label.size(); i++)
			{
				if (Component->Label.at(i)->GetVisible() && Component->Label.at(i)->getRenderOrder() == now)
					Component->Label.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Btn.size(); i++)
			{
				if (Component->Btn.at(i)->GetVisible() && Component->Btn.at(i)->getRenderOrder() == now)
					Component->Btn.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itextmul.size(); i++)
			{
				if (Component->Itextmul.at(i)->GetVisible() && Component->Itextmul.at(i)->getRenderOrder() == now)
					Component->Itextmul.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itext.size(); i++)
			{
				if (Component->Itext.at(i)->GetVisible() && Component->Itext.at(i)->getRenderOrder() == now)
					Component->Itext.at(i)->Render();
			}
			for (size_t i = 0; i < Component->separators.size(); i++)
			{
				if (Component->separators.at(i)->getRenderOrder() == now)
					Component->separators.at(i)->Render();
			}
			for (size_t i = 0; i < Component->CollpsHeader.size(); i++)
			{
				if (Component->CollpsHeader.at(i)->Collapse() && Component->CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					Component->CollpsHeader.at(i)->Render();
			}
			for (size_t i = 0; i < Component->childs.size(); i++)
			{
				if (Component->childs.at(i)->getCountOrderRenderInDial() == now)
					Component->childs.at(i)->Render();
			}
			for (size_t i = 0; i < Component->UText.size(); i++)
			{
				if (Component->UText.at(i)->getRenderOrder() == now)
					Component->UText.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TList.size(); i++)
			{
				if (Component->TList.at(i)->GetVisible() && Component->TList.at(i)->getRenderOrder() == now)
					Component->TList.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TNode.size(); i++)
			{
				if (Component->TNode.at(i)->getRenderOrder() == now)
					Component->TNode.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Tabs.size(); i++)
			{
				if (Component->Tabs.at(i)->getCountOrderRenderInDial() == now)
					Component->Tabs.at(i)->Render();
			}

			now++;
		}

		ImGui::EndTabBar();
	}
}

void TreeNode::Render()
{
	Flags = 0;

	if (!HasFlags)
		Flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_None;

	if (ImGui::TreeNodeEx(IDTitle.c_str(), Flags))
	{
		int Count = OrderlyRender, now = 0;

		while (Count >= now)
		{
			for (size_t i = 0; i < Component->Label.size(); i++)
			{
				if (Component->Label.at(i)->GetVisible() &&
					Component->Label.at(i)->getRenderOrder() == now)
					Component->Label.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Btn.size(); i++)
			{
				if (Component->Btn.at(i)->GetVisible() &&
					Component->Btn.at(i)->getRenderOrder() == now)
					Component->Btn.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itextmul.size(); i++)
			{
				if (Component->Itextmul.at(i)->GetVisible() &&
					Component->Itextmul.at(i)->getRenderOrder() == now)
					Component->Itextmul.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itext.size(); i++)
			{
				if (Component->Itext.at(i)->GetVisible() &&
					Component->Itext.at(i)->getRenderOrder() == now)
					Component->Itext.at(i)->Render();
			}
			for (size_t i = 0; i < Component->separators.size(); i++)
			{
				if (Component->separators.at(i)->getRenderOrder() == now)
					Component->separators.at(i)->Render();
			}
			for (size_t i = 0; i < Component->CollpsHeader.size(); i++)
			{
				if (Component->CollpsHeader.at(i)->Collapse() &&
					Component->CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					Component->CollpsHeader.at(i)->Render();
			}
			for (size_t i = 0; i < Component->childs.size(); i++)
			{
				if (Component->childs.at(i)->getCountOrderRenderInDial() == now)
					Component->childs.at(i)->Render();
			}
			for (size_t i = 0; i < Component->UText.size(); i++)
			{
				if (Component->UText.at(i)->getRenderOrder() == now)
					Component->UText.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TList.size(); i++)
			{
				if (Component->TList.at(i)->GetVisible() &&
					Component->TList.at(i)->getRenderOrder() == now)
					Component->TList.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Tabs.size(); i++)
			{
				if (Component->Tabs.at(i)->getCountOrderRenderInDial() == now)
					Component->Tabs.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TNode.size(); i++)
			{
				if (Component->TNode.at(i)->getCountOrderRenderInDial() == now)
					Component->TNode.at(i)->Render();
			}

			now++;
		}

		ImGui::TreePop();
	}
}

void Child::Render()
{
	Flags = 0;

	if (IsHScroll)
		Flags = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::BeginChild(IDTitle.c_str(), size, IsBorder, Flags))
	{
		int Count = OrderlyRender, now = 0;

		while (Count >= now)
		{
			for (size_t i = 0; i < Component->Label.size(); i++)
			{
				if (Component->Label.at(i)->GetVisible() && Component->Label.at(i)->getRenderOrder() == now)
					Component->Label.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Btn.size(); i++)
			{
				if (Component->Btn.at(i)->GetVisible() && Component->Btn.at(i)->getRenderOrder() == now)
					Component->Btn.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itextmul.size(); i++)
			{
				if (Component->Itextmul.at(i)->GetVisible() && Component->Itextmul.at(i)->getRenderOrder() == now)
					Component->Itextmul.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Itext.size(); i++)
			{
				if (Component->Itext.at(i)->GetVisible() && Component->Itext.at(i)->getRenderOrder() == now)
					Component->Itext.at(i)->Render();
			}
			for (size_t i = 0; i < Component->separators.size(); i++)
			{
				if (Component->separators.at(i)->getRenderOrder() == now)
					Component->separators.at(i)->Render();
			}
			for (size_t i = 0; i < Component->CollpsHeader.size(); i++)
			{
				if (Component->CollpsHeader.at(i)->Collapse() &&
					Component->CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
					Component->CollpsHeader.at(i)->Render();
			}
			for (size_t i = 0; i < Component->childs.size(); i++)
			{
				if (Component->childs.at(i)->getCountOrderRenderInDial() == now)
					Component->childs.at(i)->Render();
			}
			for (size_t i = 0; i < Component->UText.size(); i++)
			{
				if (Component->UText.at(i)->getRenderOrder() == now)
					Component->UText.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TList.size(); i++)
			{
				if (Component->TList.at(i)->GetVisible() && Component->TList.at(i)->getRenderOrder() == now)
					Component->TList.at(i)->Render();
			}
			for (size_t i = 0; i < Component->TNode.size(); i++)
			{
				if (Component->TNode.at(i)->getCountOrderRenderInDial() == now)
					Component->TNode.at(i)->Render();
			}
			for (size_t i = 0; i < Component->Tabs.size(); i++)
			{
				if (Component->Tabs.at(i)->getCountOrderRenderInDial() == now)
					Component->Tabs.at(i)->Render();
			}

			now++;
		}
	}

	ImGui::EndChild();
}

void ITextMulti::Render()
{
	Flags = 0;
	Flags = ImGuiInputTextFlags_AllowTabInput;

	if (ReadOnly)
		Flags |= ImGuiInputTextFlags_ReadOnly;

	if (IsCtrlNewLine)
		Flags |= ImGuiInputTextFlags_CtrlEnterForNewLine;

	if (clText.empty())
		ImGui::InputTextMultiline(IDTitle.c_str(), &Text, ImVec2(0, 0), Flags);
	else
	{
		for (size_t i = 0; i < clText.size(); i++)
		{
			if (clText.at(i).type == Type::Normal)
				ImGui::InputTextMultiline(IDTitle.c_str(), &clText.at(i).str, ImVec2(0, 0), Flags);
			else if (clText.at(i).type == Type::Information)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
				ImGui::InputTextMultiline(IDTitle.c_str(), &clText.at(i).str, ImVec2(0, 0), Flags);
				ImGui::PopStyleColor();
			}
			else if (clText.at(i).type == Type::Error)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
				ImGui::InputTextMultiline(IDTitle.c_str(), &clText.at(i).str, ImVec2(0, 0), Flags);
				ImGui::PopStyleColor();
			}
		}
	}
}

void IText::Render()
{
	Flags = 0;

	if (IsNeedHistory)
		Flags |= ImGuiInputTextFlags_CallbackHistory;

	if (EnterReturnsTrue)
		Flags |= ImGuiInputTextFlags_EnterReturnsTrue;

	if (NeedToUseTAB)
		Flags |= ImGuiInputTextFlags_CallbackCompletion;

	if (IsNeedHint)
		if (ImGui::InputTextWithHint(IDTitle.c_str(), TextHint.c_str(), &Text, Flags))
			IsTextChange = true;
		else
			IsTextChange = false;
	else
		if (ImGui::InputText(IDTitle.c_str(), &Text, Flags))
			IsTextChange = true;
		else
			IsTextChange = false;
	Active = ImGui::IsItemActive();
}

void UnformatedText::Render()
{
	for (size_t i = 0; i < Buffer.size(); i++)
	{
		auto Obj = getString(Buffer.at(i));
		if (Obj && Obj->getType() == Type::Normal)
			ImGui::TextUnformatted(Buffer.at(i).c_str());
		else if (Obj && Obj->getType() == Type::Information)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
			ImGui::TextUnformatted(Buffer.at(i).c_str());
			ImGui::PopStyleColor();
		}
		else if (Obj && Obj->getType() == Type::Error)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			ImGui::TextUnformatted(Buffer.at(i).c_str());
			ImGui::PopStyleColor();
		}
	}
}
