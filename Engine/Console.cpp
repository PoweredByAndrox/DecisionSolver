#include "pch.h"

#include "Console.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "File_system.h"

shared_ptr<Commands> Console::ProcessCommand = make_shared<Commands>();
shared_ptr<dialogs> Console::Dialog;

HRESULT Console::Init()
{
	try
	{
		ProcessCommand->Init();
		Dialog = Application->getUI()->getDialog("Console");

		auto Log = Application->getFS()->getDataFromFileVector(Application->getFS()->getLogFName().string(), true);

		for (size_t i = 0; i < Log.size(); i++)
		{
			if (Log.at(i).find("[ERROR]") != string::npos)
				Dialog->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Error,
					Log.at(i));
			else if (Log.at(i).find("[INFO]") != string::npos)
				Dialog->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Information,
					Log.at(i));
			else
				Dialog->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(Type::Normal,
					Log.at(i));
		}

		InitClass = true;
		return S_OK;
	}
	catch (const exception &Catch)
	{
#if defined (_DEBUG)
		DebugTrace(string(string("Console::catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
#if defined (ExceptionWhenEachError)
		throw exception(string(string("Console::catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
		Console::LogError(string(string("Console: Something is wrong with Console Init Function!\nReturn Error Text:")
			+ Catch.what()).c_str());
		InitClass = false;
		return E_FAIL;
	}
}

void Console::Reload()
{
	Application->getUI()->ReloadXML(Application->getFS()->GetFile(string("All.xml"))->PathA.c_str());
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

	auto IText = Dialog->getComponents()->Itext.back();
	auto text = IText->GetText();
	auto History = ProcessCommand->getHistoryCommands();
	auto TextList = Dialog->getComponents()->TList.back();
	if (text.empty() && !(IText->isPressUp() || IText->isPressDown()))
	{
		TextList->clearItems();
		return;
	}

	if (!IText->isActive() && !History.empty() && (IText->isPressUp() || IText->isPressDown()))
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
			auto cmd = ProcessCommand->FindPieceCommand(Dialog, text);
			if (cmd.operator bool() && !cmd->CommandStr.empty())
			{
				if (cmd->type == 1)
				{
					if (!TextList->FindInItems(cmd->CommandStr))
						TextList->addItem(cmd->CommandStr);
				}
				else
					if (!TextList->FindInItems(cmd->CommandStr + string(" ") + cmd->CommandNeededParams))
						TextList->addItem(cmd->CommandStr + string(" ") + cmd->CommandNeededParams);
			}
		}
	}

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (!Dialog->getComponents()->childs.empty())
		Dialog->getComponents()->childs.back()->setSize(ImVec2(0, -footer_height_to_reserve));
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

void Console::AddCmd(LPCSTR Text)
{
	if (!Text || !ProcessCommand.operator bool() || !Dialog.operator bool())
		return;
	ProcessCommand->Work(Dialog, Text);
}

void Console::LogError(string Msg)
{
	if (Msg.empty())
		return;

	if (!ProcessCommand.operator bool() || !Dialog.operator bool() || !Application->getUI().operator bool()
		|| !Application->getUI()->getDialog("Console").operator bool()
		|| Application->getUI()->getDialog("Console")->getComponents()->childs.empty()
		|| Application->getUI()->getDialog("Console")->getComponents()->childs.back()->getComponent()->UText.empty())
	{
		File_system::AddTextToLog(Msg, Type::Error);
		return;
	}

	Application->getUI()->getDialog("Console")->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(
		Type::Error, Msg);

	File_system::AddTextToLog(Msg, Type::Error);
}

void Console::LogInfo(string Msg)
{
	if (Msg.empty())
		return;

	if (!ProcessCommand.operator bool() || !Dialog.operator bool() || !Application->getUI().operator bool()
		|| !Application->getUI()->getDialog("Console").operator bool()
		|| Application->getUI()->getDialog("Console")->getComponents()->childs.empty()
		|| Application->getUI()->getDialog("Console")->getComponents()->childs.back()->getComponent()->UText.empty())
	{
		File_system::AddTextToLog(Msg, Type::Information);
		return;
	}

	Application->getUI()->getDialog("Console")->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(
		Type::Information, Msg);

	File_system::AddTextToLog(Msg, Type::Information);
}

void Console::LogNormal(string Msg)
{
	if (Msg.empty())
		return;

	if (!ProcessCommand.operator bool() || !Dialog.operator bool() || !Application->getUI().operator bool()
		|| !Application->getUI()->getDialog("Console").operator bool()
		|| Application->getUI()->getDialog("Console")->getComponents()->childs.empty()
		|| Application->getUI()->getDialog("Console")->getComponents()->childs.back()->getComponent()->UText.empty())
	{
		File_system::AddTextToLog(Msg, Type::Normal);
		return;
	}

	Application->getUI()->getDialog("Console")->getComponents()->childs.back()->getComponent()->UText.back()->AddCLText(
		Type::Normal, Msg);

	File_system::AddTextToLog(Msg, Type::Normal);
}

/*
void CALLBACK Engine::Console::OnGUIEvent(UINT nEvent, int nControlID, Control *pControl,
vector<void *> pUserContext)
{
	USES_CONVERSION;

	Button *Cache_Button = nullptr;
	EditBox *Cache_Edit = nullptr;
	ListBox *Chat = nullptr;

	Dialog *All = (Dialog *)pUserContext.at(1);
	if (All)
	{
		for (int i = 0; i < All->getAllControls().size(); i++)
		{
			if (All->getAllControls().at(i)->GetType() == CONTROL_EDITBOX)
				Cache_Edit = (EditBox *)All->getAllControls().at(i);
			else if (All->getAllControls().at(i)->GetType() == CONTROL_LISTBOX)
				Chat = (ListBox *)All->getAllControls().at(i);
		}
	}

	if (pControl->GetType() == CONTROL_TYPE::CONTROL_BUTTON)
	{
		Cache_Button = (Button *)pControl;
		if (Cache_Button != nullptr & Cache_Button->m_bMouseOver)
		{
			if (All)
			{
				if (wcsstr(Cache_Edit->GetText(), L"Close Dialog"))
				{
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					All->SetMinimized(true);
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Reload XML"))
				{
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);
					Cache_UI->getUI()->ReloadXML(Cache_UI->FS->GetFile(string("All.xml"))->PathA.c_str());
					Cache_UI->Settings(true);
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Get_size_phys_obj"))
				{
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					Chat->AddItem(wstring(wstring(L"Size of Dynamic PhysX Objects is: ") +
					wstring(to_wstring(Cache_UI->Phys->GetPhysDynamicObject().size()))).c_str(), All);
					Chat->AddItem(wstring(wstring(L"Size of Static PhysX Objects is: ") +
					wstring(to_wstring(Cache_UI->Phys->GetPhysStaticObject().size()))).c_str(), All);
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Get_size_model"))
				{
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					Chat->AddItem(wstring(wstring(L"Size of Models is: ") +
					wstring(to_wstring(Cache_UI->level->getObjects().size()))).c_str(), All);
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Set_phys_obj_pos"))
				{
					string Text = string(W2A(Cache_Edit->GetText()));
					if (wstring(Cache_Edit->GetText()) == L"Set_phys_obj_pos")
						deleteWord(Text, string("Set_phys_obj_pos "));
					else
						deleteWord(Text, string("Set_phys_obj_position "));

					Vector3 XYZ = Vector3::Zero;
					int ID = 0, Type = 0;
					to_lower(Text);
					sscanf_s(Text.c_str(), "%d, %d, %f, %f, %f", &Type, &ID, &XYZ.x, &XYZ.y, &XYZ.z);

					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					if (Type == 0)
					{
						auto Obj = Cache_UI->Phys->GetPhysDynamicObject();
						if (Obj.size()-1 >= ID)
							Obj.at(ID)->setGlobalPose(PxTransform(ToPxVec3(XYZ)));
						else
						{
							Chat->AddItem(L"Wrong ID!!! Return.", All);
							return;
						}
					}
					if (Type == 1)
					{
						auto Obj = Cache_UI->Phys->GetPhysStaticObject();
						if (Obj.size()-1 >= ID)
							Obj.at(ID)->setGlobalPose(PxTransform(ToPxVec3(XYZ)));
						else
						{
							Chat->AddItem(L"Wrong ID!!! Return.", All);
							return;
						}
					}
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Set_model_pos"))
				{
					string Text = string(W2A(Cache_Edit->GetText()));
					if (wstring(Cache_Edit->GetText()) == L"Set_model_pos")
						deleteWord(Text, string("Set_model_pos "));
					else
						deleteWord(Text, string("Set_model_position "));
					Vector3 XYZ = Vector3::Zero;
					int ID = 0;
					to_lower(Text);
					sscanf_s(Text.c_str(), "%d, %f, %f, %f", &ID, &XYZ.x, &XYZ.y, &XYZ.z);

					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					auto Obj = Cache_UI->level->getObjects();
					if (Obj.size() - 1 >= ID)
						Obj.at(ID).model->setPosition(XYZ);
					else
					{
						Chat->AddItem(L"Wrong ID!!! Return.", All);
						return;
					}
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Set_model_scale"))
				{
					string Text = string(W2A(Cache_Edit->GetText()));
					deleteWord(Text, string("Set_model_scale "));
					Vector3 XYZ = Vector3::Zero;
					int ID = 0;
					to_lower(Text);
					sscanf_s(Text.c_str(), "%d, %f, %f, %f", &ID, &XYZ.x, &XYZ.y, &XYZ.z);

					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					auto Obj = Cache_UI->level->getObjects();
					if (Obj.size() - 1 >= ID)
						Obj.at(ID).model->setScale(XYZ);
					else
					{
						Chat->AddItem(L"Wrong ID!!! Return.", All);
						return;
					}
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Set_model_rotate"))
				{
					string Text = string(W2A(Cache_Edit->GetText()));
					deleteWord(Text, string("Set_model_rotate "));
					Vector3 XYZ = Vector3::Zero;
					int ID = 0;
					to_lower(Text);
					sscanf_s(Text.c_str(), "%d, %f, %f, %f", &ID, &XYZ.x, &XYZ.y, &XYZ.z);

					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					auto Obj = Cache_UI->level->getObjects();
					if (Obj.size() - 1 >= ID)
						Obj.at(ID).model->setRotation(XYZ);
					else
					{
						Chat->AddItem(L"Wrong ID!!! Return.", All);
						return;
					}
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Clear"))
					Chat->RemoveAllItems();
				else if (wcsstr(Cache_Edit->GetText(), L"Get_pos_models"))
				{
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					for (int i = 0; i < Cache_UI->level->getObjects().size(); i++)
					{
						auto Cache = Cache_UI->level->getObjects().at(i).model;
						wstring buff = formatstr("X: %f, Y: %f, Z: %f", Cache->getPosition().x,
						Cache->getPosition().y, Cache->getPosition().z);
						Chat->AddItem(wstring(wstring(L"[")+wstring(to_wstring(i)) + wstring(L"] Position of Model[ID: ") +
						wstring(A2W(Cache_UI->level->getObjects().at(i).ID_TEXT))+wstring(L"] is: ") + buff).c_str(), All);
					}
					for (int i = 0; i < Cache_UI->level->getNPC().size(); i++)
					{
						auto Cache = Cache_UI->level->getNPC().at(i).model;
						wstring buff = formatstr("X: %f, Y: %f, Z: %f", Cache->getPosition().x,
						Cache->getPosition().y, Cache->getPosition().z);
						Chat->AddItem(wstring(wstring(L"[") + wstring(to_wstring(i)) + wstring(L"] Position of Model[ID: ") +
						wstring(A2W(Cache_UI->level->getNPC().at(i).ID_TEXT)) + wstring(L"] is: ") + buff).c_str(), All);
					}
				}
				else
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText()) +
					wstring(L" -> Nothing Happens")).c_str(), All);
			}
		}
	}
}
*/