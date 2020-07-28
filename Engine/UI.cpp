#include "pch.h"
#include <commdlg.h>

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Render_Buffer.h"
#include "UI.h"
#include "Console.h"
#include "File_system.h"

#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"

bool BaseComponent::OnlyRenderID = false;

HRESULT UI::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &IO = ImGui::GetIO();
	IO.IniFilename = NULL; IO.LogFilename = NULL;
	IO.MouseDrawCursor = true;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NoMouseCursorChange |
		ImGuiConfigFlags_DockingEnable;

	IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos;
	IO.BackendPlatformName = "DecisionSolver";

	ImGui_ImplWin32_Init(Application->GetHWND());
	ImGui_ImplDX11_Init(Application->getDevice(), Application->getDeviceContext());

	InitUI = true;
	return S_OK;
}
void dialogs::ChangeFont(string FontName, float SizePixel, float Brighten)
{
	SAFE_DELETE(Font);

	ImGuiIO &IO = ImGui::GetIO();
	shared_ptr<ImFontConfig> font_config = make_shared<ImFontConfig>();
	font_config->RasterizerMultiply = Brighten;
	font_config->OversampleH = 3;
	font_config->OversampleV = 1;
	font_config->PixelSnapH = true;
	Font = IO.Fonts->AddFontFromFileTTF(FontName.c_str(), SizePixel, font_config.get(),
		IO.Fonts->GetGlyphRangesCyrillic());
}

ToDo("Need To Reformat It!")
void UI::ResizeWnd()
{
	if (!Application->getUI()->getDialogs().empty() &&
		Application->getUI()->getDialog("Console").operator bool())
	{
		Application->getUI()->getDialog("Console")->ChangePosition(0.f, 0.f);
		Application->getUI()->getDialog("Console")->ChangeSize(
			(float)Application->getWorkAreaSize(Application->GetHWND()).x,
			(float)Application->getWorkAreaSize(Application->GetHWND()).y / 3.0f);
	}
}

void UI::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void UI::FrameEnd()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//Buf->RenderUI(ImGui::GetDrawData(), Application->IsWireFrame());
}

void UI::Destroy()
{
	ImGui::DestroyContext();
}

HRESULT UI::LoadFileUI(string File)
{
	doc = make_shared<tinyxml2::XMLDocument>();

	doc->LoadFile(File.c_str());
	if (doc->ErrorID() != XML_SUCCESS)
	{
		Engine::LogError((boost::format("UI::LoatXmlUI() ErrorID > 0!\nReturn Error ID: %s") %
			to_string(doc->ErrorID())).str(),
			string(__FILE__) + ": " + to_string(__LINE__),
			(boost::format("UI: Something is wrong with load XML File!\nReturn Error Text: %s"\
				"\nErrorID(see tinyXml doc): %s") % doc->ErrorStr() % to_string(doc->ErrorID())).str());
#if defined (_DEBUG)
		Engine::StackTrace(doc->ErrorStr());
#endif
		return E_FAIL;
	}
	if (doc->Parse(Application->getFS()->getDataFromFile(File).c_str()) > 0)
	{
		Engine::LogError((boost::format("UI::LoatXmlUI() Parse File ErrorID > 0!\nReturn Error ID: %s")
			% to_string(doc->ErrorID())).str(),
			string(__FILE__) + ": " + to_string(__LINE__),
			(boost::format("UI: Something is wrong with Parse XML File!\nReturn Error Text: %s"
				"\nErrorID (see tinyXml doc): %s") % doc->ErrorStr() % to_string(doc->ErrorID())).str());
#if defined (_DEBUG)
		Engine::StackTrace(doc->ErrorStr());
#endif
		return E_FAIL;
	}

	ProcessXML();
	return S_OK;
}

void UI::WorkOnComponents(shared_ptr<XMLComponents> Component, shared_ptr<AllTheComponent> DoneComponent,
	int &CountOrder)
{
	//	Button
	for (size_t i = 0; i < Component->IDbuttons.size(); i++)
	{
		CountOrder++;
		auto button = make_shared<Buttons>();
		GetParam(Component->buttons.at(i)->ToElement(), button);
		button->ChangeOrder(Component->IDbuttons.at(i));
		DoneComponent->Btn.push_back(make_pair(button->GetID(), button));
	}
	//	InputText
	for (size_t i = 0; i < Component->IDtexts.size(); i++)
	{
		CountOrder++;
		auto itext = make_shared<IText>();
		GetParam(Component->texts.at(i)->ToElement(), itext);
		itext->ChangeOrder(Component->IDtexts.at(i));
		DoneComponent->Itext.push_back(make_pair(itext->GetID(), itext));
	}
	//	InputTextMultiline
	for (size_t i = 0; i < Component->IDtextmuls.size(); i++)
	{
		CountOrder++;
		auto itextmul = make_shared<ITextMulti>();
		GetParam(Component->textmuls.at(i)->ToElement(), itextmul);
		itextmul->ChangeOrder(Component->IDtextmuls.at(i));
		DoneComponent->Itextmul.push_back(make_pair(itextmul->GetID(), itextmul));
	}
	//	Label
	for (size_t i = 0; i < Component->IDlabels.size(); i++)
	{
		CountOrder++;
		auto label = make_shared<Labels>();
		GetParam(Component->labels.at(i)->ToElement(), label);
		label->ChangeOrder(Component->IDlabels.at(i));
		DoneComponent->Label.push_back(make_pair(label->GetID(), label));
	}
	//	Combobox
	for (size_t i = 0; i < Component->IDcombo.size(); i++)
	{
		CountOrder++;
		auto combo = make_shared<Combobox>();
		GetParam(Component->combo.at(i)->ToElement(), combo);
		combo->ChangeOrder(Component->IDcombo.at(i));
		DoneComponent->combo.push_back(make_pair(combo->GetID(), combo));
	}
	//	Separator
	for (size_t i = 0; i < Component->IDseparators.size(); i++)
	{
		CountOrder++;
		shared_ptr<_Separator> separator = make_shared<_Separator>();
		separator->ChangeOrder(Component->IDseparators.at(i));
		DoneComponent->separators.push_back(make_pair(separator->GetID(), separator));
	}
	//	UnformatedText
	for (size_t i = 0; i < Component->IDutext.size(); i++)
	{
		CountOrder++;
		shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
		GetParam(Component->utext.at(i)->ToElement(), UText);
		UText->ChangeOrder(Component->IDutext.at(i));
		DoneComponent->UText.push_back(make_pair(UText->GetID(), UText));
	}
	//	Selectable
	for (size_t i = 0; i < Component->IDselect.size(); i++)
	{
		CountOrder++;
		auto select = make_shared<Selectable>();
		GetParam(Component->select.at(i)->ToElement(), select);
		select->ChangeOrder(Component->IDselect.at(i));
		DoneComponent->selectable.push_back(make_pair(select->GetID(), select));
	}
	//	Collapse
	for (size_t i = 0; i < Component->XMLCHead.size(); i++)
	{
		CountOrder++;
		int Count = 0;
		auto cheader = make_shared<CollapsingHeaders>();
		shared_ptr<AllTheComponent> CHeader = make_shared<AllTheComponent>();

		WorkOnComponents(Component->CpsHead.at(i), CHeader, Count);
		GetParam(Component->XMLCHead.at(i)->ToElement(), cheader);

		cheader->ChangeOrderInDial(Component->CpsHead.back()->OrderlyRenderInDial);
		cheader->ChangeOrder(Count);
		cheader->setComponents(CHeader);
		DoneComponent->CollpsHeader.push_back(make_pair(cheader->GetID(), cheader));
	}
	//	ChildDialog
	for (size_t i = 0; i < Component->XMLDChild.size(); i++)
	{
		CountOrder++;
		int Count = 0;
		auto child = make_shared<Child>();
		shared_ptr<AllTheComponent> Childs = make_shared<AllTheComponent>();

		WorkOnComponents(Component->DialChild.at(i), Childs, Count);
		GetParam(Component->XMLDChild.at(i)->ToElement(), child);

		child->ChangeOrderInDial(Component->DialChild.back()->OrderlyRenderInDial);
		child->ChangeOrder(Count);
		child->setComponents(Childs);
		DoneComponent->childs.push_back(make_pair(child->GetID(), child));
	}
	//	Tab
	for (size_t i = 0; i < Component->XML_Tab.size(); i++)
	{
		CountOrder++;
		int Count = 0;
		auto tab = make_shared<Tab>();
		shared_ptr<AllTheComponent> Tabs = make_shared<AllTheComponent>();

		WorkOnComponents(Component->_Tab.at(i)->Component.back(), Tabs, Count);
		GetParam(Component->XML_Tab.at(i)->ToElement(), tab);

		tab->ChangeOrderInDial(Component->_Tab.back()->OrderlyRenderInDial);
		tab->ChangeOrder(Count);
		tab->getTabItem().back()->TabItemComp.push_back(Tabs);
		DoneComponent->Tabs.push_back(make_pair(tab->GetID(), tab));
	}
	//	TreeNode
	for (size_t i = 0; i < Component->XML_TreeNode.size(); i++)
	{
		CountOrder++;
		int Count = 0;
		auto TNode = make_shared<TreeNode>();
		shared_ptr<AllTheComponent> tnode = make_shared<AllTheComponent>();

		WorkOnComponents(Component->_TreeNode.at(i), tnode, Count);
		GetParam(Component->XML_TreeNode.at(i)->ToElement(), TNode);

		TNode->ChangeOrderInDial(Component->_TreeNode.back()->OrderlyRenderInDial);
		TNode->ChangeOrder(Count);
		TNode->setComponents(tnode);
		DoneComponent->TNode.push_back(make_pair(TNode->GetID(), TNode));
	}
	//	Column
	for (size_t i = 0; i < Component->XMLColumn.size(); i++)
	{
		CountOrder++;
		int Count = 0;
		auto column = make_shared<Column>();
		for (size_t i = 0; i < Component->_Column.size(); i++)
		{
			shared_ptr<AllTheComponent> Columns = make_shared<AllTheComponent>();
			WorkOnComponents(Component->_Column.at(i), Columns, Count);
			column->setComponents(Columns);
		}
		GetParam(Component->XMLColumn.at(i)->ToElement(), column);

		column->ChangeOrderInDial(Component->_Column.back()->OrderlyRenderInDial);
		column->ChangeOrder(Count);
		DoneComponent->column.push_back(make_pair(column->GetID(), column));
	}
}

