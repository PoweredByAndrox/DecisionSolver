#pragma once
#if !defined(__UI_H__)
#define __UI_H__
#include "pch.h"

#include "File_system.h"
#include "tinyxml2.h"
#include "imgui.h"

using namespace tinyxml2;

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Render_Buffer.h"

#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"

static auto vector_getter = [](void *vec, int idx, const char **out_text)
{
	vector<string> &Vector = *static_cast<vector<string> *>(vec);
	if (idx < 0 || idx >= static_cast<int>(Vector.size()))
		return false;
	*out_text = Vector.at(idx).c_str();
	return true;
};

class Buttons;
class Labels;
class ITextMulti;
class IText;
class _Separator;
class CollapsingHeaders;
class Child;
class UnformatedText;
class TextList;
class TreeNode;
class Tab;
class Column;
class Selectable;
struct AllTheComponent
{
	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<ITextMulti>> Itextmul;
	vector<shared_ptr<IText>> Itext;
	vector<shared_ptr<_Separator>> separators;
	vector<shared_ptr<Column>> column;
	vector<shared_ptr<CollapsingHeaders>> CollpsHeader;
	vector<shared_ptr<Child>> childs;
	vector<shared_ptr<UnformatedText>> UText;
	vector<shared_ptr<TextList>> TList;
	vector<shared_ptr<TreeNode>> TNode;
	vector<shared_ptr<Tab>> Tabs;
	vector<shared_ptr<Selectable>> selectable;

	void RenderComponents(int &now);
	void RenderColumn(int OrderCount, int CountColumn, LPCSTR IDColumn = "", bool border = true);
};
struct XMLComponents;
struct TItem
{
	vector<shared_ptr<XMLComponents>> Component;
	int OrderlyRenderInDial = 0;
	vector<string> TabItems;
};
struct XMLComponents
{
	vector<XMLNode *> buttons;
	vector<int> IDbuttons;

	vector<XMLNode *> labels;
	vector<int> IDlabels;

	vector<XMLNode *> texts;
	vector<int> IDtexts;

	vector<XMLNode *> textmuls;
	vector<int> IDtextmuls;

	vector<XMLNode *> separators;
	vector<int> IDseparators;

	vector<XMLNode *> utext;
	vector<int> IDutext;

	vector<XMLNode *> tlist;
	vector<int> IDtlist;

	vector<XMLNode *> select;
	vector<int> IDselect;

	vector<shared_ptr<XMLComponents>> CpsHead;
	vector<XMLNode *> XMLCHead;
	vector<shared_ptr<XMLComponents>> DialChild;
	vector<XMLNode *> XMLDChild;
	vector<shared_ptr<TItem>> _Tab;
	vector<XMLNode *> XML_Tab;
	vector<shared_ptr<XMLComponents>> _TreeNode;
	vector<XMLNode *> XML_TreeNode;
	vector<shared_ptr<XMLComponents>> _Column;
	vector<XMLNode *> XMLColumn;

	// It's only for render here!
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
};
struct XMLDial
{
	XMLNode *Dial = nullptr;

	shared_ptr<XMLComponents> Components = make_shared<XMLComponents>();

	int OrderlyRender = 0;
	int getCountOrder() { return OrderlyRender; }

	XMLDial(XMLNode *dial): Dial(dial) {}
};

class Selectable
{
public:
	void ChangeOrder(int num) { OrderlyRender = num; }
	int getRenderOrder() { return OrderlyRender; }
	LPCSTR GetID() { return ID; }
	bool GetSelect() { return selected; }

	void ChangeID(LPCSTR ID) { this->ID = ID; }

	Selectable() {}
	~Selectable() {}

	void Render()
	{
		ImGui::Selectable(ID, selected);
	}
private:
	int OrderlyRender = 0;
	LPCSTR ID = "";
	bool selected = false;
};
struct TabItem
{
	vector<string> Name;
	vector<shared_ptr<AllTheComponent>> TabItemComp;
};
class Column
{
public:
	int getCountColumn() { return CountColumn; }
	int getRenderOrder() { return OrderlyRender; }
	int getCountOrderRenderInDial() { return OrderlyRenderInDial; }
	LPCSTR GetID() { return ID; }
	bool GetBorder() { return Border; }

