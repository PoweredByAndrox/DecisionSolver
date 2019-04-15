#include "pch.h"

#include "CCommands.h"
#include "CLua.h"
#include "UI.h"
#include "Physics.h"
#include "Camera.h"

static const vector<string> ListCommands =
{
	string("Help"), string("TestMsg"),
	string("Quit"), string("Clear"),
	string("DoTorque")
};
static const vector<string> ListCommandsWithParams =
{
	string("ChangeSize #float W, #float H"), string("AddPhysBox #float MassObj, #float SizeModel"),
	string("Exec_lua #PathToFile.function(param)")
};

void Commands::Work(shared_ptr<dialogs> &Console, string Text)
{
	if (!Text.empty())
	{
		auto cmd = FindPieceCommand(Console, Text);
		if (cmd.operator bool() && !cmd->CommandStr.empty())
			ExecCommand(Console, cmd);
		else
			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Error,
				string(string("You're typed: ") + Text + string("\n[error]: Unknown command type Help for help!")));
	}
}

void Commands::Init()
{
	for (int i = 0; i < ListCommands.size(); i++)
	{
		commands.push_back(make_shared<Command>(ListCommands.at(i), "", Command::TypeOfCommand::WithoutParam));
	}
	for (int i = 0; i < ListCommandsWithParams.size(); i++)
	{
		if (ListCommandsWithParams.at(i) == "Exec_lua #PathToFile.function(param)")
		{
			auto deleteStringParam = ListCommandsWithParams.at(i), deleteStringCommand = ListCommandsWithParams.at(i);
			deleteWord(deleteStringParam, string("#"), ModeProcessString::UntilTheBegin);
			deleteWord(deleteStringCommand, string("#"), ModeProcessString::UntilTheEnd);
			commands.push_back(make_shared<Command>(deleteStringCommand, deleteStringParam, Command::TypeOfCommand::Lua));
		}
		auto deleteStringParam = ListCommandsWithParams.at(i), deleteStringCommand = ListCommandsWithParams.at(i);
		deleteWord(deleteStringParam, string("#"), ModeProcessString::UntilTheBegin);
		deleteWord(deleteStringCommand, string("#"), ModeProcessString::UntilTheEnd);
		commands.push_back(make_shared<Command>(deleteStringCommand, deleteStringParam, Command::TypeOfCommand::WithParam));
	}
}

void Commands::ExecCommand(shared_ptr<dialogs> &Console, shared_ptr<Command> &cmd)
{
	if (cmd->type == Command::TypeOfCommand::WithParam || cmd->type == Command::TypeOfCommand::Lua)
	{
		if (cmd->CommandUnprocessed.empty())
		{
			Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Error,
				string(cmd->CommandStr + string(": No parameters found! You must add several parameters, such like: ") + cmd->CommandNeededParams));
			return;
		}

		cmd->CheckNeededParam();
	}

	if (stricmp(cmd->CommandStr.c_str(), "Help") == 0)
	{
		string all;
		for (int i = 0; i < ListCommands.size(); i++)
		{
			all.append(string(string("\n") + ListCommands.at(i)));
		}

		Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
			string("#list of available command: ") + all);
	}
	if (cmd->type == Command::TypeOfCommand::WithoutParam)
	{
		if (stricmp(cmd->CommandStr.c_str(), "Quit") == 0)
			Application->Quit();
		else if (stricmp(cmd->CommandStr.c_str(), "Clear") == 0)
			Console->getChilds().back()->getUTexts().back()->ClearText();

		else if (stricmp(cmd->CommandStr.c_str(), "DoTorque") == 0)
		{
			auto ObjPhys = Application->getPhysics()->GetPhysDynamicObject();
			if (!ObjPhys.empty())
			{
				Application->getPhysics()->AddTorque(ObjPhys.at((std::rand() % ObjPhys.size())),
					PxVec3(
						Application->getCamera()->GetEyePt().x + 10,
						Application->getCamera()->GetEyePt().y + 5,
						Application->getCamera()->GetEyePt().z + 10), PxForceMode::eFORCE);
			}
			else
				Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Error,
					string(cmd->CommandStr + string(": GetPhysDynamicObject() return NULL!!!")));
		}
	}
	if (cmd->type == Command::TypeOfCommand::Lua)
	{
		if (stricmp(cmd->CommandStr.c_str(), "Exec_lua") == 0)
			Application->getCLua()->callFunction(cmd->S_One, cmd->S_Two, cmd->S_Three);
	}

	if (cmd->type == Command::TypeOfCommand::WithParam)
	{
		if (stricmp(cmd->CommandStr.c_str(), "ChangeSize") == 0)
			Console->ChangeSize(cmd->One, cmd->Two);
		else if (stricmp(cmd->CommandStr.c_str(), "AddPhysBox") == 0)
		{
			string Cache = cmd->CommandStr;

			float Size = cmd->Two, Mass = cmd->One;
			Application->getPhysics()->AddNewActor(Vector3(
				Application->getCamera()->GetEyePt().x + 10,
				Application->getCamera()->GetEyePt().y + 5,
				Application->getCamera()->GetEyePt().z + 10), Vector3(Size, Size, Size), Mass, Size);
		}
	}
	if (cmd->CommandParamsProcess.empty())
		History.push_back(cmd->CommandStr);
	else
		History.push_back(cmd->CommandStr + " " + cmd->CommandParamsProcess);

	Console->getChilds().back()->getUTexts().back()->AddCLText(UnformatedText::Type::Information,
		string(cmd->CommandStr + string(" #Apply")));

	cmd->CommandParamsProcess = cmd->CommandUnprocessed = "";
	cmd->One = cmd->Two = cmd->Three = 0.f;
	cmd->CountOfParams = 0;
	cmd->Checked = false;
}

