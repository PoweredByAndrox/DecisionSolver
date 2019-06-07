#include "pch.h"

#include "File_system.h"

File_system::File_system()
{
	p = _wgetcwd(NULL, 512);
	if (p.empty())
	{
		DebugTrace("File_system: Error get resources folder\n");
		throw exception("p == empty!!!");
	}
	ScanFiles();
}

void File_system::ScanFiles()
{
	USES_CONVERSION;

	if (Files.empty())
		Files.push_back(make_shared<AllFile>());

	int i = 0;
	auto file = getFilesInFolder("", true, true);
	for (size_t i = 0; i < file.size(); i++)
	{
		string someFile = file.at(i),
			ext = extension(someFile);
		auto Fname = path(someFile);
		auto type = GetTypeFile(someFile);
		switch (type)
		{
		case MODELS:
			Files.back()->Models.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Models.back()->ExtW = Fname.extension().wstring();
			Files.back()->Models.back()->FileW = Fname.filename().wstring();
			Files.back()->Models.back()->PathW = Fname.wstring().c_str();
			break;
		case TEXTURES:
			Files.back()->Textures.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Textures.back()->ExtW = Fname.extension().wstring();
			Files.back()->Textures.back()->FileW = Fname.filename().wstring();
			Files.back()->Textures.back()->PathW = Fname.wstring().c_str();
			break;
		case LEVELS:
			Files.back()->Levels.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Levels.back()->ExtW = Fname.extension().wstring();
			Files.back()->Levels.back()->FileW = Fname.filename().wstring();
			Files.back()->Levels.back()->PathW = Fname.wstring().c_str();
			break;
		case DIALOGS:
			Files.back()->Dialogs.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Dialogs.back()->ExtW = Fname.extension().wstring();
			Files.back()->Dialogs.back()->FileW = Fname.filename().wstring();
			Files.back()->Dialogs.back()->PathW = Fname.wstring().c_str();
			break;
		case SOUNDS:
			Files.back()->Sounds.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Sounds.back()->ExtW = Fname.extension().wstring();
			Files.back()->Sounds.back()->FileW = Fname.filename().wstring();
			Files.back()->Sounds.back()->PathW = Fname.wstring().c_str();
			break;
		case SHADERS:
			Files.back()->Shaders.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Shaders.back()->ExtW = Fname.extension().wstring();
			Files.back()->Shaders.back()->FileW = Fname.filename().wstring();
			Files.back()->Shaders.back()->PathW = Fname.wstring().c_str();
			break;
		case UIS:
			Files.back()->Uis.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Uis.back()->ExtW = Fname.extension().wstring();
			Files.back()->Uis.back()->FileW = Fname.filename().wstring();
			Files.back()->Uis.back()->PathW = Fname.wstring().c_str();
			break;
		case SCRIPTS:
			Files.back()->Scripts.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Scripts.back()->ExtW = Fname.extension().wstring();
			Files.back()->Scripts.back()->FileW = Fname.filename().wstring();
			Files.back()->Scripts.back()->PathW = Fname.wstring().c_str();
			break;
		case FONTS:
			Files.back()->Fonts.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->Fonts.back()->ExtW = Fname.extension().wstring();
			Files.back()->Fonts.back()->FileW = Fname.filename().wstring();
			Files.back()->Fonts.back()->PathW = Fname.wstring().c_str();
			break;
		default:
			Files.back()->None.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files.back()->None.back()->ExtW = Fname.extension().wstring();
			Files.back()->None.back()->FileW = Fname.filename().wstring();
			Files.back()->None.back()->PathW = Fname.wstring().c_str();
			break;
		}
	}
}

_TypeOfFile File_system::GetTypeFile(string file)
{
	auto F = path(file);
	if (F.extension().string() == ".obj")
		return MODELS;
	else if (F.extension().string() == ".hlsl" || F.extension().string() == ".fx"
		|| F.extension().string() == ".vs" || F.extension().string() == ".ps")
		return SHADERS;
	else if (F.extension().string() == ".dds" || F.extension().string() == ".png"
		|| F.extension().string() == ".bmp" || F.extension().string() == ".mtl")
		return TEXTURES;
	else if (F.extension().string() == ".wav")
		return SOUNDS;
	else if (F.extension().string() == ".lua")
		return SCRIPTS;
	else if (F.extension().string() == ".ttf")
		return FONTS;
	else if (F.extension().string() == ".xml")
	{
		if (FindSubStr(F.string(), string("UI")))
			return UIS;
		if (FindSubStr(F.string(), string("maps")))
			return LEVELS;
		else
			return DIALOGS;
	}
	else
		return NONE;
}

