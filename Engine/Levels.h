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
		Node() {}
		Node(shared_ptr<GameObjects::Object> GObj) { GM = GObj; }

		shared_ptr<GameObjects::Object> GM = make_shared<GameObjects::Object>();

		bool IsItChanged = false; // Needed To Save Action
		string ID = ""; // Only ID Of Node
	};
	struct Child
	{
	private:
		vector<shared_ptr<Node>> Nodes;

	public:
		void AddNewNode(shared_ptr<Node> ND);
		void DeleteNode(string ID);
		void Update();
		auto GetNodes() { return Nodes; }
		shared_ptr<Node> getNodeByID(string ID);
	};
	shared_ptr<Child> MainChild = make_shared<Child>(); // It's a Main Scene
public:
	HRESULT Init();

	HRESULT LoadXML(string File);
	void ProcessXML();
	void Reload_Level(string File);

	void Update();

	void Add(_TypeOfFile T, string PathModel);
	void AddTo(string ID, shared_ptr<SimpleLogic> Logic);
	void AddTo(shared_ptr<Node> nd, shared_ptr<SimpleLogic> Logic);
	void Remove(string ID);

	auto getChild() { return MainChild; }
	void Destroy();

//	static vector<shared_ptr<GameObjects::Object>> Obj_other, Obj_npc;
//	static vector<string> IDModels;
protected:
	// **********
	shared_ptr<tinyxml2::XMLDocument> doc = make_shared<tinyxml2::XMLDocument>();

	// **********
	vector<XMLNode *> Nods;

	vector<shared_ptr<GameObjects::Object>> XMLPreparing(vector<XMLElement *> Attrib);
	
	static void Spawn(Vector3 pos, GameObjects::TYPE type);
};
#endif // !__LEVELS__H_