void UI::XMLPreparing(shared_ptr<XMLDial> InDial, XMLNode *everything, int &countComp)
{
	string Val = everything->Value();
	to_lower(Val);
	
	countComp++;

	if (Val == "button")
	{
		InDial->Components->IDbuttons.push_back(countComp);
		InDial->Components->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InDial->Components->IDtexts.push_back(countComp);
		InDial->Components->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InDial->Components->IDtextmuls.push_back(countComp);
		InDial->Components->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InDial->Components->IDlabels.push_back(countComp);
		InDial->Components->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InDial->Components->IDseparators.push_back(countComp);
		InDial->Components->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InDial->Components->IDselect.push_back(countComp);
		InDial->Components->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InDial->Components->IDutext.push_back(countComp);
		InDial->Components->utext.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InDial->Components->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InDial->Components->XML_Tab.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InDial->Components->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InDial->Components->XMLCHead.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InDial->Components->IDtlist.push_back(countComp);
		InDial->Components->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InDial->Components->IDcombo.push_back(countComp);
		InDial->Components->combo.push_back(everything);
		return;
	}
}
void UI::XMLPreparingCollps(shared_ptr<XMLDial> InCHead, XMLNode *everything, int &countComp)
{
	if (InCHead->Components->CpsHead.empty())
		InCHead->Components->CpsHead.push_back(make_shared<XMLComponents>());

	string Val = everything->Value();
	to_lower(Val);
	
	countComp++;

	if (Val == "button")
	{
		InCHead->Components->CpsHead.back()->IDbuttons.push_back(countComp);
		InCHead->Components->CpsHead.back()->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InCHead->Components->CpsHead.back()->IDtexts.push_back(countComp);
		InCHead->Components->CpsHead.back()->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InCHead->Components->CpsHead.back()->IDtextmuls.push_back(countComp);
		InCHead->Components->CpsHead.back()->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InCHead->Components->CpsHead.back()->IDlabels.push_back(countComp);
		InCHead->Components->CpsHead.back()->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InCHead->Components->CpsHead.back()->IDseparators.push_back(countComp);
		InCHead->Components->CpsHead.back()->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InCHead->Components->CpsHead.back()->IDselect.push_back(countComp);
		InCHead->Components->CpsHead.back()->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InCHead->Components->CpsHead.back()->IDutext.push_back(countComp);
		InCHead->Components->CpsHead.back()->utext.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InCHead->Components->CpsHead.back()->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InCHead->Components->CpsHead.back()->XML_Tab.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InCHead->Components->CpsHead.back()->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InCHead->Components->CpsHead.back()->XMLCHead.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InCHead->Components->CpsHead.back()->IDtlist.push_back(countComp);
		InCHead->Components->CpsHead.back()->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InCHead->Components->CpsHead.back()->IDcombo.push_back(countComp);
		InCHead->Components->CpsHead.back()->combo.push_back(everything);
		return;
	}
}
void UI::XMLPreparingChild(shared_ptr<XMLDial> InChild, XMLNode *everything, int &countComp)
{
	if (InChild->Components->DialChild.empty())
		InChild->Components->DialChild.push_back(make_shared<XMLComponents>());

	string Val = everything->Value();
	to_lower(Val);
	
	countComp++;

	if (Val == "button")
	{
		InChild->Components->DialChild.back()->IDbuttons.push_back(countComp);
		InChild->Components->DialChild.back()->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InChild->Components->DialChild.back()->IDtexts.push_back(countComp);
		InChild->Components->DialChild.back()->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InChild->Components->DialChild.back()->IDtextmuls.push_back(countComp);
		InChild->Components->DialChild.back()->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InChild->Components->DialChild.back()->IDlabels.push_back(countComp);
		InChild->Components->DialChild.back()->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InChild->Components->DialChild.back()->IDseparators.push_back(countComp);
		InChild->Components->DialChild.back()->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InChild->Components->DialChild.back()->IDselect.push_back(countComp);
		InChild->Components->DialChild.back()->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InChild->Components->DialChild.back()->IDutext.push_back(countComp);
		InChild->Components->DialChild.back()->utext.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InChild->Components->DialChild.back()->IDtlist.push_back(countComp);
		InChild->Components->DialChild.back()->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InChild->Components->DialChild.back()->IDcombo.push_back(countComp);
		InChild->Components->DialChild.back()->combo.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InChild->Components->DialChild.back()->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InChild->Components->DialChild.back()->XML_Tab.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InChild->Components->DialChild.back()->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InChild->Components->DialChild.back()->XMLCHead.push_back(everything);
		return;
	}
}
void UI::XMLPreparingTNode(shared_ptr<XMLDial> InTNode, XMLNode *everything, int &countComp)
{
	if (InTNode->Components->_TreeNode.empty())
		InTNode->Components->_TreeNode.push_back(make_shared<XMLComponents>());

	string Val = everything->Value();
	to_lower(Val);
	
	countComp++;

	if (Val == "button")
	{
		InTNode->Components->_TreeNode.back()->IDbuttons.push_back(countComp);
		InTNode->Components->_TreeNode.back()->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InTNode->Components->_TreeNode.back()->IDtexts.push_back(countComp);
		InTNode->Components->_TreeNode.back()->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InTNode->Components->_TreeNode.back()->IDtextmuls.push_back(countComp);
		InTNode->Components->_TreeNode.back()->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InTNode->Components->_TreeNode.back()->IDlabels.push_back(countComp);
		InTNode->Components->_TreeNode.back()->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InTNode->Components->_TreeNode.back()->IDseparators.push_back(countComp);
		InTNode->Components->_TreeNode.back()->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InTNode->Components->_TreeNode.back()->IDselect.push_back(countComp);
		InTNode->Components->_TreeNode.back()->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InTNode->Components->_TreeNode.back()->IDutext.push_back(countComp);
		InTNode->Components->_TreeNode.back()->utext.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InTNode->Components->_TreeNode.back()->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InTNode->Components->_TreeNode.back()->XML_Tab.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InTNode->Components->_TreeNode.back()->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InTNode->Components->_TreeNode.back()->XMLCHead.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InTNode->Components->_TreeNode.back()->IDtlist.push_back(countComp);
		InTNode->Components->_TreeNode.back()->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InTNode->Components->_TreeNode.back()->IDcombo.push_back(countComp);
		InTNode->Components->_TreeNode.back()->combo.push_back(everything);
		return;
	}
}
void UI::XMLPreparingTab(shared_ptr<XMLDial> InTab, XMLNode *everything, int &countComp)
{
	if (InTab->Components->_Tab.empty())
		InTab->Components->_Tab.back()->Component.push_back(make_shared<XMLComponents>());
	
	string Val = everything->Value();
	to_lower(Val);

	countComp++;

	if (Val == "button")
	{
		InTab->Components->_Tab.back()->Component.back()->IDbuttons.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InTab->Components->_Tab.back()->Component.back()->IDtexts.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InTab->Components->_Tab.back()->Component.back()->IDtextmuls.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InTab->Components->_Tab.back()->Component.back()->IDlabels.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InTab->Components->_Tab.back()->Component.back()->IDseparators.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InTab->Components->_Tab.back()->Component.back()->IDselect.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InTab->Components->_Tab.back()->Component.back()->IDutext.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->utext.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InTab->Components->_Tab.back()->Component.back()->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InTab->Components->_Tab.back()->Component.back()->XML_Tab.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InTab->Components->_Tab.back()->Component.back()->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InTab->Components->_Tab.back()->Component.back()->XMLCHead.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InTab->Components->_Tab.back()->Component.back()->IDtlist.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InTab->Components->_Tab.back()->Component.back()->IDcombo.push_back(countComp);
		InTab->Components->_Tab.back()->Component.back()->combo.push_back(everything);
		return;
	}
}
void UI::XMLPreparingColumn(shared_ptr<XMLDial> InColumn, XMLNode *everything, int &countComp)
{
	if (InColumn->Components->_Column.empty())
		InColumn->Components->_Column.push_back(make_shared<XMLComponents>());
	
	string Val = everything->Value();
	to_lower(Val);

	countComp++;

	if (Val == "button")
	{
		InColumn->Components->_Column.back()->IDbuttons.push_back(countComp);
		InColumn->Components->_Column.back()->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InColumn->Components->_Column.back()->IDtexts.push_back(countComp);
		InColumn->Components->_Column.back()->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InColumn->Components->_Column.back()->IDtextmuls.push_back(countComp);
		InColumn->Components->_Column.back()->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InColumn->Components->_Column.back()->IDlabels.push_back(countComp);
		InColumn->Components->_Column.back()->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InColumn->Components->_Column.back()->IDseparators.push_back(countComp);
		InColumn->Components->_Column.back()->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InColumn->Components->_Column.back()->IDselect.push_back(countComp);
		InColumn->Components->_Column.back()->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InColumn->Components->_Column.back()->IDutext.push_back(countComp);
		InColumn->Components->_Column.back()->utext.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InColumn->Components->_Column.back()->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InColumn->Components->_Column.back()->XMLColumn.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InColumn->Components->_Column.back()->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InColumn->Components->_Column.back()->XMLCHead.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InColumn->Components->_Column.back()->IDtlist.push_back(countComp);
		InColumn->Components->_Column.back()->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InColumn->Components->_Column.back()->IDcombo.push_back(countComp);
		InColumn->Components->_Column.back()->combo.push_back(everything);
		return;
	}
}

