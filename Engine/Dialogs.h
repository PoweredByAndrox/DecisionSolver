#pragma once
#ifndef __DIALOGS_H__
#define __DIALOGS_H__
#include "pch.h"

#include "File_system.h"

class Dialogs
{
public:
	Dialogs() {}
	~Dialogs() {}

	HRESULT Init();
	HRESULT LoadFile(string *FileName);

	//*******
		// Get Main Actor Dialogs Replices In Vector Strings!
	void getReplices();
	bool IsInit() { return InitClass; }

	vector<string> getAllReplices() { if (!Replices.empty()) return Replices; }
protected:
	//********
	bool InitClass = false;

	//********
	shared_ptr<tinyxml2::XMLDocument> doc;

	//********
	vector<string> Replices; // Only Text
	vector<XMLNode *> Nodes; // Only Identifier
};
#endif // !__DIALOGS_H__

