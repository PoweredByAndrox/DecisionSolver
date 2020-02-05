#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Console.h"
#include "File_system.h"

path File_system::p = "";
static shared_ptr<boost::filesystem::ofstream> LogFile;
path File_system::LogFName = "Engine.log";

File_system::File_system()
{
	p = _wgetcwd(nullptr, 512);
	if (p.empty())
		Engine::LogError("File System::File_system Failed!",
			"File System::File_system Failed!", "File System: Something is wrong with Get Resource Folder or Path!");

	LogFName = p.string() + "\\" + LogFName.string();

	CreateLog();
	ScanFiles();

	//string buf = "";
	//CompressFile(string("G:\\DecisionSolver\\Engine\\resource\\maps\\first_level.xml"), buf);
}

int GetMaxCompressedLen(int nLenSrc)
{
	int n16kBlocks = (nLenSrc + 16383) / 16384; // round up any fraction of a block
	return (nLenSrc + 6 + (n16kBlocks * 5));
}

int GetMaxDeCompressedLen(int nLenSrc)
{
	int n16kBlocks = (nLenSrc + 16383) / 16384; // round up any fraction of a block
	return (nLenSrc - 6 - (n16kBlocks * 5));
}

int CompressData(const BYTE *abSrc, int nLenSrc, BYTE *abDst, int nLenDst)
{
	z_stream zInfo = { 0 };
	zInfo.total_in = zInfo.avail_in = nLenSrc;
	zInfo.total_out = zInfo.avail_out = nLenDst;
	zInfo.next_in = (BYTE *)abSrc;
	zInfo.next_out = abDst;

	int nErr, nRet = -1;
	nErr = deflateInit(&zInfo, Z_DEFAULT_COMPRESSION); // zlib function

	if (nErr == Z_OK)
	{
		nErr = deflate(&zInfo, Z_FINISH);              // zlib function

		if (nErr == Z_STREAM_END)
			nRet = zInfo.total_out;
	}
	deflateEnd(&zInfo);    // zlib function
	return(nRet);
}

int UncompressData(const BYTE *abSrc, int nLenSrc, BYTE *abDst, int nLenDst)
{
	z_stream zInfo = { 0 };
	zInfo.total_in = zInfo.avail_in = nLenSrc;
	zInfo.total_out = zInfo.avail_out = nLenDst;
	zInfo.next_in = (BYTE*)abSrc;
	zInfo.next_out = abDst;

	int nErr, nRet = -1;
	nErr = inflateInit(&zInfo);               // zlib function
	if (nErr == Z_OK)
	{
		nErr = inflate(&zInfo, Z_FINISH);     // zlib function

		if (nErr == Z_STREAM_END)
			nRet = zInfo.total_out;
	}
	inflateEnd(&zInfo);   // zlib function
	return(nRet); // -1 or len of output
}

//Use it only to save files
void File_system::CompressFile(string File, string &Buffer)
{
	BYTE *In = {}, *Out = {};
	size_t InLen = 0,
		OutLen = 0;

	// Get Full Path File
	auto thisFile = File.c_str();
	ReadFileMemory(thisFile, (size_t *)&InLen, (void **)&In);

	OutLen = GetMaxCompressedLen(InLen);
	Out = (BYTE *)malloc(OutLen);
	
	if (CompressData(In, InLen, Out, OutLen) == -1)
		throw exception("HERE!!!");
	
	reinterpret_cast<char *>(Out)[OutLen] = '\0';

	ToDo("Delete This!");
	strcat_s(const_cast<char *>(thisFile), strlen(".Compress"), ".Compress");
	auto res = boost::filesystem::remove(path(thisFile));

	std::ofstream(thisFile, std::ofstream::binary).write(reinterpret_cast<char *>(Out), OutLen);

	Buffer = (reinterpret_cast<char *>(Out));
}

