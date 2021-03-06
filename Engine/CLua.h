#pragma once
#if !defined(__CLUA__H_)
#define __CLUA__H_
#include "pch.h"

#include <sol/sol.hpp>
using namespace sol;

class CLua
{
public:
	void Init();

	void Update();
	static void callFunction(string FileName, string Function, string params);
	static void Reinit();
private:
	static state LuaState;
};
#endif // !__CLUA__H_
