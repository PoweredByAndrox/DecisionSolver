#pragma once
#if !defined(__UI_H__)
#define __UI_H__
#include "pch.h"
#pragma comment(lib, "xinput")

#include "File_system.h"
#include "tinyxml2.h"
#include "examples\imgui_impl_dx11.h"
#include "examples/imgui_impl_win32.h"

using namespace tinyxml2;
using namespace ImGui;

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Render_Buffer.h"

#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"

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

	LPCSTR GetText()
	{
		ImGuiInputTextState *state = nullptr;
		if (state = &ImGui::GetCurrentContext()->InputTextState)
			return const_cast<const char *>(state->TextA.Data);

		return "";
	}
	LPCSTR GetTitle() { return IDTitle.c_str(); }

	bool GetVisible() { return IsVisible; }
	bool getTextChange() { return IsTextChange; }
	int getRenderOrder() { return OrderlyRender; }

	IText() {}
	~IText() {}

	IText(LPCSTR IDTitle, bool IsVisible, bool IsNeedHistory = false, bool NeedToUseTAB = false, bool EnterReturnsTrue = false, bool IsNeedHint = false):
		IDTitle(IDTitle), IsVisible(IsVisible), IsNeedHistory(IsNeedHistory), NeedToUseTAB(NeedToUseTAB), EnterReturnsTrue(EnterReturnsTrue)
	{}

	void Render()
	{
		Flags = 0;

		if (IsNeedHistory)
			Flags |= ImGuiInputTextFlags_CallbackHistory;

		else if (EnterReturnsTrue)
			Flags |= ImGuiInputTextFlags_EnterReturnsTrue;

		else if (NeedToUseTAB)
			Flags |= ImGuiInputTextFlags_CallbackCompletion;

		if (IsNeedHint)
			if (ImGui::InputTextWithHint(IDTitle.c_str(), TextHint.c_str(), &Text))
				IsTextChange = true;
			else
				IsTextChange = false;
		else
			if (ImGui::InputText(IDTitle.c_str(), &Text, Flags))
				IsTextChange = true;
			else
				IsTextChange = false;
	}

private:
	string IDTitle = "", Text = "", TextHint = "";

	bool IsVisible = false, IsNeedHistory = false,
		NeedToUseTAB = false, EnterReturnsTrue = false,
		IsTextChange = false, IsNeedHint = false;
	int OrderlyRender = 0;

	ImGuiInputTextFlags Flags = 0;
};
class ITextMulti
{
public:
	void ChangeText(string Text) { this->Text.assign(Text); }
	void ChangeTitle(string Text) { IDTitle = Text; }
	void ChangeOrder(int num) { OrderlyRender = num; }

	void setVisible(bool Visible) { IsVisible = Visible; }
	void setReadOnly(bool ReadOnly) { this->ReadOnly = ReadOnly; }

	LPCSTR GetTitle() { return IDTitle.c_str(); }
	LPCSTR GetText() { return Text.c_str(); }

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

		else if (IsCtrlNewLine)
			Flags |= ImGuiInputTextFlags_CtrlEnterForNewLine;
	//	Flags |= ImGuiInputTextFlags_CallbackResize;

		ImGui::InputTextMultiline(IDTitle.c_str(), &Text, ImVec2(0, 0), Flags);
	}
