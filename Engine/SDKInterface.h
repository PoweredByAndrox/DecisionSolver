#pragma once
#if !defined(__SDKINTERFACE_H__)
#define __SDKINTERFACE_H__
#include "pch.h"

#include "UI.h"
class SDKInterface
{
public:
	void Render();
private:
	bool Open = true, Open2 = true, Open3 = false, Open4 = true, Open5 = true, Open6 = true,
		Open7 = true;
	ImGuiTextFilter filter;
};

#endif // !__SDKINTERFACE_H__