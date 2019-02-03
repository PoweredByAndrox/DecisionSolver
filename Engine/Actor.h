#pragma once
#ifndef __ACTOR_H__
#define __ACTOR_H__
#include "pch.h"

#include "GameObjects.h"
#include "Camera.h"
#include "Dialogs.h"

namespace Engine
{
	class Actor: public GameObjects, public Camera, public Dialogs
	{
	public:

		Actor() {}
		~Actor() {}

		bool isDead() { return IsDead; }

		void ChangeHealth(float Value, char Char)
		{
			if (Char == 'G' & !IsGod)
			{
				IsGod = true;
				return;
			}
			else if (Char == 'G' & IsGod)
			{
				IsGod = false;
				return;
			}

			if (!IsGod)
			{
				if (Char == '+')
					Health += Value;
				else if (!(Health <= 0) & (Char == '-'))
					Health -= Value;
			}
		}

		void Update(float Time);
		void Destroy();
		void Render(Matrix View, Matrix Proj, float Time);

		void Hit(float Value)
		{
			if (!IsGod)
				ChangeHealth(Value, '-');
		}

		void ChangePosition(Vector3 Pos)
		{
			gCamera->setPosCam(Pos);
		}

		void SetupCamera()
		{
			gCamera = make_unique<Camera>();

			ThrowIfFailed(gCamera->Init(PhysX.get()));

			gCamera->SetScalers(0.010f, 6.0f);
			gCamera->SetRotateButtons(true, false, false);
			gCamera->SetEnableYAxisMovement(true);
			SetChangeFOV(false);
		}

		HRESULT Init(Physics *PhysX);

		Vector3 getPosition() { return Position; }
		Camera *getObjCamera() { if (gCamera.operator bool()) return gCamera.get(); return nullptr; }
		float getHealthActor() { return Health; }

		bool IsInit() { return InitClass; }
	private:
			//**********
		Vector3 Position = { 0.f, 2.5f, 0.f };

			//**********
		bool IsDead = false, InitClass = false;
		float Health = 100.0f, FOV = 0.80f;

			//**********
		unique_ptr<Camera> gCamera;

			//**********
		Vector3 GetPostitionFromCamera() { return gCamera->GetEyePt(); }

			//**********
		unique_ptr<Physics> PhysX;

			//**********
		// It's a cheat
		bool IsGod = false;

			//**********
		unique_ptr<Dialogs> DLG = make_unique<Dialogs>();
	};
}
#endif // !__ACTOR_H__
