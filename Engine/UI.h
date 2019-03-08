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

struct Labels
{
	string IDTitle = "";
	bool IsVisible = false;

	void ChangeText(string Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle.c_str(); }

	Labels() {}
	Labels(LPCSTR IDTitle, bool IsVisible): IDTitle(IDTitle), IsVisible(IsVisible) {}

	void Render() // Also Render+Check click!!!
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
struct Dialogs: Buttons, Labels
{
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef DBT_DEVNODES_CHANGED
#define DBT_DEVNODES_CHANGED 0x0007
#endif

	HWND g_hWnd = 0;
	INT64 g_Time = 0;
	INT64 g_TicksPerSecond = 0;
	ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;

	LPCSTR IDTitle = "";
	bool IsVisible = false,
		IsKeyboardSupport = false,
		ShowTitle = false,
		IsMoveble = false,
		IsResizeble = false,
		IsCollapsible = false,
		g_HasGamepad = false,
		g_WantUpdateHasGamepad = true;

	ImGuiWindowFlags window_flags = 0;

	vector<Buttons> Btn;
	vector<Labels> Label;

	int style = 0; // This is a test bool.

	void ChangeText(LPCSTR Text) { IDTitle = Text; }
	LPCSTR GetText() { return IDTitle; }
	void SetShowTitle(bool Show) { ShowTitle = Show; }
	void setVisible(bool Visible) { IsVisible = Visible; }

	bool getVisible() { return IsVisible; }

	Dialogs() {}
	~Dialogs() {}
	Dialogs(LPCSTR IDTitle): IDTitle(IDTitle) {}
	Dialogs(LPCSTR IDTitle, bool IsVisible, bool ShowTitle, bool IsMoveble, bool IsKeyboardSupport, bool IsResizeble, bool IsCollapsible, int style):
		IDTitle(IDTitle), IsVisible(IsVisible), IsKeyboardSupport(IsKeyboardSupport),
		style(style), IsMoveble(IsMoveble), IsResizeble(IsResizeble),
		IsCollapsible(IsCollapsible), ShowTitle(ShowTitle)
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
		if (!IsMoveble)
			window_flags |= ImGuiWindowFlags_NoMove;
		if (!IsResizeble)
			window_flags |= ImGuiWindowFlags_NoResize;
		if (!IsCollapsible)
			window_flags |= ImGuiWindowFlags_NoCollapse;

		if (IsVisible)
		{
			Begin(IDTitle, &IsVisible, window_flags);

			for (int i = 0; i < Btn.size(); i++)
				if (Btn.at(i).IsVisible)
					Btn.at(i).Render();

			for (int i = 0; i < Label.size(); i++)
				if (Label.at(i).IsVisible)
					Label.at(i).Render();

			End();
		}
	}

	vector<Labels> getLabels() { return Label; }
	vector<Buttons> getButtons() { return Btn; }
};

class UI
{
public:
	HRESULT Init(int Count = 1, LPCWSTR texture = L"");

	void Render(float Time, int ID = 0);

	void Destroy();

	bool IsInitUI() { return InitUI; }

	auto getDialogs() { return dialogs; }

	HRESULT LoadXmlUI(LPCSTR File);
	void ProcessXML();

	void ReloadXML(LPCSTR File);

	vector<LPCSTR> getID() { if (!ID.empty()) return ID; return vector<LPCSTR>{""}; }
	vector<LPCSTR> getText() { if (!text.empty()) return text; return vector<LPCSTR>{""}; }
	vector<int> getW() { if (!W.empty()) return W;  return vector<int>{0}; }
	vector<int> getH() { if (!H.empty()) return H; return vector<int>{0}; }
	vector<int> getX() { if (!X.empty()) return X; return vector<int>{0}; }
	vector<int> getY() { if (!Y.empty()) return Y; return vector<int>{0}; }
	vector<XMLElement *> getElement() { if (!Element.empty()) return Element; return vector<XMLElement *>{nullptr}; }

	void Begin();
	void End();

	UI() {}
	~UI() {}

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, vector<void*> pUserContext);
protected:
	// **********
	HRESULT hr = S_OK;

	// **********
	vector<Dialogs> dialogs;

	// **********
	bool InitUI = false;

	// **********
	int iY = 10;

	// **********
	unique_ptr<tinyxml2::XMLDocument> doc;

	// **********
	vector<int> W, H, X, Y;
	vector<LPCSTR> ID, text;

	// **********
	vector<XMLElement *> Element;

	void StackTrace(const char *Error)
	{
		DebugTrace("***********ERROR IN XML FILE***********\n");
		DebugTrace("===Check info below:\n");
		DebugTrace(string(string("... ") + string(Error) + string(" ...")).c_str());
		DebugTrace("***********ERROR IN XML FILE***********\n");
	}
};
#endif // !__UI_H__