#pragma once
#if !defined(__UI_H__)
#define __UI_H__
#include "pch.h"

#include "tinyxml2.h"
#include "imgui.h"
#include "Timer.h"

using namespace tinyxml2;

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
class Combobox;

static Vector3 GetColor(ImVec4 color)
{
	float R = 0.f, G = 0.f, B = 0.f;
	ImGui::ColorConvertHSVtoRGB(color.x, color.y, color.z, R, G, B);
	return Vector3(R, G, B);
}
static ImVec4 GetColorIm(ImVec4 color)
{
	float R = 0.f, G = 0.f, B = 0.f;
	ImGui::ColorConvertHSVtoRGB(color.x, color.y, color.z, R, G, B);
	return ImVec4(R, G, B, 1.f);
}

static Vector3 GetColor(Vector3 color)
{
	float R = 0.f, G = 0.f, B = 0.f;
	ImGui::ColorConvertHSVtoRGB(color.x, color.y, color.z, R, G, B);
	return Vector3(R, G, B);
}

struct AllTheComponent
{
	//			ID		Pointer to Component
	vector<pair<string, shared_ptr<Buttons>>> Btn;
	vector<pair<string, shared_ptr<Labels>>> Label;
	vector<pair<string, shared_ptr<ITextMulti>>> Itextmul;
	vector<pair<string, shared_ptr<IText>>> Itext;
	vector<pair<string, shared_ptr<_Separator>>> separators;
	vector<pair<string, shared_ptr<Column>>> column;
	vector<pair<string, shared_ptr<CollapsingHeaders>>> CollpsHeader;
	vector<pair<string, shared_ptr<Child>>> childs;
	vector<pair<string, shared_ptr<UnformatedText>>> UText;
	vector<pair<string, shared_ptr<TextList>>> TList;
	vector<pair<string, shared_ptr<TreeNode>>> TNode;
	vector<pair<string, shared_ptr<Tab>>> Tabs;
	vector<pair<string, shared_ptr<Selectable>>> selectable;
	vector<pair<string, shared_ptr<Combobox>>> combo;

	shared_ptr<Buttons> FindComponentBtn(string CmpName, bool NeedLog = true);
	shared_ptr<Labels> FindComponentLabel(string CmpName, bool NeedLog = true);
	shared_ptr<ITextMulti> FindComponentITextMul(string CmpName, bool NeedLog = true);
	shared_ptr<IText> FindComponentIText(string CmpName, bool NeedLog = true);
	shared_ptr<UnformatedText> FindComponentUText(string CmpName, bool NeedLog = true);
	shared_ptr<TextList> FindComponentTList(string CmpName, bool NeedLog = true);
	shared_ptr<Selectable> FindComponentSelectable(string CmpName, bool NeedLog = true);
	shared_ptr<Combobox> FindComponentCombo(string CmpName, bool NeedLog = true);
	shared_ptr<Column> FindComponentColumn(string CmpName, bool NeedLog);

	// For Recursive
	shared_ptr<TreeNode> FindComponentTreeNode(string CmpName, bool NeedLog = true);
	shared_ptr<Tab> FindComponentTab(string CmpName, bool NeedLog = true);
	shared_ptr<Child> FindComponentChild(string CmpName, bool NeedLog = true);
	shared_ptr<CollapsingHeaders> FindComponentCHeader(string CmpName, bool NeedLog = true);

	shared_ptr<CollapsingHeaders> AddCollps(string ID);
	shared_ptr<Child> AddChild(string ID);
	shared_ptr<TreeNode> AddTNode(string ID);
	shared_ptr<Tab> AddTab(string ID);
	shared_ptr<Column> AddColumn(string ID);
	shared_ptr<Selectable> AddSelectable(string ID);

