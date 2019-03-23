#include "pch.h"

#include "Console.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

HRESULT Console::Init()
{
	try
	{
		Dialog = Application->getUI()->getDialog("Console");

		InitClass = true;
		return S_OK;
	}
	catch (const exception &Catch)
	{
		DebugTrace(string(string("Console: Init is failed. ") + string(Catch.what())).c_str());
		throw exception("Console is failed!!!");
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

	if (Dialog->getChilds().back()->getITexts().back()->getTextChange())
		command->Work(Dialog->getChilds().back()->getUTexts().back(), Dialog->getChilds().back()->getITexts().back()->GetText());

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	Dialog->getChilds().back()->setSize(ImVec2(0, -footer_height_to_reserve));
}

void Console::OpenConsole()
{
	if (CState == Console_STATE::Close)
	{
		Application->getUI()->DisableDialog("Console");
		CState = Console_STATE::Open;
	}
}

void Console::CloseConsole()
{
	if (CState == Console_STATE::Open)
	{
		Application->getUI()->EnableDialog("Console");
		CState = Console_STATE::Close;
	}
}

/*
void CALLBACK Engine::Console::OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, vector<void *> pUserContext)
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

					Chat->AddItem(wstring(wstring(L"Size of Dynamic PhysX Objects is: ") + wstring(to_wstring(Cache_UI->Phys->GetPhysDynamicObject().size()))).c_str(), All);
					Chat->AddItem(wstring(wstring(L"Size of Static PhysX Objects is: ") + wstring(to_wstring(Cache_UI->Phys->GetPhysStaticObject().size()))).c_str(), All);
				}
				else if (wcsstr(Cache_Edit->GetText(), L"Get_size_model"))
				{
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText())).c_str(), All);
					Console *Cache_UI = (Console *)pUserContext.at(0);

					Chat->AddItem(wstring(wstring(L"Size of Models is: ") + wstring(to_wstring(Cache_UI->level->getObjects().size()))).c_str(), All);
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
						wstring buff = formatstr("X: %f, Y: %f, Z: %f", Cache->getPosition().x, Cache->getPosition().y, Cache->getPosition().z);
						Chat->AddItem(wstring(wstring(L"[")+wstring(to_wstring(i)) + wstring(L"] Position of Model[ID: ")+wstring(A2W(Cache_UI->level->getObjects().at(i).ID_TEXT))+wstring(L"] is: ") + buff).c_str(), All);
					}
					for (int i = 0; i < Cache_UI->level->getNPC().size(); i++)
					{
						auto Cache = Cache_UI->level->getNPC().at(i).model;
						wstring buff = formatstr("X: %f, Y: %f, Z: %f", Cache->getPosition().x, Cache->getPosition().y, Cache->getPosition().z);
						Chat->AddItem(wstring(wstring(L"[") + wstring(to_wstring(i)) + wstring(L"] Position of Model[ID: ") + wstring(A2W(Cache_UI->level->getNPC().at(i).ID_TEXT)) + wstring(L"] is: ") + buff).c_str(), All);
					}
				}
				else
					Chat->AddItem(wstring(wstring(L"You're typed: ") + wstring(Cache_Edit->GetText()) + wstring(L" -> Nothing Happens")).c_str(), All);
			}
		}
	}
}
*/