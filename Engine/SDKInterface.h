#pragma once
#if !defined(__SDKINTERFACE_H__)
#define __SDKINTERFACE_H__
#include "pch.h"

#include "UI.h"
#include <any>

class ptree;
class SDKInterface
{
private:
	struct Und
	{
		Und() {}
		Und(string Desc, string Fst, string Sec): Desc(Desc), Fst(Fst), Sec(Sec)
		{}
		string Desc,
			Fst,
			Sec;
		Und *AddFst(string Thing) 
		{
			Fst = Thing;
			return this;
		}
		Und *AddSec(string Thing)
		{
			Sec = Thing;
			return this;
		}
		Und *AddDesc(string Desc)
		{
			this->Desc = Desc;
			return this;
		}
	};

public:
	void Render();
	void LoadSettings(boost::property_tree::ptree fData);
	void SaveSettings();
	
	void Save();
	void SaveAs();
	
	void Open();

	void undo();
	
	void AddToUndo(string Desc, string BeforeVal, string AfterVal);
	void AddToUndo(string Desc, Vector3 BeforeVal, Vector3 AfterVal);

	auto getUndos() { return Undos; }
private:
	ImGuiTextFilter filter;
	float MovSense = 2.f, RotSense = 2.f;
	bool IsFreeCam = false;
	Vector3 Pos = Vector3::Zero, Look = Vector3::Zero;
	bool LOGO = true, HoL = true, FR = false, CS = false, LagTest = false, audio = false,
		IfNeedSave = false;

	// Utilities
	string getPos, getLook;

	//	order index
	vector<pair<UINT, Und>> Undos;
};

#endif // !__SDKINTERFACE_H__