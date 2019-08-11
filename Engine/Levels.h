#pragma once
#ifndef __LEVELS__H_
#define __LEVELS__H_
#include "pch.h"

#include "GameObjects.h"

class Levels: public GameObjects
{
public:
	HRESULT Init();

	HRESULT LoadXML(LPCSTR File);
	void ProcessXML();

	void Update(Matrix View, Matrix Proj, float Time);

	void Destroy();

	bool IsInit() { return InitClass; }
protected:
	// **********
	bool InitClass = false;

	// **********
	shared_ptr<tinyxml2::XMLDocument> doc;

	// **********
	vector<XMLNode *> Nods;

	void UpdateLogic(float Time, shared_ptr<Object> &Obj);

	vector<shared_ptr<GameObjects::Object>> XMLPreparing(vector<XMLElement *> Attrib);
	
	static void Spawn(Vector3 pos, GameObjects::TYPE type);
};
#endif // !__LEVELS__H_
