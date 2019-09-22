#include "pch.h"

#include "Console.h"
#include "File_system.h"

#include <lzoconf.h>
#include <lzo1x.h>
#include <lzo1y.h>

path File_system::p = "";
static shared_ptr<boost::filesystem::ofstream> LogFile;
path File_system::LogFName = "Engine.log";

//lzo_bytep in = nullptr, *out = nullptr;
//lzo_voidp wrkmem = nullptr;
//int best_compress = -1;
//lzo_uint orig_len = 0u, uncompressed_checksum = 0u, compressed_checksum = 0u, best_len = 0u,
//in_len = 0u, out_bufsize = 0u, out_len = 0u, wrkmem_size = 0u;

File_system::File_system()
{
	ToDo("Compress (for SDK) And Uncompress (for every game starts) Files While Game\SDK Will Load Or SDK Create Some Files")
	//if (lzo_init() != LZO_E_OK)
	//	Engine::LogError("internal error - lzo_init() failed !!!",
	//		"internal error - lzo_init() failed !!!",
	//		"(this usually indicates a compiler bug - try recompiling\n"\
	//		"without optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
	//	wrkmem_size = 1;
	//	wrkmem_size = (LZO1X_999_MEM_COMPRESS > wrkmem_size) ? LZO1X_999_MEM_COMPRESS : wrkmem_size;
	//	wrkmem_size = (LZO1Y_999_MEM_COMPRESS > wrkmem_size) ? LZO1Y_999_MEM_COMPRESS : wrkmem_size;
	//	wrkmem = (lzo_voidp)malloc(wrkmem_size);
	//	if (wrkmem == NULL)
	//		Engine::LogError("out of memory!", "out of memory!", "out of memory!");

	p = _wgetcwd(nullptr, 512);
	if (p.empty())
		Engine::LogError("File System::File_system Failed!",
			"File System::File_system Failed!", "File System: Something is wrong with Get Resource Folder or Path!");

	LogFName = p.string() + string("\\") + LogFName.string();

	CreateLog();
	ScanFiles();
}