	shared_ptr<Buttons> AddBtn(string ID);
	shared_ptr<TextList> AddTList(string ID);
	shared_ptr<Combobox> AddCombo(string ID);
	shared_ptr<Labels> AddLabel(string ID);
	shared_ptr<IText> AddIText(string ID);
	shared_ptr<ITextMulti> AddITextMul(string ID);
	shared_ptr<UnformatedText> AddUText(string ID);

	void RenderComponents(int &now);
	void RenderColumn(int OrderCount = 0, int CountColumn = 1, string IDColumn = "", bool border = true);
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

	vector<XMLNode *> combo;
	vector<int> IDcombo;

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

class BaseComponent
{
public:
	struct ColorText
	{
		Type type = Type::Normal;
		string str;

		ColorText(Type type, string str) : type(type), str(str) {}
	};

	void ChangeOrderInDial(int num) { OrderlyRenderInDial = num; }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeID(string ID);
	void ChangeText(string _Text) { Text = _Text; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	static void SetOnlyRenderID(bool b) { OnlyRenderID = b; }
	void MergeComponents(shared_ptr<AllTheComponent> Component);
	void setComponent(shared_ptr<AllTheComponent> _Component) { Component = _Component; }

	vector<ColorText> getCLText() { return clText; }
	void AddText(Type type, string str) { clText.push_back(ColorText(type, str)); }

	void ClearText()
	{
		Text.clear();
		clText.clear();
	}

	const string GetText()
	{
		if (OnlyRenderID)
		{
			// Create a new string to remove "#" and render it
			string NewString = ID;
			int Found = string::npos;
			if ((Found = NewString.find("#")) != string::npos)
				NewString.erase(Found, 1);
			return NewString;
		}
		return  Text;
	}

	const int getCountOrderRenderInDial() { return OrderlyRenderInDial; }
	const int getRenderOrder() { return OrderlyRender; }
	const string GetID() { return ID; }
	shared_ptr<AllTheComponent> getComponent() { return Component; }
	bool GetVisible() { return IsVisible; }

	bool IsClicked() { return clicked; }
	bool PressedEnter() { return pressEnter; }
protected:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	static bool OnlyRenderID;
	bool IsVisible = false,
		clicked = false,
		pressEnter = false;
	string Text;
	const string ID;
	shared_ptr<AllTheComponent> Component;
	vector<ColorText> clText;
};

class Combobox: public BaseComponent
{
public:
	static bool Combo(const char *label, int *currIndex, vector<string> &values)
	{
		if (values.empty())
			return false;
		return ImGui::Combo(label, currIndex, vector_getter, static_cast<void *>(&values), values.size());
	}

	Combobox() {}
	Combobox(string ID) { ChangeID(ID); }
	vector<string> GetItems() { return Items; }
	void AddItem(string Item) { Items.push_back(Item); }

	void ClearItems() { Items.clear(); }

	string GetSelect() { return Items.at(selected); }
	bool IsMouseSelected() { return Active; }

	void Render()
	{
		Combo(GetText().c_str(), &selected, Items);
		Active = ImGui::IsItemActive();
	}
private:
	int selected = 0;
	bool Active = false;
	vector<string> Items = { "None" };
};
class Selectable: public BaseComponent
{
public:
	Selectable() {}
	Selectable(string ID) { ChangeID(ID); }

	bool GetSelect() { return selected; }

	void Render()
	{
		ImGui::Selectable(GetText().c_str(), selected);
	}
private:
	bool selected = false;
};
struct TabItem
{
	vector<string> Name;
	vector<shared_ptr<AllTheComponent>> TabItemComp;
};
class TextList: public BaseComponent
{
public:
	static bool ListBox(const char *label, int *currIndex, vector<string> &values)
	{
		return ImGui::ListBox(label, currIndex, vector_getter, static_cast<void *>(&values), values.size());
	}

	TextList() {}
	TextList(string ID) { ChangeID(ID); }

	void addItem(string Item) { Items.push_back(Item); }

