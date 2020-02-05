#include "SDKInterface.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "GameObjects.h"
#include "Levels.h"
#include "File_system.h"
#include "CutScene.h"
#include "Camera.h"
#include <commdlg.h>

Vector3 DragFloat3(string ID, Vector3 Thing)
{
	Vector3 Ret_Thing = Thing;
	ImGui::DragFloat3(ID.c_str(), (float *)&Ret_Thing);
	return Ret_Thing;
}

static vector<string> values = { "Stay", "WalkToNPoint", "Follow" };
static vector<string> TrueFalse = { "True", "False" };
int Current = 0, Curr = 0, Cur = 1;

void SDKInterface::Render()
{
	ImGuiID dockspaceID = 0;
	ImGui::SetNextWindowPos(ImVec2(Application->getWorkAreaSize(Application->GetHWND()).x - ImGui::GetWindowWidth() - 100.f,
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
				auto LevelObjs = Application->getLevel()->Obj_other;
				for (auto Object : LevelObjs)
				{
					bool id = ImGui::TreeNode(Object->GetIdText().c_str());
					if (id)
					{
						ImGui::SetNextItemWidth(-1);
						ImGui::Checkbox("Is Render", &Object->RenderIt);

						ImGui::Separator();
						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Pos#" + Object->GetIdText()).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						Object->SetPositionCoords(DragFloat3(("##Pos#" + Object->GetIdText()).c_str(), Object->GetPositionCord()));
						ImGui::NextColumn();

						//OutputDebugStringA((boost::format("\nPosition: X: %.3f, Y: %.3f, Z: %.3f\n") % Object->GetPositionCord().x
						//	% Object->GetPositionCord().y
						//	% Object->GetPositionCord().z).str().c_str());

						ImGui::Separator();
						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Rot#" + Object->GetIdText()).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Rotation");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						Object->SetRotationCoords(DragFloat3(("##Rot#" + Object->GetIdText()).c_str(), Object->GetRotCord()));
						ImGui::NextColumn();
						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);
						ImGui::TreeNodeEx(("Scl#" + Object->GetIdText()).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Scale");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						Object->SetScaleCoords(DragFloat3(("##Scl#" + Object->GetIdText()).c_str(), Object->GetScaleCord()));
						ImGui::NextColumn();
						ImGui::Separator();

						ImGui::SetNextItemWidth(-1);
						if (ImGui::Button("Reset All"))
						{
							Object->SetPositionCoords(Object->GetRPos());
							Object->SetRotationCoords(Object->GetRRot());
							Object->SetScaleCoords(Object->GetRScale());
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::Button("Reload Level"))
				Application->getLevel()->Reload_Level(Application->getFS()->GetFileByType(_TypeOfFile::LEVELS).back()->PathA);
			ImGui::Columns(1);
		}
		dockspaceID = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
	}

	ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	if (Open2)
	{
		if (ImGui::Begin("Add Things", &Open2))
		{
			ImGui::Separator();

			ImGui::AlignTextToFramePadding();

			auto ThisLevel = Application->getLevel();
			bool id = ImGui::TreeNode("Phys Box");
			if (id)
			{
				if (ImGui::Button("Spawn Cobe"))
					Application->getPhysics()->SpawnObject();
				if (ImGui::Button("Delete Cobes"))
					Application->getPhysics()->DestroyObj();

				ImGui::TreePop();
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
				for (auto it : SndFiles)
				{
					if (filter.PassFilter(it->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Mdl = ImGui::TreeNode("Models");
			if (Mdl)
			{
				auto MldFiles = FS->GetFileByType(_TypeOfFile::MODELS);
				for (auto it : MldFiles)
				{
					if (filter.PassFilter(it->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Txtrs = ImGui::TreeNode("Textures");
			if (Txtrs)
			{
				auto TxtrFiles = FS->GetFileByType(_TypeOfFile::TEXTURES);
				for (auto it : TxtrFiles)
				{
					if (filter.PassFilter(it->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Lvls = ImGui::TreeNode("Levels");
			if (Lvls)
			{
				auto LvlFiles = FS->GetFileByType(_TypeOfFile::LEVELS);
				for (auto it : LvlFiles)
				{
					if (filter.PassFilter(it->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Shdrs = ImGui::TreeNode("Shaders");
			if (Shdrs)
			{
				auto ShdrFiles = FS->GetFileByType(_TypeOfFile::SHADERS);
				for (auto it : ShdrFiles)
				{
					if (filter.PassFilter(it->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it->PathA.c_str());
				}

				ImGui::TreePop();
			}

			bool Scrpts = ImGui::TreeNode("Scripts");
			if (Scrpts)
			{
				auto ScrFiles = FS->GetFileByType(_TypeOfFile::SCRIPTS);
				for (auto it : ScrFiles)
				{
					if (filter.PassFilter(it->FileA.c_str()))
						ImGui::TreeNodeEx(("File#" + it->FileA).c_str(), ImGuiTreeNodeFlags_Leaf |
							ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, it->PathA.c_str());
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
							This->GetPoints().at(i)->SetPos(DragFloat3(("##SLogic_Pos" + to_string(i)), This->GetPoints().at(i)->GetPos()));
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
			bool Cam = ImGui::TreeNode("Camera");
			if (Cam)
			{
				ImGui::Separator();
				ImGui::TreeNodeEx("Position:", ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				ImGui::SameLine();
				Vector3 Pos = Application->getCamera()->GetEyePt(), Look = Application->getCamera()->GetLookAtPt();
				bool BPos = ImGui::DragFloat3("##Position", (float *)&Pos);

				ImGui::Text("Look: ");
				ImGui::SameLine();
				bool BLook = ImGui::DragFloat2("##Look", (float *)&Look);
				if (BPos || BLook)
					Application->getCamera()->Teleport(Pos, Look);

				ImGui::Separator();
				if (Combobox::Combo("Is Free Cam?", &Cur, TrueFalse))
					Application->getCamera()->SetFreeMoveCam((bool)Cur);
				ImGui::Separator();

				ImGui::TreeNodeEx("Move Senses:", ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
				float MovSense = Application->getCamera()->getMoveScale(), RotSense = Application->getCamera()->getRotateScale();
				ImGui::Text("Move Scaler: ");
				ImGui::SameLine();
				bool MSense = ImGui::DragFloat("##MSense", &MovSense);

				ImGui::Text("Rotation Scaler: ");
				ImGui::SameLine();
				bool RSense = ImGui::DragFloat("##RSense", &RotSense);
				if (MSense || RSense)
					Application->getCamera()->SetScalers(RotSense, MovSense);

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
			if (ImGui::MenuItem("Open Project", "Ctrl+O"))
			{
				auto Obj = UI::GetWndDlgOpen(const_cast<LPSTR>(Application->getFS()->getPathFromType(_TypeOfFile::LEVELS).c_str()));
				if (Obj.first) // If Dialog Wasn't Skip
				{
					if (Obj.second.empty()) return;
					Application->getFS()->GetProjects()->SetCurProject(path(Obj.second.back()));
					Application->getFS()->GetProjects()->OpenFile(Obj.second.back());
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
				if (Application->getFS()->GetProjects().operator bool())
				{
					auto Recent = Application->getFS()->GetProjects()->GetRecent();
					for (size_t i = 0; i < Recent.size(); i++)
					{
						auto Obj = std::next(Recent.begin(), i);
						if (ImGui::MenuItem((ReplaceSymbols(Obj->second.filename().string())).c_str(),
							to_string(Obj->first).c_str()))
							Application->getFS()->GetProjects()->OpenFile(Obj->second);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save Project", "Ctrl+S"))
			{
				Application->getFS()->GetProjects()->SaveCurrProj();
			}
			if (ImGui::MenuItem("Save Project As..", "Ctrl+Shift+S"))
				UI::GetWndDlgSave(const_cast<LPSTR>(Application->getFS()->getPathFromType(_TypeOfFile::LEVELS).c_str()));
			
			ImGui::Separator();
			if (ImGui::MenuItem("Quit Programm", "Alt+F4")) Engine::Quit();
			ImGui::EndMenu();
		}

		ImGui::Separator();
		if (ImGui::BeginMenu("Options"))
		{
			ToDo("Need To Add New File For Setting And Save It After Every Change These Things");
			static bool enabled = true;
			ImGui::MenuItem("Enabled", "", &enabled);
			ImGui::BeginChild("child", ImVec2(0, 60), true);
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