	shared_ptr<AllTheComponent> getComponent() { return Component; }
	void setComponents(shared_ptr<AllTheComponent> Component) { this->Component = Component; }

	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeOrderInDial(int num) { OrderlyRenderInDial = num; }
	void ChangeCountColumn(int Count) { CountColumn = Count; }
	void ChangeID(LPCSTR ID) { this->ID = ID; }

	void SetBorder(bool Border) { this->Border = Border; }

	Column(int CountColumn): CountColumn(CountColumn) {}

	Column() {}
	~Column() {}

private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0, CountColumn = 0;
	LPCSTR ID = "";
	bool Border = false;
	shared_ptr<AllTheComponent> Component = make_shared<AllTheComponent>();
};
class Tab
{
public:
	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeOrderInDial(int num) { OrderlyRenderInDial = num; }

	void setDragTabs(bool DragTabs) { this->DragTabs = DragTabs; }
	void setCloseMidMouse(bool CloseMidMouse) { this->CloseMidMouse = CloseMidMouse; }
	void setASelectNewTab(bool ASelectNewTab) { this->ASelectNewTab = ASelectNewTab; }

	int getCountOrderRenderInDial() { return OrderlyRenderInDial; }
	vector<shared_ptr<TabItem>> getComponent() { return Component; }

	Tab() {}
	~Tab() {}

	Tab(LPCSTR IDTitle, bool DragTabs, bool ASelectNewTab, bool CloseMidMouse):
		IDTitle(IDTitle), DragTabs(DragTabs), ASelectNewTab(ASelectNewTab), CloseMidMouse(CloseMidMouse) {}

	void Render();
private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	string IDTitle = "";
	bool DragTabs = false, ASelectNewTab = false, CloseMidMouse = false;
	ImVec2 size = { 0.f, 0.f };

	ImGuiTabBarFlags Flags = 0;

	vector<shared_ptr<TabItem>> Component = { make_shared<TabItem>() };
};
class TreeNode
{
public:
	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeOrderInDial(int num) { OrderlyRenderInDial = num; }

	int getCountOrderRenderInDial() { return OrderlyRenderInDial; }
	int getRenderOrder() { return OrderlyRender; }

	shared_ptr<AllTheComponent> getComponent() { return Component; }
	void setComponents(shared_ptr<AllTheComponent> Component) { this->Component = Component; }

	TreeNode() {}
	~TreeNode() {}

	TreeNode(LPCSTR IDTitle, bool HasFlag = false): IDTitle(IDTitle), HasFlags(HasFlag) {}

	void Render();
private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	string IDTitle = "";
	bool HasFlags = false;

	ImGuiTreeNodeFlags Flags = 0;

	shared_ptr<AllTheComponent> Component = make_shared<AllTheComponent>();
};
class TextList
{
private:
	//bool Combo(const char *label, int *currIndex, vector<string> &values)
	//{
	//	if (values.empty())
	//		return false;
	//	return ImGui::Combo(label, currIndex, vector_getter, static_cast<void *>(&values), values.size());
	//}
	bool ListBox(const char *label, int *currIndex, vector<string> &values)
	{
		if (values.empty())
			return false;
		return ImGui::ListBox(label, currIndex, vector_getter, static_cast<void *>(&values), values.size());
	}

public:
	void ChangeOrder(int num) { OrderlyRender = num; }
	void addItem(string Item) { Items.push_back(Item); }
	void setVisible(bool Visible) { IsVisible = Visible; }
	void ChangeTitle(string Text) { IDTitle = Text; }

	bool FindInItems(string Item)
	{
		for (size_t i = 0; i < Items.size(); i++)
		{
			if (strcmp(Items.at(i).c_str(), Item.c_str()) == 0)
				return true;
		}

		return false;
	}

	TextList() {}
	~TextList() {}

	bool GetVisible() { return IsVisible; }
	bool IsMouseSelected() { return Active; }

	int getRenderOrder() { return OrderlyRender; }
	LPCSTR GetTitle() { return IDTitle.c_str(); }
	//	Current
	int getSelectedIndx() { return Selected; }
	//	Get Current Selected Index String
	string getSelectedIndxString(int Index)
	{
		if (Items.empty() || (Index < 0 || Index >= (int)Items.size()))
			return string("");
			
		return Items.at(Index);
	}
	auto getItems() { return Items; }

