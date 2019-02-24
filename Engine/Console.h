#pragma once
#ifndef __CONSOLE__H_
#define __CONSOLE__H_
#include "pch.h"

#include "UI.h"
#include "Physics.h"
#include "Levels.h"

namespace Engine
{
	enum Console_STATE
	{
		Open = 1,
		Close
	};

	class Console: public UI, public Physics
	{
	public:
		HRESULT Init(File_system *FS, Physics *Phys, Levels *level);

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

		unique_ptr<UI> ui = make_unique<UI>();
		unique_ptr<Physics> Phys;
		Levels *level = nullptr;

		File_system *FS = nullptr;

		static void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, Control *pControl, vector<void *> pUserContext);
	private:
		bool InitClass = false;
		Console_STATE CState = Console_STATE::Close;
	};
};
#endif // !__CONSOLE__H_

