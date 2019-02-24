#include "pch.h"

#include "File_system.h"

using namespace Engine;

File_system::File_system()
{
	p = _wgetcwd(NULL, 512);
	if (p.empty())
	{
		DebugTrace("File_system: Error getting path.\n");
		throw exception("p == empty!!!");
	}
	ScanFilesInRes();
}

void File_system::ScanFilesInRes()
{
	USES_CONVERSION;
	int i = 0;
	auto file = getFilesInFolder(&string(""), true, true);
	for (i = 0; i < file.size(); i++)
	{
		string someFile = file.at(i),
		 Fname = path(someFile).filename().string(),
		 ext = extension(someFile);
		TYPE type;

		if (ext == ".obj")
			type = TYPE::MODELS;
		else if (ext == ".hlsl" || ext == ".fx" || ext == ".vs" || ext == ".ps")
			type = TYPE::SHADERS;
		else if (ext == ".dds" || ext == ".png" || ext == ".bmp" || ext == ".mtl")
			type = TYPE::TEXTURES;
		else if (ext == ".wav")
			type = TYPE::SOUNDS;
		else if (ext == ".xml")
		{
			if (FindSubStr(path(file.at(i)).string(), string("UI")))
				type = TYPE::UIS;
			if (FindSubStr(path(file.at(i)).string(), string("maps")))
				type = TYPE::LEVELS;
			else
				type = TYPE::DIALOGS;
		}
		else
			type = TYPE::NONE;

		if (type != TYPE::NONE)
		{
			Files.push_back(new File(someFile, ext, Fname, (size_t)file_size(path(someFile)), type));
			Files.back()->ExtW = path(A2W(someFile.c_str())).extension().wstring();
			Files.back()->FileW = path(someFile).filename().wstring();
			Files.back()->PathW = wstring(A2W(someFile.c_str()));
		}
	}
}

File_system::File *File_system::GetFile(string file)
{
	USES_CONVERSION;

		// We make sure that file doesn't exist in our massive
	for (int i = 0; i < Files.size(); i++)
	{
		if (!Files.at(i)->FileA.compare(file.c_str()))
			return Files.at(i);
		if (!Files.at(i)->FileW.compare(A2W(file.c_str())))
			return Files.at(i);
	}

	if (!p.empty())
	{
		string ResPath = p.generic_path().generic_string() + string("/resource/");
		string extA = extension(file.c_str());
		wstring extW = path(A2W(file.c_str())).extension().wstring();

		to_lower(extA);

		if (!ResPath.empty())
		{
			if (extA == ".obj")
			{
				auto cache = getFilesInFolder(&string(ResPath + string("models/")), true, true);
				for (int i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.push_back(new File(cache.at(i), filePath, extA, file_size(cache.at(i)), TYPE::MODELS));
						Files.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->ExtW = extW;
						return Files.back();
					}
				}
			}
			else if (extA == ".dds" || extA == ".png" || extA == ".bmp")
			{
				auto cache = getFilesInFolder(&string(ResPath + string("textures/")), true, true);
				for (int i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.push_back(new File(cache.at(i), filePath, extA, file_size(cache.at(i)), TYPE::TEXTURES));
						Files.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->ExtW = extW;
						return Files.back();
					}
				}
			}
			else if (extA == ".hlsl" || extA == ".fx" || extA == ".vs" || extA == ".ps")
			{
				auto cache = getFilesInFolder(&string(ResPath + string("shaders/")), true, true);
				for (int i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.push_back(new File(cache.at(i), filePath, extA, file_size(cache.at(i)), TYPE::SHADERS));
							Files.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->ExtW = extW;
							return Files.back();
						}
					}
				}
			}
			else if (extA == ".wav")
			{
				auto cache = getFilesInFolder(&string(ResPath + string("sounds/")), true, true);
				for (int i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.push_back(new File(cache.at(i), filePath, extA, file_size(cache.at(i)), TYPE::TEXTURES));
						Files.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->ExtW = extW;
						return Files.back();
					}
				}
			}
			else if (extA == ".xml")
			{
					auto cache = getFilesInFolder(&string(ResPath + string("text/")), true, true);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							auto filePath = path(cache.at(i)).filename().string();
							if (filePath == string(file))
							{
								Files.push_back(new File(cache.at(i), filePath, extA, file_size(cache.at(i)), TYPE::DIALOGS));
								Files.back()->PathW = path(cache.at(i)).wstring();
								Files.back()->FileW = path(cache.at(i)).filename().wstring();
								Files.back()->ExtW = extW;
								return Files.back();
							}
						}
					}
					cache = getFilesInFolder(&string(ResPath + string("UI/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							auto filePath = path(cache.at(i)).filename().string();
							if (filePath == string(file))
							{
								Files.push_back(new File(cache.at(i), filePath, extA, (size_t)file_size(cache.at(i)), TYPE::UIS));
								Files.back()->PathW = path(cache.at(i)).wstring();
								Files.back()->FileW = path(cache.at(i)).filename().wstring();
								Files.back()->ExtW = extW;
								return Files.back();
							}
						}
					}
					cache = getFilesInFolder(&string(ResPath + string("maps/")), true, false);
					for (int i = 0; i < cache.size(); i++)
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.push_back(new File(cache.at(i), filePath, extA, file_size(cache.at(i)), TYPE::LEVELS));
							Files.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->ExtW = extW;
							return Files.back();
						}
					}
			}
		}
	}
	else
	{
		DebugTrace("File System: GetResPathA failed.\n");
		throw exception("ResPath == empty!!!");
	}
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
			if (is_directory(str))
				files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if(!is_directory(str))
				files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (!is_directory(str))
				files.push_back(replaceAll(str, string("\\"), string("/"), string("//")));
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (is_directory(str))
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
vector<string> File_system::getFilesInFolder(LPCSTR ext)
{
	vector<string> files;
	/*string ResPath;

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
	*/

	for (int i = 0; i < Files.size(); i++)
	{
		if (Files.at(i)->ExtA == ext)
			files.push_back(Files.at(i)->PathA);
	}

	return files;
}

