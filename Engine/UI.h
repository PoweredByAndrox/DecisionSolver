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

struct IText
{
	string IDTitle = "", Text = "";
	bool IsVisible = false, IsNeedHistory = false,
		NeedToUseTAB = false, EnterReturnsTrue = false;

	ImGuiInputTextFlags Flags = 0;

	void ChangeText(string Text) { this->Text = Text; }
	void ChangeTitle(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }
	LPCSTR GetTitle() { return IDTitle.c_str(); }

	IText() {}
	IText(LPCSTR IDTitle, bool IsVisible, bool IsNeedHistory = false, bool NeedToUseTAB = false, bool EnterReturnsTrue = false):
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

		ImGui::InputText(Text.c_str(), &IDTitle, Flags);
	}
};
struct ITextMulti
{
	string IDTitle = "", Text = "";
	bool IsVisible = false, ReadOnly = false, IsCtrlNewLine = false;

	ImGuiInputTextFlags Flags = 0;

	void ChangeText(string Text) { this->Text = Text; }
	void ChangeTitle(string Text) { IDTitle = Text; }
	LPCSTR GetTitle() { return IDTitle.c_str(); }
	LPCSTR GetText() { return Text.c_str(); }

	ITextMulti() {}
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

		ImGui::InputTextMultiline(IDTitle.c_str(), &Text, ImVec2(0, 0), Flags);
	}
};
struct Labels
{
	string IDTitle = "";
	bool IsVisible = false;

	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }

	Labels() {}
	Labels(LPCSTR IDTitle, bool IsVisible) : IDTitle(IDTitle), IsVisible(IsVisible) {}

	void Render()
	{
		ImGui::Text(IDTitle.c_str());
	}
};
struct Buttons
{
	LPCSTR IDTitle = "";
	bool IsVisible = false, clicked = false;

	void ChangeText(LPCSTR Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle; }
	bool IsClicked() { return clicked; }
	Buttons() {}
	Buttons(LPCSTR IDTitle, bool IsVisible): IDTitle(IDTitle), IsVisible(IsVisible) {}

	void Render()
	{
		if (ImGui::Button(IDTitle))
			clicked = true;
		else
			clicked = false;
	}
};
struct CollapsingHeaders
{
	string IDTitle = "";
	bool SelDef = false;
	ImGuiTreeNodeFlags Flags = 0;
	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<ITextMulti>> Itextmul;
	vector<shared_ptr<IText>> Itext;

	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }

	CollapsingHeaders() {}
	CollapsingHeaders(LPCSTR IDTitle, bool SelDef) : IDTitle(IDTitle), SelDef(SelDef) {}

	void Render()
	{
		Flags = 0;
		Flags = ImGuiTreeNodeFlags_CollapsingHeader;

		if (SelDef)
			Flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::CollapsingHeader(IDTitle.c_str(), Flags))
		{
			for (int i = 0; i < Btn.size(); i++)
				if (Btn.at(i)->IsVisible)
					Btn.at(i)->Render();

			for (int i = 0; i < Label.size(); i++)
				if (Label.at(i)->IsVisible)
					Label.at(i)->Render();

			for (int i = 0; i < Itextmul.size(); i++)
				if (Itextmul.at(i)->IsVisible)
					Itextmul.at(i)->Render();

			for (int i = 0; i < Itext.size(); i++)
				if (Itext.at(i)->IsVisible)
					Itext.at(i)->Render();
		}
	}
};
struct dialogs
{
private:
	void ChangeSize(float W, float H, enum ImGuiCond_ Cond)
	{
		if (NeedToResize)
		{
			SetNextWindowSize(ImVec2(W, H), Cond);
			NeedToResize = false;
		}
	}

public:
	LPCSTR IDTitle = "";
	bool IsVisible = false,
		IsKeyboardSupport = false,
		ShowTitle = false,
		IsMoveble = false,
		IsResizeble = false,
		IsCollapsible = false;

	ImGuiWindowFlags window_flags = 0;

	vector<shared_ptr<Buttons>> Btn;
	vector<shared_ptr<Labels>> Label;
	vector<shared_ptr<CollapsingHeaders>> CollpsHeader;
	vector<shared_ptr<IText>> Itext;
	vector<shared_ptr<ITextMulti>> Itextmul;

	int style = 0; // This is a test variable.
	float SizeW = 0.f, SizeH = 0.f;
	bool NeedToResize = false;

	void ChangeText(LPCSTR Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle; }
	void SetShowTitle(bool Show) { ShowTitle = Show; }
	void setVisible(bool Visible) { IsVisible = Visible; }
	bool getVisible() { return IsVisible; }
	void ChangeSize(float W, float H)
	{
		NeedToResize = true;
		this->SizeW = W;
		this->SizeH = H;
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

		if (style == 0)
		{
			this->style = style;
			StyleColorsClassic();
		}
		else if (style == 1)
		{
			this->style = style;
			StyleColorsDark();
		}
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
			if (NeedToResize)
				ChangeSize(this->SizeW, this->SizeH, ImGuiCond_Always);
			Begin(IDTitle, &IsVisible, window_flags);

			for (int i = 0; i < CollpsHeader.size(); i++)
				CollpsHeader.at(i)->Render();

			for (int i = 0; i < Btn.size(); i++)
				if (Btn.at(i)->IsVisible)
					Btn.at(i)->Render();

			for (int i = 0; i < Itext.size(); i++)
				if (Itext.at(i)->IsVisible)
					Itext.at(i)->Render();

			for (int i = 0; i < Itextmul.size(); i++)
				if (Itextmul.at(i)->IsVisible)
					Itextmul.at(i)->Render();

			for (int i = 0; i < Label.size(); i++)
				if (Label.at(i)->IsVisible)
					Label.at(i)->Render();

			End();
		}
	}

	vector<shared_ptr<Labels>> getLabels() { return Label; }
	vector<shared_ptr<Buttons>> getButtons() { return Btn; }
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
	HRESULT addCollapseHead(LPCSTR IDName, LPCSTR IDDialog = "", bool SelDef = false);
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