	void clearItems() { Items.clear(); }

	void Render()
	{
		if (IsVisible)
			ListBox(IDTitle.c_str(), &Selected, Items);

		Active = ImGui::IsItemActive();
	}

private:
	string IDTitle = "";
	vector<string> Items;

	int OrderlyRender = 0, Selected = -1;
	bool IsVisible = false, Active = false;
};
class _Separator
{
public:
	void ChangeOrder(int num) { OrderlyRender = num; }
	int getRenderOrder() { return OrderlyRender; }

	_Separator() {}
	~_Separator() {}

	void Render()
	{
		ImGui::Separator();
	}
private:
	int OrderlyRender = 0;
};
class UnformatedText
{
public:
	class ColorText
	{
	public:
		ColorText() {}
		~ColorText() {}

		ColorText(Type type, string CText): type(type), CText(CText) {}

		Type getType() { return type; }
		string getText() { return CText; }
	private:
		Type type = Normal;
		string CText = "";

	};
	vector<shared_ptr<ColorText>> clText;

	void ClearText()
	{
		Buffer.clear();
		clText.clear();
	}
	vector<shared_ptr<ColorText>> getCLText() { return clText; }
	void AddCLText(Type type, string str)
	{
			// Check if we typed the same string
		for (size_t i = 0; i < clText.size(); i++)
		{
			if (clText.at(i)->getText() == str)
			{
				addTextToBuffer(str);
				return;
			}
		}

			// Else add him our color text buffer and type the string in log
		clText.push_back(make_shared<ColorText>(type, str));
		addTextToBuffer(str);
	}

	shared_ptr<ColorText> getString(string Text)
	{
		for (size_t i = 0; i < clText.size(); i++)
		{
			if ((clText.at(i)->getText() + string("\n")) == Text)
				return clText.at(i);
		}
	
		return make_shared<ColorText>();
	}

	void ChangeOrder(int num) { OrderlyRender = num; }
	int getRenderOrder() { return OrderlyRender; }
	LPCSTR getBuffer() { return Buffer.data()->c_str(); }

	void addTextToBuffer(string Text)
	{
		Buffer.push_back(Text + string("\n"));
	}

	UnformatedText() {}
	~UnformatedText() {}

	void Render();
private:
	int OrderlyRender = 0;
	vector<string> Buffer;
};
class IText
{
public:
	void ChangeText(string Text) { this->Text = Text; }
	void ChangeTextHint(string Text) { TextHint = Text; }
	void ChangeTitle(string Text) { IDTitle = Text; }
	void ChangeOrder(int num) { OrderlyRender = num; }

	void setVisible(bool Visible) { IsVisible = Visible; }
	void setHistory(bool History) { IsNeedHistory = History; }
	void setHint(bool NeedHint) { IsNeedHint = NeedHint; }

	bool isPressUp() { return Application->getTrackerKeyboard().IsKeyPressed(Keyboard::Up); }
	bool isPressDown() { return Application->getTrackerKeyboard().IsKeyPressed(Keyboard::Down); }
	bool isActive() { return Active; }

	string GetText()
	{
		if (!Text.empty())
			return Text;

		return string("");
	}

	string GetTitle() { return IDTitle; }

	bool GetVisible() { return IsVisible; }
	bool getTextChange() { return IsTextChange; }
	int getRenderOrder() { return OrderlyRender; }

	IText() {}
	~IText() {}

	IText(LPCSTR IDTitle, bool IsVisible, bool IsNeedHistory = false, bool NeedToUseTAB = false, 
		bool EnterReturnsTrue = true, bool IsNeedHint = false):
		IDTitle(IDTitle), IsVisible(IsVisible), IsNeedHistory(IsNeedHistory),
		NeedToUseTAB(NeedToUseTAB), EnterReturnsTrue(EnterReturnsTrue) {}

	void Render();
private:
	string IDTitle = "", Text = "", TextHint = "";

	bool IsVisible = false, IsNeedHistory = false,
		NeedToUseTAB = false, EnterReturnsTrue = true,
		IsTextChange = false, IsNeedHint = false,
		Active = false;
	int OrderlyRender = 0;

