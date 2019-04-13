#pragma once
#ifndef __CONSOLE__H_
#define __CONSOLE__H_
#include "pch.h"

#include "UI.h"
#include "CCommands.h"

/*
template<typename... Args>
void callFunc(state &lua, LPCSTR name, Args &&... args)
{
	try
	{
		lua.get<function>(name).template call<void>(args...);
	}
	catch (error e)
	{
		printf("Lua error: %s", e.what());
	}
}
*/

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

	static void AddCmd(LPCSTR Text);
private:
	bool InitClass = false;
	Console_STATE CState = Console_STATE::Open;

	static shared_ptr<dialogs> Dialog;
	static shared_ptr<Commands> ProcessCommand;
};
#endif // !__CONSOLE__H_

