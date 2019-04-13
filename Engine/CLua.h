#pragma once
#if !defined(__CLUA_H__)
#define __CLUA_H__

#include <sol/sol.hpp>
using namespace sol;

// Need To Add FS Here!!! And Get All The Script Files And Init Them
class CLua
{
public:
	void Init();
	void GetDate(string Text);

	void Update();
	template<typename... Args>
	//void callFuncion(state &lua, LPCSTR name, Args &&... args);
	CLua() {}
	~CLua() {}

	state &getLState() { return LuaState; }
private:
	state LuaState;
};
#endif // !__CLUA_H__