	bool FindInItems(string Item)
	{
		for (size_t i = 0; i < Items.size(); i++)
		{
			if (Items.at(i) == Item)
				return true;
		}

		return false;
	}

	bool IsMouseSelected() { return Active; }

	//	Current
	int getSelectedIndx() { return Selected; }
	//	Get Current Selected Index String
	string getSelectedIndxString(int Index)
	{
		if (Items.empty() || (Index < 0 || Index >= (int)Items.size()))
			return "";
			
		return Items.at(Index);
	}
	auto getItems() { return Items; }

	void clearItems() { Items.clear(); }

	void Render()
	{
		if (IsVisible)
			ListBox(GetText().c_str(), &Selected, Items);

		Active = ImGui::IsItemActive();
	}

private:
	vector<string> Items;

	int Selected = -1;
	bool Active = false;
};
class _Separator: public BaseComponent
{
public:
	void Render()
	{
		ImGui::Separator();
	}
};
class UnformatedText: public BaseComponent
{
public:
	UnformatedText() {}
	UnformatedText(string ID) { this->ChangeID(ID); }

	ColorText getString(string _Text)
	{
		for (size_t i = 0; i < clText.size(); i++)
		{
			if (clText.at(i).str == _Text)
				return clText.at(i);
		}
	
		return ColorText(Type::Normal, "###");
	}
	void Render();
};

class Tab: public BaseComponent
{
public:
	void setDragTabs(bool _DragTabs) { DragTabs = _DragTabs; }
	void setCloseMidMouse(bool _CloseMidMouse) { CloseMidMouse = _CloseMidMouse; }
	void setASelectNewTab(bool _ASelectNewTab) { ASelectNewTab = _ASelectNewTab; }

	vector<shared_ptr<TabItem>> getTabItem() { return TBItm; }
	void setComponents(shared_ptr<AllTheComponent> _Component) { Components.push_back(_Component); }
	vector<shared_ptr<AllTheComponent>> getMassComponents() { return Components; }

	Tab() {}
	Tab(string ID, bool DragTabs = false, bool ASelectNewTab = false, bool CloseMidMouse = false) :
		DragTabs(DragTabs), ASelectNewTab(ASelectNewTab), CloseMidMouse(CloseMidMouse)
	{
		ChangeID(ID);
	}

	void Render();
private:
	bool DragTabs = false, ASelectNewTab = false, CloseMidMouse = false;
	ImVec2 size = { 0.f, 0.f };

	ImGuiTabBarFlags Flags = 0;
	vector<shared_ptr<TabItem>> TBItm{ make_shared<TabItem>() };
	vector<shared_ptr<AllTheComponent>> Components;
};
class TreeNode: public BaseComponent
{
public:
	TreeNode() {}
	TreeNode(string ID, bool HasFlag = false) : HasFlags(HasFlag)
	{
		ChangeID(ID);
	}
	void setComponents(shared_ptr<AllTheComponent> _Component) { Components.push_back(_Component); }
	vector<shared_ptr<AllTheComponent>> getMassComponents() { return Components; }

	void Render();
private:
	bool HasFlags = false;

	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<AllTheComponent>> Components;
};
class CollapsingHeaders: public BaseComponent
{
public:
	void setCollapse(bool Collapse) { IsCollapse = Collapse; }
	void setSelDefault(bool _SelDef) { SelDef = _SelDef; }

	bool Collapse() { return IsCollapse; }

	CollapsingHeaders() {}
	CollapsingHeaders(string ID, bool SelDef = false, bool IsCollapse = true) : SelDef(SelDef),
		IsCollapse(IsCollapse)
	{
		ChangeID(ID);
	}
	void setComponents(shared_ptr<AllTheComponent> _Component) { Components.push_back(_Component); }
	vector<shared_ptr<AllTheComponent>> getMassComponents() { return Components; }

