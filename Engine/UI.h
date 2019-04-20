#pragma once
#if !defined(__UI_H__)
#define __UI_H__
#include "pch.h"
#pragma comment(lib, "xinput")

#include "File_system.h"
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace ImGui;

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Render_Buffer.h"

#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"

static auto vector_getter = [](void *vec, int idx, const char **out_text)
{
	std::vector<string> &vector = *static_cast<std::vector<string> *>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size()))
		return false;
	*out_text = vector.at(idx).c_str();
	return true;
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
		for (int i = 0; i < Items.size(); i++)
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
		if (Items.empty() || (Index < 0 || Index >= Items.size()))
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
	enum Type
	{
		Normal = 0,
		Information,
		Error
	} type = Normal;
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
		for (int i = 0; i < clText.size(); i++)
		{
			if (clText.at(i)->getText() == str)
			{
				addTextToBuffer(str);
				return;
			}
		}

			// Else add him our color text buffer and type the string in log
		clText.push_back(make_unique<ColorText>(type, str));
		addTextToBuffer(str);
	}

	shared_ptr<ColorText> getString(string Text)
	{
		for (int i = 0; i < clText.size(); i++)
		{
			if ((clText.at(i)->getText() + string("\n")) == Text)
				return clText.at(i);
		}
	
		return make_unique<ColorText>();
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

	void Render()
	{
		for (int i = 0; i < Buffer.size(); i++)
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

	IText(LPCSTR IDTitle, bool IsVisible, bool IsNeedHistory = false, bool NeedToUseTAB = false, bool EnterReturnsTrue = true, bool IsNeedHint = false):
		IDTitle(IDTitle), IsVisible(IsVisible), IsNeedHistory(IsNeedHistory), NeedToUseTAB(NeedToUseTAB), EnterReturnsTrue(EnterReturnsTrue)
	{}

	void Render()
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

		for (int i = 0; i < clText.size(); i++)
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

	void Render()
	{
		Flags = 0;
		Flags = ImGuiInputTextFlags_AllowTabInput;

		if (ReadOnly)
			Flags |= ImGuiInputTextFlags_ReadOnly;

		if (IsCtrlNewLine)
			Flags |= ImGuiInputTextFlags_CtrlEnterForNewLine;
	//	Flags |= ImGuiInputTextFlags_CallbackResize;

		if (clText.empty())
			ImGui::InputTextMultiline(IDTitle.c_str(), &Text, ImVec2(0, 0), Flags);
		else
		{
			for (int i = 0; i < clText.size(); i++)
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
private:
	string IDTitle = "", Text = "";
	bool IsVisible = false, ReadOnly = false, IsCtrlNewLine = false;
	int OrderlyRender = 0;

	ImGuiInputTextFlags Flags = 0;
	struct ColorText
	{
		Type type = Normal;
		string str = "";

		ColorText(Type type, string str) : type(type), str(str) {}
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

	Labels(LPCSTR IDTitle, bool IsVisible) : IDTitle(IDTitle), IsVisible(IsVisible) {}

	void Render()
	{
		if (NeedToChangeColor)
			ImGui::PushStyleColor(ImGuiCol_Text, Color);//ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			
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
class Child;
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

	bool Collapse() { return IsCollapse; }

	vector<shared_ptr<Labels>> getLabels() { return Label; }
	vector<shared_ptr<Buttons>> getButtons() { return Btn; }
	vector<shared_ptr<IText>> getITexts() { return Itext; }
	vector<shared_ptr<ITextMulti>> getITextMultis() { return Itextmul; }
	vector<shared_ptr<_Separator>> getSeparators() { return separators; }
	vector<shared_ptr<Child>> getChilds() { return childs; }
	vector<shared_ptr<UnformatedText>> getUTexts() { return UText; }
	vector<shared_ptr<TextList>> getTextLists() { return TList; }

	void setComponent(shared_ptr<Buttons> Btn)
	{
		this->Btn.push_back(Btn);
	}
	void setComponent(shared_ptr<Labels> Label)
	{
		this->Label.push_back(Label);
	}
	void setComponent(shared_ptr<IText> Itext)
	{
		this->Itext.push_back(Itext);
	}
	void setComponent(shared_ptr<ITextMulti> Itextmul)
	{
		this->Itextmul.push_back(Itextmul);
	}
	void setComponent(shared_ptr<_Separator> separator)
	{
		this->separators.push_back(separator);
	}
	void setComponent(shared_ptr<Child> child)
	{
		this->childs.push_back(child);
	}
	void setComponent(shared_ptr<CollapsingHeaders> CHeader)
	{
		this->CollpsHeader.push_back(CHeader);
	}
	void setComponent(shared_ptr<UnformatedText> UText)
	{
		this->UText.push_back(UText);
	}
	void setComponent(shared_ptr< TextList> TList)
	{
		this->TList.push_back(TList);
	}
	
	CollapsingHeaders() {}
	~CollapsingHeaders() {}

	CollapsingHeaders(LPCSTR IDTitle, bool SelDef, bool IsCollapse = true) : IDTitle(IDTitle), SelDef(SelDef), IsCollapse(IsCollapse) {}

	void Render();
private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	string IDTitle = "";
	bool SelDef = false, IsCollapse = true;
	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<ITextMulti>> Itextmul;
	vector<shared_ptr<IText>> Itext;
	vector<shared_ptr<_Separator>> separators;
	vector<shared_ptr<Child>> childs;
	vector<shared_ptr<CollapsingHeaders>> CollpsHeader;
	vector<shared_ptr<UnformatedText>> UText;
	vector<shared_ptr<TextList>> TList;
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

	vector<shared_ptr<Labels>> getLabels() { return Label; }
	vector<shared_ptr<Buttons>> getButtons() { return Btn; }
	vector<shared_ptr<IText>> getITexts() { return Itext; }
	vector<shared_ptr<ITextMulti>> getITextMultis() { return Itextmul; }
	vector<shared_ptr<Child>> getChilds() { return childs; }
	vector<shared_ptr<CollapsingHeaders>> getCHeaders() { return CollpsHeader; }
	vector<shared_ptr<UnformatedText>> getUTexts() { return UText; }
	vector<shared_ptr<TextList>> getTLists() { return TList; }

	void setComponent(shared_ptr<Buttons> Btn)
	{
		this->Btn.push_back(Btn);
	}
	void setComponent(shared_ptr<Labels> Label)
	{
		this->Label.push_back(Label);
	}
	void setComponent(shared_ptr<IText> Itext)
	{
		this->Itext.push_back(Itext);
	}
	void setComponent(shared_ptr<ITextMulti> Itextmul)
	{
		this->Itextmul.push_back(Itextmul);
	}
	void setComponent(shared_ptr<_Separator> seporator)
	{
		this->seporators.push_back(seporator);
	}
	void setComponent(shared_ptr<CollapsingHeaders> CollapsingHeaders)
	{
		this->CollpsHeader.push_back(CollapsingHeaders);
	}
	void setComponent(shared_ptr<Child> child)
	{
		this->childs.push_back(child);
	}
	void setComponent(shared_ptr<UnformatedText> UText)
	{
		this->UText.push_back(UText);
	}
	void setComponent(shared_ptr<TextList> TList)
	{
		this->TList.push_back(TList);
	}

	Child() {}
	~Child() {}

	Child(LPCSTR IDTitle, ImVec2 size, bool IsHScroll = false, bool IsBorder = false) :
		IDTitle(IDTitle), IsHScroll(IsHScroll), IsBorder(IsBorder), size(size) {}

	void Render()
	{
		Flags = 0;

		if (IsHScroll)
			Flags = ImGuiWindowFlags_HorizontalScrollbar;

		if (ImGui::BeginChild(IDTitle.c_str(), size, IsBorder, Flags))
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

				for (int i = 0; i < seporators.size(); i++)
				{
					if (seporators.at(i)->getRenderOrder() == now)
						seporators.at(i)->Render();
				}

				for (int i = 0; i < CollpsHeader.size(); i++)
				{
					if (CollpsHeader.at(i)->Collapse() && CollpsHeader.at(i)->getCountOrderRenderInDial() == now)
						CollpsHeader.at(i)->Render();
				}

				for (int i = 0; i < childs.size(); i++)
				{
					if (childs.at(i)->getCountOrderRenderInDial() == now)
						childs.at(i)->Render();
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

		ImGui::EndChild();
	}
private:
	int OrderlyRender = 0, OrderlyRenderInDial = 0;
	string IDTitle = "";
	bool IsHScroll = false, IsBorder = false;
	ImVec2 size = { 0.f, 0.f };

	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<ITextMulti>> Itextmul;
	vector<shared_ptr<IText>> Itext;
	vector<shared_ptr<_Separator>> seporators;
	vector<shared_ptr<CollapsingHeaders>> CollpsHeader;
	vector<shared_ptr<Child>> childs;
	vector<shared_ptr<UnformatedText>> UText;
	vector<shared_ptr<TextList>> TList;
};
class dialogs
{
public:
	void ChangeTitle(LPCSTR Title) { IDTitle = Title; }
	void ChangeOrder(int num) { OrderlyRender = num; }
	void ChangeSize(float W, float H)
	{
		SizeW = W;
		SizeH = H;
	}

	void SetShowTitle(bool Show) { ShowTitle = Show; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	string GetTitle() { return IDTitle; }

	bool getVisible() { return IsVisible; }
	int getOrderCount() { return OrderlyRender; }

	void setResizeble(bool Resizeble) { IsResizeble = Resizeble; }
	void setMoveble(bool Moveble) { IsMoveble = Moveble; }
	void setCollapsible(bool Collapsible) { IsCollapsible = Collapsible; }
	void setBringToFont(bool BringToFont) { IsNeedBringToFont = BringToFont; }

	vector<shared_ptr<Labels>> getLabels() { return Label; }
	vector<shared_ptr<Buttons>> getButtons() { return Btn; }
	vector<shared_ptr<CollapsingHeaders>> getCollapsHeaders() { return CollpsHeader; }
	vector<shared_ptr<IText>> getITexts() { return Itext; }
	vector<shared_ptr<ITextMulti>> getITextMultis() { return Itextmul; }
	vector<shared_ptr<Child>> getChilds() { return child; }
	vector<shared_ptr<UnformatedText>> getUTexts() { return UText; }
	vector<shared_ptr<TextList>> getTLists() { return TList; }
	vector<shared_ptr<TextList>> getTextLists() { return TList; }

	void setComponent(shared_ptr<Buttons> Btn)
	{
		this->Btn.push_back(Btn);
	}
	void setComponent(shared_ptr<Labels> Label)
	{
		this->Label.push_back(Label);
	}
	void setComponent(shared_ptr<CollapsingHeaders> CollapsingHeaders)
	{
		this->CollpsHeader.push_back(CollapsingHeaders);
	}
	void setComponent(shared_ptr<IText> Itext)
	{
		this->Itext.push_back(Itext);
	}
	void setComponent(shared_ptr<ITextMulti> Itextmul)
	{
		this->Itextmul.push_back(Itextmul);
	}
	void setComponent(shared_ptr<_Separator> separator)
	{
		this->separator.push_back(separator);
	}
	void setComponent(shared_ptr<Child> child)
	{
		this->child.push_back(child);
	}
	void setComponent(shared_ptr<UnformatedText> UText)
	{
		this->UText.push_back(UText);
	}
	void setComponent(shared_ptr< TextList> TList)
	{
		this->TList.push_back(TList);
	}

	auto GetCurrentWindow() { return ImGui::GetCurrentWindow(); }

	dialogs() {}
	~dialogs() {}

	dialogs(LPCSTR IDTitle): IDTitle(IDTitle) {}
	dialogs(LPCSTR IDTitle, bool IsVisible, bool ShowTitle, bool IsMoveble, bool IsKeyboardSupport, bool IsResizeble, bool IsCollapsible, int style,
		bool IsNeedBringToFont, float SizeW = 0.f, float SizeH = 0.f):
		IDTitle(IDTitle), IsVisible(IsVisible), IsKeyboardSupport(IsKeyboardSupport), style(style), IsMoveble(IsMoveble), IsResizeble(IsResizeble),
		IsCollapsible(IsCollapsible), ShowTitle(ShowTitle), IsNeedBringToFont(IsNeedBringToFont), SizeW(SizeW), SizeH(SizeH)
	{
		ImGuiIO &io = GetIO();
		if (IsKeyboardSupport)
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		//if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		//if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;

		//if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		//if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		StyleColorsClassic();
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

	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<CollapsingHeaders>> CollpsHeader;
	vector<shared_ptr<IText>> Itext;
	vector<shared_ptr<ITextMulti>> Itextmul;
	vector<shared_ptr<_Separator>> separator;
	vector<shared_ptr<Child>> child;
	vector<shared_ptr<UnformatedText>> UText;
	vector<shared_ptr<TextList>> TList;

	int style = 1, // <-- This is a test variable.
		OrderlyRender = 0;
	float SizeW = 400.f, SizeW_Last, SizeH = 250.f, SizeH_Last;
};

class UI
{
public:
	HRESULT Init();

	void Begin();
	void Render();
	void FrameEnd();

	void Destroy();

	bool IsInitUI() { return InitUI; }

	auto getDialogs() { return Dialogs; }

	HRESULT LoadXmlUI(string File);
	void ProcessXML();

	void ReloadXML(LPCSTR File);

	HRESULT addDialog(LPCSTR IDName);
	HRESULT addButton(LPCSTR IDName, LPCSTR IDDialog = "");
	HRESULT addLabel(LPCSTR IDName, LPCSTR IDDialog = "");
	HRESULT addCollapseHead(LPCSTR IDName, LPCSTR IDDialog = "", bool SelDef = false, bool Collapse = false);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<Labels>);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<Buttons>);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<CollapsingHeaders>);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<IText>);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<ITextMulti>);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<_Separator> separator);
	HRESULT addComponentToCollapseHead(LPCSTR IDColpsHead, LPCSTR IDDialog, shared_ptr<TextList> separator);

	void DisableDialog(LPCSTR IDDialog);
	void EnableDialog(LPCSTR IDDialog);

	shared_ptr<dialogs> getDialog(LPCSTR IDDialog);

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
	bool InitUI = false, Reload = false;

	// **********
	int iY = 10;

	// **********
	shared_ptr<tinyxml2::XMLDocument> doc;
	//unique_ptr<Render_Buffer> Buf = make_unique<Render_Buffer>();

	struct collpheader
	{
		vector<XMLNode *> buttons;
		vector<int> IDbuttons;

		vector<XMLNode *> labels;
		vector<int> IDlabels;

		vector<XMLNode *> texts;
		vector<int> IDtexts;

		vector<XMLNode *> textmuls;
		vector<int> IDtextmuls;

		vector<XMLNode *> collpheaders;
		vector<int> IDcollpheaders;

		vector<XMLNode *> childs;
		vector<int> IDchilds;

		vector<XMLNode *> separators;
		vector<int> IDseparators;

		vector<XMLNode *> utext;
		vector<int> IDutext;

		vector<XMLNode *> tlist;
		vector<int> IDtlist;

		XMLNode *CollpsHead = nullptr;

		int OrderlyRender = 0;
		int getCountOrder() { return OrderlyRender; }

		collpheader(XMLNode *Component, bool Main = false)
		{
			setComponentCHeader(Component, Main);
		}

		void setComponentCHeader(XMLNode *Component, bool Main = false)
		{
			if (strcmp(Component->Value(), "Button") == 0)
				buttons.push_back(Component);
			else if (strcmp(Component->Value(), "Label") == 0)
				labels.push_back(Component);
			else if (strcmp(Component->Value(), "InputText") == 0)
				texts.push_back(Component);
			else if (strcmp(Component->Value(), "InputTextMultiline") == 0)
				textmuls.push_back(Component);
			else if (strcmp(Component->Value(), "ChildDialog") == 0)
				childs.push_back(Component);
			else if (strcmp(Component->Value(), "Separator") == 0)
				separators.push_back(Component);
			else if (strcmp(Component->Value(), "Collapse") == 0)
			{
				if (Main)
				{
					if (!CollpsHead)
						CollpsHead = Component;
					return;
				}

				collpheaders.push_back(Component);
			}
			else if (strcmp(Component->Value(), "UnformatedText") == 0)
				utext.push_back(Component);
		}
	};
	struct child
	{
		vector<XMLNode *> buttons;
		vector<int> IDbuttons;

		vector<XMLNode *> labels;
		vector<int> IDlabels;

		vector<XMLNode *> texts;
		vector<int> IDtexts;

		vector<XMLNode *> textmuls;
		vector<int> IDtextmuls;

		vector<XMLNode *> collpheaders;
		vector<int> IDcollpheaders;

		vector<XMLNode *> childs;
		vector<int> IDchilds;

		vector<XMLNode *> separators;
		vector<int> IDseparators;

		vector<XMLNode *> utext;
		vector<int> IDutext;

		vector<XMLNode *> tlist;
		vector<int> IDtlist;

		XMLNode *_Child = nullptr;

		int OrderlyRender = 0;
		int getCountOrder() { return OrderlyRender; }

		child(XMLNode *Component, bool Main = false)
		{
			setComponentChild(Component, Main);
		}

		void setComponentChild(XMLNode *Component, bool Main = false)
		{
			if (strcmp(Component->Value(), "Button") == 0)
				buttons.push_back(Component);
			else if (strcmp(Component->Value(), "Label") == 0)
				labels.push_back(Component);
			else if (strcmp(Component->Value(), "InputText") == 0)
				texts.push_back(Component);
			else if (strcmp(Component->Value(), "InputTextMultiline") == 0)
				textmuls.push_back(Component);
			else if (strcmp(Component->Value(), "Separator") == 0)
				separators.push_back(Component);
			else if (strcmp(Component->Value(), "Collapse") == 0)
				collpheaders.push_back(Component);
			else if (strcmp(Component->Value(), "ChildDialog") == 0)
			{
				if (Main)
				{
					if (!_Child)
						_Child = Component;
					return;
				}
					childs.push_back(Component);
			}
			else if (strcmp(Component->Value(), "UnformatedText") == 0)
				utext.push_back(Component);
			else if (strcmp(Component->Value(), "ListBox") == 0)
				tlist.push_back(Component);
		}
	};

	// **********
	struct dial
	{
		vector<XMLNode *> buttons;
		vector<int> IDbuttons;

		vector<XMLNode *> labels;
		vector<int> IDlabels;

		vector<XMLNode *> texts;
		vector<int> IDtexts;

		vector<XMLNode *> textmuls;
		vector<int> IDtextmuls;

		vector<shared_ptr<collpheader>> collpheaders;
		vector<int> IDcollpheaders;

		vector<shared_ptr<child>> childs;
		vector<int> IDchilds;

		vector<XMLNode *> separators;
		vector<int> IDseparators;

		vector<XMLNode *> utext;
		vector<int> IDutext;

		vector<XMLNode *> tlist;
		vector<int> IDtlist;

		XMLNode *Dial = nullptr;

		int OrderlyRender = 0;
		int getCountOrder() { return OrderlyRender; }

		dial(XMLNode *dial): Dial(dial) {}

		void setComponentChild(XMLNode *Component)
		{
			if (strcmp(Component->Value(), "Button") == 0)
				buttons.push_back(Component);
			else if (strcmp(Component->Value(), "Label") == 0)
				labels.push_back(Component);
			else if (strcmp(Component->Value(), "InputText") == 0)
				texts.push_back(Component);
			else if (strcmp(Component->Value(), "InputTextMultiline") == 0)
				textmuls.push_back(Component);
			else if (strcmp(Component->Value(), "ChildDialog") == 0)
				childs.push_back(make_unique<child>(Component));
			else if (strcmp(Component->Value(), "Separator") == 0)
				separators.push_back(Component);
			else if (strcmp(Component->Value(), "Collapse") == 0)
				collpheaders.push_back(make_unique<collpheader>(Component));
			else if (strcmp(Component->Value(), "UnformatedText") == 0)
				utext.push_back(Component);
			else if (strcmp(Component->Value(), "ListBox") == 0)
				tlist.push_back(Component);
		}
	};

	vector<shared_ptr<dial>> XMLDialogs;

	void StackTrace(const char *Error)
	{
		DebugTrace("***********ERROR IN XML FILE***********\n");
		DebugTrace("===Check info below:\n");
		DebugTrace(string(string("... ") + string(Error) + string(" ...")).c_str());
		DebugTrace("***********ERROR IN XML FILE***********\n");
	}
	INT64 g_Time = 0, g_TicksPerSecond = 0;
	ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;
	bool g_HasGamepad = false, g_WantUpdateHasGamepad = true;
	void Gamepads();
	static bool UpdateMouseCursor();
	void UpdateMousePos();

	void WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<Buttons> &btn, int &CountOrder);
	void WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<Labels> &Label, int &CountOrder);
	void WorkOnComponents(shared_ptr<dialogs> &dialog, shared_ptr<collpheader> XMLCHeader, shared_ptr<CollapsingHeaders> &CHeader,
		int &CountOrder);
	void WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<IText> &Itext, int &CountOrder);
	void WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<ITextMulti> &ItextMul, int &CountOrder);
	void WorkOnComponents(shared_ptr<dialogs> &dialog, XMLElement *element, shared_ptr<TextList> &TList, int &CountOrder);
	void WorkOnComponents(shared_ptr<dialogs> &dialog, shared_ptr<child> XMLchild, shared_ptr<Child> &child, int &CountOrder);

	void WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<Buttons> &btn, int &CountOrder);
	void WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<Labels> &Label, int &CountOrder);
	void WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<IText> &Itext, int &CountOrder);
	void WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<ITextMulti> &ItextMul, int &CountOrder);
	void WorkOnComponents(shared_ptr<Child> &InChild, XMLElement *element, shared_ptr<TextList> &TList, int &CountOrder);

	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<Buttons> &btn, int &CountOrder);
	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<Labels> &Label, int &CountOrder);
	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<IText> &Itext, int &CountOrder);
	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<ITextMulti> &ItextMul, int &CountOrder);
	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, XMLElement *element, shared_ptr<TextList> &TList, int &CountOrder);

	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, shared_ptr<child> XMLchild, shared_ptr<Child> &child, int &CountOrder);
	void WorkOnComponents(shared_ptr<CollapsingHeaders> &InCollaps, shared_ptr<collpheader> XMLCHeader, shared_ptr<CollapsingHeaders> &CHeader,
		int &CountOrder);
};
#endif // !__UI_H__