shared_ptr<File_system::AllFile::File> File_system::GetFileByType(string file)
{
	USES_CONVERSION;
	to_lower(file);

	switch (GetTypeFile(file))
	{
	case MODELS:
	{
		for (size_t i = 0; i < Files.back()->Models.size(); i++)
		{
			auto LowerA = Files.back()->Models.at(i)->FileA;
			auto LowerW = Files.back()->Models.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Models.at(i);
		}

		break;
	}
	case SHADERS:
	{
		for (size_t i = 0; i < Files.back()->Shaders.size(); i++)
		{
			auto LowerA = Files.back()->Shaders.at(i)->FileA;
			auto LowerW = Files.back()->Shaders.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Shaders.at(i);
		}

		break;
	}
	case TEXTURES:
	{
		for (size_t i = 0; i < Files.back()->Textures.size(); i++)
		{
			auto LowerA = Files.back()->Textures.at(i)->FileA;
			auto LowerW = Files.back()->Textures.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Textures.at(i);
		}

		break;
	}
	case SOUNDS:
	{
		for (size_t i = 0; i < Files.back()->Sounds.size(); i++)
		{
			auto LowerA = Files.back()->Sounds.at(i)->FileA;
			auto LowerW = Files.back()->Sounds.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Sounds.at(i);
		}

		break;
	}
	case SCRIPTS:
	{
		for (size_t i = 0; i < Files.back()->Scripts.size(); i++)
		{
			auto LowerA = Files.back()->Scripts.at(i)->FileA;
			auto LowerW = Files.back()->Scripts.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Scripts.at(i);
		}

		break;
	}
	case FONTS:
	{
		for (size_t i = 0; i < Files.back()->Fonts.size(); i++)
		{
			auto LowerA = Files.back()->Fonts.at(i)->FileA;
			auto LowerW = Files.back()->Fonts.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Fonts.at(i);
		}

		break;
	}
		// XML Files
	case DIALOGS:
	case UIS:
	{
		for (size_t i = 0; i < Files.back()->Dialogs.size(); i++)
		{
			auto LowerA = Files.back()->Dialogs.at(i)->FileA;
			auto LowerW = Files.back()->Dialogs.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Dialogs.at(i);
		}
		for (size_t i = 0; i < Files.back()->Uis.size(); i++)
		{
			auto LowerA = Files.back()->Uis.at(i)->FileA;
			auto LowerW = Files.back()->Uis.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Uis.at(i);
		}

		break;
	}
	case LEVELS:
	{
		for (size_t i = 0; i < Files.back()->Levels.size(); i++)
		{
			auto LowerA = Files.back()->Levels.at(i)->FileA;
			auto LowerW = Files.back()->Levels.at(i)->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Levels.at(i);
		}

		break;
	}
	}

	return make_shared<AllFile::File>();
}
vector<shared_ptr<File_system::AllFile::File>> File_system::GetFileByType(_TypeOfFile T)
{
	switch (T)
	{
	case MODELS:
		return Files.back()->Models;
	case TEXTURES:
		return Files.back()->Textures;
	case LEVELS:
		return Files.back()->Levels;
	case DIALOGS:
		return Files.back()->Dialogs;
	case SOUNDS:
		return Files.back()->Sounds;
	case SHADERS:
		return Files.back()->Shaders;
	case UIS:
		return Files.back()->Uis;
	case SCRIPTS:
		return Files.back()->Scripts;
	case FONTS:
		return Files.back()->Fonts;
	case NONE:
		return vector<shared_ptr<File_system::AllFile::File>>{make_unique<File_system::AllFile::File>()};
	}
}

