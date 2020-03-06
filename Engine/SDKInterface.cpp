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

pair<Vector3, bool> DragFloat3(string ID, Vector3 Thing)
{
	pair<Vector3, bool> Ret_Thing = make_pair(Thing, false);
	Ret_Thing.second = ImGui::DragFloat3(ID.c_str(), (float *)&Ret_Thing, 0.001f);
	return Ret_Thing;
}

static vector<string> values = { "Stay", "WalkToNPoint", "Follow" };
static vector<string> TrueFalse = { "True", "False" };
int Current = 0, Curr = 0, Cur = 1, Open6Cur = 0;
float NewTarget = 0.f, Aud_Targt1 = 0.f, Aud_Targt2 = 1.f, Aud_Targt3 = 1.f;

#include "Models.h"
void SDKInterface::Render()
{
	auto LevelObjs = Application->getLevel()->getChild();
	auto Cam = Application->getCamera();
	auto Obj = LevelObjs->GetNodes();

	ImGuiID dockspaceID = 0;
	ImGui::SetNextWindowPos(ImVec2(Application->getWorkAreaSize(Application->GetHWND()).x -
		ImGui::GetWindowWidth() - 100.f,
		0.f), ImGuiCond_::ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	if (Open)
	{
		if (ImGui::Begin("List Of Game Objects", &Open))
		{
			ImGui::Columns(2);
			ImGui::Separator();

			ImGui::AlignTextToFramePadding();

			bool Othrs = ImGui::TreeNode("Others Object");
			if (Othrs)
			{
				for (auto Object: LevelObjs->GetNodes())
				{
					bool id = ImGui::TreeNode(Object->ID.c_str());
					ImGui::SameLine();
					if (ImGui::Button("X"))
					{
						Application->getLevel()->Remove(Object->ID);
						Application->getSound()->Remove(Object->ID);
						ImGui::TreePop();

						continue; // Don't work with this object anymore
					}
					if (id)
					{
						ImGui::SetNextItemWidth(-1);
						if (ImGui::Checkbox("Is Render", &Object->GM->RenderIt))
						{
							Object->IsItChanged = true;
							Object->SaveInfo->IsVisible = Object->GM->RenderIt;
						}
						ImGui::Separator();
						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Pos#" + Object->ID).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");

						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						pair<Vector3, bool> Ret;
						Object->GM->SetPositionCoords((Ret = DragFloat3(("##Pos#" + Object->ID).c_str(),
							Object->GM->GetPositionCord())).first);
						ImGui::NextColumn();

						if (Ret.second)
						{
							Object->IsItChanged = true;
							Object->SaveInfo->Pos = true;
						}
						//OutputDebugStringA((boost::format("\nPosition: X: %.3f, Y: %.3f, Z: %.3f\n")
						//  % Object->GetPositionCord().x
						//	% Object->GetPositionCord().y
						//	% Object->GetPositionCord().z).str().c_str());

						ImGui::Separator();
						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Rot#" + Object->ID).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Rotation");

						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						Object->GM->SetRotationCoords(
							(Ret = DragFloat3(("##Rot#" + Object->ID).c_str(), Object->GM->GetRotCord())).first);
						ImGui::NextColumn();
						ImGui::Separator();
						if (Ret.second)
						{
							Object->IsItChanged = true;
							Object->SaveInfo->Rot = true;
						}

						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Scl#" + Object->ID).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Scale");

						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						Object->GM->SetScaleCoords(
							(Ret = DragFloat3(("##Scl#" + Object->ID).c_str(), Object->GM->GetScaleCord())).first);
						ImGui::NextColumn();
						ImGui::Separator();
						if (Ret.second)
						{
							Object->IsItChanged = true;
							Object->SaveInfo->Scale = true;
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

						ImGui::TreePop();
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

	for (size_t i = 0; i < Obj.size(); i++)
	{
		auto ObjObj = Obj.at(i);
		if (ObjObj->GM->GetType() != GameObjects::TYPE::Sound_Obj) continue;
		auto ObjSnd = Application->getSound()->GetSound(ObjObj->ID);
		if (ObjSnd.operator bool())
		{
			if (Application->getTrackerKeyboard().pressed.F7)
				ObjSnd->Play();
			ObjSnd->setSoundPosition(Obj.at(i)->GM->GetPositionCord());
			Application->getSound()->Update(Cam->GetEyePt(), Cam->GetWorldAhead(), Cam->GetWorldUp());
		}
	}

	ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	if (Open2)
	{
		if (ImGui::Begin("Hierarchy of Level", &Open2))
		{
			if (ImGui::Button("Add One"))
				ImGui::OpenPopup("##my_toggle_popup");
			if (ImGui::BeginPopup("##my_toggle_popup"))
			{
				ImGui::Separator();
				ImGui::AlignTextToFramePadding();
				if (ImGui::MenuItem("Test Phys Cobe"))
					Application->getPhysics()->SpawnObject();

				ImGui::Separator();
				if (ImGui::MenuItem("Model"))
				{
					auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(Application->getFS()->getPathFromType(
						_TypeOfFile::MODELS).c_str()), "", "All\0*.*\0", true);
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
							Application->getLevel()->Add(_TypeOfFile::MODELS, It);
						}
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Sound Source")) // It Means That Add To Scene Sphere 3d Source of Sound
				{
					auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(
						Application->getFS()->getPathFromType(_TypeOfFile::SOUNDS).c_str()), "", "All\0*.*\0");
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
							Application->getSound()->AddNewFile(It, true);
							auto OurNode = Application->getLevel()->Add(_TypeOfFile::MODELS,
								Application->getFS()->getPathFromType(_TypeOfFile::MODELS) + "cube_with_diffuse_texture.3ds");
							OurNode->ID = path(It).filename().string();
							OurNode->GM->SetScaleCoords(Vector3(0.01f, 0.01f, 0.01f));
							OurNode->GM->SetPositionCoords(Cam->GetEyePt());
							OurNode->GM->SetType(GameObjects::TYPE::Sound_Obj);
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

	if (Open3)
	{
		ToDo("Drag and drop here?");
		if (ImGui::Begin("File Resources", &Open3))
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
				for (auto it : MldFiles)
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
				for (auto it : TxtrFiles)
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
				for (auto it : LvlFiles)
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
				for (auto it : ShdrFiles)
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
				for (auto it : ScrFiles)
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

	/*	if (Open4)
		{
			if (ImGui::Begin("Logic 'n' Cut-Scene", &Open4))
			{
				bool SLogic = ImGui::TreeNode("Simple Logic");
				if (SLogic)
				{
					UI::HelpMarker("The following 'tree node' is ONLY for the change of current logic of model"
						"To change logic needs to choice model (e.g. just choice it in the combobox)\nAnd change it's there");
					Combobox::Combo("Models", &Current, Application->getLevel()->IDModels);
					auto This = Application->getLevel()->Obj_other.at(Current)->GetLogic();

					ImGui::Columns(2);
					ImGui::Separator();
					ImGui::AlignTextToFramePadding();
					for (size_t i = 0; i < This->GetPoints().size(); i++)
					{
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
							This->GetPoints().at(i)->SetPos(DragFloat3(("##SLogic_Pos" + to_string(i)),
							This->GetPoints().at(i)->GetPos()));
							ImGui::NextColumn();

							ImGui::Separator();
							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("SLogic_Rot##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Rotation");
							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);
							This->GetPoints().at(i)->SetRotate(DragFloat3(("##SLogic_Rot" + to_string(i)),
								This->GetPoints().at(i)->GetRotate()));
							ImGui::NextColumn();
							ImGui::Separator();

							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("SLogic_St##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "State");
							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);
							if (Combobox::Combo(("##SLogic_St" + to_string(i)).c_str(), &Curr, values))
								This->GetPoints().at(i)->SetState((SimpleLogic::LogicMode)Curr);
							ImGui::NextColumn();
							ImGui::Separator();

							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Delete This Point"))
								This->GetPoints().erase(This->GetPoints().begin() + i);

							ImGui::TreePop();
						}
					}

					ImGui::SetNextItemWidth(-1);
					if (ImGui::Button("Start Over"))
						This->Restart(); ImGui::SameLine();
					ImGui::SetNextItemWidth(-1);
					if (ImGui::Button("Add Point"))
						This->AddNewPoint(Vector3::Zero, Vector3::Zero, SimpleLogic::LogicMode::Follow);
					ImGui::SetNextItemWidth(-1);
					if (ImGui::Button("Delete All Points"))
						This->GetPoints().clear();

					ImGui::Columns(1);
					ImGui::TreePop();
				}
				bool CScene = ImGui::TreeNode("Cut-Scene");
				if (CScene)
				{
					if (!Application->getCScene().operator bool())
						Application->setCScene(make_shared<CutScene>());

					UI::HelpMarker("The following 'tree node' is ONLY for the change of settings cut-scene camera\n");
					ImGui::Columns(2);
					ImGui::Separator();
					ImGui::AlignTextToFramePadding();
					for (size_t i = 0; i < CutScene::Points.size(); i++)
					{
						if (Application->getCScene()->getCurPos() == i)
						{
							ImGui::Text("Current"); ImGui::SameLine();
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
							ImGui::DragFloat3(("##Pos" + to_string(i)).c_str(), (float *)&CutScene::Points.at(i).Pos, 0.1f);
							ImGui::NextColumn();

							ImGui::Separator();
							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("Look##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Look");
							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);
							ImGui::DragFloat2(("##Look" + to_string(i)).c_str(), (float *)&CutScene::Points.at(i).Look, 0.1f);
							ImGui::NextColumn();
							ImGui::Separator();

							ImGui::SetNextItemWidth(-1);
							ImGui::TreeNodeEx(("Time##" + to_string(i)).c_str(), ImGuiTreeNodeFlags_Leaf |
								ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Time");
							ImGui::NextColumn();
							ImGui::SetNextItemWidth(-1);
							ImGui::DragFloat(("##Time" + to_string(i)).c_str(), &CutScene::Points.at(i).Time);
							ImGui::NextColumn();
							ImGui::Separator();

							ImGui::SetNextItemWidth(-1);
							if (ImGui::Button("Delete This Point"))
								CutScene::Points.erase(CutScene::Points.begin() + i);

							ImGui::TreePop();
						}
					}

					// In the end
					ImGui::SetNextItemWidth(-1);
					if (ImGui::Button("Start Over"))
						Application->getCScene()->Restart(); ImGui::SameLine();
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
		}*/

	if (Open5)
	{
		if (ImGui::Begin("Everything", &Open5))
		{
			ImGui::SetNextItemOpen(true);
			bool Cam = ImGui::TreeNode("Camera");
			if (Cam)
			{
				auto Cam = Application->getCamera();
				ImGui::Separator();
				ImGui::TreeNodeEx("Position:", ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				ImGui::SameLine();
				Vector3 Pos = Cam->GetEyePt(), Look = Cam->GetLookAtPt();
				bool BPos = ImGui::DragFloat3("##Position", (float *)&Pos);

				ImGui::Text("Look: ");
				ImGui::SameLine();
				bool BLook = ImGui::DragFloat2("##Look", (float *)&Look);
				if (BPos || BLook)
					Cam->Teleport(Pos, Look);

				ImGui::Separator();
				Cur = Cam->GetIsFreeCam();
				if (Combobox::Combo("Is Free Cam?", &Cur, TrueFalse))
					Cam->SetFreeMoveCam((bool)Cur);
				ImGui::Separator();

				ImGui::TreeNodeEx("Move Senses:", ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				float MovSense = Cam->getMoveScale(), RotSense = Cam->getRotateScale();
				ImGui::Text("Move Scaler: ");
				ImGui::SameLine();
				bool MSense = ImGui::DragFloat("##MSense", &MovSense);

				ImGui::Text("Rotation Scaler: ");
				ImGui::SameLine();
				bool RSense = ImGui::DragFloat("##RSense", &RotSense);
				if (MSense || RSense)
					Cam->SetScalers(RotSense, MovSense);

				ImGui::Separator();
				ImGui::TreePop();
			}

			//bool Gm = ImGui::TreeNode("Game");
			//if (Gm)
			//{
			//	ImGui::TreeNodeEx("File", ImGuiTreeNodeFlags_Leaf |
			//		ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			//	ImGui::TreePop();
			//}
		}
		ImGui::End();
	}
	
	if (Open6)
	{
		if (ImGui::Begin("Lag Testing", &Open6))
		{
			auto Timer = Application->getMainThread();
			ImGui::Separator();
			Open6Cur = !Timer->GetIsFixedTimeStep();

			if (Combobox::Combo("Tartet Seconds Timer:", &Open6Cur, TrueFalse))
				Timer->SetFixedTimeStep(!(bool)Open6Cur);
			if (!Open6Cur)
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
				Open6Cur = false;
				Timer->SetFixedTimeStep(Open6Cur);
				Timer->SetTargetElapsedSeconds(0.f);
				NewTarget = 0.0001f;
			}

			//ImGui::TreeNodeEx("Current Seconds:", ImGuiTreeNodeFlags_Leaf |
			//	ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);

			//bool BLook = ImGui::DragFloat2("##Look", (float *)&Look);
			//if (BPos || BLook)
			//	Application->getCamera()->Teleport(Pos, Look);

			//ImGui::Separator();
			//if (Combobox::Combo("Is Free Cam?", &Cur, TrueFalse))
			//	Application->getCamera()->SetFreeMoveCam((bool)Cur);
			//ImGui::Separator();

			//float MovSense = Application->getCamera()->getMoveScale(), RotSense =
			// Application->getCamera()->getRotateScale();
			//ImGui::Text("Move Scaler: ");
			//ImGui::SameLine();
			//bool MSense = ImGui::DragFloat("##MSense", &MovSense);

			//ImGui::Text("Rotation Scaler: ");
			//ImGui::SameLine();
			//bool RSense = ImGui::DragFloat("##RSense", &RotSense);
			//if (MSense || RSense)
			//	Application->getCamera()->SetScalers(RotSense, MovSense);

			//ImGui::Separator();

			//bool Gm = ImGui::TreeNode("Game");
			//if (Gm)
			//{
			//	ImGui::TreeNodeEx("File", ImGuiTreeNodeFlags_Leaf |
			//		ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
			//	ImGui::TreePop();
			//}
		}
		ImGui::End();
	}

	if (Open7)
	{
		if (ImGui::Begin("Audio", &Open7))
		{
			auto snd = Application->getSound();
			ImGui::Separator();

			if (ImGui::Button("Open Sound File"))
			{
				auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(Application->getFS()->getPathFromType(
					_TypeOfFile::MODELS).c_str()), "", "All\0*.*\0");

				if (Obj.first) // If Dialog Wasn't Skip
				{
					if (Obj.second.empty())
					{
						ImGui::EndPopup();
						ImGui::End();
						return;
					}

					Application->getSound()->PlayFile(Obj.second.back(), false, false);
				}
			}
			if (ImGui::Button("Play"))
				snd->getAllSources().back().first->Play();

			if (ImGui::Button("Stop"))
				snd->getAllSources().back().first->Stop();

			if (ImGui::Button("Reset"))
			{
				Open6Cur = false;
				Aud_Targt1 = 0.f;
				Aud_Targt2 = 1.f;
				Aud_Targt3 = 1.f;
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
			{
					// Use it if we don't save the current file yet or offer to save this file and can change name
				Application->getFS()->CreateProjectFile("New File");
			}

			ToDo("Add Dialogs For Open and Save actions");
			// Needs To Shortcuts
			//if (Application->getTrackerKeyboard().pressed.LeftControl && Application->getTrackerKeyboard().pressed.O)
			//	ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
			if (ImGui::MenuItem("Open Project", "Ctrl+O"))
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

					Application->getFS()->GetProject()->SetCurProject(path(Obj.second.back()));
					Application->getFS()->GetProject()->OpenFile(Obj.second.back());
				}
			}
			if (ImGui::BeginMenu("Open Recent"))
			{
				if (ImGui::MenuItem("Super Secret File Here (CLICK ME!!!).proj"))
				{
					MessageBoxA(Engine::GetHWND(), "You clicked this item but nothing happen!!! xD", "Just Joke!", MB_OK);
					MessageBoxA(Engine::GetHWND(), "But just wait a second.... i need to think and decide...", "Just Joke!", MB_OK);
					MessageBoxA(Engine::GetHWND(),
						"Yeah, i decided and... i wanna exit this application to you. I hope you're happy now, good bye! xD",
						"Bye!", MB_OK);

					Engine::Quit();
				}
				if (Application->getFS()->GetProject().operator bool())
				{
					auto Recent = Application->getFS()->GetProject()->GetRecent();
					for (size_t i = 0; i < Recent.size(); i++)
					{
						auto Obj = std::next(Recent.begin(), i);
						if (ImGui::MenuItem((ReplaceSymbols(Obj->second.filename().string())).c_str(),
							to_string(Obj->first).c_str()))
							Application->getFS()->GetProject()->OpenFile(Obj->second);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save Project", "Ctrl+S"))
				Application->getFS()->GetProject()->SaveCurrProj();
			if (ImGui::MenuItem("Save Project As..", "Ctrl+Shift+S"))
			{
				auto Obj = UI::GetWndDlgSave(const_cast<LPSTR>(Application->getFS()->getPathFromType(_TypeOfFile::LEVELS).c_str()));
				if (Obj.first) // If Dialog Wasn't Skip
				{
					if (Obj.second.empty())
						ImGui::EndMenu();
				
					Application->getFS()->GetProject()->SaveFile(path(Obj.second.back()));
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Quit Programm", "Alt+F4"))
				Engine::Quit();

			ImGui::EndMenu();
		}

		ImGui::Separator();
		if (ImGui::BeginMenu("Options"))
		{
			ToDo("Need To Add New File For Setting And Save It After Every Change These Things");
			static bool enabled = true;
			ImGui::MenuItem("Enabled", "", &enabled);
			if (ImGui::BeginChild("child", ImVec2(0, 60), true))
				for (int i = 0; i < 10; i++)
					ImGui::Text("Scrolling Text %d", i);
			ImGui::EndChild();
			static float f = 0.5f;
			static int n = 0;
			static bool b = true;
			ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
			ImGui::InputFloat("Input", &f, 0.1f);
			ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
			ImGui::Checkbox("Check", &b);
			ImGui::EndMenu();
		}
		//if (ImGui::BeginMenu("Disabled", false)) {} // Disabled
		//if (ImGui::MenuItem("Checked", NULL, true)) {}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, false)) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
