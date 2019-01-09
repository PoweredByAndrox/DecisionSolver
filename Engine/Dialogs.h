#pragma once
#ifndef __DIALOGS_H__
#define __DIALOGS_H__
#include "pch.h"

#include "File_system.h"
#include "tinyxml2.h"
using namespace tinyxml2;

namespace Engine
{

	class Dialogs: public File_system
	{
	public:
		Dialogs() {}
		~Dialogs() {}

		HRESULT Init();
		HRESULT LoadFile(string *FileName);
	
		//*******
			// Get Main Actor Dialogs Replices In Vector Strings!
		void getMAReplices();
		bool IsInit() { return InitClass; }

		vector<string> getReplices() { if (!Replices.empty()) return Replices; }
	private:
			//********
		bool InitClass = false;

			//********
		unique_ptr<File_system> FS = make_unique<File_system>();

			//********
		unique_ptr<tinyxml2::XMLDocument> doc;

			//********
		vector<string> Replices; // Only Text
		vector<XMLNode *> Nodes; // Only Identifier
	};
}
#endif // !__DIALOGS_H__

