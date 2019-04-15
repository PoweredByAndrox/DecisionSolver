#include "pch.h"

bool FindSubStr(wstring context, wstring const from)
{
	if (context.empty() || from.empty())
		return false;

	if (context.find(from) != std::string::npos)
		//found
		return true;
	else
		//not found
		return false;
}

bool FindSubStr(string context, string const from)
{
	if (context.empty() || from.empty())
		return false;

	if (context.find(from) != std::string::npos)
		//found
		return true;
	else
		//not found
		return false;
}

void replaceAll(wstring &context, wstring const &from, wstring const &to, bool OneTime, bool FindInEnd)
{
	size_t lookHere = FindInEnd ? context.length() : 0;
	size_t foundHere = 0;
	if (OneTime)
	{
		if ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	else
	{
		while ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
}
void replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also, bool OneTime, bool FindInEnd)
{
	size_t lookHere = FindInEnd ? context.length() : 0;
	size_t foundHere = 0;
	if (OneTime)
	{
		if ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	else
	{
		while ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}

	lookHere = FindInEnd ? context.length() : 0;
	foundHere = 0;
	if (OneTime)
	{
		if ((foundHere = FindInEnd ? context.rfind(also, lookHere) : context.find(also, lookHere)) != string::npos)
		{
			context.replace(foundHere, also.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	else
	{
		while ((foundHere = FindInEnd ? context.rfind(also, lookHere) : context.find(also, lookHere)) != string::npos)
		{
			context.replace(foundHere, also.size(), to);
			lookHere = foundHere + to.size();
		}
	}
}

void replaceAll(string &context, string const &from, string const &to, bool OneTime, bool FindInEnd)
{
	size_t lookHere = FindInEnd ? context.length() : 0;
	size_t foundHere = 0;
	if (OneTime)
	{
		if ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	else
	{
		while ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
}
void replaceAll(string &context, string const &from, string const &to, string const &also, bool OneTime, bool FindInEnd)
{
	size_t lookHere = FindInEnd ? context.length() : 0;
	size_t foundHere = 0;
	if (OneTime)
	{
		if ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	else
	{
		while ((foundHere = FindInEnd ? context.rfind(from, lookHere) : context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	lookHere = FindInEnd ? context.length() : 0;
	foundHere = 0;
	if (OneTime)
	{
		if ((foundHere = FindInEnd ? context.rfind(also, lookHere) : context.find(also, lookHere)) != string::npos)
		{
			context.replace(foundHere, also.size(), to);
			lookHere = foundHere + to.size();
		}
	}
	else
	{
		while ((foundHere = FindInEnd ? context.rfind(also, lookHere) : context.find(also, lookHere)) != string::npos)
		{
			context.replace(foundHere, also.size(), to);
			lookHere = foundHere + to.size();
		}
	}
}

void deleteWord(string &context, string const &what, bool OneTime, bool FindInEnd)
{
	string::size_type pos = FindInEnd ? context.rfind(what.c_str()) : context.find(what.c_str());
	if (OneTime)
	{
		if (pos != string::npos)
		{
			context.erase(pos, strlen(what.c_str()));
			pos = FindInEnd ? context.rfind(what.c_str(), pos + 1) : context.find(what.c_str(), pos + 1);
		}
	}
	else
	{
		while (pos != string::npos)
		{
			context.erase(pos, strlen(what.c_str()));
			pos = FindInEnd ? context.rfind(what.c_str(), pos + 1) : context.find(what.c_str(), pos + 1);
		}
	}
}

void deleteWord(string &context, char const what, char const OnWhat)
{
	for (int i = 0; i < context.length(); i++)
	{
		if (context.at(i) == what)
			context[i] = OnWhat;
	}
}

void deleteWord(string &context, string const start, string const end, bool OneTime, bool FindInEnd, bool AlsoDeleteSpace)
{
	size_t One = 0, Two = 0;
	string str = context;

	if (AlsoDeleteSpace)
		str.erase(remove(str.begin(), str.end(), '	'), str.end());

	if (OneTime)
	{
		if (FindInEnd ? str.rfind(start) : str.find(start) != string::npos && FindInEnd ? str.rfind(end) : str.find(end) != string::npos)
		{
			str.erase(FindInEnd ? str.find(start) : str.rfind(start), (FindInEnd ? str.rfind(end) : str.find(end) + end.size()) +
				1 - FindInEnd ? str.rfind(start) : str.find(start));
		}
	}
	else
	{
		while (FindInEnd ? str.rfind(start) : str.find(start) != string::npos && FindInEnd ? str.rfind(end) : str.find(end) != string::npos)
		{
			str.erase(FindInEnd ? str.find(start) : str.rfind(start), (FindInEnd ? str.rfind(end) : str.find(end) + end.size()) +
				1 - FindInEnd ? str.rfind(start) : str.find(start));
		}
	}
}

void deleteWord(string &context, string const start, ModeProcessString const mode, bool FindInEnd, bool AlsoDeleteSpace)
{
	if (mode == ModeProcessString::UntilTheEnd)
		if (context.find(start) != string::npos)
			context.erase(context.find(start), context.length());

	if (mode == ModeProcessString::UntilTheBegin)
		if (FindInEnd ? context.rfind(start) : context.find(start) != string::npos)
			context.erase(0, context.find(start));

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), ' '), context.end());
}

wstring formatstr(const char *Buff, float X, float Y, float Z)
{
	USES_CONVERSION;
	return A2W((boost::format(Buff) % X % Y % Z).str().c_str());
}