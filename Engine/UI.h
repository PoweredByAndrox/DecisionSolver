#include "pch.h"
#pragma once
#ifndef __UI_H__
#define __UI_H__

class UI
{
public:
	HRESULT Init();

	HRESULT AddButton(int ID, wstring Text, int X, int Y, int W, int H, int Key);
	HRESULT AddButton(int ID, wstring Text, int X, int Y, int W, int H);
	HRESULT AddButton(int ID, wstring Text);

	HRESULT AddStatic(int ID, wstring Text, int X, int Y, int W, int H, int Key);
	HRESULT AddStatic(int ID, wstring Text, int X, int Y, int W, int H);
	HRESULT AddStatic(int ID, wstring Text);

	HRESULT AddButton_Mass(vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *Keys);
	HRESULT AddStatic_Mass(vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y);

	void SetLocationButton(int ID, int X, int Y);
	void SetTextButton(int ID, wstring *Text);

	void SetTextButton(int ID, string *Text, float Format[3]);
	void SetTextButton(int ID, string *Text, XMMATRIX Format);
	void SetTextButton(int ID, string *Text, float Format);
	void SetTextButton(int ID, string *Text, vector<size_t> Format);
	void SetTextButton(int ID, string *Text, XMVECTOR Format);

	void SetTextButton(int ID, wstring *Text, float Format[3]);
	void SetTextButton(int ID, wstring *Text, XMMATRIX Format);
	void SetTextButton(int ID, wstring *Text, float Format);
	void SetTextButton(int ID, wstring *Text, vector<size_t> Format);
	void SetTextButton(int ID, wstring *Text, XMVECTOR Format);

	void SetLocationStatic(int ID, int X, int Y);
	void SetTextStatic(int ID, wstring *Text);

	void SetTextStatic(int ID, string *Text, float Format[3]);
	void SetTextStatic(int ID, string *Text, XMMATRIX Format);
	void SetTextStatic(int ID, string *Text, float Format);
	void SetTextStatic(int ID, string *Text, vector<size_t> Format);
	void SetTextStatic(int ID, string *Text, XMVECTOR Format);

	void SetTextStatic(int ID, wstring *Text, float Format[3]);
	void SetTextStatic(int ID, wstring *Text, XMMATRIX Format);
	void SetTextStatic(int ID, wstring *Text, float Format);
	void SetTextStatic(int ID, wstring *Text, vector<size_t> Format);
	void SetTextStatic(int ID, wstring *Text, XMVECTOR Format);

	bool IsInitUI() { return InitUI; }

	auto *getDialogResManager() { return &g_DialogResourceManager; }
	auto *getHUD() { return &g_HUD; }

	auto *getObjButton() { return &ObjButton; }
	auto *getObjNameButton() { return &ObjNameButton; }
	auto *getObjStatic() { return &ObjStatic; }
	auto *getObjNameStatic() { return &ObjNameStatic; }

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

		// **********
	CDXUTDialogResourceManager g_DialogResourceManager;
	CDXUTDialog g_HUD;

	int iY = 10;

};
#endif // !__UI_H__