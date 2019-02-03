#pragma once
#ifndef __GAME_OBJECTS_H__
#define __GAME_OBJECTS_H__
#include "pch.h"

#include "Physics.h"
#include "Model.h"
#include "Audio.h"

namespace Engine
{
	class GameObjects: public Models
	{
	public:
		//virtual void Render(Matrix View, Matrix Proj, float Time) = 0;
		//virtual void Update(float Time) = 0;
		//virtual void Destroy() = 0;

		auto getObjects() { return Objects; }

		GameObjects() {}
		~GameObjects() {}
	protected:
		//********
		HRESULT hr = S_OK;

		//********
		// Audio *Sound = new Audio;
		
		struct Object
		{
			Object(Models *model) { this->model = model; }
			void Destroy() { ID = 0; ID_TEXT = ""; HasScale = false; ScaleCoords = Vector3::Zero; SAFE_DELETE(model); }
			int ID = 0;
			
			LPCSTR ID_TEXT = "";

			bool HasScale = false;
			Vector3 ScaleCoords = Vector3::Zero;

			Models *model = nullptr;

			void SetID_TEXT(LPCSTR ID_TEXT) { this->ID_TEXT = ID_TEXT; }
			void SetScaleCoords(Vector3 ScaleCoords) { this->ScaleCoords = ScaleCoords; }
			void SetModel(Models *model) { this->model = model; }
		};
		vector<Object> Objects;
	};
}
#endif // !__GAME_OBJECTS_H__