	ImGuiInputTextFlags Flags = 0;
};
class ITextMulti
{
	struct ColorText;
public:
	enum Type
	{
		Normal = 0,
		Information,
		Error
	} type = Normal;

	void ChangeText(string Text) { this->Text.append(string(string("\n") + Text).c_str()); }
	void ChangeTitle(string Text) { IDTitle = Text; }
	void ChangeOrder(int num) { OrderlyRender = num; }

	void setVisible(bool Visible) { IsVisible = Visible; }
	void setReadOnly(bool ReadOnly) { this->ReadOnly = ReadOnly; }

	vector<ColorText> getCLText() { return clText; }
	void AddCLText(Type type, string str)
	{
		if (clText.empty())
			clText.push_back(ColorText(type, str));

		for (size_t i = 0; i < clText.size(); i++)
		{
			if (clText.at(i).type == type && clText.at(i).str == str)
				return;
			else
				clText.push_back(ColorText(type, str));
		}
	}

	LPCSTR GetTitle() { return IDTitle.c_str(); }
	LPCSTR GetText() { return Text.c_str(); }
	void ClearText() 
	{
		Text.clear();
		clText.clear();
	}

	bool GetVisible() { return IsVisible; }
	int getRenderOrder() { return OrderlyRender; }

	ITextMulti() {}
	~ITextMulti() {}

	ITextMulti(LPCSTR IDTitle, bool IsVisible, bool ReadOnly = false, bool IsCtrlNewLine = false):
		IDTitle(IDTitle), IsVisible(IsVisible), ReadOnly(ReadOnly), IsCtrlNewLine(IsCtrlNewLine)
	{}

	void Render();
private:
	string IDTitle = "", Text = "";
	bool IsVisible = false, ReadOnly = false, IsCtrlNewLine = false;
	int OrderlyRender = 0;

	ImGuiInputTextFlags Flags = 0;
	struct ColorText
	{
		Type type = Normal;
		string str = "";

		ColorText(Type type, string str): type(type), str(str) {}
	};

	vector<ColorText> clText;
};
class Labels
{
public:
	void ChangeText(string Text) { IDTitle = Text; }
	void ChangeOrder(int num) { OrderlyRender = num; }

	void setVisible(bool Visible) { IsVisible = Visible; }

	LPCSTR GetText() { return IDTitle.c_str(); }

	bool GetVisible() { return IsVisible; }

	int getRenderOrder() { return OrderlyRender; }

	void SetColorText(ImVec4 Color)
	{
		this->Color = Color;
		NeedToChangeColor = true;
	}

	Labels() {}
	~Labels() {}

	Labels(LPCSTR IDTitle, bool IsVisible): IDTitle(IDTitle), IsVisible(IsVisible) {}

	void Render()
	{
		if (NeedToChangeColor)
			ImGui::PushStyleColor(ImGuiCol_Text, Color);
			
		ImGui::Text(IDTitle.c_str());

		if (NeedToChangeColor)
			ImGui::PopStyleColor();
	}
private:
	string IDTitle = "";
	bool IsVisible = false, NeedToChangeColor = false;
	int OrderlyRender = 0;
	ImVec4 Color = ImVec4(0.f, 0.f, 0.f, 1.f);
};
class Buttons
{
public:
	void ChangeText(LPCSTR Text) { IDTitle = Text; }

	LPCSTR GetText() { return IDTitle; }

	bool GetVisible() { return IsVisible; }
	int getRenderOrder() { return OrderlyRender; }

	void ChangeOrder(int num) { OrderlyRender = num; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	bool IsClicked() { return clicked; }

	Buttons() {}
	~Buttons() {}

	Buttons(LPCSTR IDTitle, bool IsVisible): IDTitle(IDTitle), IsVisible(IsVisible) {}

	void Render()
	{
		if (ImGui::Button(IDTitle))
			clicked = true;
		else
			clicked = false;
	}
private:
	LPCSTR IDTitle = "";
	bool IsVisible = false, clicked = false;
	int OrderlyRender = 0;
};
class CollapsingHeaders
{
public:
	void ChangeText(string Text) { IDTitle = Text; }
	string GetText() { return IDTitle; }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeOrderInDial(int num) { OrderlyRenderInDial = num; }

