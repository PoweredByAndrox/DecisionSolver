#pragma once
#ifndef __GAME_OBJECTS_H__
#define __GAME_OBJECTS_H__
#include "pch.h"

#include "Physics.h"
#include "Model.h"
#include "Audio.h"

namespace Engine
{
	class GameObjects: public Physics
	{
	public:
		virtual void Render(Matrix View, Matrix Proj) = 0;
		virtual void Update() = 0;
		virtual void Destroy() = 0;

		GameObjects() {}
		~GameObjects() {}

		bool IsInitClass() { return isInit; }

	private:
			//********
		HRESULT hr = S_OK;

			//********
		bool isInit = false;

			//********
		//Audio *Sound = new Audio;

			//********
		vector<UINT> CountOfObjects;
	};
}
#endif // !__GAME_OBJECTS_H__
