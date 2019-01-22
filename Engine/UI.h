#pragma once
#ifndef __UI_H__
#define __UI_H__
#include "pch.h"

#include "File_system.h"
#include "tinyxml2.h"

using namespace tinyxml2;

namespace Engine
{
	class UI: public File_system
	{
	public:
		HRESULT Init(int Count = 1);

		void Render(float Time, int ID = 0);

		HRESULT AddButton(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H, int Key);
		HRESULT AddButton(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H);
		HRESULT AddButton(Dialog *Dial, int ID, wstring Text);

		HRESULT AddStatic(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H, int Key);
		HRESULT AddStatic(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H);
		HRESULT AddStatic(Dialog *Dial, int ID, wstring Text);

		HRESULT AddButton_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *Keys);
		HRESULT AddButton_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y);

		HRESULT AddStatic_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H);

		void SetLocationButton(Dialog *Dial, int ID, int X, int Y, bool Align);
		void SetTextButton(Dialog *Dial, int ID, wstring *Text);

		void SetTextButton(Dialog *Dial, int ID, string *Text, float Format[3]);
		void SetTextButton(Dialog *Dial, int ID, string *Text, XMMATRIX Format);
		void SetTextButton(Dialog *Dial, int ID, string *Text, float Format);
		void SetTextButton(Dialog *Dial, int ID, string *Text, vector<size_t> Format);
		void SetTextButton(Dialog *Dial, int ID, string *Text, XMVECTOR Format);

		void SetTextButton(Dialog *Dial, int ID, wstring *Text, float Format[3]);
		void SetTextButton(Dialog *Dial, int ID, wstring *Text, XMMATRIX Format);
		void SetTextButton(Dialog *Dial, int ID, wstring *Text, float Format);
		void SetTextButton(Dialog *Dial, int ID, wstring *Text, vector<size_t> Format);
		void SetTextButton(Dialog *Dial, int ID, wstring *Text, XMVECTOR Format);

		void SetLocationStatic(Dialog *Dial, int ID, int X, int Y, bool Align);
		void SetTextStatic(Dialog *Dial, int ID, wstring *Text);

		void SetTextStatic(Dialog *Dial, int ID, string *Text, float Format[3]);
		void SetTextStatic(Dialog *Dial, int ID, string *Text, XMMATRIX Format);
		void SetTextStatic(Dialog *Dial, int ID, string *Text, float Format);
		void SetTextStatic(Dialog *Dial, int ID, string *Text, vector<size_t> Format);
		void SetTextStatic(Dialog *Dial, int ID, string *Text, Vector3 *Format);
		void SetTextStatic(Dialog *Dial, int ID, string *Text, size_t Format);

		void SetTextStatic(Dialog *Dial, int ID, wstring *Text, float Format[3]);
		void SetTextStatic(Dialog *Dial, int ID, wstring *Text, XMMATRIX Format);
		void SetTextStatic(Dialog *Dial, int ID, wstring *Text, float Format);
		void SetTextStatic(Dialog *Dial, int ID, wstring *Text, vector<size_t> Format);
		void SetTextStatic(Dialog *Dial, int ID, wstring *Text, Vector3 *Format);
		void SetTextStatic(Dialog *Dial, int ID, wstring *Text, size_t Format);

		void SetLocationCheck(Dialog *Dial, int ID, int X, int Y, bool Align);

		HRESULT AddSlider(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Min, int Max);
		HRESULT AddSlider_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H, vector<int> *Min, vector<int> *Max, vector<int> *DefValue);

		HRESULT AddCheckBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked, int HotKey);
		HRESULT AddCheckBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text,
			vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked, vector<int> *HotKey);

		HRESULT AddCheckBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked);
		HRESULT AddCheckBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text,
			vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked);

		HRESULT AddCheckBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H);
		HRESULT AddCheckBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text,
			vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H);

		HRESULT AddComboBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked, int HotKey);
		HRESULT AddComboBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H, vector<int> *Checked, vector<int> *HotKey);

		HRESULT AddComboBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked);
		HRESULT AddComboBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H, vector<int> *Checked);

		HRESULT AddComboBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H);
		HRESULT AddComboBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H);

		bool IsInitUI() { return InitUI; }

		auto *getDialogResManager() { return &g_DialogResourceManager; }
		auto *getDialog() { return &g_Dialog; }

		auto getObjButton() { if (!ObjButton.empty()) return ObjButton; return vector<int>{0}; }
		auto getObjNameButton() { if (!ObjNameButton.empty()) return ObjNameButton; return vector<wstring>{wstring(L"")}; }

		auto getObjStatic() { if (!ObjStatic.empty()) return ObjStatic; return vector<int>{0}; }
		auto getObjNameStatic() { if (!ObjNameStatic.empty()) return ObjNameStatic; return vector<wstring>{wstring(L"")}; }

		auto getObjComboBox() { if (!ObjComboBox.empty()) return ObjComboBox; return vector<int>{0}; }
		auto getObjNameComboBox() { if (!ObjNameComboBox.empty()) return ObjNameComboBox; return vector<wstring>{wstring(L"")}; }

		auto getObjSlider() { if (!ObjSlider.empty()) return ObjSlider; return vector<int>{0}; }
		auto getObjNameSlider() { if (!ObjNameSlider.empty()) return ObjNameSlider; return vector<wstring>{wstring(L"")}; }

		auto getObjCheckBox() { if (!ObjCheckBox.empty()) return ObjCheckBox; return vector<int>{0}; }
		auto getObjNameCheckBox() { if (!ObjNameCheckBox.empty()) return ObjNameCheckBox; return vector<wstring>{wstring(L"")}; }

		int getAllComponentsCount();
		vector<int> addToBackComponentBy_ID(int ID);

		HRESULT LoadXmlUI(LPCSTR File);
		void ProcessXML();

		vector<LPCSTR> getID() { if (!ID.empty()) return ID; return vector<LPCSTR>{""}; }
		vector<LPCSTR> getText() { if (!Text.empty()) return Text; return vector<LPCSTR>{""}; }
		vector<int> getW() { if (!W.empty()) return W;  return vector<int>{0}; }
		vector<int> getH() { if (!H.empty()) return H; return vector<int>{0}; }
		vector<int> getX() { if (!X.empty()) return X; return vector<int>{0}; }
		vector<int> getY() { if (!Y.empty()) return Y; return vector<int>{0}; }

		UI() {}
		~UI() {}

	protected:
		HRESULT hr = S_OK;

		// **********
		bool InitUI = false;

		// **********
		vector<int> ObjButton;
		vector<wstring> ObjNameButton;

		vector<int> ObjStatic;
		vector<wstring> ObjNameStatic;

		vector<int> ObjSlider;
		vector<wstring> ObjNameSlider;

		vector<int> ObjCheckBox;
		vector<wstring> ObjNameCheckBox;

		vector<int> ObjComboBox;
		vector<wstring> ObjNameComboBox;

		// **********
		vector<DialogResourceManager *> g_DialogResourceManager;
		vector<Dialog *> g_Dialog;

		// **********
		int iY = 10;
		
		// **********
		unique_ptr<File_system> fs = make_unique<File_system>();

		// **********
		unique_ptr<tinyxml2::XMLDocument> doc;

		// **********
		vector<int> W, H, X, Y;
		vector<LPCSTR> ID, Text;

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
};
#endif // !__UI_H__