	void setCollapse(bool Collapse) { IsCollapse = Collapse; }
	void setSelDefault(bool SelDef) { this->SelDef = SelDef; }

	int getCountOrderRenderInDial() { return OrderlyRenderInDial; }
	int getRenderOrder() { return OrderlyRender; }

	shared_ptr<AllTheComponent> getComponent() { return Component; }
	void setComponents(shared_ptr<AllTheComponent> Component) { this->Component = Component; }

	bool Collapse() { return IsCollapse; }
	
	CollapsingHeaders() {}
	~CollapsingHeaders() {}

	CollapsingHeaders(LPCSTR IDTitle, bool SelDef, bool IsCollapse = true): IDTitle(IDTitle), SelDef(SelDef), IsCollapse(IsCollapse) {}

	void Render();
private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	string IDTitle = "";
	bool SelDef = false, IsCollapse = true;
	ImGuiTreeNodeFlags Flags = 0;

	shared_ptr<AllTheComponent> Component = make_shared<AllTheComponent>();
};
class Child
{
public:
	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeOrderInDial(int num) { OrderlyRenderInDial = num; }

	void setHScroll(bool HScroll) { IsHScroll = HScroll; }
	void setSize(ImVec2 size) { this->size = size; }
	void setBorder(bool Border) { IsBorder = Border; }

	int getCountOrderRenderInDial() { return OrderlyRenderInDial; }

	shared_ptr<AllTheComponent> getComponent() { return Component; }
	void setComponents(shared_ptr<AllTheComponent> Component) { this->Component = Component; }

	Child() {}
	~Child() {}

	Child(LPCSTR IDTitle, ImVec2 size, bool IsHScroll = false, bool IsBorder = false):
		IDTitle(IDTitle), IsHScroll(IsHScroll), IsBorder(IsBorder), size(size) {}

	void Render();
private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	string IDTitle = "";
	bool IsHScroll = false, IsBorder = false;
	ImVec2 size = { 0.f, 0.f };

	ImGuiTreeNodeFlags Flags = 0;

	shared_ptr<AllTheComponent> Component = make_shared<AllTheComponent>();
};
class dialogs
{
public:
	void ChangeTitle(LPCSTR Title) { IDTitle = Title; }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeSize(float W, float H) { SizeW = W; SizeH = H; }
	void ChangePosition(float X, float Y, ImVec2 Pivot = { 0.f, 0.f })
	{
		PosX = X; PosY = Y;
		this->Pivot = Pivot;
	}
	void ChangeFont(string FontName, float SizePixel = 14.0f, float Brighten = -1.f);

	void SetShowTitle(bool Show) { ShowTitle = Show; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	string GetTitle() { return IDTitle; }

	bool getVisible() { return IsVisible; }
	int getOrderCount() { return OrderlyRender; }

	void setResizeble(bool Resizeble) { IsResizeble = Resizeble; }
	void setMoveble(bool Moveble) { IsMoveble = Moveble; }
	void setCollapsible(bool Collapsible) { IsCollapsible = Collapsible; }
	void setBringToFont(bool BringToFont) { IsNeedBringToFont = BringToFont; }

	shared_ptr<AllTheComponent> getComponents() { return Component; }
	auto GetCurrentWindow() { return ImGui::GetCurrentWindow(); }

	dialogs() {}
	~dialogs() {}

	dialogs(LPCSTR IDTitle): IDTitle(IDTitle) {}
	dialogs(LPCSTR IDTitle, bool IsVisible, bool ShowTitle, bool IsMoveble, bool IsKeyboardSupport, bool IsResizeble, bool IsCollapsible, int style,
		bool IsNeedBringToFont, float SizeW = 0.f, float SizeH = 0.f):
		IDTitle(IDTitle), IsVisible(IsVisible), IsKeyboardSupport(IsKeyboardSupport), IsMoveble(IsMoveble), IsResizeble(IsResizeble),
		IsCollapsible(IsCollapsible), ShowTitle(ShowTitle), IsNeedBringToFont(IsNeedBringToFont), SizeW(SizeW), SizeH(SizeH)
	{
		ImGuiIO &io = ImGui::GetIO();
		if (IsKeyboardSupport)
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsClassic();
	}

	void Render();
private:
	string IDTitle = "";
	bool IsVisible = false,
		IsKeyboardSupport = false,
		ShowTitle = false,
		IsMoveble = false,
		IsResizeble = false,
		IsCollapsible = false,
		IsNeedBringToFont = false;

	ImGuiWindowFlags window_flags = 0;

	shared_ptr<AllTheComponent> Component = make_shared<AllTheComponent>();

	ImFont *Font = nullptr;

	int OrderlyRender = 0;
	float SizeW = 400.f, SizeW_Last, SizeH = 250.f, SizeH_Last,
		PosX = 0.f, PosX_Last, PosY = 0.f, PosY_Last;
	ImVec2 Pivot;
};

class UI
{
public:
	HRESULT Init();