void UI::XMLPreparingRecursion(shared_ptr<XMLComponents> InCHead, XMLNode *everything, int &countComp)
{
	countComp++;
	string Val = everything->Value();
	to_lower(Val);

	if (Val == "button")
	{
		InCHead->IDbuttons.push_back(countComp);
		InCHead->buttons.push_back(everything);
		return;
	}
	if (Val == "inputtext")
	{
		InCHead->IDtexts.push_back(countComp);
		InCHead->texts.push_back(everything);
		return;
	}
	if (Val == "inputtextmultiline")
	{
		InCHead->IDtextmuls.push_back(countComp);
		InCHead->textmuls.push_back(everything);
		return;
	}
	if (Val == "label")
	{
		InCHead->IDlabels.push_back(countComp);
		InCHead->labels.push_back(everything);
		return;
	}
	if (Val == "separator")
	{
		InCHead->IDseparators.push_back(countComp);
		InCHead->separators.push_back(everything);
		return;
	}
	if (Val == "selectable")
	{
		InCHead->IDselect.push_back(countComp);
		InCHead->select.push_back(everything);
		return;
	}
	if (Val == "unformatedtext")
	{
		InCHead->IDutext.push_back(countComp);
		InCHead->utext.push_back(everything);
		return;
	}
	if (Val == "treenode")
	{
		InCHead->XML_TreeNode.push_back(everything);
		return;
	}
	if (Val == "tab")
	{
		InCHead->XML_Tab.push_back(everything);
		return;
	}
	if (Val == "childdialog")
	{
		InCHead->XMLDChild.push_back(everything);
		return;
	}
	if (Val == "collapse")
	{
		InCHead->XMLCHead.push_back(everything);
		return;
	}
	if (Val == "listbox")
	{
		InCHead->IDtlist.push_back(countComp);
		InCHead->tlist.push_back(everything);
		return;
	}
	if (Val == "combo")
	{
		InCHead->IDcombo.push_back(countComp);
		InCHead->combo.push_back(everything);
		return;
	}
}

