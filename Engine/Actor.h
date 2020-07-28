#pragma once
#if !defined(__ACTOR_H__)
#define __ACTOR_H__
#include "pch.h"

//#include "Dialogs.h"

class Actor //, public Dialogs
{
public:
	bool isDead() { return IsDead; }

	void ChangeHealth(float Value, char Char)
	{
		if (Char == 'G' && !IsGod)
		{
			IsGod = true;
			return;
		}
		else if (Char == 'G' && IsGod)
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
	void Render(float Time);

	void Hit(float Value)
	{
		if (!IsGod)
			ChangeHealth(Value, '-');
	}

	void ChangePosition(Vector3 Pos);

	HRESULT Init();

	Vector3 getPosition() { return Position; }
	float getHealthActor() { return Health; }

	bool IsInit() { return InitClass; }
private:
	HRESULT hr = S_OK;

	//**********
	Vector3 Position = { 0.f, 2.5f, 0.f };

	//**********
	bool IsDead = false, InitClass = false;
	float Health = 100.0f, FOV = 0.80f;


	//**********
		// This is a cheat
	bool IsGod = false;

	//**********
//unique_ptr<Dialogs> DLG = make_unique<Dialogs>();
};
#endif // !__ACTOR_H__
