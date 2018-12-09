#include "pch.h"
#include "File_system.h"

File_system::File_system()
{
	p = _wgetcwd(NULL, 512);
	if (p.empty())
	{
		DebugTrace("File_system: Error getting path. Line: 7\n");
		throw exception("p == empty!!!");
	}
}

string *File_system::GetResPathA(string *File)
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
				return File;
			}
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
			{
				*File = ResPath + string("textures//") + string(*File);
				return File;
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				*File = ResPath + string("shaders//") + string(*File);
				return File;
			}
			else if (ext == ".obj")
			{
				*File = ResPath + string("sounds//") + string(*File);
				return File;
			}
			else
			{
				*File = "Unsupported File!";
				return File;
			}
		}
		else
		{
			DebugTrace("File System: GetResPathA failed. Line: 20\n");
			throw exception("ResPath == empty!!!");
			return &string("");
		}

	}
	else
		return &string("");
}

wstring *File_system::GetResPathW(wstring *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".obj")
			{
				*File = ResPath + wstring(L"models//") + wstring(*File);
				return File;
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				*File = ResPath + wstring(L"shaders//") + wstring(*File);
				return File;
			}
			else if (ext == ".png" || ext == ".dds" || ext == ".jpg" || ext == ".bmp")
			{
				*File = ResPath + wstring(L"textures//") + wstring(*File);
				return File;
			}
			else if (ext == ".wav")
			{
				*File = ResPath + wstring(L"sounds//") + wstring(*File);
				return File;
			}
			else
			{
				*File = wstring(L"Unsupported File!");
				return File;
			}
		}
		else
		{
			DebugTrace("File System: GetResPathA failed. Line: 62\n");
			throw exception("ResPath == empty!!!");
			return &wstring(L"");
		}
	}
	else
		return &wstring(L"");
}

wstring File_system::GetResPathW(string *File)
{
	USES_CONVERSION;
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_string() + string("//resource//");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".obj")
			{
				*File = ResPath + string("models//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
			{
				*File = ResPath + string("textures//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				*File = ResPath + string("shaders//") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".wav")
			{
				*File = ResPath + string("sounds//") + *File;
				return A2W(File->c_str());
			}
			else
			{
				File = &string("Unsupported File!");
				return A2W(File->c_str());
			}
		}
		else
		{
			DebugTrace("File System: GetResPathA failed. Line: 108\n");
			throw exception("ResPath == empty!!!");
			return L"";
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
			if (!ResPath[i].data()->empty())
			if (ext == ".obj")
				ResPath[i].at(i).append(wstring(L"models//") + wstring(*File[i]->data()));
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
				ResPath[i].at(i).append(wstring(L"textures//") + wstring(File[i]->data()->c_str()));
			else if (ext == ".wav")
				ResPath[i].at(i).append(wstring(L"sounds//") + wstring(File[i]->data()->c_str()));
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
				ResPath[i].at(i).append(wstring(L"shaders//") + wstring(File[i]->data()->c_str()));
			return ResPath[sizeof(*File)];
		}
	}
}

vector<wstring> File_system::GetResPathW(wstring File)
{
	vector<wstring> file;
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"//resource//");
		auto ext = boost::filesystem::extension(File.c_str());

		if (!ResPath.empty())
		if (ext == ".obj")
			file.push_back(ResPath + wstring(L"models//") + wstring(File));
		else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
			file.push_back(ResPath + wstring(L"textures//") + wstring(File));
		else if (ext == ".wav")
			file.push_back(ResPath + wstring(L"sounds//") + wstring(File));
		else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			file.push_back(ResPath + wstring(L"shaders//") + wstring(File));
		return file;
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
