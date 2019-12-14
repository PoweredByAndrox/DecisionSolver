#pragma once
#if !defined(__CONSOLE__H_)
#define __CONSOLE__H_
#include "pch.h"

#include "UI.h"
#include "CCommands.h"

class Console
{
public:
	enum Console_STATE
	{
		Open = 1,
		Close
	};

	HRESULT Init();

	void Reload();

	void Render();
	void OpenConsole();

	Console() {}
	~Console() {}

	bool IsInit() { return InitClass; }

	Console_STATE getState() { return CState; }
	void ChangeState(Console_STATE Cstate) { CState = Cstate; }

	shared_ptr<dialogs> getConsoleDial() { return Dialog; }
	shared_ptr<Commands> getCmd() { return ProcessCommand; }

	static void PushCMD(LPCSTR CMD);
	static void LogError(string Msg);
	static void LogInfo(string Msg);
	static void LogNormal(string Msg);
private:
	bool InitClass = false;
	Console_STATE CState = Console_STATE::Open;

	static shared_ptr<dialogs> Dialog;
	static shared_ptr<Commands> ProcessCommand;
};
#endif // !__CONSOLE__H_
