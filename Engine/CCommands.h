#pragma once
#if !defined(__COMMANDS_H__)
#define __COMMANDS_H__
#include "pch.h"

static vector<string> ListCommands = { string("Help"), string("TestMsg"), string("Quit") };
static vector<string> History;

class Commands
{
public:
	Commands() {}
	~Commands() {}
	void Work(shared_ptr<ITextMulti> TextInputMul, string Text)
	{
		if (!Text.empty())
		{
			string FindCommand = FindPieceCommand(Text);
			if (!FindCommand.empty())
				ExecCommand(TextInputMul, FindCommand);
		}
	}
	void ExecCommand(shared_ptr<ITextMulti> TextInputMul, string Text)
	{
		if (strcmp(Text.c_str(), "Help") == 0)
		{
			string all;
			for (size_t i = 0; i < ListCommands.size(); i++)
			{
				all.append(string(string("\n") + ListCommands.at(i)));
			}
			TextInputMul->ChangeText(string("#list of available command: ") + all);
		}
		else if (strcmp(Text.c_str(), "TestMsg") == 0)
		{
			MessageBoxA(Application->GetHWND(), "You call this message box)", "DecisionSolver", MB_OK);
		}
		else if (strcmp(Text.c_str(), "Quit") == 0)
		{
			Application->Quit();
		}
	}
	string FindPieceCommand(string Text)
	{
		for (int i = 0; i < ListCommands.size(); i++)
		{
			if (Text.length() >= 2)
				if (ListCommands.at(i).find(Text) != string::npos)
					return ListCommands.at(i);
		}

		return "";
	}

private:
};
#endif // __COMMANDS_H__