#pragma once
#ifndef __CONSOLE__H_
#define __CONSOLE__H_
#include "pch.h"

#include "UI.h"

class Console
{
private:
	enum Console_STATE
	{
		Open = 1,
		Close
	};

public:
	HRESULT Init();

	//HRESULT Settings(bool Reset);

	//void Render(float Time);
	//void Open();
	//void Close();

	Console() {}
	~Console() {}

	bool IsInit() { return InitClass; }
	Console_STATE *getState() { return &CState; }
	void ChangeState(Console_STATE Cstate) { CState = Cstate; }

private:
	bool InitClass = false;
	Console_STATE CState = Console_STATE::Close;
};
#endif // !__CONSOLE__H_

