#pragma once
#ifndef __CONSOLE__H_
#define __CONSOLE__H_
#include "pch.h"

#include "UI.h"

namespace Engine
{
	enum Console_STATE
	{
		Open = 1,
		Close
	};

	class Console: public UI
	{
	public:
		HRESULT Init();

		HRESULT Settings(bool Reset);

		void Render(float Time);
		void Open();
		void Close();

		Console() {}
		~Console() {}

		bool IsInit() { return InitClass; }
		Console_STATE *getState() { return &CState; }
		void ChangeState(Console_STATE Cstate) { CState = Cstate; }

		UI *getUI() { if (ui.operator bool()) return ui.get(); return nullptr; }
	protected:
		HRESULT hr = S_OK;

		static bool FindSubStr(wstring &context, wstring const &from)
		{
			auto Pos = context.find(from);
			if (Pos != std::string::npos)
				//found
				return true;
			else
				//not found
				return false;
		}

		unique_ptr<UI> ui = make_unique<UI>();
		static void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, vector<void *> pUserContext);
	private:
		bool InitClass = false;
		Console_STATE CState = Console_STATE::Close;
	};
};
#endif // !__CONSOLE__H_

