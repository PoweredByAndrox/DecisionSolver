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
		to_lower(ext);

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

				if (!boost::filesystem::exists(ResPath + string("UI/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("UI/")), true, false);
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
					*File = ResPath + string("UI/") + string(*File);

				if (!boost::filesystem::exists(ResPath + string("maps/") + string(*File)))
				{
					auto cache = getFilesInFolder(&string(ResPath + string("maps/")), true, false);
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
					*File = ResPath + string("maps/") + string(*File);
			}
			else
				*File = "Unsupported File!";

			return File;
		}
	}
	else
	{
		DebugTrace("File System: GetResPathA failed.\n");
		throw exception("ResPath == empty!!!");
		return &string("");
	}

	return &string("Unsupported File!");
}

wstring *File_system::GetResPathW(wstring *File)
{
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/");
		auto ext = boost::filesystem::extension(File->c_str());
		to_lower(ext);
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

				if (!boost::filesystem::exists(ResPath + wstring(L"UI/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"UI/")), true, false);
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
					*File = ResPath + wstring(L"UI/") + wstring(*File);

				if (!boost::filesystem::exists(ResPath + wstring(L"maps/") + wstring(*File)))
				{
					auto cache = getFilesInFolder(&wstring(ResPath + wstring(L"maps/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filename = boost::filesystem::path(cache.at(i)).filename().string();
						if (filename == wstring(*File))
						{
							*File = cache.at(i);
							break;
						}
					}
				}
				else
					*File = ResPath + wstring(L"maps/") + wstring(*File);
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
		to_lower(ext);

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

	return vector<wstring>{wstring(L"Unsupported File!")};
}

vector<wstring> File_system::GetResPathW(wstring File)
{
	vector<wstring> file;
	if (!p.empty())
	{
		auto ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/");
		auto ext = boost::filesystem::extension(File.c_str());
		to_lower(ext);

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

	return vector<wstring>{wstring(L"Unsupported File!")};
}

vector<wstring> File_system::getFilesInFolder(wstring *Folder, bool Recursive, bool onlyFile)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder->c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"/resource/")).c_str()) != NULL)
		ResPath = wstring(Folder->c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/") + wstring(Folder->c_str());	// No!

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
vector<wstring> File_system::getFilesInFolder(wstring *Folder)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder->c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"/resource/")).c_str()) != NULL)
		ResPath = wstring(Folder->c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/") + wstring(Folder->c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path().wstring();
		files.push_back(replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//")));
	}

	return files;
}

vector<string> File_system::getFilesInFolder(string *Folder, bool Recursive, bool onlyFile)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder->c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(Folder->c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(Folder->c_str());	// No!

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
vector<string> File_system::getFilesInFolder(string *Folder)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder->c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(Folder->c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(Folder->c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path().string();
		files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
	}

	return files;
}
vector<string> File_system::getFilesInFolder(string *Folder, LPCSTR ext)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder->c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(Folder->c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(Folder->c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path();
		if (strstr(str.extension().string().c_str(), ext) != NULL)
			files.push_back(replaceAll(*const_cast<string*>(&str.string()), string("\\"), string("/"), string("//")));
	}

	return files;
}

string File_system::getDataFromFile(string *File, bool LineByline)
{
	auto FileName = GetResPathA(File)->c_str();
	string Returned_val, Cache;
	auto streamObj = std::ifstream(FileName);
	if (LineByline)
		while (!streamObj.eof())
		{
			getline(streamObj, Cache);
			Returned_val.append(Cache);
		}
	else
		while (!streamObj.eof())
			streamObj >> Returned_val;

	if (!Returned_val.empty())
		return Returned_val;

	return "";
}

vector<string> File_system::getDataFromFileVector(string *File, bool LineByline)
{
	auto FileName = GetResPathA(File)->c_str();
	vector<string> Returned_val;
	string Cache;
	auto streamObj = std::ifstream(FileName);
	if (LineByline)
		while (!streamObj.eof())
		{
			getline(streamObj, Cache);
			Returned_val.back().append(Cache);
		}
	else
		while (!streamObj.eof())
		{
			streamObj >> Cache;
			Returned_val.push_back(Cache);
		}

	if (!Returned_val.empty())
		return Returned_val;

	return vector<string> {""};
}

bool File_system::ReadFileMemory(LPCSTR filename, size_t *FileSize, UCHAR **FilePtr)
{
	FILE *stream;
	int retVal = 1;
	stream = fopen(filename, "rb");
	if (!stream)
		return false;

	fseek(stream, 0, SEEK_END);

	*FileSize = ftell(stream);
	fseek(stream, 0, SEEK_SET);
	if ((*FilePtr = (UCHAR *)malloc((*FileSize) + 1)) == nullptr)
		return false;
	else
		if (fread(*FilePtr, 1, *FileSize, stream) != (*FileSize))
			return false;

	fclose(stream);
	return true;
}