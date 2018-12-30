#pragma once
#ifndef __MAIN_ACTOR_H__
#define __MAIN_ACTOR_H__
#include "pch.h"

#include "GameObjects.h"
#include "Camera.h"

namespace Engine
{
	class MainActor: public GameObjects, public CFirstPersonCamera
	{
	public:

		MainActor() {}
		~MainActor() {}

		bool isDead() { return IsDead; }

		void ChangeHealth(float Value, char Char)
		{
			if (Char == '+')
				Health += Value;
			else if (!(Health <= 0))
				Health -= Value;
		}

		virtual void Update();
		virtual void Destroy();
		virtual void Render(Matrix View, Matrix Proj);

		void Hit(float Value)
		{
			ChangeHealth(Value, '-');
		}

		void ChangePosition(Vector3 Pos)
		{
			gCamera->setPosCam(Pos);
		}

		void SetupCamera()
		{
			gCamera->SetScalers(0.010f, 6.0f);
			gCamera->SetRotateButtons(true, false, false);
		}

		Vector3 getPosition() { return Position; }
		auto getObjCamera() { if (gCamera.operator bool()) return gCamera.get(); }
		float getHealthActor() { return Health; }

	private:
		//**********
		float FOV = 0.80f;
		Vector3 Position = { 0.f, 2.5f, 0.f };

		//**********
		bool IsDead = false;
		float Health = 100.0f;

		unique_ptr<CFirstPersonCamera> gCamera = make_unique<CFirstPersonCamera>();

		Vector3 GetPostitionFromCamera() { return gCamera->GetEyePt(); }
	};
}
#endif // !__MAIN_ACTOR_H__