	void Render();
private:
	bool SelDef = false, IsCollapse = true;
	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<AllTheComponent>> Components;
};
class Child: public BaseComponent
{
public:
	void setHScroll(bool HScroll) { IsHScroll = HScroll; }
	void setSize(ImVec2 Size) { size = Size; }
	void setBorder(bool Border) { IsBorder = Border; }
	void setAutoScroll(bool AScrl) { AutoScroll = AScrl; }

	Child() {}
	Child(string ID, ImVec2 size = { 0, 0 }, bool IsHScroll = false, bool IsBorder = false) :
		IsHScroll(IsHScroll), IsBorder(IsBorder), size(size)
	{
		ChangeID(ID);
	}
	void setComponents(shared_ptr<AllTheComponent> _Component) { Components.push_back(_Component); }
	vector<shared_ptr<AllTheComponent>> getMassComponents() { return Components; }

	void Render();
private:
	bool IsHScroll = false, IsBorder = false, AutoScroll = false;
	ImVec2 size = { 0.f, 0.f };

	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<AllTheComponent>> Components;
};
class Column: public BaseComponent
{
public:
	Column() {}
	Column(int CountColumn) : CountColumn(CountColumn) {}
	void setComponents(shared_ptr<AllTheComponent> _Component) { Components.push_back(_Component); }
	vector<shared_ptr<AllTheComponent>> getMassComponents() { return Components; }

	int getCountColumn() { return CountColumn; }
	bool GetBorder() { return Border; }

	void ChangeCountColumn(int Count) { CountColumn = Count; }

	void SetBorder(bool _Border) { Border = _Border; }
private:
	int CountColumn = 0;
	bool Border = false;
	vector<shared_ptr<AllTheComponent>> Components;
};

class IText: public BaseComponent
{
public:
	void ChangeTextHint(string _Text) { TextHint = _Text; }

	void setHistory(bool History) { IsNeedHistory = History; }
	void setHint(bool NeedHint) { IsNeedHint = NeedHint; }

	bool isActive() { return Active; }
	
	bool getTextChange() { return IsTextChange; }
	bool getHistory() { return IsNeedHistory; }

	IText() {}
	IText(string ID, bool IsVisible = true, bool IsNeedHistory = false, bool NeedToUseTAB = false, 
		bool EnterReturnsTrue = true):
		IsNeedHistory(IsNeedHistory),
		NeedToUseTAB(NeedToUseTAB), EnterReturnsTrue(EnterReturnsTrue)
	{
		ChangeID(ID);
		setVisible(IsVisible);
	}

	void Render();
private:
	string TextHint;

	bool IsNeedHistory = false,
		NeedToUseTAB = false, EnterReturnsTrue = true,
		IsTextChange = false, IsNeedHint = false,
		Active = false;

	ImGuiInputTextFlags Flags = 0;
};
class ITextMulti: public BaseComponent
{
	struct ColorText;
public:
	void setReadOnly(bool _ReadOnly) { ReadOnly = _ReadOnly; }

	ITextMulti() {}
	ITextMulti(string ID, bool IsVisible = true, bool ReadOnly = false, bool IsCtrlNewLine = false):
		ReadOnly(ReadOnly), IsCtrlNewLine(IsCtrlNewLine)
	{
		ChangeID(ID);
		setVisible(IsVisible);
	}

	void Render();
private:
	bool ReadOnly = false, IsCtrlNewLine = false;

	ImGuiInputTextFlags Flags = 0;
};
class Labels: public BaseComponent
{
public:
	
	void SetColorText(ImVec4 _Color)
	{
		Color = _Color;
		NeedToChangeColor = true;
	}

	Labels() {}
	Labels(string ID, bool IsVisible = true)
	{
		ChangeID(ID);
		setVisible(IsVisible);
	}