//Use it only to open files
void File_system::DecompressFile(string File, string &Buffer)
{
	BYTE *In = {}, *Out = {};
	size_t InLen = 0,
		OutLen = 0;

	// Compress Input File And Read It To Its Buffer To Work It Later
	ReadFileMemory(File.c_str(), (size_t *)&InLen, (void **)&Out);

	auto thisFile = File.c_str();

	OutLen = GetMaxDeCompressedLen(InLen);

	In = (BYTE *)malloc(OutLen);
	if (UncompressData(Out, InLen, In, OutLen) == -1)
		throw exception("Here!!!");

	reinterpret_cast<char *>(In)[OutLen] = '\0';
	
	ToDo("Delete This!");
	strcat_s(const_cast<char *>(thisFile), strlen(".DeCompress"), ".DeCompress");
	auto res = boost::filesystem::remove(path(thisFile));

	std::ofstream(thisFile, std::ofstream::binary).write(reinterpret_cast<char *>(Out), OutLen);

	Buffer = (reinterpret_cast<char *>(Out));
}

void File_system::ScanFiles()
{
	if (!Files.operator bool())
		Files = make_shared<AllFile>();

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
			Files->Models.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Models.back()->ExtW = Fname.extension().wstring();
			Files->Models.back()->FileW = Fname.filename().wstring();
			Files->Models.back()->PathW = Fname.wstring().c_str();
			break;
		case TEXTURES:
			Files->Textures.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Textures.back()->ExtW = Fname.extension().wstring();
			Files->Textures.back()->FileW = Fname.filename().wstring();
			Files->Textures.back()->PathW = Fname.wstring().c_str();
			break;
		case LEVELS:
			Files->Levels.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Levels.back()->ExtW = Fname.extension().wstring();
			Files->Levels.back()->FileW = Fname.filename().wstring();
			Files->Levels.back()->PathW = Fname.wstring().c_str();
			break;
		case DIALOGS:
			Files->Dialogs.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Dialogs.back()->ExtW = Fname.extension().wstring();
			Files->Dialogs.back()->FileW = Fname.filename().wstring();
			Files->Dialogs.back()->PathW = Fname.wstring().c_str();
			break;
		case SOUNDS:
			Files->Sounds.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Sounds.back()->ExtW = Fname.extension().wstring();
			Files->Sounds.back()->FileW = Fname.filename().wstring();
			Files->Sounds.back()->PathW = Fname.wstring().c_str();
			break;
		case SHADERS:
			Files->Shaders.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Shaders.back()->ExtW = Fname.extension().wstring();
			Files->Shaders.back()->FileW = Fname.filename().wstring();
			Files->Shaders.back()->PathW = Fname.wstring().c_str();
			break;
		case UIS:
			Files->Uis.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Uis.back()->ExtW = Fname.extension().wstring();
			Files->Uis.back()->FileW = Fname.filename().wstring();
			Files->Uis.back()->PathW = Fname.wstring().c_str();
			break;
		case SCRIPTS:
			Files->Scripts.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Scripts.back()->ExtW = Fname.extension().wstring();
			Files->Scripts.back()->FileW = Fname.filename().wstring();
			Files->Scripts.back()->PathW = Fname.wstring().c_str();
			break;
		case FONTS:
			Files->Fonts.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->Fonts.back()->ExtW = Fname.extension().wstring();
			Files->Fonts.back()->FileW = Fname.filename().wstring();
			Files->Fonts.back()->PathW = Fname.wstring().c_str();
			break;
		default:
			if (contains(Fname.filename().string(), "proj")) continue;

			Files->None.push_back(make_shared<AllFile::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type));
			Files->None.back()->ExtW = Fname.extension().wstring();
			Files->None.back()->FileW = Fname.filename().wstring();
			Files->None.back()->PathW = Fname.wstring().c_str();
			break;
		}
	}
}

