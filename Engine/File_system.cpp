#include "pch.h"

#include "DXUT.h"
#include "SDKmisc.h"
#include "DXUTmisc.h"

#include <stdio.h> 
#include <direct.h>
#include <tchar.h>

#include <windows.h>
#include "File_system.h"

#include <direct.h>

File_system::File_system()
{
	p = _wgetcwd(NULL, 512);
	if (p.empty())
		TestMsgBox("Error in getting path. Class_File_System. Line: 11", "ERROR!");
}


File_system::~File_system()
{
}

void File_system::GetPath()
{
	TestMsgBox(p.root_path().generic_string().c_str());
}

LPCSTR File_system::GetResPathA(string *File)
{
	if (!p.empty())
	{
		auto static ResPath = p.generic_path().generic_string() + string("//resource//");
		if (!ResPath.empty())
			return ResPath.c_str();
	}
	else
		return "";
}

LPCTSTR File_system::GetResPathW(wstring *File)
{
	if (!p.empty())
	{
		auto static ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//");
		if (!ResPath.empty())
			if (File->substr(File->find_last_of(L".") + 1) == L"sdkmesh")
			{
				ResPath = ResPath + wstring(L"models//") + wstring(*File);
				return ResPath.c_str();
			}
			else if (File->substr(File->find_last_of(L".") + 1) == L"hlsl")
			{
				ResPath = ResPath + wstring(L"shaders//") + wstring(*File);
				return ResPath.c_str();
			}
	}
	else
		return L"";
}

wstring File_system::GetResPathW(string File)
{
	USES_CONVERSION;
	auto static wsFile = File;
	if (!p.empty())
	{
		auto static ResPath = p.generic_path().generic_string() + string("//resource//");
		if (!ResPath.empty())
			if (wsFile.substr(wsFile.find_last_of(".") + 1) == "sdkmesh")
			{
				ResPath = ResPath + string("models//") + wsFile;
				return A2W(ResPath.c_str());
			}
			else if (wsFile.substr(wsFile.find_last_of(".") + 1) == "dds")
			{
				ResPath = ResPath + string("textures//") + wsFile;
				return A2W(ResPath.c_str());
			}
			else if (wsFile.substr(wsFile.find_last_of(".") + 1) == "hlsl")
			{
				ResPath = ResPath + string("shaders//") + wsFile;
				return A2W(ResPath.c_str());
			}
	}
	else
		return L"";
}

LPCTSTR File_system::GetResPathW(LPCTSTR File)
{
	auto static wsFile = wstring(File);
	if (!p.empty())
	{
		auto static ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//");
		if (!ResPath.empty())
			if (wsFile.substr(wsFile.find_last_of(L".") + 1) == "sdkmesh")
			{
				ResPath = ResPath + wstring(L"models//") + wstring(wsFile);
				return ResPath.c_str();
			}
		else if (wsFile.substr(wsFile.find_last_of(L".") + 1) == "dds")
		{
			ResPath = ResPath + wstring(L"textures//") + wstring(wsFile);
			return ResPath.c_str();
		}
	}
	else
		return L"";
}

vector<wstring> File_system::GetResPathW(vector<wstring> File[])
{
	static vector<wstring> ResPath[sizeof(File)];
	if (!p.empty())
	{
		for (int i = 0; i < sizeof(File); i++)
		{
			ResPath[i].insert(ResPath[i].end(), p.generic_path().generic_wstring() + wstring(L"//resource//"));
			if (ResPath[i].data()->empty())
				break;

			if (File[i].data()->substr(File[i].data()->find_last_of(L".") + 1) == "sdkmesh")
				ResPath[i].at(i).append(wstring(L"models//") + wstring(*File[i].data()));
			else if (File[i].data()->substr(File[i].data()->find_last_of(L".") + 1) == "dds")
				ResPath[i].at(i).append(wstring(L"textures//") + wstring(File[i].data()->c_str()));
			return ResPath[sizeof(*File)];
		}
	}
}

vector<wstring> File_system::GetResPathW(wstring File)
{
	static vector<wstring> ResPath[1];
	if (!p.empty())
	{
		ResPath[0].push_back(p.generic_path().generic_wstring() + wstring(L"//resource//"));
		if (ResPath[0].empty())
			return ResPath[0];

		if (File.substr(File.find_last_of(L".") + 1) == "sdkmesh")
			ResPath[0].insert(ResPath[0].end(), wstring(L"models//") + wstring(File));
		else if (File.substr(File.find_last_of(L".") + 1) == "dds")
			ResPath[0].at(0).append(wstring(L"textures//") + wstring(File));
		return ResPath[0];
	}
}

void File_system::TestMsgBox(LPCSTR Text)
{
	MessageBoxA(DXUTGetHWND(), Text, "Test", MB_OK);
}

void File_system::TestMsgBox(LPCSTR Text, LPCSTR CaptionText, int MButtons)
{
	MessageBoxA(DXUTGetHWND(), Text, CaptionText, MButtons);
}
