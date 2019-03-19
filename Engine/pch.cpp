#include "pch.h"

bool FindSubStr(wstring context, wstring const from)
{
	if (context.empty())
		return false;

	auto Pos = context.find(from);
	if (Pos != std::string::npos)
		//found
		return true;
	else
		//not found
		return false;
}

bool FindSubStr(string context, string const from)
{
	if (context.empty())
		return false;

	auto Pos = context.find(from);
	if (Pos != std::string::npos)
		//found
		return true;
	else
		//not found
		return false;
}

physx::PxVec3 ToPxVec3(Vector3 var)
{
	return physx::PxVec3(var.x, var.y, var.z);
}

Vector3 ToVec3(physx::PxVec3 var)
{
	return Vector3(var.x, var.y, var.z);
}

wstring &replaceAll(wstring &context, wstring const &from, wstring const &to)
{
	size_t lookHere = 0;
	size_t foundHere = 0;
	while ((foundHere = context.find(from, lookHere)) != string::npos)
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}

	return context;
}
wstring &replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also)
{
	size_t lookHere = 0;
	size_t foundHere = 0;
	while ((foundHere = context.find(from, lookHere)) != string::npos)
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}

	lookHere = 0;
	foundHere = 0;
	while ((foundHere = context.find(also, lookHere)) != string::npos)
	{
		context.replace(foundHere, also.size(), to);
		lookHere = foundHere + to.size();
	}

	return context;
}

string &replaceAll(string &context, string const &from, string const &to)
{
	size_t lookHere = 0;
	size_t foundHere = 0;
	while ((foundHere = context.find(from, lookHere)) != string::npos)
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}

	return context;
}
string &replaceAll(string &context, string const &from, string const &to, string const &also)
{
	size_t lookHere = 0;
	size_t foundHere = 0;
	while ((foundHere = context.find(from, lookHere)) != string::npos)
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}

	lookHere = 0;
	foundHere = 0;
	while ((foundHere = context.find(also, lookHere)) != string::npos)
	{
		context.replace(foundHere, also.size(), to);
		lookHere = foundHere + to.size();
	}

	return context;
}

void deleteWord(string &context, string const &what)
{
	string::size_type pos = context.find(what.c_str());
	while (pos != string::npos)
	{
		context.erase(pos, strlen(what.c_str()));
		pos = context.find(what.c_str(), pos + 1);
	}
}

string deleteWord(string context, char const what, char const OnWhat)
{
	string str = context;
	for (int i = 0; i < str.length(); i++)
	{
		if (str.at(i) == what)
			str[i] = OnWhat;
	}

	return str;
}

string deleteWord(string context, string const start, string const end)
{
	size_t One = 0, Two = 0;
	string str = context;
	str.erase(remove(str.begin(), str.end(), '	'), str.end());

	while (str.find(start) != string::npos & str.find(end) != string::npos)
	{
		str.erase(str.find(start), (str.find(end) + end.size()) + 1 - str.find(start));
	}

	return str;
}

wstring formatstr(const char *Buff, float X, float Y, float Z)
{
	USES_CONVERSION;
	return A2W((boost::format(Buff) % X % Y % Z).str().c_str());
}