#include "pch.h"

#include "Console.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"
#include "UI.h"

shared_ptr<Commands> Console::ProcessCommand = make_shared<Commands>();
shared_ptr<dialogs> Console::Dialog;

HRESULT Console::Init()
{
	if (!Application->getUI().operator bool()) return E_FAIL;

	Dialog = Application->getUI()->getDialog("Console");
	if (!Dialog.operator bool() || Dialog->GetTitle().empty())
	{
		Engine::LogError("Console::Init() failed!",
			"Console::Init() failed!!!", "Console: Something is wrong with getting 'Console Dialog From XML'!");
		return E_POINTER;
	}

	ProcessCommand->Init();

	ChangeState(Dialog->getVisible() ? Console_STATE::Open : Console_STATE::Close);

	auto Log = Application->getFS()->getDataFromFileVector(Application->getFS()->getLogFName().string(), true);

	for (size_t i = 0; i < Log.size(); i++)
	{
		auto Cmp = Dialog->getComponents()->FindComponentChild("##ConsoleTextBox")->getMassComponents().back();
		if (Log.at(i).find("[ERROR]") != string::npos)
			Cmp->FindComponentUText("##CText")->AddText(Type::Error, Log.at(i));
		else if (Log.at(i).find("[INFO]") != string::npos)
			Cmp->FindComponentUText("##CText")->AddText(Type::Information, Log.at(i));
		else
			Cmp->FindComponentUText("##CText")->AddText(Type::Normal, Log.at(i));
	}

	InitClass = true;
	return S_OK;
}

void Console::Reload()
{
	Application->getUI()->ReloadXML(Application->getFS()->GetFile("All.xml")->PathA.c_str());
	Dialog = Application->getUI()->getDialog("Console");
}

void Console::Render()
{
	if (CState == Console_STATE::Close)
		return;

	Dialog = Application->getUI()->getDialog("Console");
	if (!Dialog.operator bool() || Dialog->GetTitle().empty())
		return;

	Dialog->Render();

	auto IText = Dialog->getComponents()->FindComponentIText("##Console_CmdIText");
	auto text = IText->GetText();
	auto History = ProcessCommand->getHistoryCommands();
	auto TextList = Dialog->getComponents()->FindComponentTList("##HintCmd");
	ToDo("Change the following buttons to work with History in Console");
	if (text.empty() && !Application->getTrackerKeyboard().pressed.Tab)
	{
		TextList->clearItems();
		return;
	}

	// History
	if (IText->getHistory() && !IText->isActive() && !History.empty() &&
		Application->getTrackerKeyboard().pressed.Tab)
	{
		int PosHistory = ProcessCommand->getPosHistory();
		if (PosHistory == -1 || PosHistory >= (int)History.size())
		{
			ProcessCommand->changePosHistory(-1);
			PosHistory = ProcessCommand->getPosHistory();
		}

		if (PosHistory == -1)
			IText->ChangeText("");
		else
			IText->ChangeText(History.at(PosHistory));

		PosHistory++;
		ProcessCommand->changePosHistory(PosHistory);
	}

	if (TextList->IsMouseSelected() && !text.empty() && TextList->getSelectedIndx() != -1 && !TextList->getItems().empty())
	{
		string text = TextList->getSelectedIndxString(TextList->getSelectedIndx());
		if (text.find("#") != string::npos)
			deleteWord(text, "#", ModeProcessString::UntilTheEnd);
		IText->ChangeText(text);
	}

	if (IText->getTextChange())
		ProcessCommand->Work(Dialog, text);

	if (!text.empty())
	{
		for (size_t i = 0; i < ProcessCommand->getAllCommands().size(); i++)
		{
			auto cmd = ProcessCommand->FindPieceCommand(text);
			if (cmd.operator bool() && !cmd->CommandStr.empty())
			{
				if (cmd->type == 1)
				{
					if (!TextList->FindInItems(cmd->CommandStr))
						TextList->addItem(cmd->CommandStr);
				}
				else if (!TextList->FindInItems(cmd->CommandStr + " " + cmd->CommandNeededParams))
						TextList->addItem(cmd->CommandStr + " " + cmd->CommandNeededParams);
			}
			else
				TextList->clearItems();
		}
	}

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (!Dialog->getComponents()->childs.empty())
		Dialog->getComponents()->FindComponentChild("##ConsoleTextBox")->setSize(ImVec2(0, -footer_height_to_reserve));
}

void Console::OpenConsole()
{
	if (CState == Console_STATE::Close)
	{
		Application->getUI()->EnableDialog("Console");
		CState = Console_STATE::Open;
		return;
	}
	if (CState == Console_STATE::Open)
	{
		Application->getUI()->DisableDialog("Console");
		CState = Console_STATE::Close;
		return;
	}
}

void Console::PushCMD(LPCSTR Text)
{
	if (!Text || !ProcessCommand.operator bool() || !Dialog.operator bool())
		return;

	ProcessCommand->Work(Dialog, Text);
}

void Console::LogError(string Msg)
{
	if (Msg.empty() || !Application->getUI().operator bool())
		return;

	auto Consl = Application->getUI()->getDialog("Console");
	if (!ProcessCommand.operator bool() || !Dialog.operator bool() || !Application->getUI().operator bool()
		|| !Consl.operator bool()
		|| Consl->getComponents()->childs.empty()
		|| Consl->getComponents()->FindComponentChild("##ConsoleTextBox")->GetID().empty())
	{
		File_system::AddTextToLog(Msg, Type::Error);
		return;
	}

	Consl->getComponents()->FindComponentChild("##ConsoleTextBox")->getMassComponents().front()->
		FindComponentUText("##CText")->AddText(Type::Error, Msg);

	File_system::AddTextToLog(Msg, Type::Error);
}

extern bool IsNotification;
extern void CreateNotification(string Text, Vector4 Color);

void Console::LogInfo(string Msg)
{
	if (Msg.empty() || !Application->getUI().operator bool())
		return;

	if (IsNotification)
		CreateNotification(Msg, Colors::Orange.operator DirectX::XMVECTOR());

	auto Consl = Application->getUI()->getDialog("Console");
	if (!ProcessCommand.operator bool() || !Dialog.operator bool() || !Application->getUI().operator bool()
		|| !Consl.operator bool()
		|| Consl->getComponents()->childs.empty()
		|| Consl->getComponents()->FindComponentChild("##ConsoleTextBox")->GetID().empty())
	{
		File_system::AddTextToLog(Msg, Type::Information);
		return;
	}

	Consl->getComponents()->FindComponentChild("##ConsoleTextBox")->getMassComponents().front()->
		FindComponentUText("##CText")->AddText(Type::Information, Msg);

	File_system::AddTextToLog(Msg, Type::Information);
}

void Console::LogNormal(string Msg)
{
	if (Msg.empty() || !Application->getUI().operator bool())
		return;

	auto Consl = Application->getUI()->getDialog("Console");
	if (!ProcessCommand.operator bool() || !Dialog.operator bool() || !Application->getUI().operator bool()
		|| !Consl.operator bool()
		|| Consl->getComponents()->childs.empty()
		|| Consl->getComponents()->FindComponentChild("##ConsoleTextBox")->GetID().empty())
	{
		File_system::AddTextToLog(Msg, Type::Normal);
		return;
	}

	Consl->getComponents()->FindComponentChild("##ConsoleTextBox")->getMassComponents().front()->
		FindComponentUText("##CText")->AddText(Type::Normal, Msg);

	File_system::AddTextToLog(Msg, Type::Normal);
}
