#include "SDKInterface.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "GameObjects.h"
#include "Levels.h"
#include "File_system.h"
#include "CutScene.h"
#include "Camera.h"
#include "SimpleLogic.h"
#include "Audio.h"
#include "DebugDraw.h"
#include "Models.h"
#include "Actor.h"

//pair<float, bool> DragFloat(string ID, float Thing)
//{
//	ImGui::SameLine();
//	pair<float, bool> Ret_Thing = make_pair(Thing, false);
//	if (Application->getMouse()->GetState().middleButton)
//	{
//		float f = (static_cast<float>(Application->getMouse()->GetState().scrollWheelValue) / 120.f);
//		Ret_Thing.second = ImGui::DragFloat(ID.c_str(), &f, 0.001f);
//		Ret_Thing.first = f;
//	}
//	else
//		Ret_Thing.second = ImGui::DragFloat(ID.c_str(), (float *)&Ret_Thing, 0.001f);
//	return Ret_Thing;
//}

static vector<string> values = { "Stay", "WalkToNPoint", "Follow" };
static vector<string> TrueFalse = { "True", "False" };
static vector<string> LeftRightWithout = { "Left", "Right", "Without" };
int Current = 0, Cur = 1;
float NewTarget = 0.f, Aud_Targt1 = 0.f, Aud_Targt2 = 1.f, Aud_Targt3 = 0.5f;
static bool dont_ask_me_next_time = false;

bool IsNeed = false, NeedUndo = false, StepTimer = false;

shared_ptr<Child> Main;

pair<Vector3, bool> DragFloat3(string ID, Vector3 Thing)
{
	pair<Vector3, bool> Ret_Thing = make_pair(Thing, false);
	Ret_Thing.second = ImGui::DragFloat3(ID.c_str(), (float *)&Ret_Thing, 0.001f);
	if (!ImGui::IsItemActive())
		NeedUndo = true;

	return Ret_Thing;
}

static std::function<void(void)> ShowPopup;
pair<string, pair<std::function<void(void)>, std::function<void(void)>>> store;
bool local;

void SDKInterface::WarningDial(string Name, std::function<void(void)> OK,
	std::function<void(void)> Cancel)
{
	store = make_pair(Name, make_pair(OK, Cancel));
	ShowPopup = ([&]
	{
		ImGui::OpenPopup(store.first.c_str());

		if (dont_ask_me_next_time)
		{
			ImGui::CloseCurrentPopup();
			ShowPopup = nullptr;
		}

		if (Application->getLevel()->IsNotSaved())
		{
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal(store.first.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("You have unsaved changes. Are you sure skip changes and do it?"\
					"\nThis operation cannot be undone!"\
					"\nOK-Ignore SaveAnd Do Action\tCancel-Save And Do Action");
				ImGui::Separator();
				
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::Checkbox("Don't ask me next time", &local);
				ImGui::PopStyleVar();

				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("OK", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					dont_ask_me_next_time = local;
					Save();
					store.second.first();
					ShowPopup = nullptr;
					ImGui::EndPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					Save();
					ImGui::CloseCurrentPopup();
					ShowPopup = nullptr;
					store.second.second();
					dont_ask_me_next_time = local;
					ImGui::EndPopup();
				}
				ImGui::EndPopup();
			}
		}
		else
		{
			store.second.first();
			ShowPopup = nullptr;
		}
	});
}