	void Render()
	{
		if (NeedToChangeColor)
			ImGui::PushStyleColor(ImGuiCol_Text, GetColorIm(Color));

		ImGui::Text(GetText().c_str());

		if (NeedToChangeColor)
			ImGui::PopStyleColor();
	}
private:
	bool NeedToChangeColor = false;
	ImVec4 Color = ImVec4(0.f, 0.f, 0.f, 1.f);
};
class Buttons: public BaseComponent
{
public:
	Buttons() {}
	Buttons(string ID, bool IsVisible = true)
	{
		ChangeID(ID);
		setVisible(IsVisible);
	}

	void Render()
	{
		if (ImGui::Button(GetText().c_str()))
			clicked = true;
		else
			clicked = false;
	}
};
class dialogs
{
public:
	void ChangeTitle(string Title) { IDTitle = Title; }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeSize(float W, float H) { SizeW = W; SizeH = H; }
	void ChangePosition(float X, float Y, ImVec2 _Pivot = { 0.f, 0.f })
	{
		PosX = X; PosY = Y;
		Pivot = _Pivot;
	}
	void ChangeFont(string FontName, float SizePixel = 14.0f, float Brighten = -1.f);

	void SetShowTitle(bool Show) { ShowTitle = Show; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	string GetTitle() { return IDTitle; }

	bool getVisible() { return IsVisible; }
	bool getIsFullScreen() { return IsFullScreen; }

	bool getIsClosed() { return IsClosed; }

	int getOrderCount() { return OrderlyRender; }

	void setResizeble(bool Resizeble) { IsResizeble = Resizeble; }
	void setMoveble(bool Moveble) { IsMoveble = Moveble; }
	void setCollapsible(bool Collapsible) { IsCollapsible = Collapsible; }
	void setBringToFont(bool BringToFont) { IsNeedBringToFont = BringToFont; }
	void setFullScreen(bool FullScreen) { IsFullScreen = FullScreen; }

	shared_ptr<AllTheComponent> getComponents() { return Component; }
	auto GetCurrentWindow() { return ImGui::GetCurrentWindow(); }

	dialogs() {}
	dialogs(string IDTitle): IDTitle(IDTitle) {}
	dialogs(string IDTitle, bool IsVisible, bool ShowTitle, bool IsMoveble, bool IsKeyboardSupport, bool IsResizeble,
		bool IsCollapsible, bool IsNeedBringToFont, bool IsFullScreen, float SizeW = 0.f, float SizeH = 0.f):
		IDTitle(IDTitle), IsVisible(IsVisible), IsKeyboardSupport(IsKeyboardSupport), IsMoveble(IsMoveble),
		IsResizeble(IsResizeble), IsCollapsible(IsCollapsible), ShowTitle(ShowTitle), IsNeedBringToFont(IsNeedBringToFont),
		IsFullScreen(IsFullScreen), SizeW(SizeW), SizeH(SizeH)
	{
		ImGui::StyleColorsClassic();
	}

	void Render();
private:
	string IDTitle;
	bool IsKeyboardSupport = false,
		ShowTitle = false,
		IsMoveble = false,
		IsResizeble = false,
		IsCollapsible = false,
		IsNeedBringToFont = false,
		IsFullScreen = false,
		IsVisible = false,
		IsClosed = false;

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

	HRESULT LoadFileUI(string File);
	void ProcessXML();

	void ReloadXML(string File);

	void DisableDialog(string IDDialog);
	void EnableDialog(string IDDialog);

	shared_ptr<dialogs> getDialog(string IDDialog);

	auto GetIO() { return &ImGui::GetIO(); }

	static void ResizeWnd();
	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void HelpMarker(const char *desc, ImVec4 Color = ImVec4(1.f, 1.f, 1.f, 1.f))
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			ImGui::PushStyleColor(ImGuiCol_Text, GetColorIm(Color));

			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();

			ImGui::PopStyleColor();

			ImGui::EndTooltip();
		}
	}