void UI::GetParam(XMLNode *Nods, shared_ptr<Child> InChild)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			InChild->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "border")
		{
			InChild->setBorder(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "horizontal_scroll")
		{
			InChild->setHScroll(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<Tab> InTab)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);
		
		if (Val == "id")
		{
			InTab->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "dragging_tabs")
		{
			InTab->setDragTabs(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			InTab->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "auto_select_new_tab")
		{
			InTab->setASelectNewTab(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "no_close_midle_mouse")
		{
			InTab->setCloseMidMouse(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<CollapsingHeaders> InCollaps)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			InCollaps->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "seldefault")
		{
			InCollaps->setSelDefault(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			InCollaps->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "collapsible")
		{
			InCollaps->setCollapse(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<TreeNode> InTNode)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			InTNode->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			InTNode->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<Column> column)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "count")
		{
			column->ChangeCountColumn(FirstAttr->IntValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "id")
		{
			column->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "border")
		{
			column->SetBorder(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLNode *Nods, shared_ptr<Selectable> select)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			select->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}

void UI::GetParam(XMLElement *Nods, shared_ptr<Buttons> btn)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			btn->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "visible")
		{
			btn->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			btn->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLElement *Nods, shared_ptr<TextList> TList)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			TList->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "visible")
		{
			TList->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "label")
		{
			TList->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLElement *Nods, shared_ptr<Combobox> Combo)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			Combo->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLElement *Nods, shared_ptr<Labels> Label)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			Label->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "visible")
		{
			Label->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			Label->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLElement *Nods, shared_ptr<IText> Itext)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			Itext->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "visible")
		{
			Itext->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			Itext->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "history")
		{
			Itext->setHistory(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "hint")
		{
			Itext->setHint(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
break;
		}
		if (Val == "texthint")
		{
			Itext->ChangeTextHint(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLElement *Nods, shared_ptr<ITextMulti> ItextMul)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			ItextMul->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "visible")
		{
			ItextMul->setVisible(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "text")
		{
			ItextMul->ChangeText(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
		if (Val == "readonly")
		{
			ItextMul->setReadOnly(FirstAttr->BoolValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}
void UI::GetParam(XMLElement *Nods, shared_ptr<UnformatedText> UText)
{
	if (!Nods) return;

	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Nods->ToElement()->FirstAttribute());
	for (;;)
	{
		string Val = FirstAttr->Name();
		to_lower(Val);

		if (Val == "id")
		{
			UText->ChangeID(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
}

void UI::GetRecursion(vector<XMLNode *> SomeComponents, int &countComponents, shared_ptr<XMLComponents> SomeComponent)
{
	for (auto Two: SomeComponents)
	{
		int i = 0;
		while (!Two->NoChildren())
		{
			string T_Parnt_Val = Two->Parent()->Value(), T_Val = Two->Value();
			to_lower(T_Parnt_Val);
			to_lower(T_Val);

			// Check Our TabItem First
			if (Two->Parent() && T_Parnt_Val == "tab" && T_Val == "tabitem")
			{
				vector<XMLNode *> SecondComponent; // Look for The Components From Sub-Component
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(Two->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three: SecondComponent)
				{
					if (!Three->NoChildren())
					{
						vector<XMLNode *> Four = { Three };
						GetRecursion(Four, SomeComponent->OrderlyRender, SomeComponent);
					}
					else
						XMLPreparingRecursion(SomeComponent, Three, SomeComponent->OrderlyRender);
				}
			}
			else if (T_Val == "tab")
			{
				SomeComponent->_Tab.back()->Component.push_back(make_shared<XMLComponents>());
				SomeComponent->XML_Tab.push_back(Two);
				SomeComponent->OrderlyRender++;
				SomeComponent->_Tab.back()->OrderlyRenderInDial = SomeComponent->OrderlyRender;

				vector<XMLNode *> SecondComponent; // Look for The Components From Sub-Component
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(Two->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three: SecondComponent)
				{
					if (!Three->NoChildren())
					{
						vector<XMLNode *> Four = { Three };
						GetRecursion(Four,
							SomeComponent->_Tab.back()->Component.back()->OrderlyRender,
							SomeComponent->_Tab.back()->Component.back());
					}
					else
						XMLPreparingRecursion(SomeComponent->_Tab.back()->Component.back(), Three,
							SomeComponent->_Tab.back()->Component.back()->OrderlyRender);
				}
			}
			else if (T_Val == "column")
			{
				SomeComponent->XMLColumn.push_back(Two);
				SomeComponent->_Column.push_back(make_shared<XMLComponents>());
				SomeComponent->OrderlyRender++;
				SomeComponent->_Column.back()->OrderlyRenderInDial = SomeComponent->OrderlyRender;

				vector<XMLNode *> SecondComponent; // Look for The Components From Sub-Component
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(Two->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three: SecondComponent)
				{
					string Attr = Three->ToElement()->Name();
					to_lower(Attr);

					if (Attr == "next")
					{
						SomeComponent->_Column.push_back(make_shared<XMLComponents>());
						SomeComponent->_Column.back()->OrderlyRenderInDial++;
						continue;
					}
					if (!Three->NoChildren())
					{
						vector<XMLNode *> Four = { Three };
						GetRecursion(Four, SomeComponent->_Column.back()->OrderlyRender,
							SomeComponent->_Column.back());
					}
					else
						XMLPreparingRecursion(SomeComponent->_Column.back(), Three,
							SomeComponent->_Column.back()->OrderlyRender);
				}
			}
			else if (T_Val == "treenode")
			{
				SomeComponent->_TreeNode.push_back(make_shared<XMLComponents>());
				SomeComponent->XML_TreeNode.push_back(Two);
				SomeComponent->OrderlyRender++;
				SomeComponent->_TreeNode.back()->OrderlyRenderInDial = SomeComponent->OrderlyRender;

				vector<XMLNode *> SecondComponent; // Look for The Components From Sub-Component
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(Two->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three: SecondComponent)
				{
					if (!Three->NoChildren())
					{
						vector<XMLNode *> Four = { Three };
						GetRecursion(Four, SomeComponent->_TreeNode.back()->OrderlyRender,
							SomeComponent->_TreeNode.back());
					}
					else
						XMLPreparingRecursion(SomeComponent->_TreeNode.back(), Three,
							SomeComponent->_TreeNode.back()->OrderlyRender);
				}
			}
			else if (T_Val == "collapse")
			{
				SomeComponent->CpsHead.push_back(make_shared<XMLComponents>());
				SomeComponent->XMLCHead.push_back(Two);
				SomeComponent->OrderlyRender++;
				SomeComponent->CpsHead.back()->OrderlyRenderInDial = SomeComponent->OrderlyRender;

				vector<XMLNode *> SecondComponent; // Look for The Components From Sub-Component
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(Two->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three: SecondComponent)
				{
					if (!Three->NoChildren())
					{
						vector<XMLNode *> Four = { Three };
						GetRecursion(Four, SomeComponent->CpsHead.back()->OrderlyRender,
							SomeComponent->CpsHead.back());
					}
					else
						XMLPreparingRecursion(SomeComponent->CpsHead.back(), Three,
							SomeComponent->CpsHead.back()->OrderlyRender);
				}
			}
			else if (T_Val == "childdialog")
			{
				SomeComponent->DialChild.push_back(make_shared<XMLComponents>());
				SomeComponent->XMLDChild.push_back(Two);
				SomeComponent->OrderlyRender++;
				SomeComponent->DialChild.back()->OrderlyRenderInDial = SomeComponent->OrderlyRender;

				vector<XMLNode *> SecondComponent; // Look for The Components From Sub-Component
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(Two->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Three: SecondComponent)
				{
					if (!Three->NoChildren())
					{
						vector<XMLNode *> Four = { Three };
						GetRecursion(Four, SomeComponent->DialChild.back()->OrderlyRender,
							SomeComponent->DialChild.back());
					}
					else
						XMLPreparingRecursion(SomeComponent->DialChild.back(), Three,
							SomeComponent->DialChild.back()->OrderlyRender);
				}
			}

			i++;
			break;
		}
	}
}
void UI::GetRecursionForAddComponents(shared_ptr<dialogs> RequiredComponent,
	shared_ptr<XMLComponents> SomeComponent)
{
	shared_ptr<AllTheComponent> tmpComponent;

	//	Collapse
	for (size_t i = 0; i < SomeComponent->XMLCHead.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto CHeader = make_shared<CollapsingHeaders>();
		// Get Params From New Component!
		GetParam(SomeComponent->XMLCHead.at(i), CHeader);
		CHeader->ChangeOrderInDial(SomeComponent->CpsHead.at(i)->OrderlyRenderInDial);

		GetRecursionAdd(SomeComponent->CpsHead.at(i), tmpComponent, SomeComponent->CpsHead.at(i)->OrderlyRender);
		CHeader->setComponents(tmpComponent);
		tmpComponent.reset();

		CHeader->ChangeOrder(SomeComponent->CpsHead.at(i)->OrderlyRender);

		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->getComponents()->CollpsHeader.push_back(make_pair(CHeader->GetID(), CHeader));
	}
	//	ChildDialog
	for (size_t i = 0; i < SomeComponent->XMLDChild.size(); i++)
	{
		if(!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto child = make_shared<Child>();
		GetParam(SomeComponent->XMLDChild.at(i), child);
		child->ChangeOrderInDial(SomeComponent->DialChild.at(i)->OrderlyRenderInDial);

		GetRecursionAdd(SomeComponent->DialChild.at(i), tmpComponent, SomeComponent->DialChild.at(i)->OrderlyRender);
		child->setComponents(tmpComponent);
		tmpComponent.reset();

		child->ChangeOrder(SomeComponent->DialChild.at(i)->OrderlyRender);

		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->getComponents()->childs.push_back(make_pair(child->GetID(), child));
	}
	// Tab
	for (size_t i = 0; i < SomeComponent->XML_Tab.size(); i++)
	{
		int CountComp = 0;
		auto tab = make_shared<Tab>();
		GetParam(SomeComponent->XML_Tab.at(i), tab);
		tab->ChangeOrderInDial(SomeComponent->_Tab.at(i)->OrderlyRenderInDial);

		for (size_t i1 = 0; i1 < SomeComponent->_Tab.at(i)->Component.size(); i1++)
		{
			tab->getTabItem().back()->TabItemComp.push_back(make_shared<AllTheComponent>());
			tab->getTabItem().back()->Name.push_back(SomeComponent->_Tab.at(i)->TabItems.at(i1));

			GetRecursionAdd(SomeComponent->_Tab.at(i)->Component.at(i1), tab->getTabItem().back()->TabItemComp.back(),
				CountComp);
		}
		tab->ChangeOrder(CountComp);

		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->getComponents()->Tabs.push_back(make_pair(tab->GetID(), tab));
	}
	// TreeNode
	for (size_t i = 0; i < SomeComponent->XML_TreeNode.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		shared_ptr<TreeNode> TNode = make_shared<TreeNode>();
		GetParam(SomeComponent->XML_TreeNode.at(i), TNode);
		TNode->ChangeOrderInDial(SomeComponent->_TreeNode.at(i)->OrderlyRenderInDial);

		GetRecursionAdd(SomeComponent->_TreeNode.at(i), tmpComponent, SomeComponent->_TreeNode.at(i)->OrderlyRender);
		TNode->setComponents(tmpComponent);
		tmpComponent.reset();

		TNode->ChangeOrder(SomeComponent->_TreeNode.at(i)->OrderlyRender);

		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->getComponents()->TNode.push_back(make_pair(TNode->GetID(), TNode));
	}
	// Column
	for (size_t i = 0; i < SomeComponent->XMLColumn.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto column = make_shared<Column>();
		GetParam(SomeComponent->XMLColumn.at(i), column);
		column->ChangeOrderInDial(SomeComponent->_Column.at(i)->OrderlyRenderInDial);

		for (size_t i = 0; i < SomeComponent->_Column.size(); i++)
		{
			GetRecursionAdd(SomeComponent->_Column.at(i), tmpComponent, SomeComponent->_Column.at(i)->OrderlyRender);
			column->setComponents(tmpComponent);
			tmpComponent.reset();
		}
		column->ChangeOrder(SomeComponent->_Column.at(i)->OrderlyRender);

		RequiredComponent->ChangeOrder(SomeComponent->OrderlyRender);
		RequiredComponent->getComponents()->column.push_back(make_pair(column->GetID(), column));
	}
}
void UI::GetRecursionAdd(shared_ptr<XMLComponents> SomeComponent, shared_ptr<AllTheComponent> AllComponent,
	int &Count)
{
	shared_ptr<AllTheComponent> tmpComponent;

	for (size_t i = 0; i < SomeComponent->XMLDChild.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto child = make_shared<Child>();
		WorkOnComponents(SomeComponent->DialChild.at(i), tmpComponent, Count);
		child->setComponents(tmpComponent);
		tmpComponent.reset();

		GetParam(SomeComponent->XMLDChild.at(i)->ToElement(), child);
		child->ChangeOrder(Count);
		child->ChangeOrderInDial(SomeComponent->DialChild.at(i)->OrderlyRenderInDial);

		AllComponent->childs.push_back(make_pair(child->GetID(), child));
	}
	for (size_t i = 0; i < SomeComponent->XML_Tab.size(); i++)
	{
		auto tab = make_shared<Tab>();
		WorkOnComponents(SomeComponent->_Tab.at(i)->Component.back(),
			tab->getTabItem().back()->TabItemComp.back(), Count);

		GetParam(SomeComponent->XML_Tab.at(i)->ToElement(), tab);
		tab->ChangeOrder(Count);
		tab->ChangeOrderInDial(SomeComponent->_Tab.at(i)->OrderlyRenderInDial);

		AllComponent->Tabs.push_back(make_pair(tab->GetID(), tab));
	}
	for (size_t i = 0; i < SomeComponent->XML_TreeNode.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto treenode = make_shared<TreeNode>();
		WorkOnComponents(SomeComponent->_TreeNode.at(i), tmpComponent, Count);
		treenode->setComponents(tmpComponent);
		tmpComponent.reset();

		GetParam(SomeComponent->XML_TreeNode.at(i)->ToElement(), treenode);
		treenode->ChangeOrder(Count);
		treenode->ChangeOrderInDial(SomeComponent->_TreeNode.at(i)->OrderlyRenderInDial);

		AllComponent->TNode.push_back(make_pair(treenode->GetID(), treenode));
	}
	for (size_t i = 0; i < SomeComponent->XMLCHead.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto collps = make_shared<CollapsingHeaders>();
		WorkOnComponents(SomeComponent->CpsHead.at(i), tmpComponent, Count);
		collps->setComponents(tmpComponent);
		tmpComponent.reset();

		GetParam(SomeComponent->XMLCHead.at(i)->ToElement(), collps);
		collps->ChangeOrder(Count);
		collps->ChangeOrderInDial(SomeComponent->CpsHead.at(i)->OrderlyRenderInDial);

		AllComponent->CollpsHeader.push_back(make_pair(collps->GetID(), collps));
	}
	for (size_t i = 0; i < SomeComponent->XMLColumn.size(); i++)
	{
		if (!tmpComponent.operator bool())
			tmpComponent = make_shared<AllTheComponent>();

		auto column = make_shared<Column>();
		
		for (size_t i = 0; i < SomeComponent->_Column.size(); i++)
		{
			WorkOnComponents(SomeComponent->_Column.at(i), tmpComponent, Count);
			column->setComponents(tmpComponent);
			tmpComponent.reset();
		}
		GetParam(SomeComponent->XMLColumn.at(i)->ToElement(), column);
		column->ChangeOrder(Count);
		column->ChangeOrderInDial(SomeComponent->_Column.at(i)->OrderlyRenderInDial);

		AllComponent->column.push_back(make_pair(column->GetID(), column));
	}

	WorkOnComponents(SomeComponent, AllComponent, Count);
}

void UI::ProcessXML()
{
	vector<shared_ptr<dialogs>> dialog;

	// Get All The Dialogs
	for (;;)
	{
		if (XMLDialogs.empty())
			XMLDialogs.push_back(make_shared<XMLDial>(doc->RootElement()->Parent()->FirstChild()->NextSibling()));

		if (doc->RootElement()->Parent()->LastChild() != XMLDialogs.back()->Dial)
		{
			if (XMLDialogs.back()->Dial && XMLDialogs.back()->Dial->NextSibling())
				XMLDialogs.push_back(make_shared<XMLDial>(XMLDialogs.back()->Dial->NextSibling()));
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
		vector<XMLNode *> FirstComponent;
		for (;;)
		{
			if (FirstComponent.empty())
				FirstComponent.push_back((*It)->Dial->FirstChild());
			else if (FirstComponent.back()->NextSibling())
				FirstComponent.push_back(FirstComponent.back()->NextSibling());
			else
				break;
		}
		// Work With All The XML Nodes!
		int OrderlyRenderInDial = 0;
		for (auto First: FirstComponent)
		{
			OrderlyRenderInDial++;
			string Val = First->Value();
			to_lower(Val);

			if (Val == "collapse")
			{
				XMLDialogs.at(i)->Components->CpsHead.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XMLCHead.push_back(First);
				XMLDialogs.at(i)->Components->CpsHead.back()->OrderlyRenderInDial = OrderlyRenderInDial;

				vector<XMLNode *> SecondComponent;
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(First->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two: SecondComponent)
				{
					if (!Two->NoChildren()) // If We Need Recursion!
						GetRecursion(SecondComponent, XMLDialogs.at(i)->Components->CpsHead.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->CpsHead.back());
					else
						XMLPreparingCollps(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->CpsHead.back()->OrderlyRender);
				}
			}
			else if (Val == "childdialog")
			{
				XMLDialogs.at(i)->Components->DialChild.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XMLDChild.push_back(First);
				XMLDialogs.at(i)->Components->DialChild.back()->OrderlyRenderInDial = OrderlyRenderInDial;

				vector<XMLNode *> SecondComponent;
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(First->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two: SecondComponent)
				{
					if (!Two->NoChildren())
						GetRecursion(SecondComponent, XMLDialogs.at(i)->Components->DialChild.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->DialChild.back());
					else
						XMLPreparingChild(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->DialChild.back()->OrderlyRender);
				}
			}
			else if (Val == "treenode")
			{
				XMLDialogs.at(i)->Components->_TreeNode.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XML_TreeNode.push_back(First);
				XMLDialogs.at(i)->Components->_TreeNode.back()->OrderlyRenderInDial = OrderlyRenderInDial;

				vector<XMLNode *> SecondComponent;
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(First->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two: SecondComponent)
				{
					if (!Two->NoChildren()) // If We Need Recursion!
						GetRecursion(SecondComponent, XMLDialogs.at(i)->Components->_TreeNode.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->_TreeNode.back());
					else
						XMLPreparingTNode(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->_TreeNode.back()->OrderlyRender);
				}
			}
			else if (Val == "tab")
			{
				XMLDialogs.at(i)->Components->_Tab.push_back(make_shared<TItem>());
				XMLDialogs.at(i)->Components->XML_Tab.push_back(First);
				XMLDialogs.at(i)->Components->_Tab.back()->OrderlyRenderInDial = OrderlyRenderInDial;

				vector<XMLNode *> SecondComponent, SpecialComp;
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(First->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two: SecondComponent)
				{
					SpecialComp.push_back(Two);
					if (!Two->NoChildren()) // If we have a component and it also has a component
					{
						string Val = Two->Value();
						to_lower(Val);

						if (Val == "tabitem")
						{
							auto Attr = Two->ToElement()->FirstAttribute();
							string name = Attr->Name();
							to_lower(name);

							if (name == "text")
							{
								XMLDialogs.at(i)->Components->_Tab.back()->TabItems.push_back(Attr->Value());
								XMLDialogs.at(i)->Components->_Tab.back()->Component.push_back(make_shared<XMLComponents>());

								GetRecursion(SpecialComp,
									XMLDialogs.at(i)->Components->_Tab.back()->Component.back()->OrderlyRender,
									XMLDialogs.at(i)->Components->_Tab.back()->Component.back());

								SpecialComp.clear();
							}
						}
					}
					else // If we don't have any "sub-components" at all
						XMLPreparingTab(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->_Tab.back()->Component.back()->OrderlyRender);
				}
			}
			else if (Val == "column")
			{
				XMLDialogs.at(i)->Components->_Column.push_back(make_shared<XMLComponents>());
				XMLDialogs.at(i)->Components->XMLColumn.push_back(First);
				XMLDialogs.at(i)->Components->_Column.back()->OrderlyRenderInDial = OrderlyRenderInDial;

				vector<XMLNode *> SecondComponent;
				for (;;)
				{
					if (SecondComponent.empty())
						SecondComponent.push_back(First->ToElement()->FirstChild());
					else if (SecondComponent.back()->NextSibling())
						SecondComponent.push_back(SecondComponent.back()->NextSibling());
					else
						break;
				}
				for (auto Two: SecondComponent)
				{
					string Attr = Two->ToElement()->Name();
					to_lower(Attr);

					if (Attr == "next")
					{
						XMLDialogs.at(i)->Components->_Column.push_back(make_shared<XMLComponents>());
						XMLDialogs.at(i)->Components->_Column.back()->OrderlyRenderInDial++;
						continue;
					}

					if (!Two->NoChildren()) // If We Need Recursion!
						GetRecursion(SecondComponent, XMLDialogs.at(i)->Components->_Column.back()->OrderlyRender,
							XMLDialogs.at(i)->Components->_Column.back());
					else
						XMLPreparingColumn(XMLDialogs.at(i), Two,
							XMLDialogs.at(i)->Components->_Column.back()->OrderlyRender);
				}
			}
			// *******
			else
			{
				XMLPreparing(XMLDialogs.at(i), First, XMLDialogs.at(i)->Components->OrderlyRender);
				XMLDialogs.at(i)->Components->OrderlyRender--; // Return the value
			}

			XMLDialogs.at(i)->Components->OrderlyRender++;
		}
		// ID (Count) of XMLDialogs
		i++;
	}

	for (size_t IDDial = 0; IDDial < XMLDialogs.size(); IDDial++)
	{
		// ********
			// Dialog
		dialog.push_back(make_shared<dialogs>());
		float W = 0.f, H = 0.f, SizeFont = 0.f, Bright = 0.f;
		string nameFont;
		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(XMLDialogs.at(IDDial)->Dial->ToElement()->FirstAttribute());
		for (;;)
		{
			string Val = FirstAttr->Name();
			to_lower(Val);

			if (Val == "id")
			{
				dialog.at(IDDial)->ChangeTitle(FirstAttr->Value());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "width")
			{
				W = FirstAttr->FloatValue();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "height")
			{
				H = FirstAttr->FloatValue();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "visible")
			{
				dialog.at(IDDial)->setVisible(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "resize")
			{
				dialog.at(IDDial)->setResizeble(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "show_title")
			{
				dialog.at(IDDial)->SetShowTitle(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "moveble")
			{
				dialog.at(IDDial)->setMoveble(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "collapsible")
			{
				dialog.at(IDDial)->setCollapsible(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "bring_to_font")
			{
				dialog.at(IDDial)->setBringToFont(FirstAttr->BoolValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "font")
			{
				nameFont = FirstAttr->Value();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "sizefont")
			{
				SizeFont = FirstAttr->FloatValue();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "brightness")
			{
				Bright = FirstAttr->FloatValue();
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
			if (Val == "fullscreene")
			{
				dialog.at(IDDial)->setFullScreen(FirstAttr->BoolValue());
				W = 0.f; H = 0.f;
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
		}

		// ********
			// Other

		// Collapse, Child, Tab, TreeNode, etc
		GetRecursionForAddComponents(dialog.at(IDDial), XMLDialogs.at(IDDial)->Components);

		//	Button
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDbuttons.size(); i++)
		{
			auto btn = make_shared<Buttons>();
			GetParam(XMLDialogs.at(IDDial)->Components->buttons.at(i)->ToElement(), btn);
			btn->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDbuttons.at(i));
			dialog.at(IDDial)->getComponents()->Btn.push_back(make_pair(btn->GetID(), btn));
		}
		//	InputText
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDtexts.size(); i++)
		{
			auto text = make_shared<IText>();
			GetParam(XMLDialogs.at(IDDial)->Components->texts.at(i)->ToElement(), text);
			text->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDtexts.at(i));
			dialog.at(IDDial)->getComponents()->Itext.push_back(make_pair(text->GetID(), text));
		}
		//	InputTextMultiline
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDtextmuls.size(); i++)
		{
			auto itextmul = make_shared<ITextMulti>();
			GetParam(XMLDialogs.at(IDDial)->Components->textmuls.at(i)->ToElement(), itextmul);
			itextmul->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDtextmuls.at(i));
			dialog.at(IDDial)->getComponents()->Itextmul.push_back(make_pair(itextmul->GetID(), itextmul));
		}
		//	Label
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDlabels.size(); i++)
		{
			auto label = make_shared<Labels>();
			GetParam(XMLDialogs.at(IDDial)->Components->labels.at(i)->ToElement(), label);
			label->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDlabels.at(i));
			dialog.at(IDDial)->getComponents()->Label.push_back(make_pair(label->GetID(), label));
		}
		//	Separator
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDseparators.size(); i++)
		{
			shared_ptr<_Separator> separator = make_shared<_Separator>();
			separator->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDseparators.at(i));
			dialog.at(IDDial)->getComponents()->separators.push_back(make_pair(separator->GetID(), separator));
		}
		//	UnformatedText
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDutext.size(); i++)
		{
			shared_ptr<UnformatedText> UText = make_shared<UnformatedText>();
			GetParam(XMLDialogs.at(IDDial)->Components->utext.at(i)->ToElement(), UText);
			UText->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDutext.at(i));
			dialog.at(IDDial)->getComponents()->UText.push_back(make_pair(UText->GetID(), UText));
		}
		//	TextList
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDtlist.size(); i++)
		{
			auto tlist = make_shared<TextList>();
			GetParam(XMLDialogs.at(IDDial)->Components->tlist.at(i)->ToElement(), tlist);
			tlist->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDtlist.at(i));
			dialog.at(IDDial)->getComponents()->TList.push_back(make_pair(tlist->GetID(), tlist));
		}
		//	Combobox
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDcombo.size(); i++)
		{
			auto combo = make_shared<Combobox>();
			GetParam(XMLDialogs.at(IDDial)->Components->combo.at(i)->ToElement(), combo);
			combo->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDcombo.at(i));
			dialog.at(IDDial)->getComponents()->combo.push_back(make_pair(combo->GetID(), combo));
		}
		//	Selectable
		for (size_t i = 0; i < XMLDialogs.at(IDDial)->Components->IDselect.size(); i++)
		{
			auto select = make_shared<Selectable>();
			GetParam(XMLDialogs.at(IDDial)->Components->select.at(i)->ToElement(), select);
			select->ChangeOrder(XMLDialogs.at(IDDial)->Components->IDselect.at(i));
			dialog.at(IDDial)->getComponents()->selectable.push_back(make_pair(select->GetID(), select));
		}

		// Count of all the components only in dialog!
		dialog.at(IDDial)->ChangeOrder(XMLDialogs.at(IDDial)->Components->OrderlyRender);
		dialog.at(IDDial)->ChangeSize(W, H);
		dialog.at(IDDial)->ChangeFont(Application->getFS()->GetFile(nameFont)->PathA, SizeFont, Bright);
	}
	Dialogs = dialog;
}

ToDo("Need To Reformat It!")
void UI::ReloadXML(string File)
{
	Dialogs.clear();
	XMLDialogs.clear();

	LoadFileUI(File);
}

void UI::DisableDialog(string IDDialog)
{
	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (Dialogs.at(i)->GetTitle()== IDDialog)
			Dialogs.at(i)->setVisible(false);
	}
}
void UI::EnableDialog(string IDDialog)
{
	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (Dialogs.at(i)->GetTitle() == IDDialog)
			Dialogs.at(i)->setVisible(true);
	}
}