shared_ptr<Commands::Command> Commands::FindPieceCommand(shared_ptr<dialogs> &Console, string Text)
{
	for (int i = 0; i < commands.size(); i++)
	{
		if (Text.length() >= 2)
		{
			string GetCommand = Text;
			string GetParam = Text;

			if (GetParam.length() > commands.at(i)->CommandStr.length())
				deleteWord(GetParam, " ", ModeProcessString::UntilTheBegin, false, false);

			deleteWord(GetCommand, " ", ModeProcessString::UntilTheEnd);

			if (commands.at(i)->CommandStr.find(GetCommand) != string::npos)
				if (commands.at(i)->CommandStr.substr(commands.at(i)->CommandStr.find(GetCommand), GetCommand.length()) == GetCommand)
				{
					if (Text.length() <= commands.at(i)->CommandStr.length() || GetParam.empty())
						return commands.at(i);

					commands.at(i)->CommandUnprocessed = GetParam;
					return commands.at(i);
				}
		}
		else
			Console->getTextLists().back()->clearItems();
	}

	return make_shared<Command>();
}

void Commands::Command::CheckNeededParam()
{
	if (type == Lua)
	{
		auto Path = CommandUnprocessed;
		deleteWord(Path, ".", ModeProcessString::UntilTheEnd);
		S_One = Path;

		deleteWord(CommandUnprocessed, ".", ModeProcessString::UntilTheBegin, true, false);
		if (CommandUnprocessed[0] == '.')
			deleteWord(CommandUnprocessed, ".", true);

		S_Two = CommandUnprocessed;
		deleteWord(S_Two, "(", ModeProcessString::UntilTheEnd);

		S_Three = CommandUnprocessed;
		deleteWord(S_Three, "(", ModeProcessString::UntilTheBegin, false, false);
		if (S_Three[0] == '(')
			deleteWord(S_Three, "(", true, false);

		replaceAll(S_Three, ")", "", true, true);

		CommandParamsProcess = S_One + string(" ") + S_Two + string(" ") + S_Three;
	}
	deleteWord(CommandUnprocessed, " ");

	int i = 0;
	for (int i1 = 1; INT16_MAX; i1++)
	{
		if (i == CommandNeededParams.length())
			break;

		if (CommandNeededParams.at(i) == '#')
			CountOfParams++;

		i++;
	}
	if (CountOfParams == 2)
	{
		sscanf_s(CommandUnprocessed.c_str(), "%f, %f", &One, &Two);
		CommandParamsProcess = (boost::format("%.3f, %.3f") % One % Two).str();
	}
	if (CountOfParams == 3)
	{
		sscanf_s(CommandUnprocessed.c_str(), "%f, %f, %f", &One, &Two, &Three);
		CommandParamsProcess = (boost::format("%.3f, %.3f, %.3f") % One % Two % Three).str();
	}

	Checked = true;
}