ToDo("Think it over later!");
void File_system::RescanFilesByType(_TypeOfFile Type)
{
	// Get New Files
	GetFileByType(Type).clear();
	auto Files = getFilesInFolder(getPathFromType(Type), true, true);
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
		|| F.extension().string() == ".bmp" || F.extension().string() == ".mtl"
		|| F.extension().string() == ".jpg")
		return TEXTURES;
	else if (F.extension().string() == ".wav")
		return SOUNDS;
	else if (F.extension().string() == ".lua")
		return SCRIPTS;
	else if (F.extension().string() == ".ttf")
		return FONTS;
	else if (F.extension().string() == ".xml")
	{
		if (FindSubStr(F.string(), "UI"))
			return UIS;
		if (FindSubStr(F.string(), "maps"))
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
		return p.string() + "\\resource\\models\\";
	case TEXTURES:
		return p.string() + "\\resource\\textures\\";
	case LEVELS:
		return p.string() + "\\resource\\maps\\";
	case DIALOGS:
		return p.string() + "\\resource\\text\\";
	case SOUNDS:
		return p.string() + "\\resource\\sounds\\";
	case SHADERS:
		return p.string() + "\\resource\\shaders\\";
	case UIS:
		return p.string() + "\\resource\\ui\\";
	case SCRIPTS:
		return p.string() + "\\resource\\scripts\\";
	case FONTS:
		return p.string() + "\\resource\\fonts\\";
	case NONE:
		return "";
	}
	return "";
}

