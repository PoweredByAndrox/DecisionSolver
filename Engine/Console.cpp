#include "pch.h"

#include "Console.h"

HRESULT EngineNS::Console::Init(Physics *Phys, Levels *level)
{
	try
	{
		//if (!ui->IsInitUI())
		//{
			ui->Init(1, Application->getFS()->GetFile(string("Main_texures_UI.dds"))->PathW.c_str());
			ui->LoadXmlUI(Application->getFS()->GetFile(string("All.xml"))->PathA.c_str());
		//}

		this->Phys.reset(Phys);

		this->level = level;

		//V(Settings(false));

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

/*
HRESULT Engine::Console::Settings(bool Reset)
{
	USES_CONVERSION;
	try
	{
		vector<int> W = ui->getW(), H = ui->getH(), X = ui->getX(), Y = ui->getY();
		vector<LPCSTR> Text = ui->getText();

		if (!Reset)
			ui->getDialog()->at(0)->SetCallback(OnGUIEvent, vector<void *>{this});

		if (!Reset)
		{
			ui->getDialog()->at(0)->AddButton(0, A2W(Text.at(0)), X.at(0), Y.at(0), W.at(1), H.at(1));
			ui->getDialog()->at(0)->AddListBox(2, X.at(1), Y.at(1), W.at(2), H.at(2));
			ui->getDialog()->at(0)->AddEditBox(1, A2W(Text.at(1)), X.at(2), Y.at(2), W.at(3), H.at(3));
		}
		else
		{
			ui->getDialog()->at(0)->GetButton(0)->SetText(A2W(Text.at(0)));
			ui->getDialog()->at(0)->GetButton(0)->SetLocation(X.at(0), Y.at(0));
			ui->getDialog()->at(0)->GetButton(0)->SetSize(W.at(1), H.at(1));

			ui->getDialog()->at(0)->GetListBox(2)->SetLocation(X.at(1), Y.at(1));
			ui->getDialog()->at(0)->GetListBox(2)->SetSize(W.at(2), H.at(2));

			//ui->getDialog()->at(0)->GetEditBox(1)->SetText(A2W(Text.at(1)));
			ui->getDialog()->at(0)->GetEditBox(1)->SetLocation(X.at(2), Y.at(2));
			ui->getDialog()->at(0)->GetEditBox(1)->SetSize(W.at(3), H.at(3));
		}

		ui->getDialog()->at(0)->SetSize(W.at(0), H.at(0));
		ui->getDialog()->at(0)->SetBackgroundColors(D3DCOLOR_ARGB(rand() % 255, rand() % 255, rand() % 255, rand() % 255),
			D3DCOLOR_ARGB(rand() % 255, rand() % 255, rand() % 105, rand() % 255),
			D3DCOLOR_ARGB(rand() % 255, rand() % 298, rand() % 255, rand() % 255),
			D3DCOLOR_ARGB(rand() % 255, rand() % 10, rand() % 764, rand() % 179));
		ui->getDialog()->at(0)->SetLocation(W.at(0), H.at(0));
		ui->getDialog()->at(0)->SetCaptionText(L"Console!");

		return S_OK;
	}
	catch (const std::exception &)
	{
		DebugTrace("Console->Settings() is failed.\n");
		throw exception("Console is failed!!!");

		return E_FAIL;
	}
}

void Engine::Console::Render(float Time)
{
	if (CState == Console_STATE::Close)
		return;

	ui->Render(Time);
	ui->getDialog()->at(0)->SetSize(DXUTGetDXGIBackBufferSurfaceDesc()->Width, DXUTGetDXGIBackBufferSurfaceDesc()->Height -256);
	ui->getDialog()->at(0)->SetLocation(0, DXUTGetDXGIBackBufferSurfaceDesc()->Height -256);
	ui->getDialog()->at(0)->GetListBox(2)->SetSize(ui->getDialog()->at(0)->GetWidth(), ui->getDialog()->at(0)->GetListBox(2)->m_height);
	ui->getDialog()->at(0)->GetButton(0)->SetLocation(ui->getDialog()->at(0)->GetWidth() / 3, ui->getDialog()->at(0)->GetButton(0)->m_y);
	ui->getDialog()->at(0)->GetEditBox(1)->SetLocation(ui->getDialog()->at(0)->GetButton(0)->m_x + 150, ui->getDialog()->at(0)->GetEditBox(1)->m_y);
	ui->getDialog()->at(0)->GetEditBox(1)->SetSize(ui->getDialog()->at(0)->GetWidth() / 4,
		ui->getDialog()->at(0)->GetEditBox(1)->m_height);
}

void Engine::Console::Open()
{
	if (CState == Console_STATE::Close)
	{
		ui->getDialog()->at(0)->SetMinimized(false);
		CState = Console_STATE::Open;
	}
}

void Engine::Console::Close()
{
	if (CState == Console_STATE::Open)
	{
		ui->getDialog()->at(0)->SetMinimized(true);
		CState = Console_STATE::Close;
	}
}
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