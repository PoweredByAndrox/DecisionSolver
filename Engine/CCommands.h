#pragma once
#if !defined(__COMMANDS_H__)
#define __COMMANDS_H__
#include "pch.h"

class dialogs;
class Commands
{
private:
	struct Command
	{
		enum TypeOfCommand
		{
			WithParam = 0, WithoutParam, Lua
		};
		TypeOfCommand type;

		string CommandStr = "", CommandParamsProcess = "", CommandUnprocessed = "",
				S_One = "", S_Two = "", S_Three = "", TypedCmd = "";
		const string CommandNeededParams = "";
		int CountOfParams = 0;
		float One = 0.f, Two = 0.f, Three = 0.f;
		bool Checked = false;

		void CheckRequiredParam();

		Command() {}
		Command(string CommandStr, string CommandNeededParams, TypeOfCommand type): CommandStr(CommandStr),
			CommandNeededParams(CommandNeededParams), type(type) {}
	};
	vector<shared_ptr<Command>> commands;
	vector<string> History;
	int PosHistory = 0;
public:
	void Init();

	auto getAllCommands() { return commands; }
	auto getHistoryCommands() { return History; }

	int getPosHistory() { return PosHistory; }

	void changePosHistory(int num) { PosHistory = num; }

	void Work(shared_ptr<dialogs> &Console, string Text);
	void ExecCommand(shared_ptr<dialogs> &Console, shared_ptr<Command> &cmd);

	shared_ptr<Command> FindPieceCommand(string Text);
};
#endif // __COMMANDS_H__