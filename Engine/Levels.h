#pragma once
#ifndef __LEVELS__H_
#define __LEVELS__H_
#include "pch.h"

#include "GameObjects.h"

enum _TypeOfFile;

class SimpleLogic;
class Levels: public GameObjects
{
private:
	struct Node
	{
	private:
		struct NewInfo
		{
			bool Pos = false, Scale = false, Rot = false; // If We Change These Params
			bool IsVisible = false, IsRemoved = false;
			Vector3 _Pos, _Scale, _Rot; // To Set It When Change From SDK
			TYPE T = TYPE::NONE;
		};

	public:
		Node() {}
		Node(shared_ptr<GameObjects::Object> GObj) { GM = GObj; }

		shared_ptr<GameObjects::Object> GM = make_shared<GameObjects::Object>();

		bool IsItChanged = false; // Needed To Save Action
		string ID; // Only ID Of Node
		string RenderName;
		shared_ptr<NewInfo> SaveInfo = make_shared<NewInfo>();
	};
	struct Child
	{
	private:
		vector<shared_ptr<Node>> Nodes;

	public:
		shared_ptr<Node> AddNewNode(shared_ptr<Node> ND);
		void DeleteNode(string ID);
		void Update();
		auto GetNodes() { return Nodes; }
		shared_ptr<Node> getNodeByID(string ID);
	};
	shared_ptr<Child> MainChild = make_shared<Child>(); // It's a Main Scene
public:
	HRESULT Init();

	HRESULT Load(string FileBuff);
	void Process();
	void Reload_Level(string File);
	void Update();

	shared_ptr<Node> Add(string PathModel);
	shared_ptr<Node> Add(shared_ptr<GameObjects::Object> GM);
	void AddTo(string ID, shared_ptr<SimpleLogic> Logic);
	void AddTo(shared_ptr<Node> nd, shared_ptr<SimpleLogic> Logic);
	void Remove(string ID);
	void RemoveFrom(shared_ptr<Node> nd);

	auto getChild() { return MainChild; }
	void Destroy();

	shared_ptr<tinyxml2::XMLDocument> getDocXMLFile() { return doc; }

	string Save(shared_ptr<tinyxml2::XMLDocument> Doc, shared_ptr<Node> Node);

	void SetNotSaved(bool b) { NotSaved = b; }
	bool IsNotSaved() { return NotSaved; }
protected:
	// **********
	shared_ptr<tinyxml2::XMLDocument> doc = make_shared<tinyxml2::XMLDocument>();
	static void Spawn(/*Vector3 pos, GameObjects::TYPE type*/);
	bool NotSaved = false;
};
#endif // !__LEVELS__H_
