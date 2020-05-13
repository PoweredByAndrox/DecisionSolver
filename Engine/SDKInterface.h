#pragma once
#if !defined(__SDKINTERFACE_H__)
#define __SDKINTERFACE_H__
#include "pch.h"

#include "UI.h"
class ptree;
class SDKInterface
{
public:
	void Render();
	void Load(boost::property_tree::ptree fData);
	void Save();
private:
	ImGuiTextFilter filter;
	float MovSense = 2.f, RotSense = 2.f;
	bool IsFreeCam = false;
	Vector3 Pos = Vector3::Zero, Look = Vector3::Zero;
	bool LOGO = true, HoL = true, FR = false, CS = false, LagTest = false, audio = false;

	// Utilities
	string getPos, getLook;
};

#endif // !__SDKINTERFACE_H__