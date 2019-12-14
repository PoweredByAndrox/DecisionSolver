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

void replaceAll(wstring &context, wstring const &from, wstring const &to, bool OneTime, bool FindInEnd, bool AlsoDeleteSpace)
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

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}
void replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also, bool OneTime,
	bool FindInEnd, bool AlsoDeleteSpace)
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

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}

void replaceAll(string &context, string const &from, string const &to, bool OneTime, bool FindInEnd, bool AlsoDeleteSpace)
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

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}
void replaceAll(string &context, string const &from, string const &to, string const &also, bool OneTime,
	bool FindInEnd, bool AlsoDeleteSpace)
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

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}

void deleteWord(string &context, string const &what, bool OneTime, bool FindInEnd, bool AlsoDeleteSpace)
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

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}

void deleteWord(string &context, char const what, char const OnWhat, bool AlsoDeleteSpace)
{
	for (size_t i = 0; i < context.length(); i++)
	{
		if (context.at(i) == what)
			context[i] = OnWhat;
	}

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}

void deleteWord(string &context, string const start, string const end, bool OneTime, bool AlsoDeleteSpace)
{
	if (OneTime)
	{
		size_t pos1 = 0, pos2 = string::npos;
		if ((pos1 = context.find(start, pos1)) != string::npos)
		{
			if ((pos2 = context.find(end, pos1)) != string::npos)
			{
				pos2 += end.length();
				context.erase(pos1, pos2 - pos1);
			}
		}
	}
	else
	{
		size_t pos1 = 0, pos2 = string::npos;
		for (;;)
		{
			if ((pos1 = context.find(start, pos1)) != string::npos)
			{
				if ((pos2 = context.find(end, pos1)) != string::npos)
				{
					pos2 += end.length();
					context.erase(pos1, pos2 - pos1);
				}
				else break;
			}
			else break;
		}
	}

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), '	'), context.end());
}

void deleteWord(string &context, string const start, ModeProcessString const mode, bool FindInEnd, bool AlsoDeleteSpace)
{
	if (mode == ModeProcessString::UntilTheEnd)
		if (context.find(start) != string::npos)
			context.erase(context.find(start), context.length());

	if (mode == ModeProcessString::UntilTheBegin)
		if (FindInEnd ? context.rfind(start) : context.find(start) + 1 /* with start char */ != string::npos)
			context.erase(0, FindInEnd ? context.rfind(start) : context.find(start) + 1);

	if (AlsoDeleteSpace)
		context.erase(remove(context.begin(), context.end(), ' '), context.end());
}

void getFloat3Text(string context, string Char2Split, vector<float> &Float3)
{
	vector<string> Result;
	boost::split(Result, context, boost::is_any_of(","));
	for (size_t i = 0; i < Result.size(); i++)
	{
		Float3.push_back((float)atof(Result.at(i).c_str()));
	}
}

void getTextFloat3(string &context, string Char2Split, vector<float> Float3)
{
	for (size_t i = 0; i < Float3.size(); i++)
	{
		context += i == 0 ? to_string(Float3.at(i)) : Char2Split + to_string(Float3.at(i));
	}
}

void ParseText(string &Text, Type type)
{
	vector<string> strs;
	deleteWord(Text, "\t", false, false, true);
	boost::split(strs, Text, boost::is_any_of("\n"));
	Text.clear();

	for (size_t i = 0; i < strs.size(); i++)
	{
		if (type == Type::Information)
		{
			if (strs.at(i).empty())
				continue;
			Text += strs.at(i).insert(0, (strs.size() >= 2 && i >= 1 ? "\n[INFO] " : "[INFO] "));
		}
		else if (type == Type::Error)
		{
			if (strs.at(i).empty())
				continue;
			Text += strs.at(i).insert(0, (strs.size() >= 2 && i >= 1 ? "\n[ERROR] " : "[ERROR] "));
		}
		else
		{
			if (strs.at(i).empty())
				continue;
			else
				Text.insert(0, "\n" + strs.at(i));
		}
	}
}

Vector3 ConstrainToBoundary(Vector3 Pos, Vector3 Min, Vector3 Max)
{
	Pos.Clamp(Min, Max);
	return Pos;
}