shared_ptr<File_system::AllFile::File> File_system::GetFile(string file)
{
	USES_CONVERSION;

		// We make sure that file doesn't exist in our massive
	auto F = GetFileByType(file);
	if (!F->FileA.empty() || !F->FileW.empty())
		return F;

	if (!p.empty())
	{
		string ResPath = p.generic_path().generic_string() + string("/resource/");
		string extA = extension(file.c_str());
		wstring extW = path(file.c_str()).extension().wstring();

		to_lower(extA);

		if (!ResPath.empty())
		{
			if (extA == ".obj")
			{
				auto cache = getFilesInFolder(string(ResPath + string("models/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Models.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
							(size_t)file_size(cache.at(i)), MODELS));
						Files.back()->Models.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Models.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Models.back()->ExtW = extW;
						return Files.back()->Models.back();
					}
				}
			}
			else if (extA == ".dds" || extA == ".png" || extA == ".bmp")
			{
				auto cache = getFilesInFolder(string(ResPath + string("textures/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Textures.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
							(size_t)file_size(cache.at(i)), TEXTURES));
						Files.back()->Textures.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Textures.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Textures.back()->ExtW = extW;
						return Files.back()->Textures.back();
					}
				}
			}
			else if (extA == ".hlsl" || extA == ".fx" || extA == ".vs" || extA == ".ps")
			{
				auto cache = getFilesInFolder(string(ResPath + string("shaders/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.back()->Shaders.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
								(size_t)file_size(cache.at(i)), SHADERS));
							Files.back()->Shaders.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->Shaders.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->Shaders.back()->ExtW = extW;
							return Files.back()->Shaders.back();
						}
					}
				}
			}
			else if (extA == ".wav")
			{
				auto cache = getFilesInFolder(string(ResPath + string("sounds/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Textures.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
							(size_t)file_size(cache.at(i)), TEXTURES));
						Files.back()->Textures.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Textures.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Textures.back()->ExtW = extW;
						return Files.back()->Textures.back();
					}
				}
			}
			else if (extA == ".xml")
			{
				auto cache = getFilesInFolder(string(ResPath + string("text/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.back()->Dialogs.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
								(size_t)file_size(cache.at(i)), DIALOGS));
							Files.back()->Dialogs.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->Dialogs.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->Dialogs.back()->ExtW = extW;
							return Files.back()->Dialogs.back();
						}
					}
				}
				cache = getFilesInFolder(string(ResPath + string("UI/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.back()->Uis.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
								(size_t)file_size(cache.at(i)), UIS));
							Files.back()->Uis.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->Uis.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->Uis.back()->ExtW = extW;
							return Files.back()->Uis.back();
						}
					}
				}
				cache = getFilesInFolder(string(ResPath + string("maps/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Levels.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
							(size_t)file_size(cache.at(i)), LEVELS));
						Files.back()->Levels.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Levels.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Levels.back()->ExtW = extW;
						return Files.back()->Levels.back();
					}
				}
			}
			else if (extA == ".lua")
			{
				auto cache = getFilesInFolder(string(ResPath + string("scripts/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Scripts.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
							(size_t)file_size(cache.at(i)), SCRIPTS));
						Files.back()->Scripts.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Scripts.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Scripts.back()->ExtW = extW;
						return Files.back()->Scripts.back();
					}
				}
			}
			else if (extA == ".ttf")
			{
				auto cache = getFilesInFolder(string(ResPath + string("fonts/")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Fonts.push_back(make_shared<AllFile::File>(cache.at(i), filePath, extA,
							(size_t)file_size(cache.at(i)), FONTS));
						Files.back()->Fonts.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Fonts.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Fonts.back()->ExtW = extW;
						return Files.back()->Fonts.back();
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

	ToDo("Add Processing Errors!")
	return make_shared<File_system::AllFile::File>();
}

vector<wstring> File_system::getFilesInFolder(wstring Folder, bool Recursive, bool onlyFile, _TypeOfFile type)
{
	vector<wstring> files;
	wstring ResPath;

	if (type != NONE)
		switch (type)
		{
		case MODELS:
			for (size_t i = 0; i < Files.back()->Models.size(); i++)
			{
				files.push_back(Files.back()->Models.at(i)->FileW);
			}
			break;
		case TEXTURES:
			for (size_t i = 0; i < Files.back()->Textures.size(); i++)
			{
				files.push_back(Files.back()->Textures.at(i)->FileW);
			}
			break;
		case LEVELS:
			for (size_t i = 0; i < Files.back()->Levels.size(); i++)
			{
				files.push_back(Files.back()->Levels.at(i)->FileW);
			}
			break;
		case DIALOGS:
			for (size_t i = 0; i < Files.back()->Dialogs.size(); i++)
			{
				files.push_back(Files.back()->Dialogs.at(i)->FileW);
			}
			break;
		case SOUNDS:
			for (size_t i = 0; i < Files.back()->Sounds.size(); i++)
			{
				files.push_back(Files.back()->Sounds.at(i)->FileW);
			}
			break;
		case SHADERS:
			for (size_t i = 0; i < Files.back()->Shaders.size(); i++)
			{
				files.push_back(Files.back()->Shaders.at(i)->FileW);
			}
			break;
		case UIS:
			for (size_t i = 0; i < Files.back()->Uis.size(); i++)
			{
				files.push_back(Files.back()->Uis.at(i)->FileW);
			}
			break;
		case SCRIPTS:
			for (size_t i = 0; i < Files.back()->Scripts.size(); i++)
			{
				files.push_back(Files.back()->Scripts.at(i)->FileW);
			}
			break;
		case FONTS:
			for (size_t i = 0; i < Files.back()->Fonts.size(); i++)
			{
				files.push_back(Files.back()->Fonts.at(i)->FileW);
			}
			break;
		}

	if (wcsstr(Folder.c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"/resource/")).c_str()) != NULL)
		ResPath = wstring(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/") + wstring(Folder.c_str());	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//"));
			files.push_back(str);
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//"));
			files.push_back(str);
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//"));
			files.push_back(str);
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().wstring();
			replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//"));
			files.push_back(str);
		}

	return files;
}
vector<wstring> File_system::getFilesInFolder(wstring Folder, _TypeOfFile type)
{
	vector<wstring> files;
	wstring ResPath;

	if (type != NONE)
		switch (type)
		{
		case MODELS:
			for (size_t i = 0; i < Files.back()->Models.size(); i++)
			{
				files.push_back(Files.back()->Models.at(i)->FileW);
			}
			break;
		case TEXTURES:
			for (size_t i = 0; i < Files.back()->Textures.size(); i++)
			{
				files.push_back(Files.back()->Textures.at(i)->FileW);
			}
			break;
		case LEVELS:
			for (size_t i = 0; i < Files.back()->Levels.size(); i++)
			{
				files.push_back(Files.back()->Levels.at(i)->FileW);
			}
			break;
		case DIALOGS:
			for (size_t i = 0; i < Files.back()->Dialogs.size(); i++)
			{
				files.push_back(Files.back()->Dialogs.at(i)->FileW);
			}
			break;
		case SOUNDS:
			for (size_t i = 0; i < Files.back()->Sounds.size(); i++)
			{
				files.push_back(Files.back()->Sounds.at(i)->FileW);
			}
			break;
		case SHADERS:
			for (size_t i = 0; i < Files.back()->Shaders.size(); i++)
			{
				files.push_back(Files.back()->Shaders.at(i)->FileW);
			}
			break;
		case UIS:
			for (size_t i = 0; i < Files.back()->Uis.size(); i++)
			{
				files.push_back(Files.back()->Uis.at(i)->FileW);
			}
			break;
		case SCRIPTS:
			for (size_t i = 0; i < Files.back()->Scripts.size(); i++)
			{
				files.push_back(Files.back()->Scripts.at(i)->FileW);
			}
			break;
		case FONTS:
			for (size_t i = 0; i < Files.back()->Fonts.size(); i++)
			{
				files.push_back(Files.back()->Fonts.at(i)->FileW);
			}
			break;
		}

	if (wcsstr(Folder.c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"/resource/")).c_str()) != NULL)
		ResPath = wstring(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"/resource/") + wstring(Folder.c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path().wstring();
		{
			replaceAll(str, wstring(L"\\"), wstring(L"/"), wstring(L"//"));
			files.push_back(str);
		}
	}

	return files;
}
vector<string> File_system::getFilesInFolder(string Folder, bool Recursive, bool onlyFile, _TypeOfFile type)
{
	vector<string> files;
	string ResPath;

	if (type != NONE)
		switch (type)
		{
		case MODELS:
			for (size_t i = 0; i < Files.back()->Models.size(); i++)
			{
				files.push_back(Files.back()->Models.at(i)->FileA);
			}
			break;
		case TEXTURES:
			for (size_t i = 0; i < Files.back()->Textures.size(); i++)
			{
				files.push_back(Files.back()->Textures.at(i)->FileA);
			}
			break;
		case LEVELS:
			for (size_t i = 0; i < Files.back()->Levels.size(); i++)
			{
				files.push_back(Files.back()->Levels.at(i)->FileA);
			}
			break;
		case DIALOGS:
			for (size_t i = 0; i < Files.back()->Dialogs.size(); i++)
			{
				files.push_back(Files.back()->Dialogs.at(i)->FileA);
			}
			break;
		case SOUNDS:
			for (size_t i = 0; i < Files.back()->Sounds.size(); i++)
			{
				files.push_back(Files.back()->Sounds.at(i)->FileA);
			}
			break;
		case SHADERS:
			for (size_t i = 0; i < Files.back()->Shaders.size(); i++)
			{
				files.push_back(Files.back()->Shaders.at(i)->FileA);
			}
			break;
		case UIS:
			for (size_t i = 0; i < Files.back()->Uis.size(); i++)
			{
				files.push_back(Files.back()->Uis.at(i)->FileA);
			}
			break;
		case SCRIPTS:
			for (size_t i = 0; i < Files.back()->Scripts.size(); i++)
			{
				files.push_back(Files.back()->Scripts.at(i)->FileA);
			}
			break;
		case FONTS:
			for (size_t i = 0; i < Files.back()->Fonts.size(); i++)
			{
				files.push_back(Files.back()->Fonts.at(i)->FileA);
			}
			break;
		}

	if (strstr(Folder.c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(Folder.c_str());	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (is_directory(str))
			{
				replaceAll(str, string("\\"), string("/"), string("//"));
				files.push_back(str);
			}
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if(!is_directory(str))
			{
				replaceAll(str, string("\\"), string("/"), string("//"));
				files.push_back(str);
			}
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (!is_directory(str))
			{
				replaceAll(str, string("\\"), string("/"), string("//"));
				files.push_back(str);
			}
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (is_directory(str))
			{
				replaceAll(str, string("\\"), string("/"), string("//"));
				files.push_back(str);
			}
		}

	return files;
}
vector<string> File_system::getFilesInFolder(string Folder, _TypeOfFile type)
{
	vector<string> files;
	string ResPath;

	if (type != NONE)
		switch (type)
		{
		case MODELS:
			for (size_t i = 0; i < Files.back()->Models.size(); i++)
			{
				files.push_back(Files.back()->Models.at(i)->FileA);
			}
			break;
		case TEXTURES:
			for (size_t i = 0; i < Files.back()->Textures.size(); i++)
			{
				files.push_back(Files.back()->Textures.at(i)->FileA);
			}
			break;
		case LEVELS:
			for (size_t i = 0; i < Files.back()->Levels.size(); i++)
			{
				files.push_back(Files.back()->Levels.at(i)->FileA);
			}
			break;
		case DIALOGS:
			for (size_t i = 0; i < Files.back()->Dialogs.size(); i++)
			{
				files.push_back(Files.back()->Dialogs.at(i)->FileA);
			}
			break;
		case SOUNDS:
			for (size_t i = 0; i < Files.back()->Sounds.size(); i++)
			{
				files.push_back(Files.back()->Sounds.at(i)->FileA);
			}
			break;
		case SHADERS:
			for (size_t i = 0; i < Files.back()->Shaders.size(); i++)
			{
				files.push_back(Files.back()->Shaders.at(i)->FileA);
			}
			break;
		case UIS:
			for (size_t i = 0; i < Files.back()->Uis.size(); i++)
			{
				files.push_back(Files.back()->Uis.at(i)->FileA);
			}
			break;
		case SCRIPTS:
			for (size_t i = 0; i < Files.back()->Scripts.size(); i++)
			{
				files.push_back(Files.back()->Scripts.at(i)->FileA);
			}
			break;
		case FONTS:
			for (size_t i = 0; i < Files.back()->Fonts.size(); i++)
			{
				files.push_back(Files.back()->Fonts.at(i)->FileA);
			}
			break;
		}

	if (strstr(Folder.c_str(), string(p.generic_path().generic_string() + string("/resource/")).c_str()) != NULL)
		ResPath = string(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("/resource/") + string(Folder.c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		auto str = it->path().string();
		replaceAll(str, string("\\"), string("/"), string("//"));
		files.push_back(str);
	}

	return files;
}

string File_system::getDataFromFile(string File, bool LineByline, string start, string end)
{
	if (File.empty())
		return "";

	string Returned_val;
	std::ifstream streamObj = std::ifstream(File.c_str());
	streamObj >> noskipws;
	if (streamObj.is_open())
	{
		copy(istream_iterator<char>(streamObj), istream_iterator<char>(), back_inserter(Returned_val));

		if (!Returned_val.empty())
			if (!start.empty() & !end.empty())
			{
				deleteWord(Returned_val, start, end);
				return Returned_val;
			}
			else
				return Returned_val;
		else
			return "";
	}
	else
	{
		DebugTrace("File System::getDataFromFile() failed.\n");
		throw exception("streamObj == NOT OPEN!!!");
		return "";
	}

	return "";
}

vector<string> File_system::getDataFromFileVector(string File, bool LineByline)
{
	vector<string> Returned_val;
	string Cache;
	auto streamObj = std::ifstream(File.c_str());
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
	fopen_s(&stream, filename, "rb");
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