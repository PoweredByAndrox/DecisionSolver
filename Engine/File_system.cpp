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

LPCSTR File_system::GetResPathA(string *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_string() + string("//resource//");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".sdkmesh" || ext == ".obj")
			{
				*File = ResPath + string("models//") + string(*File);
				return File->c_str();
			}
			else if (ext == ".dds" || ext == ".png")
			{
				*File = ResPath + string("textures//") + string(*File);
				return File->c_str();
			}
			else if (ext == ".hlsl" || ext == ".fx")
			{
				*File = ResPath + string("shaders//") + string(*File);
				return File->c_str();
			}
		}
	}
	else
		return "";
}

LPCTSTR File_system::GetResPathW(wstring *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
			if (ext == ".sdkmesh" || ext == ".obj")
			{
				*File = ResPath + wstring(L"models//") + wstring(*File);
				return File->c_str();
			}
			else if (ext == ".hlsl" || ext == ".fx")
			{
				*File = ResPath + wstring(L"shaders//") + wstring(*File);
				return File->c_str();
			}
			else if (ext == ".png" || ext == ".dds")
			{
				*File = ResPath + wstring(L"textures//") + wstring(*File);
				return File->c_str();
			}
	}
	else
		return L"";
}

wstring File_system::GetResPathW(string *File)
{
	USES_CONVERSION;
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_string() + string("//resource//");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
			if (ext == ".sdkmesh" || ext == ".obj")
			{
				*File = ResPath + string("models//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".dds" || ext == ".png")
			{
				*File = ResPath + string("textures//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".hlsl" || ".fx")
			{
				*File = ResPath + string("shaders//") + *File;
				return A2W(File->c_str());
			}
	}
	else
		return L"";
}

vector<wstring> File_system::GetResPathW(vector<wstring> *File[])
{
	vector<wstring> ResPath[sizeof(File)];
	if (!p.empty())
	{
		for (int i = 0; i < sizeof(File); i++)
		{
			auto ext = boost::filesystem::extension(File[i]->data()->c_str());
			ResPath[i].insert(ResPath[i].end(), p.generic_path().generic_wstring() + wstring(L"//resource//"));
			if (ResPath[i].data()->empty())
				break;

			if (ext == ".sdkmesh" || ext == ".obj")
				ResPath[i].at(i).append(wstring(L"models//") + wstring(*File[i]->data()));
			else if (ext == ".dds" || ext == ".png")
				ResPath[i].at(i).append(wstring(L"textures//") + wstring(File[i]->data()->c_str()));
			return ResPath[sizeof(*File)];
		}
	}
}

vector<wstring> File_system::GetResPathW(wstring File)
{
	vector<wstring> ResPath[1];
	if (!p.empty())
	{
		ResPath[0].push_back(p.generic_path().generic_wstring() + wstring(L"//resource//"));
		auto ext = boost::filesystem::extension(File.c_str());
		if (ResPath[0].empty())
			return ResPath[0];

		if (ext == ".sdkmesh" || ".obj")
			ResPath[0].insert(ResPath[0].end(), wstring(L"models//") + wstring(File));
		else if (ext == ".dds" || ext == ".png")
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