shared_ptr<File_system::AllFile::File> File_system::GetFileByType(string file)
{
	to_lower(file);
	bool has_branch_path = path(file).has_branch_path();

	switch (GetTypeFile(file))
	{
	case MODELS:
	{
		for (size_t i = 0; i < Files->Models.size(); i++)
		{
			auto File = Files->Models.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Models.at(i);
		}

		break;
	}
	case SHADERS:
	{
		for (size_t i = 0; i < Files->Shaders.size(); i++)
		{
			auto File = Files->Shaders.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Shaders.at(i);
		}

		break;
	}
	case TEXTURES:
	{
		for (size_t i = 0; i < Files->Textures.size(); i++)
		{
			auto File = Files->Textures.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Textures.at(i);
		}

		break;
	}
	case SOUNDS:
	{
		for (size_t i = 0; i < Files->Sounds.size(); i++)
		{
			auto File = Files->Sounds.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Sounds.at(i);
		}

		break;
	}
	case SCRIPTS:
	{
		for (size_t i = 0; i < Files->Scripts.size(); i++)
		{
			auto File = Files->Scripts.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Scripts.at(i);
		}

		break;
	}
	case FONTS:
	{
		for (size_t i = 0; i < Files->Fonts.size(); i++)
		{
			auto File = Files->Fonts.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Fonts.at(i);
		}

		break;
	}
		// XML Files
	case DIALOGS:
	case UIS:
	{
		for (size_t i = 0; i < Files->Dialogs.size(); i++)
		{
			auto File = Files->Dialogs.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Dialogs.at(i);
		}
		for (size_t i = 0; i < Files->Uis.size(); i++)
		{
			auto File = Files->Uis.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Uis.at(i);
		}

		break;
	}
	case LEVELS:
	{
		for (size_t i = 0; i < Files->Levels.size(); i++)
		{
			auto File = Files->Levels.at(i);
			string LowerA = has_branch_path ? File->PathA : File->FileA;
			wstring LowerW = has_branch_path ? File->PathW : File->FileW;
			to_lower(LowerA);
			to_lower(LowerW);
			if ((contains(LowerA, file)) || (contains(LowerW, file)))
				return Files->Levels.at(i);
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
		return Files->Models;
	case TEXTURES:
		return Files->Textures;
	case LEVELS:
		return Files->Levels;
	case DIALOGS:
		return Files->Dialogs;
	case SOUNDS:
		return Files->Sounds;
	case SHADERS:
		return Files->Shaders;
	case UIS:
		return Files->Uis;
	case SCRIPTS:
		return Files->Scripts;
	case FONTS:
		return Files->Fonts;
	case NONE:
		return Files->None;
	}

	return vector<shared_ptr<File_system::AllFile::File>>();
}

shared_ptr<File_system::AllFile::File> File_system::GetFile(string file)
{
		// We make sure that file doesn't exist in our massive
	auto F = GetFileByType(file);
	if (!F->FileA.empty() || !F->FileW.empty())
		return F;

	bool has_branch_path = path(file).has_branch_path(), WasReplace = false;

	if (!has_branch_path)
	{
		// Replace Chars For Not To Find an Extension File. It uses in Models to try to find an ID
		string Cache = file;
		replaceAll(file, ".", "_");
		if (Cache != file)
			WasReplace = true;
	}

	if (!p.empty())
	{
		string ResPath = p.string() + "\\resource\\",
		extA = extension(file);
		wstring extW = path(file).extension().wstring();

		if (!has_branch_path)
			if (WasReplace)
				// Revert Chars For "Normal" Find a File. It uses in Models to try to find an ID 
				replaceAll(file, "_", ".");

		to_lower(extA);

		// Getting File Without Ext!
		if (extA.empty())
		{
			auto AllFiles = getFilesInFolder(ResPath, true, true);
			for (size_t i = 0; i < AllFiles.size(); i++)
			{
				// Models
				if (contains(AllFiles.at(i), file + ".obj"))
				{
					auto F = GetFileByType(file + ".obj");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				if (contains(AllFiles.at(i), file + ".3ds"))
				{
					auto F = GetFileByType(file + ".3ds");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Textures
				else if (contains(AllFiles.at(i), file + ".dds"))
				{
					auto F = GetFileByType(file + ".dds");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + ".png"))
				{
					auto F = GetFileByType(file + ".png");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + ".bmp"))
				{
					auto F = GetFileByType(file + ".bmp");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + ".jpg"))
				{
					auto F = GetFileByType(file + ".jpg");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Shaders
				else if (contains(AllFiles.at(i), file + ".hlsl"))
				{
					auto F = GetFileByType(file + ".hlsl");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + ".fx"))
				{
					auto F = GetFileByType(file + ".fx");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + ".vs"))
				{
					auto F = GetFileByType(file + ".vs");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}
				else if (contains(AllFiles.at(i), file + ".ps"))
				{
					auto F = GetFileByType(file + ".ps");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Sounds
				else if (contains(AllFiles.at(i), file + ".wav"))
				{
					auto F = GetFileByType(file + ".wav");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Maps, UI and etc
				else if (contains(AllFiles.at(i), file + ".xml"))
				{
					auto F = GetFileByType(file + ".xml");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Scripts
				else if (contains(AllFiles.at(i), file + ".lua"))
				{
					auto F = GetFileByType(file + ".lua");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Fonts
				else if (contains(AllFiles.at(i), file + ".ttf"))
				{
					auto F = GetFileByType(file + ".ttf");
					if (!F->FileA.empty() || !F->FileW.empty())
						return F;
				}

				// Project Files
				else if (contains(AllFiles.at(i), file + ".proj"))
				{
					auto F = GetFileByType(file + ".proj");
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
				auto cache = getFilesInFolder(ResPath + "models\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						Files->Models.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), MODELS));
						Files->Models.back()->PathW = path(cache.at(i)).wstring();
						Files->Models.back()->FileW = path(cache.at(i)).filename().wstring();
						Files->Models.back()->ExtW = extW;
						return Files->Models.back();
					}
				}
			}
			else if (extA == ".dds"
				|| extA == ".png"
				|| extA == ".bmp"
				|| extA == ".jpg")
			{
				auto cache = getFilesInFolder(ResPath + "textures\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						Files->Textures.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), TEXTURES));
						Files->Textures.back()->PathW = path(cache.at(i)).wstring();
						Files->Textures.back()->FileW = path(cache.at(i)).filename().wstring();
						Files->Textures.back()->ExtW = extW;
						return Files->Textures.back();
					}
				}
			}
			else if (extA == ".hlsl"
				|| extA == ".fx"
				|| extA == ".vs"
				|| extA == ".ps")
			{
				auto cache = getFilesInFolder(ResPath + "shaders\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						auto filePath = has_branch_path ? path(cache.at(i)).string() :
							path(cache.at(i)).filename().string();
						if (filePath == file)
						{
							Files->Shaders.push_back(make_shared<AllFile::File>(cache.at(i),
								extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), SHADERS));
							Files->Shaders.back()->PathW = path(cache.at(i)).wstring();
							Files->Shaders.back()->FileW = path(cache.at(i)).filename().wstring();
							Files->Shaders.back()->ExtW = extW;
							return Files->Shaders.back();
						}
					}
				}
			}
			else if (extA == ".wav")
			{
				auto cache = getFilesInFolder(ResPath + "sounds\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						Files->Sounds.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), SOUNDS));
						Files->Sounds.back()->PathW = path(cache.at(i)).wstring();
						Files->Sounds.back()->FileW = path(cache.at(i)).filename().wstring();
						Files->Sounds.back()->ExtW = extW;
						return Files->Sounds.back();
					}
				}
			}
			else if (extA == ".xml")
			{
				auto cache = getFilesInFolder(ResPath + "text\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						auto filePath = has_branch_path ? path(cache.at(i)).string() :
							path(cache.at(i)).filename().string();
						if (filePath == file)
						{
							Files->Dialogs.push_back(make_shared<AllFile::File>(cache.at(i),
								extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), DIALOGS));
							Files->Dialogs.back()->PathW = path(cache.at(i)).wstring();
							Files->Dialogs.back()->FileW = path(cache.at(i)).filename().wstring();
							Files->Dialogs.back()->ExtW = extW;
							return Files->Dialogs.back();
						}
					}
				}
				cache = getFilesInFolder(ResPath + "UI\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						auto filePath = has_branch_path ? path(cache.at(i)).string() :
							path(cache.at(i)).filename().string();
						if (filePath == file)
						{
							Files->Uis.push_back(make_shared<AllFile::File>(cache.at(i),
								extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), UIS));
							Files->Uis.back()->PathW = path(cache.at(i)).wstring();
							Files->Uis.back()->FileW = path(cache.at(i)).filename().wstring();
							Files->Uis.back()->ExtW = extW;
							return Files->Uis.back();
						}
					}
				}
				cache = getFilesInFolder(ResPath + "maps\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						Files->Levels.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), LEVELS));
						Files->Levels.back()->PathW = path(cache.at(i)).wstring();
						Files->Levels.back()->FileW = path(cache.at(i)).filename().wstring();
						Files->Levels.back()->ExtW = extW;
						return Files->Levels.back();
					}
				}
			}
			else if (extA == ".lua")
			{
				auto cache = getFilesInFolder(ResPath + "scripts\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						Files->Scripts.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), SCRIPTS));
						Files->Scripts.back()->PathW = path(cache.at(i)).wstring();
						Files->Scripts.back()->FileW = path(cache.at(i)).filename().wstring();
						Files->Scripts.back()->ExtW = extW;
						return Files->Scripts.back();
					}
				}
			}
			else if (extA == ".ttf")
			{
				auto cache = getFilesInFolder(ResPath + "fonts\\", true, true);
				for (size_t i = 0; i < cache.size(); i++)
				{
					auto filePath = has_branch_path ? path(cache.at(i)).string() :
						path(cache.at(i)).filename().string();
					if (filePath == file)
					{
						Files->Fonts.push_back(make_shared<AllFile::File>(cache.at(i),
							extA, path(cache.at(i)).filename().string(), (size_t)file_size(cache.at(i)), FONTS));
						Files->Fonts.back()->PathW = path(cache.at(i)).wstring();
						Files->Fonts.back()->FileW = path(cache.at(i)).filename().wstring();
						Files->Fonts.back()->ExtW = extW;
						return Files->Fonts.back();
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

	Engine::LogError("File System: ERROR_FILE_NOT_FOUND!\n", "File: " + file + " not found\n",
		"File: " + file + " not found\n");
	return make_shared<File_system::AllFile::File>("", "", "", 0, _TypeOfFile::NONE);
}

vector<wstring> File_system::getFilesInFolder(wstring Folder, bool Recursive, bool onlyFile)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder.c_str(), (p.wstring() + L"\\resource\\").c_str()) != NULL)
		ResPath = Folder;	// If found
	else
		ResPath = p.wstring() + L"\\resource\\" + Folder;	// No!

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

	if (wcsstr(Folder.c_str(), (p.wstring() + L"\\resource\\").c_str()) != NULL)
		ResPath = Folder;	// If found
	else
		ResPath = p.wstring() + L"\\resource\\" + Folder;	// No!

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

	if (strstr(Folder.c_str(), (p.string() + "\\resource\\").c_str()) != NULL)
		ResPath = Folder;	// If found
	else
		ResPath = p.string() + "\\resource\\" + Folder;	// No!

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

	if (strstr(Folder.c_str(), (p.string() + "\\resource\\").c_str()) != NULL)
		ResPath = Folder;	// If found
	else
		ResPath = p.string() + "\\resource\\" + Folder;	// No!

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
		{
			Engine::LogError("File System::getDataFromFile Failed!",
				"File System::getDataFromFile Failed!",
				"File System: Something is wrong with File System Function (getDataFromFile)!");
			return "";
		}
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
	auto streamObj = std::ifstream(File.c_str(), std::ifstream::binary);
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

	return vector<string>();
}
bool File_system::ReadFileMemory(LPCSTR filename, size_t *FileSize, void **FilePtr)
{
	std::ifstream is(filename, std::ifstream::binary);
	if (is)
	{
		is.seekg(0, is.end);
		*FileSize = is.tellg();
		is.seekg(0, is.beg);

		*FilePtr = new char[*FileSize];
		reinterpret_cast<char *>(*FilePtr)[*FileSize] = '\0';

		is.read((char *)*FilePtr, *FileSize);

		if (!is)
		{
			is.close();
			return false;
		}
	}

	is.close();
	return true;
}

