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

		HRESULT Init(File_system *FS);
		HRESULT LoadFile(string *FileName);
	
		void getAllReplices();

		bool IsInit() { return InitClass; }
	private:
			//********
		bool InitClass = false;

			//********
		unique_ptr<File_system> FS;

			//********
		unique_ptr<tinyxml2::XMLDocument> doc;

			//********
		vector<string> Replices;
	};
}
#endif // !__DIALOGS_H__