private:
	string IDTitle = "", Text = "";
	bool IsVisible = false, ReadOnly = false, IsCtrlNewLine = false;
	int OrderlyRender = 0;

	ImGuiInputTextFlags Flags = 0;

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
class CollapsingHeaders
{
public:
	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }
	void ChangeOrder(int num) { OrderlyRender = num; }

	void setCollapse(bool Collapse) { IsCollapse = Collapse; }
	void setSelDefault(bool SelDef) { this->SelDef = SelDef; }

	int getCountOrderRender() { return OrderlyRender; }
	bool Collapse() { return IsCollapse; }

	vector<shared_ptr<Labels>> getLabels() { return Label; }
	vector<shared_ptr<Buttons>> getButtons() { return Btn; }
	vector<shared_ptr<IText>> getITexts() { return Itext; }
	vector<shared_ptr<ITextMulti>> getITextMultis() { return Itextmul; }

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

	CollapsingHeaders() {}
	~CollapsingHeaders() {}

	CollapsingHeaders(LPCSTR IDTitle, bool SelDef, bool IsCollapse = true) : IDTitle(IDTitle), SelDef(SelDef), IsCollapse(IsCollapse) {}

	void Render()
	{
		Flags = 0;
		Flags = ImGuiTreeNodeFlags_CollapsingHeader;

		if (SelDef)
			Flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::CollapsingHeader(IDTitle.c_str(), &IsCollapse, Flags))
		{
			int Count = this->getCountOrderRender(), now = 0;

			while (Count != now)
			{
				now++;

				for (int i = 0; i < Label.size(); i++)
					if (Label.at(i)->GetVisible() & Label.at(i)->getRenderOrder() == now)
						Label.at(i)->Render();

				for (int i = 0; i < Btn.size(); i++)
					if (Btn.at(i)->GetVisible() & Btn.at(i)->getRenderOrder() == now)
						Btn.at(i)->Render();

				for (int i = 0; i < Itextmul.size(); i++)
					if (Itextmul.at(i)->GetVisible() & Itextmul.at(i)->getRenderOrder() == now)
						Itextmul.at(i)->Render();

				for (int i = 0; i < Itext.size(); i++)
					if (Itext.at(i)->GetVisible() & Itext.at(i)->getRenderOrder() == now)
						Itext.at(i)->Render();
			}
		}
	}
private:
	int OrderlyRender = 0;
	string IDTitle = "";
	bool SelDef = false, IsCollapse = true;
	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<ITextMulti>> Itextmul;
	vector<shared_ptr<IText>> Itext;
};
class dialogs
{
public:
	void ChangeTitle(LPCSTR Title) { IDTitle = Title; }
	void ChangeSize(float W, float H)
	{
		SizeW = W;
		SizeH = H;
	}
	void ChangeOrder(int num) { OrderlyRender = num; }