shared_ptr<dialogs> UI::getDialog(string IDDialog)
{
	if (Dialogs.empty() || IDDialog.empty())
		return make_shared<dialogs>();

	for (size_t i = 0; i < Dialogs.size(); i++)
	{
		if (Dialogs.at(i)->GetTitle() == IDDialog)
			return Dialogs.at(i);
	}

	//Engine::LogError("UI: " + string(IDDialog) + " Cannot Be found!\n",
	//	"UI: " + string(IDDialog) + " Cannot Be found!\n",
	//	"UI: " + string(IDDialog) + " Cannot Be found!\n");
	return make_shared<dialogs>();
}

#pragma warning(disable: 4129)
pair<bool, vector<string>> UI::GetWndDlgOpen(LPSTR DirByDef, LPSTR NameOfWnd, LPSTR FilterFilesExt, bool MultiSelect)
{
	char szFile[1024];
	pair<bool, vector<string>> RetVal = make_pair(true, vector<string>());
	DWORD dwError = 0;

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Engine::GetHWND();
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = FilterFilesExt;
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = NameOfWnd;
	ofn.lpstrInitialDir = DirByDef;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT |
		(MultiSelect ? OFN_ALLOWMULTISELECT : 0);

	if (!GetOpenFileNameA(&ofn))
	{
		dwError = CommDlgExtendedError();
		RetVal.first = false;
	}

	// Check for the error
	if (dwError != 0)
	{
		Engine::LogError("UI::GetWndDlgOpen Error Is: " + to_string(dwError),
			string(__FILE__) + to_string(__LINE__),
			"Something is wrong with Open File Dialog. It returns: " + to_string(dwError));
		return RetVal;
	}

	if (!RetVal.first) return RetVal;

	RetVal.first = true;

	if (path(string(szFile)).has_extension())
	{
		string File = string(szFile);
		to_lower(File);
		RetVal.second.push_back(File);
		return RetVal;
	}

	if (MultiSelect)
	{
		// Add path first
		string File = string(szFile);
		to_lower(File);
		RetVal.second.push_back(File + "\\");
		int FileOffset = ofn.nFileOffset;
		for (;;)
		{
			// Add it to massive of files (like path\File.ext)
			File = string(const_cast<const LPSTR>(&ofn.lpstrFile[FileOffset]));
			to_lower(File);
			RetVal.second.push_back(RetVal.second.front() + File);
			// Count the next file by offset from previous file (path - next offs file) + 1
			FileOffset += (RetVal.second.back().size() - RetVal.second.front().size()) + 1;
			if (ofn.lpstrFile[FileOffset] == '\0') // If the next no file
			{
				// Delete path from front (useless store it)
				RetVal.second.erase(RetVal.second.begin());
				break;
			}
		}
	}
	else
	{
		string File = string(szFile);
		to_lower(File);
		RetVal.second.push_back(File);
	}

	return RetVal;
}

