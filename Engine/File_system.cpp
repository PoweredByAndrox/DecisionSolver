#include "pch.h"

#include "File_system.h"

File_system::File_system()
{
	p = _wgetcwd(NULL, 512);
	if (p.empty())
	{
		DebugTrace("File_system: Error getting path.\n");
		throw exception("p == empty!!!");
	}
}

string *File_system::GetResPathA(string *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_string() + string("/resource/");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".obj")
			{
				if (!boost::filesystem::exists(ResPath + string("models/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("models/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("models/") + string(*File);
			}
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
			{

				if (!boost::filesystem::exists(ResPath + string("textures/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("textures/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("textures/") + string(*File);
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				if (!boost::filesystem::exists(ResPath + string("shaders/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("shaders/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("shaders/") + string(*File);
			}

			else if (ext == ".wav")
			{
				if (!boost::filesystem::exists(ResPath + string("sounds/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("sounds/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("sounds/") + string(*File);
			}
			else if (ext == ".xml")
			{
				if (!boost::filesystem::exists(ResPath + string("text/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("text/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("text/") + string(*File);
			}
			else
			{
				*File = "Unsupported File!";
			}

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

wstring *File_system::GetResPathW(wstring *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".obj")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"models/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"models/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + wstring(L"models/") + wstring(*File);
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"shaders/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"shaders/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + wstring(L"shaders/") + wstring(*File);
			}
			else if (ext == ".png" || ext == ".dds" || ext == ".jpg" || ext == ".bmp")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"textures/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"textures/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + wstring(L"textures/") + wstring(*File);
			}
			else if (ext == ".wav")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"sounds/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"sounds/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + wstring(L"sounds/") + wstring(*File);
			}
			else if (ext == ".xml")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"text/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"text/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + wstring(L"text/") + wstring(*File);
			}
			else
			{
				*File = wstring(L"Unsupported File!");
			}
			return File;
		}
		else
		{
			DebugTrace("File System: GetResPathA failed\n");
			throw exception("p == empty!!!");
			return &wstring(L"");
		}
	}
	else
	{
		DebugTrace("File System: GetResPathA failed\n");
		throw exception("ResPath == empty!!!");
		return &wstring(L"");
	}
}

wstring File_system::GetResPathW(string *File)
{
	USES_CONVERSION;
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_string() + string("/resource/");
		auto ext = boost::filesystem::extension(File->c_str());
		if (!ResPath.empty())
		{
			if (ext == ".obj")
			{
				if (!boost::filesystem::exists(ResPath + string("models/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("models/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("models/") + *File;
			}
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
			{
				if (!boost::filesystem::exists(ResPath + string("textures/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("textures/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("textures/") + *File;
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				if (!boost::filesystem::exists(ResPath + string("shaders/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("shaders/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("shaders/") + *File;
				return A2W(File->c_str());
			}
			else if (ext == ".wav")
			{
				if (!boost::filesystem::exists(ResPath + string("sounds/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("sounds/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == string(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + string("sounds/") + *File;
			}
			else
				File = &string("Unsupported File!");

			return A2W(File->c_str());
		}
		else
		{
			DebugTrace("File System: GetResPathA failed\n");
			throw exception("ResPath == empty!!!");
			return L"";
		}
	}
	else
	{
		DebugTrace("File System: GetResPathA failed\n");
		throw exception("p == empty!!!");
		return L"";
	}

}

vector<wstring> File_system::GetResPathW(vector<wstring> *File[])
{
	vector<wstring> ResPath[sizeof(File)];
	if (!p.empty())
	{
		for (int i = 0; i < sizeof(File); i++)
		{
			auto ext = boost::filesystem::extension(File[i]->data()->c_str());
			ResPath[i].insert(ResPath[i].end(), p.generic_path().generic_wstring() + wstring(L"/resource/"));
			if (!ResPath[i].data()->empty())
			if (ext == ".obj")
				ResPath[i].at(i).append(wstring(L"models/") + wstring(*File[i]->data()));
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
				ResPath[i].at(i).append(wstring(L"textures/") + wstring(File[i]->data()->c_str()));
			else if (ext == ".wav")
				ResPath[i].at(i).append(wstring(L"sounds/") + wstring(File[i]->data()->c_str()));
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
				ResPath[i].at(i).append(wstring(L"shaders/") + wstring(File[i]->data()->c_str()));
			return ResPath[sizeof(*File)];
		}
	}
}

vector<wstring> File_system::GetResPathW(wstring File)
{
	vector<wstring> file;
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/");
		auto ext = boost::filesystem::extension(File.c_str());

		if (!ResPath.empty())
		{
			if (ext == ".obj")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"models/") + wstring(File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"models/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(File))
						{
							file.push_back(cache.at(i));
							break;
						}
					}
				}
			}
			else if (ext == ".dds" || ext == ".png" || ext == ".jpg" || ext == ".bmp")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"textures/") + wstring(File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"textures/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(File))
						{
							file.push_back(cache.at(i));
							break;
						}
					}
				}
			}
			else if (ext == ".wav")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"sounds/") + wstring(File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"sounds/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(File))
						{
							file.push_back(cache.at(i));
							break;
						}
					}
				}
			}
			else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			{
				if (!boost::filesystem::exists(ResPath + wstring(L"shaders/") + wstring(File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"shaders/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().wstring();
						if (filename == wstring(File))
						{
							file.push_back(cache.at(i));
							break;
						}
					}
				}
			}

			return file;
		}
	}
}

wstring &File_system::replaceAll(wstring &context, wstring const &from, wstring const &to)
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
wstring &File_system::replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also)
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

vector<wstring> File_system::getFilesInFolder(wstring *File, bool Recursive, bool onlyFile)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(File->c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"/resource/")).c_str()) != NULL)
		ResPath = wstring(File->c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/") + wstring(File->c_str());	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			files.push_back(replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//")));
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			files.push_back(replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//")));
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			files.push_back(replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//")));
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			files.push_back(replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//")));
		}

	return files;
}
vector<wstring> File_system::getFilesInFolder(wstring *File)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(File->c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"/resource/")).c_str()) != NULL)
		ResPath = wstring(File->c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/") + wstring(File->c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path().wstring();
		files.push_back(replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//")));
	}

	return files;
}

string &File_system::replaceAll(string &context, string const &from, string const &to)
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
string &File_system::replaceAll(string &context, string const &from, string const &to, string const &also)
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

vector<string> File_system::getFilesInFolder(string *File, bool Recursive, bool onlyFile)
{
	vector<string> files;
	string ResPath;

	if (strstr(File->c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(File->c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(File->c_str());	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}

	return files;
}
vector<string> File_system::getFilesInFolder(string *File)
{
	vector<string> files;
	string ResPath;

	if (strstr(File->c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(File->c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(File->c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path().string();
		files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
	}

	return files;
}
/*
vector<wstring> File_system::getDataFromFile(wstring *File)
{
	USES_CONVERSION;

	auto FileName = GetResPathW(File)->c_str();
	auto streamObj = std::ifstream(FileName);
	streamObj.open(FileName);
	if (!streamObj.is_open())
	{
		throw exception("File_system::getDataFromFile() == nullptr");
		return nullptr;
	}
	else
	{
		string Returned_val;
		streamObj >> Returned_val;

		if (!Returned_val.empty())
			return &wstring(A2W(Returned_val.data()));
		else
			return nullptr;
	}
}
*/

string File_system::getDataFromFile(string *File)
{
	auto FileName = GetResPathA(File)->c_str();
	string Returned_val, Cache;
	auto streamObj = std::ifstream(FileName);

	while (!streamObj.eof())
	{
		getline(streamObj, Cache);
		Returned_val.append(Cache);
	}

	if (!Returned_val.empty())
		return Returned_val;
}
