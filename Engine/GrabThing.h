#pragma once
#ifndef __GRABTHING_H__
#define __GRABTHING_H__
#include "pch.h"

class GrabThing
{
public:
	void CheckType(int ID);

	void Drop(int ID = -1 /* It means the current thing*/);
	//void Drop();
	void Grab();

private:
	enum TypeOfGrab
	{
		CATCH = 0,
		NO_USE
	};
};

#endif // !__GRABTHING_H__
