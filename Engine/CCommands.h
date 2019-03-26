#pragma once
#if !defined(__COMMANDS_H__)
#define __COMMANDS_H__
#include "pch.h"

static vector<string> ListCommands = { string("Help"), string("TestMsg"), string("Quit"), string("Clear"), string("ChangeSize") };
static vector<string> History;

class Commands
{
public:
	Commands() {}
	~Commands() {}
	void Work(shared_ptr<dialogs> &Console, string Text)
	{
		if (!Text.empty())
		{
			string FindCommand = FindPieceCommand(Text);
			if (!FindCommand.empty())
				ExecCommand(Console, FindCommand);
			else
				Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Error,
					string(string("You're typed: ")+ Text + string("\n[error]: Unknown command type help for help!")));
		}
	}
	void ExecCommand(shared_ptr<dialogs> &Console, string Text)
	{
		if (strcmp(Text.c_str(), "Help") == 0)
		{
			string all;
			for (size_t i = 0; i < ListCommands.size(); i++)
			{
				all.append(string(string("\n") + ListCommands.at(i)));
			}

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information, 
				string("#list of available command: ") + all);
		}
		else if (strcmp(Text.c_str(), "Error") == 0)
		{
			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
				string("[error]: Unknown command type help for help!"));
		}
		else if (strcmp(Text.c_str(), "Quit") == 0)
		{
			Application->Quit();
		}
		else if (strcmp(Text.c_str(), "Clear") == 0)
		{
			Console->getChilds().back()->getUTexts().back()->ClearText();
		}
		else if (strcmp(Text.c_str(), "ChangeSize"))
		{
			float H = 0.f, W = 0.f;
			deleteWord(Text, string("ChangeSize "));
			sscanf_s(Text.c_str(), "%f, %f", &W, &H);
			Console->ChangeSize(W, H);
			//ImGui::SetWindowSize(ImVec2(H, W) , ImGuiCond_::ImGuiCond_Once);
		}
	}
	string FindPieceCommand(string Text)
	{
		for (int i = 0; i < ListCommands.size(); i++)
		{
			if (Text.length() >= 2)
				if (std::search(Text.begin(), Text.end(), ListCommands.at(i).begin(), ListCommands.at(i).end()) != Text.end())
					if (Text.length() > ListCommands.at(i).length())
						return Text;
					else
						return ListCommands.at(i);
		}

		return "";
	}

private:
};
#endif // __COMMANDS_H__