ToDo("Add Here Decompress File")
#include "Levels.h"
void File_system::ProjectFile::OpenFile(path file)
{
	// Add Here Uncompress Algorytm File
	// string buf = "";
	// Application->getFS()->DecompressFile(file.string(), buf);
	//Application->getLevel()->LoadXML(buf.c_str());
	
	void *Buf = { 0 };
	size_t n = 0;
	Application->getFS()->ReadFileMemory(file.string().c_str(), &n, &Buf);
	Application->getLevel()->LoadXML(reinterpret_cast<char *>(Buf));
}

ToDo("Add Here Compress File")
void File_system::ProjectFile::SaveFile(path file)
{
}

ToDo("Check It If We Use Dialog To Open File And Pass It")
void File_system::CreateProjectFile(string FName)
{
	path Fname = "";
	// Create File
	*make_shared<boost::filesystem::ofstream>(Fname = (getPathFromType(_TypeOfFile::LEVELS) + (FName + ".proj")),
		std::ofstream::in | std::ofstream::app) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<scene>\n</scene>\n";

	Projects->SetCurProject(Fname);
}

void File_system::ProjectFile::SaveCurrProj()
{
//	Application->getLevel()->getCurrent()->Save();
}

void File_system::ProjectFile::SetCurProject(path file)
{
	//CheckForSameFile(file);
	RecentFiles.push_back({ (int)RecentFiles.size(), file });
	CurrentProj = make_shared<path>(file);
}

void File_system::ProjectFile::Resort(bool Greater)
{
	if (RecentFiles.empty()) return;
	RecentFiles.sort(); // By Default It Uses Less<>() Sort!
}

ToDo("Rework It System Recent Files!")
void File_system::ProjectFile::CheckForSameFile(path Path)
{
	for (size_t i = 0; i < RecentFiles.size(); i++)
	{
		auto Obj = std::next(RecentFiles.begin(), i);
		if (Obj->second == Path)
		{
			// Reorder All Files By Less Numerations
			for (size_t id = 0; id < RecentFiles.size(); id++)
			{
				Obj = std::next(RecentFiles.begin(), i); // The Last Opened File
				Obj->first = id;
			}
			Resort();
			return;
		}
	}

	RecentFiles.push_back({ (int)RecentFiles.size(), Path });
	Resort();
}