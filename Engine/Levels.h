#pragma once
#ifndef __LEVELS__H_
#define __LEVELS__H_
#include "pch.h"

#include "File_system.h"
#include "Models.h"
#include "GameObjects.h"

namespace Engine
{
	class Levels: public GameObjects
	{
	public:
		HRESULT Init();

		HRESULT LoadXML(LPCSTR File);
		void ProcessXML();

		Levels() {}
		~Levels() {}

		void Destroy()
		{
			while (!g_Obj.empty())
			{
				g_Obj.at(0).model->Close();
				g_Obj.at(0).Destroy();
				g_Obj.erase(g_Obj.begin());
			}
		}

		auto getObjects() { return g_Obj; }

		bool IsInit() { return InitClass; }
	protected:
		// **********
		bool InitClass = false;

		// **********
		unique_ptr<File_system> FS = make_unique<File_system>();

		// **********
		unique_ptr<tinyxml2::XMLDocument> doc;

		// **********
		vector<GameObjects::Object> g_Obj;

		// **********
		vector<XMLElement *> Attrib;
		vector<XMLNode *> Nods;

		void StackTrace(const char *Error)
		{
			DebugTrace("\n***********ERROR IN XML FILE***********\n");
			DebugTrace("===Check info below:\n");
			DebugTrace(string(string("...\n") + string(Error) + string("\n...")).c_str());
			DebugTrace("\n***********ERROR IN XML FILE***********\n");
		}
	};
};
#endif // !__LEVELS__H_
