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
	{
		DebugTrace("File_system: Error getting path. Line: 19\n");
		throw std::exception("p == null!!!");
	}
}

LPCSTR File_system::GetResPathA(string *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_string() + string("//resource//");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".obj")
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
			if (ext == ".obj")
			{
				*File = ResPath + string("sounds//") + string(*File);
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
			if (ext == ".obj")
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
			else if (ext == ".wav")
			{
				*File = ResPath + wstring(L"sounds//") + wstring(*File);
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
			if (ext == ".obj")
			{
				*File = ResPath + string("models//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".dds" || ext == ".png")
			{
				*File = ResPath + string("textures//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".hlsl" || ext == ".fx")
			{
				*File = ResPath + string("shaders//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".wav")
			{
				*File = ResPath + string("sounds//") + *File;
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

			if (ext == ".obj")
				ResPath[i].at(i).append(wstring(L"models//") + wstring(*File[i]->data()));
			else if (ext == ".dds" || ext == ".png")
				ResPath[i].at(i).append(wstring(L"textures//") + wstring(File[i]->data()->c_str()));
			else if (ext == ".wav")
				ResPath[i].at(i).append(wstring(L"sounds//") + wstring(File[i]->data()->c_str()));
			else if (ext == ".hlsl" || ext == ".fx")
				ResPath[i].at(i).append(wstring(L"shaders//") + wstring(File[i]->data()->c_str()));
			return ResPath[sizeof(*File)];
		}
	}
}

vector<wstring> File_system::GetResPathW(wstring File)
{
	vector<wstring> ResPath;
	if (!p.empty())
	{
		ResPath.push_back(p.generic_path().generic_wstring() + wstring(L"//resource//"));
		auto ext = boost::filesystem::extension(File.c_str());
		if (ResPath.empty())
			return ResPath;

		if (ext == ".obj")
			ResPath.push_back(wstring(L"models//") + wstring(File));
		else if (ext == ".dds" || ext == ".png")
			ResPath.push_back(wstring(L"textures//") + wstring(File));
		else if (ext == ".wav")
			ResPath.push_back(wstring(L"sounds//") + wstring(File));
		else if (ext == ".hlsl" || ext == ".fx")
			ResPath.push_back(wstring(L"shaders//") + wstring(File));
		return ResPath;
	}
}

vector<wstring> File_system::getFilesInFolder(wstring *File, bool Recursive, bool onlyFile)
{
	vector<wstring> files;
	auto ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//") 
		+ wstring(File->c_str());
	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto Str = it->path().wstring();
			replace(Str.begin(), Str.end(), '/', '\\');
			files.push_back(Str);
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto Str = it->path().wstring();
			replace(Str.begin(), Str.end(), '/', '\\');
			files.push_back(Str);
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto Str = it->path().filename().wstring();
			replace(Str.begin(), Str.end(), '/', '\\');
			files.push_back(Str);
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto Str = it->path().wstring();
			replace(Str.begin(), Str.end(), '/', '\\');
			files.push_back(Str);
		}
	return files;
}

vector<wstring> File_system::getFilesInFolder(wstring *File)
{
	vector<wstring> files;
	auto ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//")
		+ wstring(File->c_str());
	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto Str = it->path().wstring();
		replace(Str.begin(), Str.end(), '/', '\\');
		files.push_back(Str);
	}
	return files;
}
