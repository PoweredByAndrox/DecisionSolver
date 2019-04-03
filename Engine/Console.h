#pragma once
#ifndef __CONSOLE__H_
#define __CONSOLE__H_
#include "pch.h"

#include "UI.h"
#include "CCommands.h"

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

	void Reload();

	void Render();
	void OpenConsole();
	void CloseConsole();

	Console() {}
	~Console() {}

	bool IsInit() { return InitClass; }
	Console_STATE *getState() { return &CState; }
	void ChangeState(Console_STATE Cstate) { CState = Cstate; }
private:
	bool InitClass = false;
	Console_STATE CState = Console_STATE::Open;

	shared_ptr<dialogs> Dialog;
	shared_ptr<Commands> ProcessCommand = make_unique<Commands>();
};
#endif // !__CONSOLE__H_

