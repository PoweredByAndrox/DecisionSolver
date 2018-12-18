#include "pch.h"
#pragma once
#ifndef __UI_H__
#define __UI_H__

namespace Engine
{
	class UI
	{
	public:
		HRESULT Init();

		HRESULT AddButton(CDXUTDialog *Dial, int ID, wstring Text, int X, int Y, int W, int H, int Key);
		HRESULT AddButton(CDXUTDialog *Dial, int ID, wstring Text, int X, int Y, int W, int H);
		HRESULT AddButton(CDXUTDialog *Dial, int ID, wstring Text);

		HRESULT AddStatic(CDXUTDialog *Dial, int ID, wstring Text, int X, int Y, int W, int H, int Key);
		HRESULT AddStatic(CDXUTDialog *Dial, int ID, wstring Text, int X, int Y, int W, int H);
		HRESULT AddStatic(CDXUTDialog *Dial, int ID, wstring Text);

		HRESULT AddButton_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *Keys);
		HRESULT AddButton_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y);

		HRESULT AddStatic_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H);

		void SetLocationButton(CDXUTDialog *Dial, int ID, int X, int Y, bool Align);
		void SetTextButton(CDXUTDialog *Dial, int ID, wstring *Text);

		void SetTextButton(CDXUTDialog *Dial, int ID, string *Text, float Format[3]);
		void SetTextButton(CDXUTDialog *Dial, int ID, string *Text, XMMATRIX Format);
		void SetTextButton(CDXUTDialog *Dial, int ID, string *Text, float Format);
		void SetTextButton(CDXUTDialog *Dial, int ID, string *Text, vector<size_t> Format);
		void SetTextButton(CDXUTDialog *Dial, int ID, string *Text, XMVECTOR Format);

		void SetTextButton(CDXUTDialog *Dial, int ID, wstring *Text, float Format[3]);
		void SetTextButton(CDXUTDialog *Dial, int ID, wstring *Text, XMMATRIX Format);
		void SetTextButton(CDXUTDialog *Dial, int ID, wstring *Text, float Format);
		void SetTextButton(CDXUTDialog *Dial, int ID, wstring *Text, vector<size_t> Format);
		void SetTextButton(CDXUTDialog *Dial, int ID, wstring *Text, XMVECTOR Format);

		void SetLocationStatic(CDXUTDialog *Dial, int ID, int X, int Y, bool Align);
		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text);

		void SetTextStatic(CDXUTDialog *Dial, int ID, string *Text, float Format[3]);
		void SetTextStatic(CDXUTDialog *Dial, int ID, string *Text, XMMATRIX Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, string *Text, float Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, string *Text, vector<size_t> Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, string *Text, Vector3 *Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, string *Text, size_t Format);

		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text, float Format[3]);
		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text, XMMATRIX Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text, float Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text, vector<size_t> Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text, Vector3 *Format);
		void SetTextStatic(CDXUTDialog *Dial, int ID, wstring *Text, size_t Format);

		void SetLocationCheck(CDXUTDialog *Dial, int ID, int X, int Y, bool Align);

		HRESULT AddSlider(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Min, int Max);
		HRESULT AddSlider_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H, vector<int> *Min, vector<int> *Max, vector<int> *DefValue);

		HRESULT AddCheckBox(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked, int HotKey);
		HRESULT AddCheckBox_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text,
			vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked, vector<int> *HotKey);

		HRESULT AddCheckBox(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked);
		HRESULT AddCheckBox_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text,
			vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked);

		HRESULT AddCheckBox(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H);
		HRESULT AddCheckBox_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text,
			vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H);

		HRESULT AddComboBox(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked, int HotKey);
		HRESULT AddComboBox_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H, vector<int> *Checked, vector<int> *HotKey);

		HRESULT AddComboBox(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked);
		HRESULT AddComboBox_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H, vector<int> *Checked);

		HRESULT AddComboBox(CDXUTDialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H);
		HRESULT AddComboBox_Mass(CDXUTDialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y,
			vector<int> *W, vector<int> *H);


		bool IsInitUI() { return InitUI; }

		auto *getDialogResManager() { return &g_DialogResourceManager; }
		auto *getHUD() { return &g_HUD; }

		auto *getObjButton() { return &ObjButton; }
		auto *getObjNameButton() { return &ObjNameButton; }

		auto *getObjStatic() { return &ObjStatic; }
		auto *getObjNameStatic() { return &ObjNameStatic; }

		auto *getObjComboBox() { return &ObjComboBox; }
		auto *getObjNameComboBox() { return &ObjNameComboBox; }

		auto *getObjSlider() { return &ObjSlider; }
		auto *getObjNameSlider() { return &ObjNameSlider; }

		auto *getObjCheckBox() { return &ObjCheckBox; }
		auto *getObjNameCheckBox() { return &ObjNameCheckBox; }

		//int getAllComponentsCount() { return ObjButton.size() + ObjStatic.size(); }
		//int getComponentBy_ID(int ID) { return ObjButton.size() + ObjStatic.size() + ID; }

		int getAllComponentsCount();
		vector<int> *addToBackComponentBy_ID(int ID);

		int getComponentBy_ID(UI *ui, int ID);
		int getComponentBy_ID(UI *ui, vector<int> *Obj, int ID);

		int getComponentID_By_Name(UI *ui, vector<int> *Obj, wstring *Text)
		{
			USES_CONVERSION;
			LPCSTR t;
			for (int i = 0; i < Obj->size(); i++)
				if (t = strstr(W2A(Text->c_str()), W2A(ui->getObjNameButton()->at(i).c_str())))
					return Obj->at(i);
				else if (t = strstr(W2A(Text->c_str()), W2A(ui->getObjNameStatic()->at(i).c_str())))
					return Obj->at(i);
				else if (t = strstr(W2A(Text->c_str()), W2A(ui->getObjNameCheckBox()->at(i).c_str())))
					return Obj->at(i);
				else if (t = strstr(W2A(Text->c_str()), W2A(ui->getObjNameComboBox()->at(i).c_str())))
					return Obj->at(i);
				else if (t = strstr(W2A(Text->c_str()), W2A(ui->getObjNameSlider()->at(i).c_str())))
					return Obj->at(i);
		}

		wstring getComponentName_By_ID(UI *ui, int ID);

		UI() {}
		~UI() {}

	private:
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
		CDXUTDialogResourceManager g_DialogResourceManager;
		CDXUTDialog g_HUD;

		int iY = 10;

	};
};
#endif // !__UI_H__