ToDo("Check IT!!!");
pair<bool, vector<string>> UI::GetWndDlgSave(LPSTR DirByDef, LPSTR NameOfWnd, LPCSTR FilterFilesExt)
{
	char szFile[1024];
	pair<bool, vector<string>> RetVal = make_pair(true, vector<string>());
	DWORD dwError = 0;

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = Engine::GetHWND();
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = FilterFilesExt;

	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = NameOfWnd;
	ofn.lpstrInitialDir = DirByDef;
	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetSaveFileNameA(&ofn))
	{
		dwError = CommDlgExtendedError();
		RetVal.first = false;
	}

	// Check for the error
	if (dwError != 0)
	{
		Engine::LogError("UI::GetWndDlgSave Error Is: " + to_string(dwError),
			string(__FILE__) + to_string(__LINE__),
			"Something is wrong with Save File Dialog. It returns: " + to_string(dwError));
		return RetVal;
	}

	if (!RetVal.first) return RetVal;

	RetVal.first = true;

	if (path(string(szFile)).has_extension())
	{
		string File = string(szFile);
		to_lower(File);
		RetVal.second.push_back(File);
		return RetVal;
	}
	else
	{
		string File = string(szFile);
		to_lower(File);
		if (ofn.nFileExtension == 0)
			RetVal.second.push_back(File + ".*");
		else
			RetVal.second.push_back(File + '.' + ofn.lpstrDefExt);

		return RetVal;
	}

	string File = string(szFile);
	to_lower(File);

	RetVal.second.push_back(File + "\\");
	int FileOffset = ofn.nFileOffset;
	for (;;)
	{
		// Add it to massive of files (like path\File.ext)
		File = string(const_cast<const LPSTR>(&ofn.lpstrFile[FileOffset]));
		to_lower(File);
		RetVal.second.push_back(RetVal.second.front() + File);
		// Count the next file by offset from previous file (path - next offs file) + 1
		FileOffset += (RetVal.second.back().size() - RetVal.second.front().size()) + 1;
		if (ofn.lpstrFile[FileOffset] == '\0') // If the next no file
		{
			// Delete path from front (useless store it)
			RetVal.second.erase(RetVal.second.begin());
			break;
		}
	}

	return RetVal;
}

void UI::SetRenderOnlyID(bool b)
{
	BaseComponent::SetOnlyRenderID(b);
}

shared_ptr<Buttons> AllTheComponent::FindComponentBtn(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: Btn)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Buttons>();
}

shared_ptr<Labels> AllTheComponent::FindComponentLabel(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: Label)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Labels>();
}

shared_ptr<ITextMulti> AllTheComponent::FindComponentITextMul(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: Itextmul)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<ITextMulti>();
}

shared_ptr<IText> AllTheComponent::FindComponentIText(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: Itext)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<IText>();
}

shared_ptr<UnformatedText> AllTheComponent::FindComponentUText(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: UText)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<UnformatedText>();
}