string File_system::getDataFromFile(string *File, bool LineByline, string start, string end)
{
	if (File->empty())
		return "";

	string Returned_val, Cache;
	auto streamObj = std::ifstream(File->c_str());
	if (streamObj.is_open())
	{
		if (extension(File->c_str()) == ".xml")
		{
			while (!streamObj.eof())
			{
				getline(streamObj, Cache);
				Returned_val.append(deleteWord(Cache, start, end.c_str()));
			}

			if (!Returned_val.empty())
				return Returned_val;
		}

		if (LineByline)
			while (!streamObj.eof())
			{
				getline(streamObj, Cache);
				Returned_val.append(deleteWord(Cache, start, end.c_str()));
			}
		else
			while (!streamObj.eof())
				streamObj >> Returned_val;
	}
	else
	{
		DebugTrace("File System: getDataFromFile failed.\n");
		throw exception("streamObj == NOT OPEN!!!");
		return "";
	}

	if (!Returned_val.empty())
		return Returned_val;

	return "";
}
string File_system::getDataFromFile(string *File, bool LineByline)
{
	if (File->empty())
		return "";

	string Returned_val, Cache;
	auto streamObj = std::ifstream(File->c_str());
	if (streamObj.is_open())
	{
		if (LineByline)
			while (!streamObj.eof())
			{
				getline(streamObj, Cache);
				Returned_val.append(Cache);
			}
		else
			while (!streamObj.eof())
				streamObj >> Returned_val;
	}
	else
	{
		DebugTrace("File System: getDataFromFile failed.\n");
		throw exception("streamObj == NOT OPEN!!!");
		return "";
	}

	if (!Returned_val.empty())
		return Returned_val;

	return "";
}

vector<string> File_system::getDataFromFileVector(string *File, bool LineByline)
{
	vector<string> Returned_val;
	string Cache;
	auto streamObj = std::ifstream(File->c_str());
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