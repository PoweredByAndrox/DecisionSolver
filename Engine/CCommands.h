#pragma once
#if !defined(__COMMANDS_H__)
#define __COMMANDS_H__
#include "pch.h"

static vector<string> ListCommands = { string("Help"), string("TestMsg"), string("Quit"), string("Clear") };
static vector<string> History;

class Commands
{
public:
	Commands() {}
	~Commands() {}
	void Work(shared_ptr<UnformatedText> UText, string Text)
	{
		if (!Text.empty())
		{
			string FindCommand = FindPieceCommand(Text);
			if (!FindCommand.empty())
				ExecCommand(UText, FindCommand);
			else
				UText->AddCLText(UnformatedText::Type::Error,
					string(string("You're typed: ")+ Text + string("\n[error]: Unknown command type help for help!")));
		}
	}
	void ExecCommand(shared_ptr<UnformatedText> UText, string Text)
	{
		if (strcmp(Text.c_str(), "Help") == 0)
		{
			string all;
			for (size_t i = 0; i < ListCommands.size(); i++)
			{
				all.append(string(string("\n") + ListCommands.at(i)));
			}

			UText->AddCLText(UnformatedText::Type::Information, string("#list of available command: ") + all);
		}
		else if (strcmp(Text.c_str(), "Error") == 0)
		{
			UText->AddCLText(UnformatedText::Type::Information, string("[error]: Unknown command type help for help!"));
		}
		else if (strcmp(Text.c_str(), "Quit") == 0)
		{
			Application->Quit();
		}
		else if (strcmp(Text.c_str(), "Clear") == 0)
		{
			UText->ClearText();
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