shared_ptr<TextList> AllTheComponent::FindComponentTList(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: TList)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<TextList>();
}

shared_ptr<Selectable> AllTheComponent::FindComponentSelectable(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: selectable)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Selectable>();
}

shared_ptr<Combobox> AllTheComponent::FindComponentCombo(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: combo)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Combobox>();
}

shared_ptr<TreeNode> AllTheComponent::FindComponentTreeNode(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: TNode)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<TreeNode>();
}

shared_ptr<Tab> AllTheComponent::FindComponentTab(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: Tabs)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Tab>();
}

shared_ptr<Child> AllTheComponent::FindComponentChild(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: childs)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Child>();
}

shared_ptr<CollapsingHeaders> AllTheComponent::FindComponentCHeader(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: CollpsHeader)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<CollapsingHeaders>();
}

shared_ptr<Column> AllTheComponent::FindComponentColumn(string CmpName, bool NeedLog)
{
	if (CmpName.find("##") == string::npos)
		CmpName = "##" + CmpName;
	for (auto it: column)
	{
		if (it.first == CmpName)
			return it.second;
	}
	if (NeedLog)
		Engine::LogError("UI::FindComponent: Component:" + CmpName + " Cannot found!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"UI::FindComponent: Component:" + CmpName + " Cannot found!\n");
	return make_shared<Column>();
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

	if (IsVisible /*&& !IsClosed*/)
	{
		if (PosX != PosX_Last || PosY != PosY_Last)
		{
			ImGui::SetNextWindowPos(ImVec2(PosX, PosY), ImGuiCond_::ImGuiCond_Always, Pivot);
			PosX_Last = PosX;
			PosY_Last = PosY;
		}

		if (IsFullScreen)
		{
			ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin(IDTitle.c_str(), &IsVisible, window_flags);
			//	IsClosed = false;
			//else
			//	IsClosed = true;

			ImGui::PopStyleVar(3);
		}
		else
		{
			ImGui::Begin(IDTitle.c_str(), &IsVisible, window_flags);
				//IsClosed = false;
			//else
			//	IsClosed = true;
		}

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

		int Count = OrderlyRender, now = 0;

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2);
		ImGui::PushFont(Font);

		while (Count >= now)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 4));
			Component->RenderComponents(now);
			ImGui::PopStyleVar();
		}
		ImGui::PopStyleVar();
		ImGui::PopFont();
		ImGui::End();
	}
}

void Tab::Render()
{
	Flags = 0;

	if (DragTabs)
		Flags |= ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable;

	if (ASelectNewTab)
		Flags |= ImGuiTabBarFlags_::ImGuiTabBarFlags_AutoSelectNewTabs;

	if (!CloseMidMouse)
		Flags |= ImGuiTabBarFlags_::ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;

	if (ImGui::BeginTabBar(GetText().c_str(), Flags))
	{
		UINT MainText = 0;

		for (size_t MainComponent = 0; MainComponent < getTabItem().size(); MainComponent++)
		{
			for (size_t MainSubComp = 0; MainSubComp < getTabItem().at(MainComponent)->TabItemComp.size(); MainSubComp++)
			{
				if (ImGui::BeginTabItem(getTabItem().at(MainComponent)->Name.at(MainText).c_str()))
				{
					int Count = this->getRenderOrder(), now = 0;

					while (Count >= now)
					{
						getTabItem().at(MainComponent)->TabItemComp.at(MainSubComp)->RenderComponents(now);
					}

					ImGui::EndTabItem();
				}

				if (MainText > getTabItem().at(MainComponent)->Name.size())
					MainText = 0;

				MainText++;
			}
		}

		ImGui::EndTabBar();
	}
}

void TreeNode::Render()
{
	Flags = 0;

	if (!HasFlags)
		Flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_None;

	if (ImGui::TreeNodeEx(GetText().c_str(), Flags))
	{
		int Count = getRenderOrder(), now = 0;
		while (Count >= now)
		{
			getComponent()->RenderComponents(now);
		}

		ImGui::TreePop();
	}
}

void Child::Render()
{
	Flags = 0;

	if (IsHScroll)
		Flags = ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::BeginChild(GetText().c_str(), size, IsBorder, Flags))
	{
		int Count = getRenderOrder(), now = 0;
		while (Count >= now)
		{
			getMassComponents().back()->RenderComponents(now);
		}
	}
	if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

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

	Flags |= ImGuiInputTextFlags_EnterReturnsTrue;

	for (size_t i = 0; i < clText.size(); i++)
	{
		ImGui::PushStyleColor(ImGuiCol_Text,
			clText.at(i).type == Type::Normal ? ImVec4(0.0f, 112.0f, 150.0f, 1.0f) :
			(clText.at(i).type == Type::Information ? ImVec4(1.0f, 0.8f, 0.6f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f)));
		pressEnter = ImGui::InputTextMultiline(GetID().c_str(), &clText.at(i).str, ImVec2(0, 0), Flags);
		ImGui::PopStyleColor();
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

	Flags |= ImGuiInputTextFlags_EnterReturnsTrue;

	if (IsNeedHint)
	{
		pressEnter = ImGui::InputTextWithHint("", TextHint.c_str(), &Text, Flags);
		if (pressEnter)
			IsTextChange = true;
		else
			IsTextChange = false;
	}
	else
	{
		pressEnter = ImGui::InputText(GetID().c_str(), &Text, Flags);
		if (pressEnter)
			IsTextChange = true;
		else
			IsTextChange = false;
	}

	Active = ImGui::IsItemActive();
}

void UnformatedText::Render()
{
	for (size_t i = 0; i < clText.size(); i++)
	{
		ImGui::PushStyleColor(ImGuiCol_Text,
			clText.at(i).type == Type::Normal ? ImVec4(0.0f, 112.0f, 150.0f, 1.0f) :
			(clText.at(i).type == Type::Information ? ImVec4(1.0f, 0.8f, 0.6f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f)));
		ImGui::TextUnformatted(clText.at(i).str.c_str());
		ImGui::PopStyleColor();
	}

}

void CollapsingHeaders::Render()
{
	Flags = 0;
	Flags = ImGuiTreeNodeFlags_CollapsingHeader;

	if (SelDef)
		Flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::CollapsingHeader(GetText().c_str(), &IsCollapse, Flags))
	{
		int Count = getRenderOrder(), now = 0;

		while (Count >= now)
		{
			getComponent()->RenderComponents(now);
		}
	}
}