void File_system::ScanFiles()
{
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
void File_system::RescanFiles(_TypeOfFile Type)
{
	// Get New Files
	auto Files = getFilesInFolder("sounds", true, true);
	for (size_t i = 0; i < Files.size(); i++)
	{
		// If File Doesn't Exist
		auto ThisFile = GetFileByType(Files.at(i));
		if (ThisFile->FileA.empty() || ThisFile->FileW.empty())
			GetFile(Files.at(i));
	}
}

_TypeOfFile File_system::GetTypeFile(string file)
{
	auto F = path(file);
	if (F.extension().string() == ".obj" || F.extension().string() == ".3ds")
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

void File_system::CreateLog()
{
	if (GetCurrentPath().empty())
	{
		MessageBoxA(Engine::GetHWND(), "Engine cannot get path to log file!", "Error!", MB_OK | MB_ICONERROR);
		return;
	}
	else if (!exists(LogFName))
	{
		LogFile = make_shared<boost::filesystem::ofstream>(LogFName, std::ofstream::in | std::ofstream::app);
		if (!LogFile->is_open())
		{
			MessageBoxA(Engine::GetHWND(), "Engine cannot create a log file!", "Error!", MB_OK | MB_ICONERROR);
			return;
		}
	}
	else
	{
		LogFile = make_shared<boost::filesystem::ofstream>(LogFName, std::ofstream::in | std::ofstream::app);
		OpenLog();
	}

	LogFile->close();
}

void File_system::AddTextToLog(string Text, Type type)
{
	OpenLog();

	ParseText(Text, type);
	*LogFile << Text;

	LogFile->close();
}

void File_system::OpenLog()
{
	if (!exists(LogFName))
		CreateLog();

	if (!LogFile->is_open())
	{
		LogFile = make_shared<boost::filesystem::ofstream>(LogFName, std::ofstream::in | std::ofstream::app);
		if (!LogFile->is_open())
		{
			MessageBoxA(Engine::GetHWND(), "Engine cannot open a log file!", "Error!", MB_OK | MB_ICONERROR);
			return;
		}
	}
}

void File_system::ClearLogs()
{
	boost::filesystem::remove(LogFName);
}

string File_system::getPathFromType(_TypeOfFile T)
{
	switch (T)
	{
	case MODELS:
		return p.string() + string("\\resource\\models\\");
	case TEXTURES:
		return p.string() + string("\\resource\\textures\\");
	case LEVELS:
		return p.string() + string("\\resource\\maps\\");
	case DIALOGS:
		return p.string() + string("\\resource\\text\\");
	case SOUNDS:
		return p.string() + string("\\resource\\sounds\\");
	case SHADERS:
		return p.string() + string("\\resource\\shaders\\");
	case UIS:
		return p.string() + string("\\resource\\ui\\");
	case SCRIPTS:
		return p.string() + string("\\resource\\scripts\\");
	case FONTS:
		return p.string() + string("\\resource\\fonts\\");
	case NONE:
		return "";
	}
	return "";
}

shared_ptr<File_system::AllFile::File> File_system::GetFileByType(string file)
{
	USES_CONVERSION;
	to_lower(file);
	bool has_branch_path = path(file).has_branch_path();

	switch (GetTypeFile(file))
	{
	case MODELS:
	{
		for (size_t i = 0; i < Files.back()->Models.size(); i++)
		{
			auto File = Files.back()->Models.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Shaders.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Textures.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Sounds.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Scripts.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Fonts.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Dialogs.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files.back()->Dialogs.at(i);
		}
		for (size_t i = 0; i < Files.back()->Uis.size(); i++)
		{
			auto File = Files.back()->Uis.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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
			auto File = Files.back()->Levels.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
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

	return vector<shared_ptr<File_system::AllFile::File>>{make_unique<File_system::AllFile::File>()};
}

shared_ptr<File_system::AllFile::File> File_system::GetFile(string file)
{
	USES_CONVERSION;

		// We make sure that file doesn't exist in our massive
	auto F = GetFileByType(file);
	if (!F->FileA.empty() || !F->FileW.empty())
		return F;

	bool has_branch_path = path(file).has_branch_path(), WasReplace = false;

	if (!has_branch_path)
	{
		// Replace Chars For Not To Find a Extension File
		string Cache = file;
		replaceAll(file, ".", "_");
		if (Cache != file)
			WasReplace = true;
	}

	if (!p.empty())
	{
		string ResPath = p.generic_path().generic_string() + string("\\resource\\");
		string extA = extension(file);
		wstring extW = path(file).extension().wstring();

		if (!has_branch_path)
			if (WasReplace)
				// Revert Chars For "Normal" Find a File
				replaceAll(file, "_", ".");

		to_lower(extA);

		// Getting File Without Ext!
		if (extA.empty())
		{
			auto AllFiles = getFilesInFolder(ResPath, true, true);
			for (size_t i = 0; i < AllFiles.size(); i++)
			{
				// Models
				if (contains(AllFiles.at(i), file + string(".obj")))
				{
					auto F = GetFileByType(file + string(".obj"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				if (contains(AllFiles.at(i), file + string(".3ds")))
				{
					auto F = GetFileByType(file + string(".3ds"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Textures
				else if (contains(AllFiles.at(i), file + string(".dds")))
				{
					auto F = GetFileByType(file + string(".dds"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + string(".png")))
				{
					auto F = GetFileByType(file + string(".png"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + string(".bmp")))
				{
					auto F = GetFileByType(file + string(".bmp"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Shaders
				else if (contains(AllFiles.at(i), file + string(".hlsl")))
				{
					auto F = GetFileByType(file + string(".hlsl"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + string(".fx")))
				{
					auto F = GetFileByType(file + string(".fx"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + string(".vs")))
				{
					auto F = GetFileByType(file + string(".vs"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + string(".ps")))
				{
					auto F = GetFileByType(file + string(".ps"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Sounds
				else if (contains(AllFiles.at(i), file + string(".wav")))
				{
					auto F = GetFileByType(file + string(".wav"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Maps, UI and etc
				else if (contains(AllFiles.at(i), file + string(".xml")))
				{
					auto F = GetFileByType(file + string(".xml"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Scripts
				else if (contains(AllFiles.at(i), file + string(".lua")))
				{
					auto F = GetFileByType(file + string(".lua"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Fonts
				else if (contains(AllFiles.at(i), file + string(".ttf")))
				{
					auto F = GetFileByType(file + string(".ttf"));
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
			}
		}
		if (!ResPath.empty())
		{
			if (extA == ".obj"
				|| extA == ".3ds")
			{
				auto cache = getFilesInFolder(string(ResPath + string("models\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Models.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), MODELS));
						Files.back()->Models.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Models.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Models.back()->ExtW = extW;
						return Files.back()->Models.back();
					}
				}
			}
			else if (extA == ".dds"
				|| extA == ".png"
				|| extA == ".bmp")
			{
				auto cache = getFilesInFolder(string(ResPath + string("textures\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Textures.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), TEXTURES));
						Files.back()->Textures.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Textures.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Textures.back()->ExtW = extW;
						return Files.back()->Textures.back();
					}
				}
			}
			else if (extA == ".hlsl"
				|| extA == ".fx"
				|| extA == ".vs"
				|| extA == ".ps")
			{
				auto cache = getFilesInFolder(string(ResPath + string("shaders\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = has_branch_path ? path(cache.at(i)).string() :
							path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.back()->Shaders.push_back(make_shared<AllFile::File>(cache.at(i),
								extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), SHADERS));
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
				auto cache = getFilesInFolder(string(ResPath + string("sounds\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Sounds.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), SOUNDS));
						Files.back()->Sounds.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Sounds.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Sounds.back()->ExtW = extW;
						return Files.back()->Sounds.back();
					}
				}
			}
			else if (extA == ".xml")
			{
				auto cache = getFilesInFolder(string(ResPath + string("text\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = has_branch_path ? path(cache.at(i)).string() :
							path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.back()->Dialogs.push_back(make_shared<AllFile::File>(cache.at(i),
								extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), DIALOGS));
							Files.back()->Dialogs.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->Dialogs.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->Dialogs.back()->ExtW = extW;
							return Files.back()->Dialogs.back();
						}
					}
				}
				cache = getFilesInFolder(string(ResPath + string("UI\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						auto filePath = has_branch_path ? path(cache.at(i)).string() :
							path(cache.at(i)).filename().string();
						if (filePath == string(file))
						{
							Files.back()->Uis.push_back(make_shared<AllFile::File>(cache.at(i),
								extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), UIS));
							Files.back()->Uis.back()->PathW = path(cache.at(i)).wstring();
							Files.back()->Uis.back()->FileW = path(cache.at(i)).filename().wstring();
							Files.back()->Uis.back()->ExtW = extW;
							return Files.back()->Uis.back();
						}
					}
				}
				cache = getFilesInFolder(string(ResPath + string("maps\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Levels.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), LEVELS));
						Files.back()->Levels.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Levels.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Levels.back()->ExtW = extW;
						return Files.back()->Levels.back();
					}
				}
			}
			else if (extA == ".lua")
			{
				auto cache = getFilesInFolder(string(ResPath + string("scripts\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Scripts.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), SCRIPTS));
						Files.back()->Scripts.back()->PathW = path(cache.at(i)).wstring();
						Files.back()->Scripts.back()->FileW = path(cache.at(i)).filename().wstring();
						Files.back()->Scripts.back()->ExtW = extW;
						return Files.back()->Scripts.back();
					}
				}
			}
			else if (extA == ".ttf")
			{
				auto cache = getFilesInFolder(string(ResPath + string("fonts\\")), true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == string(file))
					{
						Files.back()->Fonts.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), FONTS));
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
		Engine::LogError("File System::GetResPathA Failed!", "File System::GetResPathA Failed!",
			"File System: Something is wrong with File System Function (GetResPathA)!");

	// winerror.h 
		//	e.g ERROR_FILE_NOT_FOUND

	Engine::LogError("File System: ERROR_FILE_NOT_FOUND!\n", (boost::format("File: %s not found\n") % file).str(),
		(boost::format("File: %s not found\n") % file).str());
	return nullptr;
}

vector<wstring> File_system::getFilesInFolder(wstring Folder, bool Recursive, bool onlyFile)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder.c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"\\resource\\")).c_str()) != NULL)
		ResPath = wstring(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"\\resource\\") + wstring(Folder.c_str());	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			files.push_back(it->path().wstring());
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			files.push_back(it->path().wstring());
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			files.push_back(it->path().wstring());
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			files.push_back(it->path().wstring());
		}

	return files;
}
vector<wstring> File_system::getFilesInFolder(wstring Folder)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder.c_str(), wstring(p.generic_path().generic_wstring() + wstring(L"\\resource\\")).c_str()) != NULL)
		ResPath = wstring(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_wstring() + wstring(L"\\resource\\") + wstring(Folder.c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		files.push_back(it->path().wstring());
	}

	return files;
}
vector<string> File_system::getFilesInFolder(string Folder, bool Recursive, bool onlyFile)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder.c_str(), string(p.generic_path().generic_string() + string("\\resource\\")).c_str()) != NULL)
		ResPath = string(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("\\resource\\") + string(Folder.c_str());	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (is_directory(str))
				files.push_back(str);
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (!is_directory(str))
				files.push_back(str);
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (!is_directory(str))
				files.push_back(str);
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().string();
			if (is_directory(str))
				files.push_back(str);
		}

	return files;
}
vector<string> File_system::getFilesInFolder(string Folder)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder.c_str(), string(p.generic_path().generic_string() + string("\\resource\\")).c_str()) != NULL)
		ResPath = string(Folder.c_str());	// If found
	else
		ResPath = p.generic_path().generic_string() + string("\\resource\\") + string(Folder.c_str());	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		files.push_back(it->path().string());
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
		Engine::LogError("File System::getDataFromFile Failed!",
			"File System::getDataFromFile Failed!",
			"File System: Something is wrong with File System Function (getDataFromFile)!");
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
			Returned_val.push_back(Cache);
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