	// Get Windows Default Dialog Open
		// return pair::first bool when it clicks on the OK btn and pair::second vector<string>
		// when has some path files are there
	static pair<bool, vector<string>> GetWndDlgOpen(LPSTR DirByDef = "C://",
		LPSTR NameOfWnd = "Dialog Open Files", LPSTR FilterFilesExt = "Proj Files\0*.proj\0All\0*.*\0", bool MultiSelect = false);
	// Get Windows Default Dialog Save
		// return pair::first bool when it clicks on the OK btn and pair::second vector<string>
		// when has some path files are there
	static pair<bool, vector<string>> GetWndDlgSave(LPSTR DirByDef = "C://",
		LPSTR NameOfWnd = "Dialog Save Files", LPCSTR FilterFilesExt = "Proj Files\0*.proj\0All\0*.*\0");

	// Set All The Component's name to ID (Using for debug)
	static void SetRenderOnlyID(bool b);

	shared_ptr<Timer> getThread() { return UIThread; }
protected:
	// **********
	HRESULT hr = S_OK;

	// **********
	vector<shared_ptr<dialogs>> Dialogs;

	shared_ptr<Timer> UIThread = make_shared<Timer>();
	// **********
	bool InitUI = false;

	// **********
	shared_ptr<tinyxml2::XMLDocument> doc;

	vector<shared_ptr<XMLDial>> XMLDialogs;

	void WorkOnComponents(shared_ptr<XMLComponents> Component, shared_ptr<AllTheComponent> DoneComponent, int &CountOrder);

	void XMLPreparing(shared_ptr<XMLDial> InDial, XMLNode *everything, int &countComp);
	void XMLPreparingCollps(shared_ptr<XMLDial> InCHead, XMLNode *everything, int &countComp);
	void XMLPreparingChild(shared_ptr<XMLDial> InChild, XMLNode *everything, int &countComp);
	void XMLPreparingTNode(shared_ptr<XMLDial> InTNode, XMLNode *everything, int &countComp);
	void XMLPreparingTab(shared_ptr<XMLDial> InTab, XMLNode *everything, int &countComp);
	void XMLPreparingColumn(shared_ptr<XMLDial> InColumn, XMLNode *everything, int &countComp);
	void XMLPreparingRecursion(shared_ptr<XMLComponents> InCHead, XMLNode *everything, int &countComp);

	void GetParam(XMLNode *Nods, shared_ptr<TreeNode> InTNode);
	void GetParam(XMLNode *Nods, shared_ptr<Tab> InTab);
	void GetParam(XMLNode *Nods, shared_ptr<CollapsingHeaders> InCollaps);
	void GetParam(XMLNode *Nods, shared_ptr<Child> InChild);
	void GetParam(XMLNode *Nods, shared_ptr<Column> column);
	void GetParam(XMLNode *Nods, shared_ptr<Selectable> select);

	void GetParam(XMLElement *Nods, shared_ptr<Buttons> btn);
	void GetParam(XMLElement *Nods, shared_ptr<TextList> TList);
	void GetParam(XMLElement *Nods, shared_ptr<Combobox> Combo);
	void GetParam(XMLElement *Nods, shared_ptr<Labels> Label);
	void GetParam(XMLElement *Nods, shared_ptr<IText> Itext);
	void GetParam(XMLElement *Nods, shared_ptr<ITextMulti> ItextMul);
	void GetParam(XMLElement *Nods, shared_ptr<UnformatedText> UText);

	void GetRecursion(vector<XMLNode *> SomeComponents, int &countComponents, shared_ptr<XMLComponents> SomeComponent);
	void GetRecursionForAddComponents(shared_ptr<dialogs> RequiredComponent, shared_ptr<XMLComponents> SomeComponent);
	void GetRecursionAdd(shared_ptr<XMLComponents> SomeComponent, shared_ptr<AllTheComponent> AllComponent,
		int &Count);
};
#endif // !__UI_H__