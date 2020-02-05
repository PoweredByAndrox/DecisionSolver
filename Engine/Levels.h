#pragma once
#ifndef __LEVELS__H_
#define __LEVELS__H_
#include "pch.h"

#include "GameObjects.h"

class Levels: public GameObjects
{
public:
	HRESULT Init();

	HRESULT LoadXML(string File);
	void ProcessXML();
	void Reload_Level(string File);

	void Update(Matrix View, Matrix Proj, float Time);

	void Destroy();

	static vector<shared_ptr<GameObjects::Object>> Obj_other, Obj_npc;
	static vector<string> IDModels;
protected:
	// **********
	shared_ptr<tinyxml2::XMLDocument> doc = make_shared<tinyxml2::XMLDocument>();

	// **********
	vector<XMLNode *> Nods;

	void UpdateLogic(float Time, shared_ptr<Object> &Obj);

	vector<shared_ptr<GameObjects::Object>> XMLPreparing(vector<XMLElement *> Attrib);
	
	static void Spawn(Vector3 pos, GameObjects::TYPE type);
};
#endif // !__LEVELS__H_
