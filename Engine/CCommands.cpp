#include "pch.h"

#include "CCommands.h"
//#include "CLua.h"
#include "UI.h"
#include "Physics.h"
#include "Camera.h"

ToDo("CLua: LUA Is Disabled Now In Engine!");
static const vector<string> ListCommands =
{
	string("help"), string("testmsg"),
	string("quit"), string("clear"),
	string("dotorque"), string("cleanphysbox")
};
static const vector<string> ListCommandsWithParams =
{
	string("changesize #float W, #float H"), string("addphysbox #float MassObj, #float SizeModel"),
	//string("exec_lua #PathToFile.function(param)")
};

#include "File_system.h"
void Commands::Work(shared_ptr<dialogs> &Console, string Text)
{
	if (!Text.empty())
	{
		auto cmd = FindPieceCommand(Console, Text);
		if (cmd.operator bool() && !cmd->CommandStr.empty())
		{
			cmd->TypedCmd = Text;
			ExecCommand(Console, cmd);
		}
		else
			Console->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Error,
				string(string("You're typed: ") + Text + string("\n[error]: Unknown command type Help for help!")));

		File_system::AddTextToLog(string("\n") + string("> ") + Text + string("\n"), Type::Normal);
	}
}

void Commands::Init()
{
	for (size_t i = 0; i < ListCommands.size(); i++)
	{
		commands.push_back(make_shared<Command>(ListCommands.at(i), "", Command::TypeOfCommand::WithoutParam));
	}
	for (size_t i = 0; i < ListCommandsWithParams.size(); i++)
	{
		if (ListCommandsWithParams.at(i) == "exec_lua #PathToFile.function(param)") // HardCode CMD!!!
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
			Console->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Error,
				string(cmd->CommandStr + string(": No parameters found! You must add several parameters, such like: ") +
					cmd->CommandNeededParams));
			return;
		}

		cmd->CheckNeededParam();
	}

	string CMD = cmd->CommandStr;
	to_lower(CMD);

	if (cmd->type == Command::TypeOfCommand::WithoutParam)
	{
		if (contains(CMD, "help"))
		{
			string all;
			for (size_t i = 0; i < ListCommands.size(); i++)
			{
				all.append(string(string("\n") + ListCommands.at(i)));
			}

			Console->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(
				Type::Information, string("#list of available commands: ") + all);
		}
		else if (contains(CMD, "quit"))
			Application->Quit();
		else if (contains(CMD, "clear"))
		{
			Console->getComponents()->childs.back()->getComponent()->UText.back()->ClearText();
			File_system::ClearLogs();
			return;
		}
		else if (contains(CMD, "dotorque"))
		{
			auto ObjPhys = Application->getPhysics()->GetPhysDynamicObject();
			if (!ObjPhys.empty())
			{
				Application->getPhysics()->AddTorque(ObjPhys.at((std::rand() % ObjPhys.size())),
					PxVec3(
						Application->getCamera()->GetEyePt().x,
						Application->getCamera()->GetEyePt().y,
						Application->getCamera()->GetEyePt().z), PxForceMode::eFORCE);
			}
			else
				Console->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Error,
					string(CMD + string(": GetPhysDynamicObject() return NULL!!!")));
		}
		else if (contains(CMD, "cleanphysbox"))
			Application->getPhysics()->ClearAllObj();
	}
	if (cmd->type == Command::TypeOfCommand::Lua)
	{
		//if (contains(CMD, "exec_lua"))
		//	Application->getCLua()->callFunction(cmd->S_One, cmd->S_Two, cmd->S_Three);
	}
	if (cmd->type == Command::TypeOfCommand::WithParam)
	{
		if (contains(CMD, "changesize"))
			Console->ChangeSize(cmd->One, cmd->Two);
		else if (contains(CMD, "addphysbox"))
		{
			float Size = cmd->Two, Mass = cmd->One;
			Application->getPhysics()->AddNewActor(Application->getCamera()->GetEyePt(), Vector3(Size, Size, Size),
				Mass, Size);
		}
	}

	Console->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Normal,
		string(cmd->CommandStr + string(" #Apply")));

	for (size_t i = 0; i < History.size(); i++)
	{
		if (History.at(i) == (cmd->TypedCmd))
		{
			cmd->CommandParamsProcess = cmd->CommandUnprocessed = "";
			cmd->One = cmd->Two = cmd->Three = 0.f;
			cmd->CountOfParams = 0;
			cmd->Checked = false;
			cmd->TypedCmd = "";
			return;
		}
	}

	History.push_back(cmd->TypedCmd);
}

shared_ptr<Commands::Command> Commands::FindPieceCommand(shared_ptr<dialogs> &Console, string Text)
{
	for (size_t i = 0; i < commands.size(); i++)
	{
		string GetCommand = Text;
		string GetParam = Text;

		if (GetParam.length() > commands.at(i)->CommandStr.length())
			deleteWord(GetParam, " ", ModeProcessString::UntilTheBegin, false, false);

		deleteWord(GetCommand, " ", ModeProcessString::UntilTheEnd);
		to_lower(GetCommand);

		if (contains(commands.at(i)->CommandStr, GetCommand))
		{
			if (Text.length() <= commands.at(i)->CommandStr.length() || GetParam.empty())
				return commands.at(i);

			commands.at(i)->CommandUnprocessed = GetParam;
			return commands.at(i);
		}
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