	void SetShowTitle(bool Show) { ShowTitle = Show; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	LPCSTR GetTitle() { return IDTitle; }

	bool getVisible() { return IsVisible; }
	int getOrderCount() { return OrderlyRender; }

	void setSizeW(float W) { this->SizeW = W; }
	void setSizeH(float H) { this->SizeH = H; }
	void setResizeble(bool Resizeble) { IsResizeble = Resizeble; }
	void setMoveble(bool Moveble) { IsMoveble = Moveble; }
	void setCollapsible(bool Collapsible) { IsCollapsible = Collapsible; }

	vector<shared_ptr<Labels>> getLabels() { return Label; }
	vector<shared_ptr<Buttons>> getButtons() { return Btn; }
	vector<shared_ptr<CollapsingHeaders>> getCollapsHeaders() { return CollpsHeader; }
	vector<shared_ptr<IText>> getITexts() { return Itext; }
	vector<shared_ptr<ITextMulti>> getITextMultis() { return Itextmul; }

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

	dialogs() {}
	~dialogs() {}

	dialogs(LPCSTR IDTitle): IDTitle(IDTitle) {}
	dialogs(LPCSTR IDTitle, bool IsVisible, bool ShowTitle, bool IsMoveble, bool IsKeyboardSupport, bool IsResizeble, bool IsCollapsible, int style,
		float SizeW = 0.f, float SizeH = 0.f):
		IDTitle(IDTitle), IsVisible(IsVisible), IsKeyboardSupport(IsKeyboardSupport), style(style), IsMoveble(IsMoveble), IsResizeble(IsResizeble),
		IsCollapsible(IsCollapsible), ShowTitle(ShowTitle), SizeW(SizeW), SizeH(SizeH)
	{
		ImGuiIO &io = GetIO();
		if (IsKeyboardSupport)
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		//if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		//if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;

		//if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		//if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		//if (style == 0)
		//{
		//	this->style = style;
			StyleColorsClassic();
		//}
		//else if (style == 1)
		//{
		//	this->style = style;
		//	StyleColorsDark();
		//}
	}

	void Render()
	{
		window_flags = 0;

		if (!ShowTitle)
			window_flags |= ImGuiWindowFlags_NoTitleBar;
		else if (!IsMoveble)
			window_flags |= ImGuiWindowFlags_NoMove;
		else if (!IsResizeble)
			window_flags |= ImGuiWindowFlags_NoResize;
		else if (!IsCollapsible)
			window_flags |= ImGuiWindowFlags_NoCollapse;

		if (IsVisible)
		{
			ChangeSize(this->SizeW, this->SizeH, ImGuiCond_Appearing);
			Begin(IDTitle, &IsVisible, window_flags);
			int Count = this->getOrderCount(), now = 0;

			while (Count != now)
			{
				now++;
				for (int i = 0; i < CollpsHeader.size(); i++)
				{
					if (CollpsHeader.at(i)->Collapse() & CollpsHeader.at(i)->getCountOrderRender() == now)
						CollpsHeader.at(i)->Render();
				}

				for (int i = 0; i < Label.size(); i++)
				{
					if (Label.at(i)->GetVisible() & Label.at(i)->getRenderOrder() == now)
						Label.at(i)->Render();
				}

				for (int i = 0; i < Itext.size(); i++)
				{
					if (Itext.at(i)->GetVisible() & Itext.at(i)->getRenderOrder() == now)
						Itext.at(i)->Render();
				}

				for (int i = 0; i < Itextmul.size(); i++)
				{
					if (Itextmul.at(i)->GetVisible() & Itextmul.at(i)->getRenderOrder() == now)
						Itextmul.at(i)->Render();
				}

				for (int i = 0; i < Btn.size(); i++)
				{
					if (Btn.at(i)->GetVisible() & Btn.at(i)->getRenderOrder() == now)
						Btn.at(i)->Render();
				}
			}
			End();
		}
	}
private:
	void ChangeSize(float W, float H, enum ImGuiCond_ Cond)
	{
		auto Size = GetWindowSize();
		if (Size.x == W & Size.y == H)
			return;

		SetWindowSize(ImVec2(W, H), Cond);
		NeedToResize = true;
	}

	LPCSTR IDTitle = "";
	bool IsVisible = false,
		IsKeyboardSupport = false,
		ShowTitle = false,
		IsMoveble = false,
		IsResizeble = false,
		IsCollapsible = false,
		NeedToResize = true;

	ImGuiWindowFlags window_flags = 0;

	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<CollapsingHeaders>> CollpsHeader;
	vector<shared_ptr<IText>> Itext;
	vector<shared_ptr<ITextMulti>> Itextmul;

	int style = 1, // <-- This is a test variable.
		OrderlyRender = 0;
	float SizeW = 0.f, SizeH = 0.f;

};

class UI
{
public:
	HRESULT Init();

	void Begin();
	void Render();
	void End(bool WF);

	void Destroy();

	bool IsInitUI() { return InitUI; }

	auto getDialogs() { return Dialogs; }

	HRESULT LoadXmlUI(LPCSTR File);
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

	void DisableDialog(LPCSTR IDDialog);
	void EnableDialog(LPCSTR IDDialog);

	shared_ptr<dialogs> getDialog(LPCSTR IDDialog);

	vector<XMLElement *> getElement() { if (!Element.empty()) return Element; return vector<XMLElement *>{nullptr}; }

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
	unique_ptr<tinyxml2::XMLDocument> doc;

	// **********
	vector<XMLElement *> Element;

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
};
#endif // !__UI_H__