void SDKInterface::Render()
{
	auto LevelObjs = Application->getLevel()->getChild();

	ImGuiID dockspaceID = 0;
	ImGui::SetNextWindowPos(ImVec2(Application->getWorkAreaSize(Application->GetHWND()).x -
		ImGui::GetWindowWidth() - 100.f,
		0.f), ImGuiCond_::ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	if (LOGO)
	{
		if (ImGui::Begin("List Of Game Objects", &LOGO))
		{
			ImGui::Columns(2);
			ImGui::Separator();

			ImGui::AlignTextToFramePadding();

			bool Othrs = ImGui::TreeNode("Others Object");
			if (Othrs)
			{
				int i = 0;

				if (LevelObjs)
					for (auto Object: LevelObjs->GetNodes())
					{
						if (Object->SaveInfo->IsRemoved) continue;
						i++;
						if (ImGui::Button((" X: " + to_string(i)).c_str()))
						{
							Object->SaveInfo->IsRemoved = true;
							Object->SaveInfo->T = Object->GM->GetType();
							AddToUndo("Removed Object: " + Object->ID, to_string(false), to_string(true));
							continue; // Don't work with this object anymore
						}
						ImGui::SameLine();
						if (ImGui::TreeNode((Object->RenderName.c_str() + string("##") + to_string(i)).c_str()))
						{
							ImGui::SetNextItemWidth(-1);
							if (ImGui::Checkbox("Is Render", &Object->GM->RenderIt))
							{
								Object->IsItChanged = true;
								Object->SaveInfo->IsVisible = Object->GM->RenderIt;
								Object->SaveInfo->T = Object->GM->GetType();
							}
							ImGui::Separator();
							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("Pos#" + Object->ID).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");

							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);
							pair<Vector3, bool> Ret;
							Vector3 Crd = Object->GM->GetPositionCord();
							Object->GM->SetPositionCoords((Ret = DragFloat3(("##Pos#" + Object->ID).c_str(),
								Object->GM->GetPositionCord())).first);
							//RetX = DragFloat(("##PosX#" + Object->ID).c_str(), Crd.x);
							//RetY = DragFloat(("##PosY#" + Object->ID).c_str(), Crd.y);
							//RetZ = DragFloat(("##PosZ#" + Object->ID).c_str(), Crd.z);

							if (Ret.second && NeedUndo && Crd != Ret.first)
							{
								AddToUndo("Changed Pos In: " + Object->ID, Crd, Ret.first);
								Object->IsItChanged = true;
								Object->SaveInfo->Pos = true;
								Object->SaveInfo->T = Object->GM->GetType();
								NeedUndo = false;
							}

							ImGui::NextColumn();

							ImGui::Separator();
							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("Rot#" + Object->ID).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Rotation");

							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);

							Crd = Object->GM->GetRotCord();
							Object->GM->SetRotationCoords(
								(Ret = DragFloat3(("##Rot#" + Object->ID).c_str(), Object->GM->GetRotCord())).first);

							ImGui::NextColumn();
							ImGui::Separator();
							if (Ret.second && NeedUndo && Crd != Ret.first)
							{
								AddToUndo("Changed Rotation In: " + Object->ID, Crd, Ret.first);
								Object->IsItChanged = true;
								Object->SaveInfo->Rot = true;
								Object->SaveInfo->T = Object->GM->GetType();
								NeedUndo = false;
							}

							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("Scl#" + Object->ID).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Scale");

							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);

							Crd = Object->GM->GetScaleCord();
							Object->GM->SetScaleCoords(
								(Ret = DragFloat3(("##Scl#" + Object->ID).c_str(), Object->GM->GetScaleCord())).first);

							ImGui::NextColumn();
							ImGui::Separator();
							if (Ret.second && NeedUndo && Crd != Ret.first)
							{
								AddToUndo("Changed Scale In: " + Object->ID, Crd, Ret.first);
								Object->IsItChanged = true;
								Object->SaveInfo->Scale = true;
								Object->SaveInfo->T = Object->GM->GetType();
								NeedUndo = false;
							}

							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Reset All"))
							{
								// Do One Func To All Reset These Funcs
								Object->GM->SetPositionCoords(Object->GM->GetRPos());
								Object->GM->SetRotationCoords(Object->GM->GetRRot());
								Object->GM->SetScaleCoords(Object->GM->GetRScale());
							}
							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Add Logic"))
								Application->getLevel()->AddTo(Object, make_shared<SimpleLogic>());
							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Remove Logic"))
								Application->getLevel()->RemoveFrom(Object);

							ImGui::TreePop();
						}
						if (ImGui::BeginPopupContextItem(("##" + to_string(i)).c_str(),
							ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight))
						{
							string Copy = Object->RenderName;
							ImGui::Text("Edit name:");
							if (ImGui::InputText("##IText", &Object->RenderName))
							{
								Object->IsItChanged = true;
								AddToUndo("Changed A Node Name In: " + Object->ID, Copy, Object->RenderName);
							}
							if (ImGui::Button("Close"))
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
					}

				ImGui::TreePop();
			}

			//if (ImGui::Button("Reload Level"))
			//	Application->getLevel()->Reload_Level(
			//		Application->getFS()->GetFileByType(_TypeOfFile::LEVELS).back().first->PathA);
			ImGui::Columns(1);
		}
		dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None |
			ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
	}

	if (LevelObjs)
		for (auto It: LevelObjs->GetNodes())
		{
			if (It->GM->GetType() != GameObjects::TYPE::Sound_Obj) continue;
			auto ObjSnd = Application->getSound()->GetSound(It->ID);
			if (ObjSnd.operator bool())
			{
				if (Application->getTrackerKeyboard().pressed.F7)
					ObjSnd->Play();
				ObjSnd->setSoundPosition(It->GM->GetPositionCord());
				Application->getSound()->Update(Application->getCamera()->GetEyePt(),
					Application->getCamera()->GetWorldAhead(), Application->getCamera()->GetWorldUp());
			}
		}
	
	bool Ctrl = Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::LeftControl),
		Shift = Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::LeftShift);
	bool Z = ImGui::GetIO().KeysDown[Keyboard::Keys::Z], S = ImGui::GetIO().KeysDown[Keyboard::Keys::S],
		 O = ImGui::GetIO().KeysDown[Keyboard::Keys::O];

	ToDo("Need to Change this 'hardcode' keys!");
	if (Ctrl && O && IsNeed)
	{
		IsNeed = false;
		Open();
	}
	else
		IsNeed = true;
	if (Ctrl && S && IsNeed)
	{
		IsNeed = false;
		Save();
	}
	else
		IsNeed = true;
	if (Ctrl && !Shift && Z && IsNeed)
	{
		IsNeed = false;
		undo();
	}
	else
		IsNeed = true;
	if (Ctrl && Shift && S && IsNeed)
	{
		IsNeed = false;
		SaveAs();
	}
	else
		IsNeed = true;
		
	if (Ctrl && Shift && Z && IsNeed)
	{
		IsNeed = false;
		redo();
	}
	else
		IsNeed = true;

	ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	if (HoL)
	{
		if (ImGui::Begin("Hierarchy of Level", &HoL))
		{
			if (ImGui::Button("Add One"))
				ImGui::OpenPopup("##my_toggle_popup");
			if (ImGui::BeginPopup("##my_toggle_popup"))
			{
				ImGui::Separator();
				ImGui::AlignTextToFramePadding();
				if (ImGui::MenuItem("Test Phys Cobe"))
				{
					if (Application->getPhysics())
						Application->getPhysics()->SpawnObject();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Model"))
				{
					auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(Application->getFS()->getPathFromType(
						_TypeOfFile::MODELS).c_str()),
						"Dialog Add Models", "3DS (.3ds)\0*.3ds\0OBJECT (.obj)\0*.obj\0FBX (.fbx)\0*.fbx\0", true);
					if (Obj.first) // If Dialog Wasn't Skip
					{
						if (Obj.second.empty())
						{
							ImGui::EndPopup();
							ImGui::End();
							return;
						}

						for (auto It: Obj.second)
						{
							auto newNode = Application->getLevel()->Add(_TypeOfFile::MODELS, It);
							if (newNode)
							{
								// Need To Save It As New Object (or mark it)
								newNode->SaveInfo->T = newNode->GM->GetType();
								newNode->IsItChanged = true;
								newNode->SaveInfo->IsVisible = newNode->GM->RenderIt;
								newNode->SaveInfo->Pos = newNode->SaveInfo->Rot =
									newNode->SaveInfo->Scale = true;
							}
						}
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Sound Source")) // It Means That Add To Scene Sphere 3d Source of Sound
				{
					auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(
						Application->getFS()->getPathFromType(_TypeOfFile::SOUNDS).c_str()),
						"Dialog Add Sound Objects",
						"Sound File\0*.wav\0");
					if (Obj.first) // If Dialog Wasn't Skip
					{
						if (Obj.second.empty())
						{
							ImGui::EndPopup();
							ImGui::End();
							return;
						}

						for (auto It: Obj.second)
						{
							if (exists(Application->getFS()->getPathFromType(_TypeOfFile::MODELS)
								+ "cube_with_diffuse_texture.3ds"))
							{
								Application->getSound()->AddNewFile(It, true);
								auto OurNode = Application->getLevel()->Add(_TypeOfFile::MODELS,
									Application->getFS()->getPathFromType(_TypeOfFile::MODELS) +
									"cube_with_diffuse_texture.3ds");
								if (OurNode)
								{
									OurNode->ID = path(It).filename().string();
									OurNode->GM->SetScaleCoords(Vector3(0.01f, 0.01f, 0.01f));
									OurNode->GM->SetPositionCoords(Application->getCamera()->GetEyePt());
									OurNode->GM->SetType(OurNode->SaveInfo->T = GameObjects::TYPE::Sound_Obj);

									// Need To Save It As New Object (or mark it)
									OurNode->IsItChanged = true;
									OurNode->SaveInfo->IsVisible = OurNode->GM->RenderIt;
									OurNode->SaveInfo->Pos = OurNode->SaveInfo->Rot =
										OurNode->SaveInfo->Scale = true;
								}
							}
						}
					}
				}
				// Also would like to add particle source

				ImGui::EndPopup();
			}
			ImGui::Separator();
		}

		ImGui::End();
	}

	if (FR)
	{
		ToDo("Drag and drop here?");
		if (ImGui::Begin("File Resources", &FR))
		{
			UI::HelpMarker("Filter usage:\n"
				"  \"\"         display all lines\n"
				"  \"xxx\"      display lines containing \"xxx\"\n"
				"  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
				"  \"-xxx\"     hide lines containing \"xxx\"");

			filter.Draw();

			auto FS = Application->getFS();
			bool Snd = ImGui::TreeNode("Sounds");
			if (Snd)
			{
				auto SndFiles = FS->GetFileByType(_TypeOfFile::SOUNDS);
				for (auto it: SndFiles)
				{
					if (filter.PassFilter(it.first->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it.first->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it.first->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Mdl = ImGui::TreeNode("Models");
			if (Mdl)
			{
				auto MldFiles = FS->GetFileByType(_TypeOfFile::MODELS);
				for (auto it: MldFiles)
				{
					if (filter.PassFilter(it.first->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it.first->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it.first->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Txtrs = ImGui::TreeNode("Textures");
			if (Txtrs)
			{
				auto TxtrFiles = FS->GetFileByType(_TypeOfFile::TEXTURES);
				for (auto it: TxtrFiles)
				{
					if (filter.PassFilter(it.first->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it.first->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it.first->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Lvls = ImGui::TreeNode("Levels");
			if (Lvls)
			{
				auto LvlFiles = FS->GetFileByType(_TypeOfFile::LEVELS);
				for (auto it: LvlFiles)
				{
					if (filter.PassFilter(it.first->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it.first->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it.first->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Shdrs = ImGui::TreeNode("Shaders");
			if (Shdrs)
			{
				auto ShdrFiles = FS->GetFileByType(_TypeOfFile::SHADERS);
				for (auto it: ShdrFiles)
				{
					if (filter.PassFilter(it.first->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it.first->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it.first->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Scrpts = ImGui::TreeNode("Scripts");
			if (Scrpts)
			{
				auto ScrFiles = FS->GetFileByType(_TypeOfFile::SCRIPTS);
				for (auto it: ScrFiles)
				{
					if (filter.PassFilter(it.first->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it.first->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it.first->PathA.c_str());
				}

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

	if (CS)
	{
		if (ImGui::Begin("Logic 'n' Cut-Scene", &CS))
		{
			if (ImGui::TreeNode("Simple Logic"))
			{
				UI::HelpMarker("The following 'tree nodes' are ONLY for the change of current logic of model"
					"To change the logic you need to choice a model (e.g. just open it in the tree node)\n"\
					"And change everything is there");
				if (LevelObjs)
					for (auto It: LevelObjs->GetNodes())
					{
						if (It->SaveInfo->IsRemoved) continue;
						if (ImGui::TreeNode(It->RenderName.c_str()))
						{
							auto &This = It->GM->GetLogic();

							if (!This)
							{
								Vector4 CLErr = Colors::DarkMagenta.operator DirectX::XMVECTOR(),
									CLWarn = Colors::DarkKhaki.operator DirectX::XMVECTOR();
								ImGui::TextColored(ImVec4(CLErr.x, CLErr.y, CLErr.z, CLErr.w), "This node doesn't have a logic.");
								ImGui::TextColored(ImVec4(CLWarn.x, CLWarn.y, CLWarn.z, CLWarn.w),
									"Need to add logic to work with it here "); ImGui::SameLine();
								if (ImGui::Button("Add Logic"))
									Application->getLevel()->AddTo(It, make_shared<SimpleLogic>());
								ImGui::TreePop();
								continue;
							}

							ImGui::Columns(2);
							ImGui::Separator();
							ImGui::AlignTextToFramePadding();
							int CS_Current_StateIndx = 0;
							for (size_t i = 0; i < This->GetPoints().size(); i++)
							{
								auto ThisPoint = This->GetPoints().at(i);
								CS_Current_StateIndx = ThisPoint->GetState();
								if (This->getCurrentPoint() == i)
								{
									ImGui::Text("Current"); ImGui::SameLine();
								}
								if (ImGui::TreeNode(("Point: " + to_string(i)).c_str()))
								{
									ImGui::SetNextItemWidth(-1);

									ImGui::Separator();
									ImGui::SetNextItemWidth(-1);
									ImGui::TreeNodeEx(("SLogic_Pos##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
										ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");
									ImGui::NextColumn();
									ImGui::SetNextItemWidth(-1);
									pair<Vector3, bool> Ret;


									ThisPoint->SetPos(DragFloat3("##SLogic_Pos#" + It->ID,
										ThisPoint->GetPos()).first);
									ImGui::NextColumn();

									ImGui::Separator();
									ImGui::SetNextItemWidth(-1);
									ImGui::TreeNodeEx(("SLogic_Rot##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
										ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Rotation");
									ImGui::NextColumn();
									ImGui::SetNextItemWidth(-1);
									ThisPoint->SetRotate(DragFloat3("##SLogic_Rot" + It->ID,
										ThisPoint->GetRotate()).first);
									ImGui::NextColumn();
									ImGui::Separator();

									ImGui::SetNextItemWidth(-1);
									ImGui::TreeNodeEx(("SLogic_St##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
										ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "State");
									ImGui::NextColumn();
									ImGui::SetNextItemWidth(-1);

									if (Combobox::Combo(("##SLogic_St" + to_string(i)).c_str(), &CS_Current_StateIndx, values))
										ThisPoint->SetState((SimpleLogic::LogicMode)CS_Current_StateIndx);
									ImGui::NextColumn();
									ImGui::Separator();

									ImGui::SetNextItemWidth(-1);
									if (ImGui::Button("Delete This Point"))
									{
										if (i == It->GM->GetLogic()->GetPoints().size()-1)
										{
											It->GM->GetLogic()->GetPoints().pop_back();
											ImGui::TreePop();
											continue;
										}

										It->GM->GetLogic()->GetPoints().erase(It->GM->GetLogic()->GetPoints().begin() + i);
									}

									ImGui::TreePop();
								}
							}

							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Start Over"))
								This->Restart();
							ImGui::SameLine();
							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Pause"))
								This->Pause(true);

							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Add Point"))
								This->AddNewPoint(Vector3::Zero, Vector3::Zero, SimpleLogic::LogicMode::Follow);
							
							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Remove Logic"))
								Application->getLevel()->RemoveFrom(It);
							ImGui::SameLine();
							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Delete All Points"))
								This->GetPoints().clear();

							ImGui::Columns(1);
							ImGui::TreePop();
						}
					}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Cut-Scene"))
			{
				if (!Application->getCScene().operator bool())
					Application->setCScene(make_shared<CutScene>());

				UI::HelpMarker("The following 'tree node' is ONLY for the change of settings cut-scene camera\n");
				ImGui::Columns(2);
				ImGui::Separator();
				ImGui::AlignTextToFramePadding();
				for (size_t i = 0; i < CutScene::Points.size(); i++)
				{
					auto &Point = CutScene::Points.at(i);
					if (Application->getCScene()->getCurPos() == i)
					{
						ImGui::Text("Current");
						ImGui::SameLine();
					}
					if (ImGui::TreeNode(("Point: " + to_string(i)).c_str()))
					{
						ImGui::SetNextItemWidth(-1);

						ImGui::Separator();
						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Pos##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat3(("##Pos" + to_string(i)).c_str(), (float *)&Point.Pos, 0.1f);
						ImGui::NextColumn();

						ImGui::Separator();
						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Look##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Look");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat2(("##Look" + to_string(i)).c_str(), (float *)&Point.Look, 0.1f);
						ImGui::NextColumn();
						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Time##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Time");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat(("##Time" + to_string(i)).c_str(), &CutScene::Points.at(i).Time, 0.001f, 0.f);
						ImGui::NextColumn();
						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);
						if (ImGui::Button("Delete This Point"))
							CutScene::Points.erase(CutScene::Points.begin() + i);
						ImGui::SetNextItemWidth(-1);
						if (ImGui::Button("Set Cam Pos & Look"))
						{
							Point.Pos = Application->getCamera()->GetEyePt();
							Point.Look = Application->getCamera()->GetLookAtPt();
						}

						ImGui::TreePop();
					}
				}

				// In the end
				ImGui::SetNextItemWidth(-1);
				if (!CutScene::Points.empty() && ImGui::Button("Start Over"))
					Application->getCScene()->Restart();
				ImGui::SameLine();
				if (ImGui::Button("Pause"))
					Application->getCScene()->Pause();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::Button("Add Point"))
					Application->getCScene()->AddNewPoint(Vector3::Zero, Vector3::Zero, 0.1f);
				ImGui::SetNextItemWidth(-1);
				if (ImGui::Button("Delete All Points"))
					Application->getCScene()->Reset();

				ImGui::Columns(1);
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
	
	if (LagTest)
	{
		if (ImGui::Begin("Lag Testing", &LagTest))
		{
			auto Timer = Application->getMainThread();
			ImGui::Separator();

			ImGui::Text("Target Seconds Timer:");
			ImGui::SameLine();
			if (ImGui::Checkbox("##TargetSecondsTimer", &StepTimer))
				Timer->SetFixedTimeStep(StepTimer);
			if (StepTimer)
			{
				ImGui::DragFloat("##Trgt", &NewTarget, 0.001f);
				Timer->SetTargetElapsedSeconds(NewTarget);
			}

			ImGui::Separator();
			ImGui::TreeNodeEx("Current Seconds:", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetElapsedSeconds()).c_str());
			ImGui::Separator();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::TreeNodeEx("Current Ticks:", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetElapsedTicks()).c_str());
			ImGui::Separator();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::TreeNodeEx("Current Frames:", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetFrameCount()).c_str());
			ImGui::Separator();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::TreeNodeEx("Current FPS:", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetFramesPerSecond()).c_str());
			ImGui::Separator();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::TreeNodeEx("Current Different Time (End - Begin):", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetResultTime().count()).c_str());
			ImGui::Separator();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::TreeNodeEx("Current Total Seconds:", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetTotalSeconds()).c_str());
			ImGui::Separator();

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::TreeNodeEx("Current Total Ticks:", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			ImGui::SameLine();
			ImGui::Text(to_string(Timer->GetTotalTicks()).c_str());
			ImGui::Separator();

			if (ImGui::Button("Reset"))
			{
				StepTimer = false;
				Timer->SetFixedTimeStep(StepTimer);
				Timer->SetTargetElapsedSeconds(0.f);
				NewTarget = 0.0001f;
			}
		}
		ImGui::End();
	}

	if (audio)
	{
		if (ImGui::Begin("Audio", &audio))
		{
			auto snd = Application->getSound();
			ImGui::Separator();

			if (ImGui::Button("Open Sound File"))
			{
				auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(Application->getFS()->getPathFromType(
					_TypeOfFile::MODELS).c_str()),
					"Dialog Add Sound",
					"Sound File\0*.wav\0");

				if (Obj.first) // If Dialog Wasn't Skip
				{
					if (Obj.second.empty())
					{
						ImGui::EndPopup();
						ImGui::End();
						return;
					}
					for (auto It: Obj.second)
					{
						Application->getSound()->PlayFile(It, false, false);
					}
				}
			}
			if (ImGui::Button("Play"))
				snd->getAllSources().back().first->Play();

			if (ImGui::Button("Stop"))
				snd->getAllSources().back().first->Stop();

			if (ImGui::Button("Reset"))
			{
				Aud_Targt1 = 0.f;
				Aud_Targt2 = 1.f;
				Aud_Targt3 = 0.5f;
				snd->changePan(Aud_Targt1);
				snd->changePitch(Aud_Targt2);
				snd->changeVol(Aud_Targt3);
			}

			ImGui::TreeNodeEx("Change Panning", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			if (ImGui::DragFloat("##Pan", &Aud_Targt1, 0.001f, -1.f, 1.f))
				snd->changePan(Aud_Targt1);

			ImGui::TreeNodeEx("Change Pitch", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			if (ImGui::DragFloat("##Pth", &Aud_Targt2, 0.001f, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_DEFAULT_FREQ_RATIO))
				snd->changePitch(Aud_Targt2);

			ImGui::TreeNodeEx("Change Volume", ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			if (ImGui::DragFloat("##Vlm", &Aud_Targt3, 0.001f, 0.0f, 1.0f))
				snd->changeVol(Aud_Targt3);

			ImGui::NewLine();
			ImGui::Separator();
			for (size_t i = 0; i < snd->getAllSources().size(); i++)
			{
				bool Clicked = ImGui::Button(("[" + to_string(i) + ("] FileName:") + 
					snd->getAllSources().at(i).second).c_str());
				if (Clicked && snd->getAllSources().at(i).first->IsStop())
				{
					snd->getAllSources().at(i).first->Play();
					continue;
				}
				if (Clicked && snd->getAllSources().at(i).first->IsPlay())
				{
					snd->getAllSources().at(i).first->Stop();
					continue;
				}

				ImGui::Separator();

				//ImGui::TreeNodeEx(("[" + to_string(i) + ("] Current State:")).c_str(), ImGuiTreeNodeFlags_Leaf |
				//	ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				//ImGui::SameLine();
				//ImGui::Text(snd->getAllSources().at(i).first->IsPlay() ? "It's Playing" : "It's Stop");
				ImGui::NewLine();
			}
			ImGui::Separator();
		}
		ImGui::End();
	}

	ImGui::PopStyleVar();

	if (ImGui::BeginMainMenuBar())
	{
		//ImGui::MenuItem("(dummy menu)", NULL, false, false);
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Project"))
				// Use it if we don't save the current file yet or offer to save this file and can change name
			{
				WarningDial("Create a new project wihout save changes?",
					[&]
				{
					Application->getLevel()->Destroy();
					Application->getFS()->CreateProjectFile("New File");
				},
					[&]
				{
					Application->getLevel()->Destroy();
					Application->getFS()->CreateProjectFile("New File");
				});
			}

			// Needs To Shortcuts
			if (ImGui::MenuItem("Open Project", "Ctrl+O"))
			{
				WarningDial("Open a project wihout save changes?",
				[&]
				{
					Open();
				}, [&]
				{
					Open();
				});
			}
			if (ImGui::BeginMenu("Open Recent"))
			{
				if (ImGui::MenuItem("Super Secret File Here (CLICK ME!!!).proj"))
				{
					MessageBoxA(Engine::GetHWND(),
						"You clicked this item but nothing happen!!! xD", "Just Joke!", MB_OK);
					MessageBoxA(Engine::GetHWND(),
						"But just wait a second.... i need to think and decide...", "Just Joke!", MB_OK);
					MessageBoxA(Engine::GetHWND(),
						"Yeah, i decided and... i wanna exit this application to you. "\
						"I hope you're happy now, good bye! xD",
						"Bye!", MB_OK);

					WarningDial("Open a project wihout save changes?",
						[&]
					{
						Engine::Quit();
					},
						[&]
					{
						Engine::Quit();
					});
				}
				//if (Application->getFS()->GetProject().operator bool())
				//{
				//	auto Recent = Application->getFS()->GetProject()->GetRecent();
				//	for (size_t i = 0; i < Recent.size(); i++)
				//	{
				//		auto Obj = std::next(Recent.begin(), i);
				//		if (ImGui::MenuItem((ReplaceSymbols(Obj->second.filename().string())).c_str(),
				//			to_string(Obj->first).c_str()))
				//			Application->getFS()->GetProject()->OpenFile(Obj->second);
				//	}
				//}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save Project", "Ctrl+S"))
				Save();
			if (ImGui::MenuItem("Save Project As..", "Ctrl+Shift+S"))
				SaveAs();

			ImGui::Separator();
			if (ImGui::MenuItem("Quit Programm", "Alt+F4"))
			{
				WarningDial("Close Engine wihout save changes?",
				[&]
				{
					Engine::Quit();
				},
					[&]
				{
					Engine::Quit(); 
				});
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();
		if (ImGui::BeginMenu("Options"))
		{
			ImGui::Separator();
			if (ImGui::TreeNode("Window Settings"))
			{
				ImGui::Text("Always Open");
				if (ImGui::Checkbox("List Of Game Objects ", &LOGO))
					IfNeedSave = true;

				if (ImGui::Checkbox("Logic 'n' Cut - Scene ", &CS))
					IfNeedSave = true;

				if (ImGui::Checkbox("Hierarchy of Level ", &HoL))
					IfNeedSave = true;
				if (ImGui::Checkbox("File Resources ", &FR))
					IfNeedSave = true;
				if (ImGui::Checkbox("Lag Testing ", &LagTest))
					IfNeedSave = true;
				if (ImGui::Checkbox("Audio ", &audio))
					IfNeedSave = true;
				if (ImGui::Checkbox("Multiplayer ", &MP))
					IfNeedSave = true;
				if (ImGui::Checkbox("Changes ", &_Changes))
					IfNeedSave = true;
				if (ImGui::Checkbox("Information ", &_Information))
					IfNeedSave = true;
				ImGui::TreePop();
			}
			ImGui::Separator();
			if (ImGui::TreeNode("Camera Settings"))
			{
				ImGui::Separator();
				ImGui::TreeNodeEx("Position: ", ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				ImGui::SameLine();
				Pos = Application->getCamera()->GetEyePt(), Look = Application->getCamera()->GetLookAtPt();
				bool BPos = ImGui::DragFloat3("##Position", (float *)&Pos);

				ImGui::Text("Look: ");
				ImGui::SameLine();
				bool BLook = ImGui::DragFloat2("##Look", (float *)&Look);
				if (BPos || BLook)
				{
					IfNeedSave = true;
					Application->getCamera()->Teleport(Pos, Look);
				}

				if (Application->getPhysics().operator bool())
				{
					ImGui::Separator();
					ImGui::Text("FPC Camera: ");
					ImGui::SameLine();
					if (ImGui::Checkbox("##IsFreeCam", &IsFreeCam))
					{
						IfNeedSave = true;
						Application->getCamera()->SetFreeMoveCam(!IsFreeCam);
					}
					ImGui::Separator();
				}

				ImGui::TreeNodeEx("Move Senses: ", ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				MovSense = Application->getCamera()->getMoveScale(), RotSense = Application->getCamera()->getRotateScale();
				ImGui::Text("Move Scaler: ");
				ImGui::SameLine();
				bool MSense = ImGui::DragFloat("##MSense", &MovSense);

				ImGui::Text("Rotation Scaler: ");
				ImGui::SameLine();
				bool RSense = ImGui::DragFloat("##RSense", &RotSense, 0.001f, 0.f, 0.1f);
				if (MSense || RSense)
				{
					IfNeedSave = true;
					Application->getCamera()->SetScalers(RotSense, MovSense);
				}

				ImGui::Separator();
				
				int Indx = Application->getCamera()->getIsLeftCtrlBtn() ? 0 :
					(Application->getCamera()->getIsRightCtrlBtn() ? 1 :
					(Application->getCamera()->getIsWithoutButtonCtrl() ? 2 : 1));
				Vector4 DOrange(Colors::DarkOrange.operator DirectX::XMVECTOR());
				UI::HelpMarker("Default is Right", ImVec4(DOrange.x, DOrange.y, DOrange.z, DOrange.w));
				ImGui::Text("Camera Button Control: ");
				ImGui::SameLine();
				if (Combobox::Combo("##CameraButtonControl", (int *)&Indx, LeftRightWithout))
				{
					IfNeedSave = true;
					if (LeftRightWithout.at(Indx) == "Left")
						CamBtnLeft = true;
					else
						CamBtnLeft = false;

					if (LeftRightWithout.at(Indx) == "Right")
						CamBtnRight = true;
					else
						CamBtnRight = false;

					if (LeftRightWithout.at(Indx) == "Without")
						CamBtnWihoutBtn = true;
					else
						CamBtnWihoutBtn = false;

					Application->getCamera()->SetCameraControlButtons(CamBtnLeft, CamBtnRight, CamBtnWihoutBtn);
				}
				ImGui::Separator();

				ImGui::TreePop();
			}
			ImGui::Separator();
			if (ImGui::TreeNode("Application Settings"))
			{
				Vector4 Red(Colors::Red.operator DirectX::XMVECTOR()),
					DOrange(Colors::DarkOrange.operator DirectX::XMVECTOR());
				UI::HelpMarker("It may cause some bugs!!!", ImVec4(Red.x, Red.y, Red.z, Red.w));
				ImGui::Text("Lock 60 FPS: ");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Lock_60_FPS", &LockFPS))
					IfNeedSave = true;

				UI::HelpMarker("Default is 1000", ImVec4(DOrange.x, DOrange.y, DOrange.z, DOrange.w));
				ImGui::Text("Distance Far Plane Renderer: ");
				ImGui::SameLine();
				if (ImGui::DragFloat("##DistFarRenderer", &DistFarRender))
					IfNeedSave = true;

				UI::HelpMarker("Default is 0.1", ImVec4(DOrange.x, DOrange.y, DOrange.z, DOrange.w));
				ImGui::Text("Distance Near Plane Renderer: ");
				ImGui::SameLine();
				if (ImGui::DragFloat("##DistNearRenderer", &DistNearRender))
					IfNeedSave = true;

				ImGui::TreePop();
			}

			ImGui::Separator();
			if (IfNeedSave)
			{
				getPos.clear();
				getLook.clear();
				getTextFloat3(getPos, ",", vector<float>{Pos.x, Pos.y, Pos.z});
				getTextFloat3(getLook, ",", vector<float>{Look.x, Look.y, Look.z});
				SaveSettings();
				IfNeedSave = false;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, !Undos.empty()))
				undo();
			if (ImGui::MenuItem("Redo", "CTRL+SHIFT+Z", false, !Undos.empty()))
				redo();
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ShowPopup.operator bool())
		ShowPopup();

	if (ImGui::GetIO().WantCaptureMouse)
		Application->getCamera()->DisableCameraControl(true);
	else
		Application->getCamera()->DisableCameraControl(false);

	Changes();
	Information();
}

void SDKInterface::LoadSettings(boost::property_tree::ptree fData)
{
	if (fData.empty()) return;

	// Engine
	//Application->SetLogInf(fData.get<int>("engine.is infolog", 1));
	//Application->SetLogNrm(fData.get<int>("engine.is normallog", 0));
	Application->SetLogErr(fData.get<int>("engine.is errorlog", 0));

	// UI Dialogs
	LOGO = fData.get<int>("application.open_logo", 0);
	CS = fData.get<int>("application.open_cs", 0);
	HoL = fData.get<int>("application.open_hol", 0);
	FR = fData.get<int>("application.open_fr", 0);
	LagTest = fData.get<int>("application.open_lg", 0);
	audio = fData.get<int>("application.open_aud", 0);
	MP = fData.get<int>("application.open_mp", 0);
	_Changes = fData.get<int>("application.open_cngs", 0);
	_Information = fData.get<int>("application.open_info", 1);

	// Application, Renderer And Etc...
	LockFPS = fData.get<int>("application.lockfps", 1);
	DistFarRender = fData.get<float>("application.distfarrenderer", 1000);
	DistNearRender = fData.get<float>("application.distnearrenderer", 0.1f);

	// Camera
	IsFreeCam = fData.get<int>("application.fpccamera", 0);
	getPos = fData.get<string>("application.pos", "0,0,0");
	getLook = fData.get<string>("application.look", "0,0,0");
	MovSense = fData.get<float>("application.movesense", 6.0f);
	RotSense = fData.get<float>("application.rotsense", 0.001f);
	CamBtnLeft = fData.get<int>("application.cambuttonbyleft", 0);
	CamBtnRight = fData.get<int>("application.cambuttonbyright", 1);
	CamBtnWihoutBtn = fData.get<int>("application.cambuttonbynone", 0);

	if (Application->getCamera())
	{
		// Override default values
		Application->getCamera()->SetScalers(RotSense, MovSense);

		if (Application->getPhysics())
			Application->getCamera()->SetFreeMoveCam(!IsFreeCam);
		if (!CamBtnLeft && !CamBtnRight && !CamBtnWihoutBtn)
			CamBtnRight = true;
		Application->getCamera()->SetCameraControlButtons(CamBtnLeft, CamBtnRight, CamBtnWihoutBtn);
	}
}
void SDKInterface::SaveSettings()
{
	vector<pair<string, string>> Settings =
	{
		make_pair("engine.is infolog", "false"),
		make_pair("engine.is errorlog", "false"),
		make_pair("engine.is normallog", "false"),

		make_pair("application.open_logo", to_string(LOGO)),
		make_pair("application.open_cs", to_string(CS)),
		make_pair("application.open_hol", to_string(HoL)),
		make_pair("application.open_fr", to_string(FR)),
		make_pair("application.open_lg", to_string(LagTest)),
		make_pair("application.open_aud", to_string(audio)),
		make_pair("application.open_mp", to_string(MP)),
		make_pair("application.open_cngs", to_string(_Changes)),
		make_pair("application.open_info", to_string(_Information)),

		make_pair("application.movesense", to_string(MovSense)),
		make_pair("application.rotsense", to_string(RotSense)),
		make_pair("application.fpccamera", to_string(IsFreeCam)),
		
		make_pair("application.cambuttonbyleft", to_string(CamBtnLeft)),
		make_pair("application.cambuttonbyright", to_string(CamBtnRight)),
		make_pair("application.cambuttonbynone", to_string(CamBtnWihoutBtn)),

		make_pair("application.lockfps", to_string(LockFPS)),
		make_pair("application.distfarrenderer", to_string(DistFarRender)),
		make_pair("application.distnearrenderer", to_string(DistNearRender)),
		
		make_pair("application.pos", getPos),
		make_pair("application.look", getLook),
	};

	Application->getFS()->SaveSettings(Settings);
}

void SDKInterface::Save()
{
	Application->getFS()->GetProject()->SaveProj();
	Application->getLevel()->SetNotSaved(false);
}

void SDKInterface::SaveAs()
{
	auto Obj = UI::GetWndDlgSave(const_cast<LPSTR>(
		Application->getFS()->getPathFromType(_TypeOfFile::LEVELS).c_str()));
	if (Obj.first) // If Dialog Wasn't Skip
	{
		if (Obj.second.empty())
			ImGui::EndMenu();

		Application->getFS()->GetProject()->SaveFile(path(Obj.second.back()));
		Application->getLevel()->SetNotSaved(false);
	}
}

void SDKInterface::Open()
{
	auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(
		Application->getFS()->getPathFromType(_TypeOfFile::LEVELS).c_str()));
	if (Obj.first) // If Dialog Wasn't Skip
	{
		if (Obj.second.empty())
		{
			ImGui::EndMenu();
			ImGui::End();
			return;
		}

		Application->getLevel()->Destroy();
		Application->getFS()->GetProject()->OpenFile(Obj.second.back());
		Application->getFS()->GetProject()->SetCurProject(path(Obj.second.back()));
		Application->getLevel()->SetNotSaved(false);
	}
}

int SDKInterface::Und::Pos = 0;
void SDKInterface::undo()
{
	if (Undos.empty()) return;

	Und *It = nullptr;
	It = &Undos.at(Und::Pos);

	if (It->Redo) // Only Undo is Here!!!
	{
		Und::Pos--;
		if (Und::Pos < 0)
		{
			Application->getLevel()->SetNotSaved(false);
			Und::Pos = 0;
		}
		return;
	}

	string ID = It->Desc;
	deleteWord(ID, ": ", ModeProcessString::UntilTheBegin);
	auto Obj = Application->getLevel()->getChild()->getNodeByID(ID);
	if (!Obj || Obj->ID.empty())
		return;

	vector<float> ret;
	getFloat3Text(It->After_val, ", ", ret);
	float X = 0.f, Y = 0.f, Z = 0.f;
	bool B;
	if (ret.size() == 3)
	{
		X = ret.front();
		Y = ret.at(1);
		Z = ret.back();
	}
	else if (ret.size() == 1) // It's bool
		B = ret.back();

	string Low = It->Desc;
	to_lower(Low);

	if (It->Undo) // Only Undo is Here!!!
	{
		if (contains(Low, "removed"))
		{
			Obj->SaveInfo->IsRemoved = B;
			replaceAll(Low, "removed", "unremoved", true);
		}

		if (contains(Low, "pos"))
			Obj->GM->SetPositionCoords(Vector3(X, Y, Z));
		else if (contains(Low, "rotation"))
			Obj->GM->SetRotationCoords(Vector3(X, Y, Z));
		else if (contains(Low, "scale"))
			Obj->GM->SetScaleCoords(Vector3(X, Y, Z));
		else if (contains(Low, "node name"))
			Obj->RenderName = It->After_val;

		replaceAll(Low, "changed", "unchanged", true);
		It->Desc = Low;

		It->Redo = true;
		It->Undo = false;
		Application->getLevel()->SetNotSaved(true);

		Und::Pos++;
		if (Und::Pos > (int)Undos.size() - 1)
			Und::Pos = Undos.size() - 1;
	}
}
void SDKInterface::redo()
{
	if (Undos.empty()) return;

	Und *It = nullptr;
	It = &Undos.at(Und::Pos);
	if (It->Undo)
	{
		Und::Pos++;
		if (Und::Pos > (int)Undos.size() - 1)
		{
			Und::Pos = Undos.size() - 1;
			Application->getLevel()->SetNotSaved(true);
		}
		return;
	}

	string ID = It->Desc;
	deleteWord(ID, ": ", ModeProcessString::UntilTheBegin);
	auto Obj = Application->getLevel()->getChild()->getNodeByID(ID);
	if (!Obj || Obj->ID.empty())
		return;

	vector<float> ret;
	getFloat3Text(It->Before_val, ", ", ret);
	float X = 0.f, Y = 0.f, Z = 0.f;
	bool B;
	if (ret.size() == 3)
	{
		X = ret.front();
		Y = ret.at(1);
		Z = ret.back();
	}
	else if (ret.size() == 1) // It's bool
		B = ret.back();

	string Low = It->Desc;
	to_lower(Low);

	//OutputDebugStringA(("\nPos: " + to_string(Und::Pos) + "\n").c_str());
	if (It->Redo) // Only Redo is Here!!!
	{
		if (contains(Low, "removed"))
		{
			Obj->SaveInfo->IsRemoved = B;
			replaceAll(Low, "unremoved", "removed", true);
		}

		if (contains(Low, "pos"))
			Obj->GM->SetPositionCoords(Vector3(X, Y, Z));
		else if (contains(Low, "rotation"))
			Obj->GM->SetRotationCoords(Vector3(X, Y, Z));
		else if (contains(Low, "scale"))
			Obj->GM->SetScaleCoords(Vector3(X, Y, Z));
		else if (contains(Low, "node name"))
			Obj->RenderName = It->Before_val;

		replaceAll(Low, "unchanged", "changed", true);
		It->Desc = Low;

		It->Redo = false;
		It->Undo = true;
		Application->getLevel()->SetNotSaved(true);
		Und::Pos--;
		if (Und::Pos < 0)
			Und::Pos = 0;
	}
}

void SDKInterface::AddToUndo(string Desc, string BeforeVal, string AfterVal)
{
	Und undo;
	undo.AddDesc(Desc)->AddFst(BeforeVal)->AddSec(AfterVal)->Undo = true;
	Undos.push_back(undo);
	Und::Pos = Undos.size()-1;
	Application->getLevel()->SetNotSaved(true);
}
void SDKInterface::AddToUndo(string Desc, Vector3 BeforeVal, Vector3 AfterVal)
{
	Und undo;
	string ResB, ResA;
	getTextFloat3(ResB, ", ", vector<float>{BeforeVal.x, BeforeVal.y, BeforeVal.z});
	getTextFloat3(ResA, ", ", vector<float>{AfterVal.x, AfterVal.y, AfterVal.z});

	undo.AddDesc(Desc)->AddFst(ResB)->AddSec(ResA)->Undo = true;
	Undos.push_back(undo);
	Und::Pos = Undos.size()-1;
	Application->getLevel()->SetNotSaved(true);
}

void SDKInterface::Changes()
{
	if(_Changes)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_::ImGuiCond_Once);
		if (ImGui::Begin("Changes", &_Changes))
		{
			if (ImGui::BeginChild("child", ImVec2(0, 0), true))
			{
				for (size_t i = 0; i < Undos.size(); i++)
				{
					if (Undos.at(i).Undo)
						ImGui::Separator();
					ImGui::Text("Desc: %s, Was: %s, Become: %s", Undos.at(i).Desc.c_str(),
						Undos.at(i).Undo ? Undos.at(i).After_val.c_str() : Undos.at(i).Before_val.c_str(),
						Undos.at(i).Undo ? Undos.at(i).Before_val.c_str() : Undos.at(i).After_val.c_str());
				}
				ImGui::EndChild();
			}
		}
		ImGui::End();
	}
}
void SDKInterface::Information()
{
	if (_Information)
	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
			| ImGuiWindowFlags_NoNav;
		if (ImGui::Begin("Info", &_Information, window_flags))
		{
			auto mainActor = Application->getActor();
			if (!mainActor) return;

			float CamPos[] = { mainActor->getPosition().x, mainActor->getPosition().y, mainActor->getPosition().z };
			ImGui::Text((boost::format(string("FPS: (%.2f FPS)\nCamera pos: X(%.2f), Y(%.2f), Z(%.2f)"))
				% Application->getFPS() % CamPos[0] % CamPos[1] % CamPos[2]).str().c_str());

			Vector4 Enb(Colors::SkyBlue.operator DirectX::XMVECTOR());

			ImGui::Separator();
			if (!Application->IsWireFrame())
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetCurrentContext()->Style.Colors[ImGuiCol_TextDisabled]);
				ImGui::Text((boost::format(string("Is WireFrame? : %b")) % Application->IsWireFrame()).str().c_str());
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(Enb.x, Enb.y, Enb.z, Enb.w));
				ImGui::Text((boost::format(string("Is WireFrame? : %b")) % Application->IsWireFrame()).str().c_str());
				ImGui::PopStyleColor();
			}

			ImGui::Separator();
			if (Application->IsSimulatePhysics())
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetCurrentContext()->Style.Colors[ImGuiCol_TextDisabled]);
				ImGui::Text((boost::format(string("Is Simulation PhysX : %b"))
					% !Application->IsSimulatePhysics()).str().c_str());
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(Enb.x, Enb.y, Enb.z, Enb.w));
				ImGui::Text((boost::format(string("Is Simulation PhysX : %b"))
					% !Application->IsSimulatePhysics()).str().c_str());
				ImGui::PopStyleColor();
			}
			ImGui::Separator();
			auto Res = Application->getWorkAreaSize(Engine::GetHWND());
			ImGui::Text((boost::format(string("Resolution Window : W: %d, H: %d")) % Res.x % Res.y).str().c_str());
			ImGui::Separator();

			ImGui::Text("FrameTime : %f", Application->getframeTime());
			ImGui::Separator();

			ImGui::Separator();
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse Position: <%.1f,%.1f>", io.MousePos.x, io.MousePos.y);
			else
				ImGui::Text("Mouse Position: <invalid>");
		}

		ImGui::End();
	}
}