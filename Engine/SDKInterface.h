#pragma once
#if !defined(__SDKINTERFACE_H__)
#define __SDKINTERFACE_H__
#include "pch.h"

#include "UI.h"
#include <shellapi.h>

class ptree;
class SDKInterface
{
private:
	struct Und
	{
		Und() {}
		Und(string Desc, string Fst, string Sec): Desc(Desc), After_val(Fst), Before_val(Sec)
		{}
		string Desc,
			After_val,
			Before_val;
		bool Undo = false, Redo = false;
		static int Pos;
		Und *AddFst(string Thing) 
		{
			After_val = Thing;
			return this;
		}
		Und *AddSec(string Thing)
		{
			Before_val = Thing;
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
	void Changes();
	void Information();

	void WarningDial(string Name, std::function<void(void)> OK, std::function<void(void)> Cancel);
	void SelectMissingFiles();

	void LoadSettings(boost::property_tree::ptree fData);
	void SaveSettings();
	
	void Save();
	void SaveAs();
	
	void Open();

	void undo();
	void redo();

	void AddToUndo(string Desc, string BeforeVal, string AfterVal);
	void AddToUndo(string Desc, Vector3 BeforeVal, Vector3 AfterVal);

	auto getUndos() { return Undos; }

	bool getMP() { return MP; }
	bool getLockFPS() { return LockFPS; }
	float GetDistFarRender() { return DistFarRender; }
	float GetDistNearRender() { return DistNearRender; }

	bool getCameraWithoutBtn() { return CamBtnWihoutBtn; }
	bool getCameraRightBtn() { return CamBtnRight; }
	bool getCameraLeftBtn() { return CamBtnLeft; }
private:
	ImGuiTextFilter filter;
	float MovSense, RotSense;
	float DistFarRender = 1000.f, DistNearRender = 0.1f;
	Vector3 Pos = Vector3::Zero, Look = Vector3::Zero;
	bool LOGO = true, HoL = true, FR = false, CS = false, LagTest = false, audio = false, MP = false, LockFPS = true,
		IfNeedSave = false, IsFreeCam = false, CamBtnLeft = false, CamBtnRight = false, CamBtnWihoutBtn = false,
		_Changes = false, _Information = true;

	// Utilities
	string getPos, getLook;

	//	pos at now, mass Undos
	vector<Und> Undos;
};

#endif // !__SDKINTERFACE_H__