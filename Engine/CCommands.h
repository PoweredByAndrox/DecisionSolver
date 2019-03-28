#pragma once
#if !defined(__COMMANDS_H__)
#define __COMMANDS_H__
#include "pch.h"
#include "Physics.h"
#include "Camera.h"

static vector<string> ListCommands = { string("Help"), string("TestMsg"), 
									   string("Quit"), string("Clear"),
 string("ChangeSize"), string("AddPhysBox"), string("DoTorque") };

static vector<string> ListCommandsWithParams = { };
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
		if (strstr(Text.c_str(), "Help"))
		{
			string all;
			for (size_t i = 0; i < ListCommands.size(); i++)
			{
				all.append(string(string("\n") + ListCommands.at(i)));
			}

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information, 
				string("#list of available command: ") + all);
		}
		else if (strstr(Text.c_str(), "Quit"))
		{
			Application->Quit();

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
				string(Text + string(" #Apply")));
		}
		else if (strstr(Text.c_str(), "Clear"))
		{
			Console->getChilds().back()->getUTexts().back()->ClearText();

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
				string(Text + string(" #Apply")));
		}
		else if (strstr(Text.c_str(), "ChangeSize"))
		{
			string Cache = Text;

			float H = 0.f, W = 0.f;
			deleteWord(Text, string("ChangeSize "));
			sscanf_s(Text.c_str(), "%f, %f", &W, &H);
			Console->ChangeSize(W, H);

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
				string(Cache + string(" #Apply")));
		}
		else if (strstr(Text.c_str(), "AddPhysBox"))
		{
			string Cache = Text;

			float Size = 0.f, Mass = 0.f;
			deleteWord(Text, string("AddPhysBox "));
			sscanf_s(Text.c_str(), "%f, %f", &Mass, &Size);
			Application->getPhysics()->AddNewActor(Vector3(
				Application->getCamera()->GetEyePt().x + 10,
				Application->getCamera()->GetEyePt().y + 5, 
				Application->getCamera()->GetEyePt().z + 10), Vector3(Size, Size, Size), Mass, Size);

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
				string(Cache + string(" #Apply")));
		}
		else if (strstr(Text.c_str(), "DoTorque"))
		{
			auto ObjPhys = Application->getPhysics()->GetPhysDynamicObject();

			if (!ObjPhys.empty())
				Application->getPhysics()->AddTorque(ObjPhys.at((std::rand() % ObjPhys.size())),
					PxVec3(
						Application->getCamera()->GetEyePt().x + 10,
						Application->getCamera()->GetEyePt().y + 5,
						Application->getCamera()->GetEyePt().z + 10), PxForceMode::eFORCE);
			else
			{
				Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Error,
					string(Text + string(" GetPhysDynamicObject() return NULL!!!")));
				return;
			}

			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
				string(Text + string(" #Apply")));
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
};
#endif // __COMMANDS_H__