	void Begin();
	void FrameEnd();

	void Destroy();

	bool IsInitUI() { return InitUI; }

	auto getDialogs() { return Dialogs; }

	HRESULT LoadXmlUI(string File);
	void ProcessXML();

	void ReloadXML(LPCSTR File);

	void DisableDialog(LPCSTR IDDialog);
	void EnableDialog(LPCSTR IDDialog);

	shared_ptr<dialogs> getDialog(LPCSTR IDDialog);

	auto GetIO() { return &ImGui::GetIO(); }

	static void ResizeWnd();
	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	UI() {}
	~UI() {}
protected:
	// **********
	HRESULT hr = S_OK;

	// **********
	vector<shared_ptr<dialogs>> Dialogs;

	// **********
	bool InitUI = false;

	// **********
	shared_ptr<tinyxml2::XMLDocument> doc;

	vector<shared_ptr<XMLDial>> XMLDialogs;

	INT64 g_Time = 0, g_TicksPerSecond = 0;
	ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;
	bool g_HasGamepad = false, g_WantUpdateHasGamepad = true;
	void Gamepads();
	static bool UpdateMouseCursor();
	void UpdateMousePos();

	void WorkOnComponents(shared_ptr<XMLComponents> Component, shared_ptr<AllTheComponent> &DoneComponent, int &CountOrder);

	void XMLPreparing(shared_ptr<XMLDial> &InDial, XMLNode *everything, int &countComp);
	void XMLPreparingCollps(shared_ptr<XMLDial> &InCHead, XMLNode *everything, int &countComp);
	void XMLPreparingChild(shared_ptr<XMLDial> &InChild, XMLNode *everything, int &countComp);
	void XMLPreparingTNode(shared_ptr<XMLDial> &InTNode, XMLNode *everything, int &countComp);
	void XMLPreparingTab(shared_ptr<XMLDial> &InTab, XMLNode *everything, int &countComp);
	void XMLPreparingRecursion(shared_ptr<XMLComponents> &InCHead, XMLNode *everything, int &countComp);

	void GetParam(XMLNode *Nods, shared_ptr<TreeNode> &InTNode);
	void GetParam(XMLNode *Nods, shared_ptr<Tab> &InTab);
	void GetParam(XMLNode *Nods, shared_ptr<CollapsingHeaders> &InCollaps);
	void GetParam(XMLNode *Nods, shared_ptr<Child> &InChild);
	void GetParam(XMLNode *Nods, shared_ptr<Column> &column);
	void GetParam(XMLNode *Nods, shared_ptr<Selectable> &select);

	void GetParam(XMLElement *Nods, shared_ptr<Buttons>& btn);
	void GetParam(XMLElement *Nods, shared_ptr<TextList> &TList);
	void GetParam(XMLElement *Nods, shared_ptr<Labels> &Label);
	void GetParam(XMLElement *Nods, shared_ptr<IText> &Itext);
	void GetParam(XMLElement *Nods, shared_ptr<ITextMulti> &ItextMul);

	void GetRecursion(vector<XMLNode *> SomeComponents, int &countComponents, shared_ptr<XMLComponents> &SomeComponent);
	void GetRecursionForAddComponents(shared_ptr<dialogs> &RequiredComponent, shared_ptr<XMLComponents> &SomeComponent);
	void GetRecursionAdd(shared_ptr<XMLComponents> SomeComponent, shared_ptr<AllTheComponent> &AllComponent,
		int &Count);
};
#endif // !__UI_H__