void AllTheComponent::RenderComponents(int &now)
{
	for (auto It: Label)
	{
		if (It.second->GetVisible() &&
			It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: Btn)
	{
		if (It.second->GetVisible() &&
			It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: Itextmul)
	{
		if (It.second->GetVisible() &&
			It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: Itext)
	{
		if (It.second->GetVisible() &&
			It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: separators)
	{
		if (It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: selectable)
	{
		if (It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: combo)
	{
		if (It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: CollpsHeader)
	{
		if (It.second->Collapse() &&
			It.second->getCountOrderRenderInDial() == now)
			It.second->Render();
	}
	for (auto It: childs)
	{
		if (It.second->getCountOrderRenderInDial() == now)
			It.second->Render();
	}
	for (auto It: UText)
	{
		if (It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: TList)
	{
		if (It.second->GetVisible() &&
			It.second->getRenderOrder() == now)
			It.second->Render();
	}
	for (auto It: Tabs)
	{
		if (It.second->getCountOrderRenderInDial() == now)
			It.second->Render();
	}
	for (auto It: TNode)
	{
		if (It.second->getCountOrderRenderInDial() == now)
			It.second->Render();
	}
	for (auto It: column)
	{
		if (It.second->getCountOrderRenderInDial() == now)
		{
			for (auto Cmp: It.second->getMassComponents())
			{
				ImGui::NextColumn();
				Cmp->RenderColumn(It.second->getRenderOrder(),
					It.second->getCountColumn(), It.second->GetText(), It.second->GetBorder());
				ImGui::NextColumn();
			}
		}
	}

	now++;
}

void AllTheComponent::RenderColumn(int OrderCount, int CountColumn, string IDColumn, bool border)
{
	ImGui::Columns(CountColumn, IDColumn.c_str(), border);
	int now = 0;
	ImGui::NextColumn();
	while (OrderCount >= now)
	{
		//ImGui::NextColumn();
		RenderComponents(now);
	}

	ImGui::Columns();
}

void BaseComponent::ChangeID(string ID)
{
	const_cast<string &>(this->ID) = ID;
}

void BaseComponent::MergeComponents(shared_ptr<AllTheComponent> Component)
{
	for (auto it: Component->Btn)
	{
		this->Component->Btn.push_back(it);
	}
	for (auto it: Component->childs)
	{
		this->Component->childs.push_back(it);
	}
	for (auto it: Component->CollpsHeader)
	{
		this->Component->CollpsHeader.push_back(it);
	}
	for (auto it: Component->column)
	{
		this->Component->column.push_back(it);
	}
	for (auto it: Component->combo)
	{
		this->Component->combo.push_back(it);
	}
	for (auto it: Component->Itext)
	{
		this->Component->Itext.push_back(it);
	}
	for (auto it: Component->Itextmul)
	{
		this->Component->Itextmul.push_back(it);
	}
	for (auto it: Component->Label)
	{
		this->Component->Label.push_back(it);
	}
	for (auto it: Component->selectable)
	{
		this->Component->selectable.push_back(it);
	}
	for (auto it: Component->separators)
	{
		this->Component->separators.push_back(it);
	}
	for (auto it: Component->Tabs)
	{
		this->Component->Tabs.push_back(it);
	}
	for (auto it: Component->TList)
	{
		this->Component->TList.push_back(it);
	}
	for (auto it: Component->TNode)
	{
		this->Component->TNode.push_back(it);
	}
	for (auto it: Component->UText)
	{
		this->Component->UText.push_back(it);
	}
}

shared_ptr<CollapsingHeaders> AllTheComponent::AddCollps(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (CollpsHeader.empty())
	{
		CollpsHeader.push_back(make_pair(ID, make_shared<CollapsingHeaders>(ID)));
		column.back().second->setComponents(make_shared<AllTheComponent>());
		return CollpsHeader.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentCHeader(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		CollpsHeader.push_back(make_pair(ID + to_string(CollpsHeader.size() + 1),
			make_shared<CollapsingHeaders>(ID + to_string(CollpsHeader.size() + 1))));
		return CollpsHeader.back().second;
	}

	CollpsHeader.push_back(make_pair(ID, make_shared<CollapsingHeaders>(ID)));
	CollpsHeader.back().second->setComponents(make_shared<AllTheComponent>());
	return CollpsHeader.back().second;
}

shared_ptr<Child> AllTheComponent::AddChild(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (childs.empty())
	{
		childs.push_back(make_pair(ID, make_shared<Child>(ID)));
		childs.back().second->setComponents(make_shared<AllTheComponent>());
		return childs.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentChild(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		childs.push_back(make_pair(ID + to_string(childs.size() + 1),
			make_shared<Child>(ID + to_string(childs.size() + 1))));
		return childs.back().second;
	}

	childs.push_back(make_pair(ID, make_shared<Child>(ID)));
	childs.back().second->setComponents(make_shared<AllTheComponent>());
	return childs.back().second;
}

shared_ptr<TreeNode> AllTheComponent::AddTNode(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (TNode.empty())
	{
		TNode.push_back(make_pair(ID, make_shared<TreeNode>(ID)));
		TNode.back().second->setComponents(make_shared<AllTheComponent>());
		return TNode.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentTreeNode(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		TNode.push_back(make_pair(ID + to_string(TNode.size() + 1),
			make_shared<TreeNode>(ID + to_string(TNode.size() + 1))));
		return TNode.back().second;
	}

	TNode.push_back(make_pair(ID, make_shared<TreeNode>(ID)));
	TNode.back().second->setComponents(make_shared<AllTheComponent>());
	return TNode.back().second;
}

shared_ptr<Tab> AllTheComponent::AddTab(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (Tabs.empty())
	{
		Tabs.push_back(make_pair(ID, make_shared<Tab>(ID)));
		Tabs.back().second->setComponents(make_shared<AllTheComponent>());
		return Tabs.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentTab(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		Tabs.push_back(make_pair(ID + to_string(Tabs.size() + 1),
			make_shared<Tab>(ID + to_string(Tabs.size() + 1))));
		return Tabs.back().second;
	}

	Tabs.push_back(make_pair(ID, make_shared<Tab>(ID)));
	Tabs.back().second->setComponents(make_shared<AllTheComponent>());
	return Tabs.back().second;
}

shared_ptr<Column> AllTheComponent::AddColumn(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (column.empty())
	{
		column.push_back(make_pair(ID, make_shared<Column>()));
		column.back().second->setComponents(make_shared<AllTheComponent>());
		return column.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentBtn(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		column.push_back(make_pair(ID + to_string(column.size() + 1),
			make_shared<Column>()));
		return column.back().second;
	}

	column.push_back(make_pair(ID, make_shared<Column>()));
	column.back().second->setComponents(make_shared<AllTheComponent>());
	return column.back().second;
}

shared_ptr<Selectable> AllTheComponent::AddSelectable(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (selectable.empty())
	{
		selectable.push_back(make_pair(ID, make_shared<Selectable>(ID)));
		selectable.back().second->setComponent(make_shared<AllTheComponent>());
		return selectable.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentBtn(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		selectable.push_back(make_pair(ID + to_string(selectable.size() + 1),
			make_shared<Selectable>(ID + to_string(selectable.size() + 1))));
		return selectable.back().second;
	}

	selectable.push_back(make_pair(ID, make_shared<Selectable>(ID)));
	selectable.back().second->setComponent(make_shared<AllTheComponent>());
	return selectable.back().second;
}

shared_ptr<Buttons> AllTheComponent::AddBtn(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (Btn.empty())
	{
		Btn.push_back(make_pair(ID, make_shared<Buttons>(ID)));
		Btn.back().second->setComponent(make_shared<AllTheComponent>());
		return Btn.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentBtn(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		Btn.push_back(make_pair(ID + to_string(Btn.size() + 1),
			make_shared<Buttons>(ID + to_string(Btn.size() + 1))));
		return Btn.back().second;
	}

	Btn.push_back(make_pair(ID, make_shared<Buttons>(ID)));
	Btn.back().second->setComponent(make_shared<AllTheComponent>());
	return Btn.back().second;
}

shared_ptr<TextList> AllTheComponent::AddTList(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (TList.empty())
	{
		TList.push_back(make_pair(ID, make_shared<TextList>(ID)));
		TList.back().second->setComponent(make_shared<AllTheComponent>());
		return TList.back().second;
	}
	
	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentTList(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		TList.push_back(make_pair(ID + to_string(TList.size() + 1),
			make_shared<TextList>(ID + to_string(TList.size() + 1))));
		return TList.back().second;
	}

	TList.push_back(make_pair(ID, make_shared<TextList>(ID)));
	TList.back().second->setComponent(make_shared<AllTheComponent>());
	return TList.back().second;
}

shared_ptr<Combobox> AllTheComponent::AddCombo(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (combo.empty())
	{
		combo.push_back(make_pair(ID, make_shared<Combobox>(ID)));
		combo.back().second->setComponent(make_shared<AllTheComponent>());
		return combo.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentCombo(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		combo.push_back(make_pair(ID + to_string(combo.size() + 1),
			make_shared<Combobox>(ID + to_string(combo.size() + 1))));
		return combo.back().second;
	}

	combo.push_back(make_pair(ID, make_shared<Combobox>(ID)));
	combo.back().second->setComponent(make_shared<AllTheComponent>());
	return combo.back().second;
}

shared_ptr<Labels> AllTheComponent::AddLabel(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (Label.empty())
	{
		Label.push_back(make_pair(ID, make_shared<Labels>(ID)));
		Label.back().second->setComponent(make_shared<AllTheComponent>());
		return Label.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentLabel(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		Label.push_back(make_pair(ID + to_string(Label.size() + 1),
			make_shared<Labels>(ID + to_string(Label.size() + 1))));
		return Label.back().second;
	}

	Label.push_back(make_pair(ID, make_shared<Labels>(ID)));
	Label.back().second->setComponent(make_shared<AllTheComponent>());
	return Label.back().second;
}

shared_ptr<IText> AllTheComponent::AddIText(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (Itext.empty())
	{
		Itext.push_back(make_pair(ID, make_shared<IText>(ID)));
		Itext.back().second->setComponent(make_shared<AllTheComponent>());
		return Itext.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentIText(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		Itext.push_back(make_pair(ID + to_string(Itext.size() + 1),
			make_shared<IText>(ID + to_string(Itext.size() + 1))));
		return Itext.back().second;
	}

	Itext.push_back(make_pair(ID, make_shared<IText>(ID)));
	Itext.back().second->setComponent(make_shared<AllTheComponent>());
	return Itext.back().second;
}

shared_ptr<ITextMulti> AllTheComponent::AddITextMul(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (Itextmul.empty())
	{
		Itextmul.push_back(make_pair(ID, make_shared<ITextMulti>(ID)));
		Itextmul.back().second->setComponent(make_shared<AllTheComponent>());
		return Itextmul.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentITextMul(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		Itextmul.push_back(make_pair(ID + to_string(Itextmul.size() + 1),
			make_shared<ITextMulti>(ID + to_string(Itextmul.size() + 1))));
		return Itextmul.back().second;
	}

	Itextmul.push_back(make_pair(ID, make_shared<ITextMulti>(ID)));
	Itextmul.back().second->setComponent(make_shared<AllTheComponent>());
	return Itextmul.back().second;
}

shared_ptr<UnformatedText> AllTheComponent::AddUText(string ID)
{
	if (ID.find("##") == string::npos)
		ID = "##" + ID;
	if (UText.empty())
	{
		UText.push_back(make_pair(ID, make_shared<UnformatedText>(ID)));
		UText.back().second->setComponent(make_shared<AllTheComponent>());
		return UText.back().second;
	}

	// Find the same ID and Change it to increase by #1
	auto TheSameID = FindComponentUText(ID, false);
	if (TheSameID && TheSameID->GetID() == ID)
	{
		UText.push_back(make_pair(ID + to_string(UText.size() + 1),
			make_shared<UnformatedText>(ID + to_string(UText.size() + 1))));
		return UText.back().second;
	}

	UText.push_back(make_pair(ID, make_shared<UnformatedText>(ID)));
	UText.back().second->setComponent(make_shared<AllTheComponent>());
	return UText.back().second;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT UI::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}
