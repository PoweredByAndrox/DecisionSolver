#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Console.h"
#include "File_system.h"

path File_system::WorkDir = "";
static shared_ptr<boost::filesystem::ofstream> LogFile;
path File_system::LogFName = "Engine.log";

File_system::File_system()
{
	WorkDir = _getcwd(nullptr, 1024);
	if (WorkDir.empty())
		Engine::LogError("File System::File_system Failed!",
			string(__FILE__) + ": " + to_string(__LINE__),
			"File System: Something is wrong with Get Resource Folder or Path!");

	WorkDir.swap(WorkDir.generic_path());

	LogFName = WorkDir.generic_string() + 
		((WorkDir.generic_string().back() == '/') ? "" : "/") + LogFName.string();
	CreateLog();
	ScanFiles();
}

void File_system::ScanFiles()
{
	WorkDirSourcesA = WorkDir.generic_string() + ((WorkDir.generic_string().back() == '/')
		? "resource/"
		: "/resource/");
	WorkDirSourcesW = WorkDir.generic_wstring() + ((WorkDir.generic_wstring().back() == L'/')
		? L"resource/"
		: L"/resource/");

	if (!exists(WorkDirSourcesA))
	{
		MessageBoxA(Engine::GetHWND(), "Engine Cannot Work Without Resource Folder!", "ERROR",
			MB_OK | MB_ICONERROR);
		Engine::LogError("File System: RESOURCE_FOLDER_NOT_FOUND!\n",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Resource Folder Wasn't Found\n");
		exit(-1);
	}

	auto file = getFilesInFolder(WorkDirSourcesA, true, true);
	for (size_t i = 0; i < file.size(); i++)
	{
		string someFile = file.at(i),
			ext = extension(someFile);
		auto Fname = path(someFile);
		auto type = GetTypeFileByExt(someFile);
		switch (type)
		{
		case MODELS:
			Models.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Models.back().first->ExtW = Fname.extension().wstring();
			Models.back().first->FileW = Fname.filename().wstring();
			Models.back().first->PathW = Fname.wstring().c_str();
			break;
		case TEXTURES:
			Textures.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Textures.back().first->ExtW = Fname.extension().wstring();
			Textures.back().first->FileW = Fname.filename().wstring();
			Textures.back().first->PathW = Fname.wstring().c_str();
			break;
		case LEVELS:
			Levels.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Levels.back().first->ExtW = Fname.extension().wstring();
			Levels.back().first->FileW = Fname.filename().wstring();
			Levels.back().first->PathW = Fname.wstring().c_str();
			break;
		case DIALOGS:
			Dialogs.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Dialogs.back().first->ExtW = Fname.extension().wstring();
			Dialogs.back().first->FileW = Fname.filename().wstring();
			Dialogs.back().first->PathW = Fname.wstring().c_str();
			break;
		case SOUNDS:
			Sounds.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Sounds.back().first->ExtW = Fname.extension().wstring();
			Sounds.back().first->FileW = Fname.filename().wstring();
			Sounds.back().first->PathW = Fname.wstring().c_str();
			break;
		case SHADERS:
			Shaders.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Shaders.back().first->ExtW = Fname.extension().wstring();
			Shaders.back().first->FileW = Fname.filename().wstring();
			Shaders.back().first->PathW = Fname.wstring().c_str();
			break;
		case UIS:
			Uis.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Uis.back().first->ExtW = Fname.extension().wstring();
			Uis.back().first->FileW = Fname.filename().wstring();
			Uis.back().first->PathW = Fname.wstring().c_str();
			break;
		case SCRIPTS:
			Scripts.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Scripts.back().first->ExtW = Fname.extension().wstring();
			Scripts.back().first->FileW = Fname.filename().wstring();
			Scripts.back().first->PathW = Fname.wstring().c_str();
			break;
		case FONTS:
			Fonts.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			Fonts.back().first->ExtW = Fname.extension().wstring();
			Fonts.back().first->FileW = Fname.filename().wstring();
			Fonts.back().first->PathW = Fname.wstring().c_str();
			break;
		default:
			if (contains(Fname.filename().string(), "proj")) continue;

			None.push_back(make_pair(make_shared<File_system::File>(someFile, ext,
				Fname.filename().string(), (size_t)file_size(Fname), type), file.at(i)));
			None.back().first->ExtW = Fname.extension().wstring();
			None.back().first->FileW = Fname.filename().wstring();
			None.back().first->PathW = Fname.wstring().c_str();
			break;
		}
	}
}

void File_system::RescanFilesByType(_TypeOfFile Type)
{
	// Get New Files
	GetFileByType(Type).clear();
	auto Files = getFilesInFolder(getPathFromType(Type), true, true);
	for (size_t i = 0; i < Files.size(); i++)
	{
		GetFile(Files.at(i));
	}
}

_TypeOfFile File_system::GetTypeFileByExt(path File)
{
	string Ext = File.string();
	to_lower(Ext);
	Ext = path(Ext).extension().string();

	if (Ext == ".obj" || Ext == ".3ds" || Ext == ".fbx")
		return MODELS;
	else if (Ext == ".hlsl" || Ext == ".fx" || Ext == ".vs" || Ext == ".ps")
		return SHADERS;
	else if (Ext == ".dds" || Ext == ".png"
		|| Ext == ".bmp" || Ext == ".mtl"
		|| Ext == ".jpg")
		return TEXTURES;
	else if (Ext == ".wav")
		return SOUNDS;
	else if (Ext == ".lua")
		return SCRIPTS;
	else if (Ext == ".ttf")
		return FONTS;
	else if (Ext == ".xml")
	{
		if (!File.has_branch_path() && !File.has_parent_path() && !File.has_root_directory() &&
			!File.has_root_name() && !File.has_root_path() && File.has_filename())
		{
			string WExt = File.string();
			deleteWord(WExt, File.extension().string());
			auto Obj = Find(WExt);
			to_lower(Obj->PathA);
			if (contains(Obj->PathA, "ui"))
				return UIS;
			if (contains(Obj->PathA, "maps"))
				return LEVELS;
			if (contains(Obj->PathA, "text"))
				return DIALOGS;
		}
		string lower = File.string();
		to_lower(lower);
		if (contains(lower, "ui"))
			return UIS;
		if (contains(lower, "maps"))
			return LEVELS;
		if (contains(lower, "text"))
			return DIALOGS;
	}
	else
		return NONE;
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
	string New = WorkDirSourcesA + ((WorkDirSourcesA.back() == '/') ? "" : "/");
	switch (T)
	{
	case MODELS:
		return New + "models/";
	case TEXTURES:
		return New + "textures/";
	case LEVELS:
		return New + "maps/";
	case DIALOGS:
		return New + "text/";
	case SOUNDS:
		return New + "sounds/";
	case SHADERS:
		return New + "shaders/";
	case UIS:
		return New + "ui/";
	case SCRIPTS:
		return New + "scripts/";
	case FONTS:
		return New + "ui/fonts/";
	case NONE:
		return "";
	}

	return New;
}

shared_ptr<File_system::File> File_system::Find(path File, bool AlsoAddFile)
{
	if (File.empty())
		return make_shared<File_system::File>();

	if (!File.has_extension())
		AlsoAddFile = true;

	if (AlsoAddFile)
	{
		shared_ptr<File_system::File> NewObj = make_shared<File_system::File>();
		auto AllFiles = getFilesInFolder(WorkDirSourcesA, true, true);
		for (size_t i = 0; i < AllFiles.size(); i++)
		{
			string Files = AllFiles.at(i);
			to_lower(Files);
			// Models
			if (contains(Files, File.string() + (!File.has_extension() ? ".obj" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::MODELS;
					NewObj->ExtA = ".obj";
					return NewObj;
				}
				else
					return F;
			}
			if (contains(Files, File.string() + (!File.has_extension() ? ".3ds" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::MODELS;
					NewObj->ExtA = ".3ds";
					return NewObj;
				}
				else
					return F;

			}
			if (contains(Files, File.string() + (!File.has_extension() ? ".fbx" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::MODELS;
					NewObj->ExtA = ".fbx";
					return NewObj;
				}
				else
					return F;
			}

			// Textures
			else if (contains(Files, File.string() + (!File.has_extension() ? ".dds" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::TEXTURES;
					NewObj->ExtA = ".dds";
					return NewObj;
				}
				else
					return F;

			}
			else if (contains(Files, File.string() + (!File.has_extension() ? ".png" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{	// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::TEXTURES;
					NewObj->ExtA = ".png";
					return NewObj;
				}
				else
					return F;
			}
			else if (contains(Files, File.string() + (!File.has_extension() ? ".bmp" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::TEXTURES;
					NewObj->ExtA = ".bmp";
					return NewObj;
				}
				else
					return F;
			}
			else if (contains(Files, File.string() + (!File.has_extension() ? ".jpg" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::TEXTURES;
					NewObj->ExtA = ".jpg";
					return NewObj;
				}
				else
					return F;
			}

			// Shaders
			else if (contains(Files, File.string() + (!File.has_extension() ? ".hlsl" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::SHADERS;
					NewObj->ExtA = ".hlsl";
					return NewObj;
				}
				else
					return F;
			}
			else if (contains(Files, File.string() + (!File.has_extension() ? ".fx" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::SHADERS;
					NewObj->ExtA = ".fx";
					return NewObj;
				}
				else
					return F;
			}
			else if (contains(Files, File.string() + (!File.has_extension() ? ".vs" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::SHADERS;
					NewObj->ExtA = ".vs";
					return NewObj;
				}
				else
					return F;
			}
			else if (contains(Files, File.string() + (!File.has_extension() ? ".ps" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::SHADERS;
					NewObj->ExtA = ".ps";
					return NewObj;
				}
				else
					return F;
			}

			// Sounds
			else if (contains(Files, File.string() + (!File.has_extension() ? ".wav" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::SOUNDS;
					NewObj->ExtA = ".wav";
					return NewObj;
				}
				else 
					return F;
			}

			// Maps, UI and etc
			else if (contains(Files, File.string() + (!File.has_extension() ? ".xml" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;

					if (contains(NewObj->FileA, "ui"))
						NewObj->TypeOfFile = _TypeOfFile::UIS;
					else if (contains(NewObj->FileA, "maps"))
						NewObj->TypeOfFile = _TypeOfFile::LEVELS;
					else if (contains(NewObj->FileA, "text"))
						NewObj->TypeOfFile = _TypeOfFile::DIALOGS;
					NewObj->ExtA = ".xml";
					return NewObj;
				}
				else
					return F;
			}

			// Scripts
			else if (contains(Files, File.string() + (!File.has_extension() ? ".lua" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::SCRIPTS;
					NewObj->ExtA = ".lua";
					return NewObj;
				}
				else
					return F;
			}

			// Fonts
			else if (contains(Files, File.string() + (!File.has_extension() ? ".ttf" : "")))
			{
				auto F = GetFileByPath(Files);
				if (!F || F->FileA.empty() || F->FileW.empty())
				{
					// If need to add it to engine
					NewObj->PathA = Files;
					NewObj->TypeOfFile = _TypeOfFile::FONTS;
					NewObj->ExtA = ".ttf";
					return NewObj;
				}
				else
					return F;
			}
		}
	}
	else
	{
		switch (GetTypeFileByExt(File))
		{
		case MODELS:
			for (auto elem: Models)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case TEXTURES:
			for (auto elem: Textures)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(Path).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case LEVELS:
			for (auto elem: Levels)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case DIALOGS:
			for (auto elem: Dialogs)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case SOUNDS:
			for (auto elem: Sounds)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case SHADERS:
			for (auto elem: Shaders)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case UIS:
			for (auto elem: Uis)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case SCRIPTS:
			for (auto elem: Scripts)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
			break;
		case FONTS:
			for (auto elem: Fonts)
			{
				string Path = elem.second;
				to_lower(Path);
				if (contains(path(elem.second).filename().string(), File.filename().string()))
					return elem.first;
			}
		}
	}
	return make_shared<File_system::File>();
}

vector<pair<shared_ptr<File_system::File>, string>> File_system::GetFileByType(_TypeOfFile T)
{
	switch (T)
	{
	case MODELS:
		return Models;
	case TEXTURES:
		return Textures;
	case LEVELS:
		return Levels;
	case DIALOGS:
		return Dialogs;
	case SOUNDS:
		return Sounds;
	case SHADERS:
		return Shaders;
	case UIS:
		return Uis;
	case SCRIPTS:
		return Scripts;
	case FONTS:
		return Fonts;
	case NONE:
		return None;
	}

	return vector<pair<shared_ptr<File_system::File>, string>>();
}

shared_ptr<File_system::File> File_system::GetFileByPath(path File)
{
	string lower = File.string();
	to_lower(lower);

	switch (GetTypeFileByExt(lower))
	{
	case MODELS:
		for (auto it: Models)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case TEXTURES:
		for (auto it: Textures)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case LEVELS:
		for (auto it: Levels)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case DIALOGS:
		for (auto it: Dialogs)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case SOUNDS:
		for (auto it: Sounds)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case SHADERS:
		for (auto it: Shaders)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case UIS:
		for (auto it: Uis)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case SCRIPTS:
		for (auto it: Scripts)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	case FONTS:
		for (auto it: Fonts)
		{
			to_lower(it.second);
			if ((contains(it.second, lower)))
				return it.first;
		}
		break;
	}

	return shared_ptr<File_system::File>();
}

shared_ptr<File_system::File> File_system::GetFile(path File)
{
	if (File.empty()) return shared_ptr<File_system::File>();
	
	File.swap(File.generic());
	string Fname = File.generic_string();
	to_lower(Fname);

	// It means that we have the full-path like this "C:/SommePath/Somme.obj" only with lower cases
	if (File.has_branch_path() && File.has_parent_path() && File.has_root_directory() &&
		File.has_root_name() && File.has_root_path() && File.has_extension() && File.has_filename())
	{
		auto Obj = GetFileByPath(Fname);
		if (Obj && Obj->Size > 0)
			return Obj;
		if (File.has_extension())
			deleteWord(Fname, File.extension().string());

		// if it was empty try to add to engine
		Find(Fname);
	}

	// It means that we have the path like this "Somme" or with full-path "C:/SommePath/Somme"
	else if (File.has_filename())
	{
		if (File.has_extension())
			deleteWord(Fname, File.extension().string());

		auto Obj = Find(Fname);
		if (Obj && Obj->Size > 0)
			return Obj;
	}

	return AddFile(File);
}

#include "Models.h"
void getListTexturesFromModel(aiNode *node, const aiScene *pScene, vector<string> &ListTextures)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		auto mesh = pScene->mMeshes[node->mMeshes[i]];
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = pScene->mMaterials[mesh->mMaterialIndex];
			for (auto type = 0; (aiTextureType)type <= AI_TEXTURE_TYPE_MAX; type++)
			{
				for (UINT i = 0; i < material->GetTextureCount((aiTextureType)type); i++)
				{
					aiString str = {};
					material->GetTexture((aiTextureType)type, i, &str);
					if (str.C_Str() && str.data)
						ListTextures.push_back(path(str.C_Str()).filename().string());
				}
			}
		}

	}
	for (UINT i = 0; i < node->mNumChildren; i++)
		getListTexturesFromModel(node->mChildren[i], pScene, ListTextures);
}

shared_ptr<File_system::File> File_system::AddFile(path File, pair<string, vector<pair<bool, string>>> &ListTextures)
{
	_TypeOfFile T = NONE;
	string PathFile, ext;

	File.swap(File.generic());
	string Fname = File.generic_string();
	to_lower(Fname);

	// Try To Find This File In Resources Of Engine
	auto _Obj = Find(Fname);

	if (File.empty() && !ListTextures.second.empty())
	{
		shared_ptr<File_system::File> _Obj;
		vector<string> tmp;
		int ID = 0;
		bool Finded = false;
		vector<pair<bool, string>>::iterator It;

		for (auto It: ListTextures.second)
		{
			auto Obj = It.second;
			if (Obj.empty()) continue;

			Fname = path(Obj).filename().string();
			to_lower(Fname);

			if (!path(Obj).has_root_path() && !It.first) // Don't Add Necessary File
				tmp.push_back(Fname);
		}

		for (It = ListTextures.second.begin(); It != ListTextures.second.end(); It++)
		{
			auto Obj = It->second;
			if (Obj.empty() || !It->first || !path(Obj).has_root_path()) continue;

			Fname = path(Obj).filename().string();
			to_lower(Fname);

			vector<string>::iterator it = std::find_if(tmp.begin(), tmp.end(),
				[&](const string &val)
			{
				if (contains(path(val).filename().string(), Fname))
					return true;
				return false;
			});
			if (it != tmp.end())
			{
				ID = std::distance(tmp.begin(), it);
				Finded = true;
			}

			path _File = path(Obj);
			_Obj = Find(_File.filename().string(), false);
			vector<pair<bool, string>>::iterator begin = ListTextures.second.begin(),
				end = ListTextures.second.end();
			auto ptr = begin;
			for (ptr = begin; ptr < end; ptr++)
			{
				string FName = ptr->second;
				to_lower(FName);

				if (path(FName).has_root_path() && contains(FName, _File.filename().string()))
					break;
			}
			if (_Obj && _Obj->Size == 0 && !_Obj->HasTextures) // Find Our Undoned Files In AllFiles
			{
				// Copy Them And Change Them To Set Full-Path And Other
				bool IsCreated = false;

				//try
				//{
				ext = path(ListTextures.first).extension().string();
				string delExt = ListTextures.first,
					pathType = getPathFromType(GetTypeFileByExt(Fname)); // Replace Ext
				deleteWord(delExt, ext);
				if (!exists(pathType + delExt + "/" + Fname))
				{
					if (_Obj->PathA.empty())
					{
						path Path = pathType + delExt + "/" + Fname;

						Textures.push_back(make_pair(make_shared<File_system::File>(Path.string(), ext, Fname,
							(size_t)file_size(_File), T), Path.string()));
						Textures.back().first->ExtW = Path.extension().wstring();
						Textures.back().first->FileW = Path.filename().wstring();
						Textures.back().first->PathW = Path.wstring();

						IsCreated = true;
						if (!exists(pathType + delExt))
							create_directory(pathType + delExt);

						copy(_File, Path);
					}
					else
					{
						if (!exists(pathType + delExt))
							create_directory(pathType + delExt);

						copy(_File, path(_Obj->PathA));
					}

					if (!IsCreated)
					{
						_Obj->HasTextures = true;
						_Obj->Size = (size_t)file_size(_File);
					}
					// Anyway delete last element in list files to not to show it
					if (ptr._Ptr)
					{
						It = ListTextures.second.erase(ptr);
						if (It == ListTextures.second.end())
							It = ListTextures.second.begin();
					}
					ListTextures.second.at(ID).first = true;
					ListTextures.second.at(ID).second = _File.filename().string();
				}
				else// (boost::filesystem::filesystem_error const &e)
				{
					//OutputDebugStringA(e.what());
					ListTextures.second.at(ID).first = false;
					// Anyway delete last element in list files to not to show it
					if (ptr._Ptr)
					{
						It = ListTextures.second.erase(ptr);
						if (It == ListTextures.second.end())
							It = ListTextures.second.begin();
					}
				}
			}
			else
			{
				ListTextures.second.at(ID).first = true;
				ListTextures.second.at(ID).second = _File.filename().string();
				// Anyway delete last element in list files to not to show it
				if (ptr._Ptr)
				{
					It = ListTextures.second.erase(ptr);
					if (It == ListTextures.second.end())
						It = ListTextures.second.begin();
				}
			}
		}
		return _Obj;
	}

	// It means that we have the path like this "Somme" or with full-path
	if (!File.empty() && File.has_filename() && _Obj && _Obj->Size == 0)
	{
		if (!exists(_Obj->PathA))
		{
			Engine::LogError("File System: File Doesn't Exist Or Found!\n",
				string(__FILE__) + ": " + to_string(__LINE__),
				"File: " + Fname + " Doesn't Exist Or Found\n");
			return shared_ptr<File_system::File>();
		}
		Fname = File.filename().string();
		PathFile = File.parent_path().string();

		T = GetTypeFileByExt(Fname);

		if (T == _TypeOfFile::NONE)
		{
			Engine::LogError("File System: File Is Unsupported!\n",
				string(__FILE__) + ": " + to_string(__LINE__),
				"File: " + Fname + "Isn't Supported By Engine\n");
			return shared_ptr<File_system::File>(); // Unsupported File!
		}

		ext = File.extension().string();
		to_lower(ext);

		// Try To Find Some Textures From File And Add It To Queue Engine To Model
		if (T == _TypeOfFile::MODELS)
		{
			auto importer = new Assimp::Importer;

			auto pScene = importer->ReadFile(File.string(), 0);
			if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode || !pScene->HasMeshes())
			{
				Engine::LogError("File System: Some Trouble With This File!\n",
					string(__FILE__) + ": " + to_string(__LINE__),
					"Some Trouble With: " + Fname + "\n");
				return shared_ptr<File_system::File>();
			}
			else
			{
				vector<string> tmpList;
				getListTexturesFromModel(pScene->mRootNode, pScene, tmpList);
				if (!tmpList.empty())
					ListTextures.first = Fname;
				else
					Engine::LogError("File System: This File Doesn't Have Any Materials (Or Textures)!\n",
						string(__FILE__) + ": " + to_string(__LINE__),
						"File: " + Fname + " Doesn't Have Any Materials (Or Textures)\n");

				for (auto It : tmpList)
				{
					PathFile = getPathFromType(GetTypeFileByExt(It));
					string delExt = Fname; // Replace Ext
					deleteWord(delExt, ext);
					if (!exists(path(PathFile + delExt + "/" + It)))
						ListTextures.second.push_back(make_pair(false, It));
				}
			}
		}

		// Copy New File To Resource In Folder Of Type File
		string delExt = Fname, pathType = getPathFromType(T); // Replace Ext
		deleteWord(delExt, ext);
		path Path = pathType + delExt + "/" + Fname;
		if (!exists(Path))
		{
			if (!exists(pathType + delExt))
				create_directory(pathType + delExt);

			copy(File, path(Path));
			Models.push_back(make_pair(make_shared<File_system::File>(Path.string(), ext, Fname,
				exists(path(pathType + delExt)) ? (size_t)file_size(Path) : 0, T), Path.string()));
			Models.back().first->ExtW = Path.extension().wstring();
			Models.back().first->FileW = Path.filename().wstring();
			Models.back().first->PathW = Path.wstring();
		}

		delExt = Path.filename().string(); // Replace Ext
		deleteWord(delExt, Path.extension().string());

		// Check If It Hasn't Around Model Then Add Dialog That File Textures Need To Be Found
		for (size_t i = 0; i < ListTextures.second.size(); i++)
		{
			Fname = path(ListTextures.second.at(i).second).filename().string();
			T = GetTypeFileByExt(Fname);
			if (T != _TypeOfFile::TEXTURES)
				continue;

			pathType = getPathFromType(T);
			auto Path = pathType + delExt + "/" + Fname;
			try
			{
				if (!exists(pathType + delExt))
					create_directory(pathType + delExt);

				if (exists(path(PathFile + "/" + Fname)))
					copy(path(PathFile + "/" + Fname), path(Path));

				Textures.push_back(make_pair(make_shared<File_system::File>(Path, ext, Fname,
					exists(path(PathFile + "/" + Fname)) ? (size_t)file_size(Path) : 0, T,
					exists(path(PathFile + "/" + Fname))), path(Path).string()));
				Textures.back().first->ExtW = path(Path).extension().wstring();
				Textures.back().first->FileW = path(Path).filename().wstring();
				Textures.back().first->PathW = path(Path).wstring();
			}
			catch (boost::filesystem::filesystem_error const &e)
			{
				OutputDebugStringA(e.what());
				Engine::LogError("File System: " + string(e.what()) + "\n",
					string(__FILE__) + ": " + to_string(__LINE__),
					"Some Trouble With: " + Fname + "\n");
				return shared_ptr<File_system::File>();
			}
		}
		return shared_ptr<File_system::File>();
	}
	else if (_Obj && _Obj->Size > 0)
		return _Obj;

	switch (T)
	{
	case MODELS:
		Models.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Models.back().first->ExtW = path(PathFile).extension().wstring();
		Models.back().first->FileW = File.filename().wstring();
		Models.back().first->PathW = path(PathFile).wstring();
		return Models.back().first;
		break;
	case TEXTURES:
		Textures.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Textures.back().first->ExtW = path(PathFile).extension().wstring();
		Textures.back().first->FileW = File.filename().wstring();
		Textures.back().first->PathW = path(PathFile).wstring();
		return Textures.back().first;
		break;
	case LEVELS:
		Levels.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Levels.back().first->ExtW = path(PathFile).extension().wstring();
		Levels.back().first->FileW = File.filename().wstring();
		Levels.back().first->PathW = path(PathFile).wstring();
		return Levels.back().first;
		break;
	case DIALOGS:
		Dialogs.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Dialogs.back().first->ExtW = path(PathFile).extension().wstring();
		Dialogs.back().first->FileW = File.filename().wstring();
		Dialogs.back().first->PathW = path(PathFile).wstring();
		return Dialogs.back().first;
		break;
	case SOUNDS:
		Sounds.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Sounds.back().first->ExtW = path(PathFile).extension().wstring();
		Sounds.back().first->FileW = File.filename().wstring();
		Sounds.back().first->PathW = path(PathFile).wstring();
		return Sounds.back().first;
		break;
	case SHADERS:
		Shaders.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Shaders.back().first->ExtW = path(PathFile).extension().wstring();
		Shaders.back().first->FileW = File.filename().wstring();
		Shaders.back().first->PathW = path(PathFile).wstring();
		return Shaders.back().first;
		break;
	case UIS:
		Uis.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Uis.back().first->ExtW = path(PathFile).extension().wstring();
		Uis.back().first->FileW = File.filename().wstring();
		Uis.back().first->PathW = path(PathFile).wstring();
		return Uis.back().first;
		break;
	case SCRIPTS:
		Scripts.push_back(make_pair(make_shared<File_system::File>(PathFile, path(PathFile).extension().string(),
			Fname, (size_t)file_size(PathFile), T), PathFile));
		Scripts.back().first->ExtW = path(PathFile).extension().wstring();
		Scripts.back().first->FileW = File.filename().wstring();
		Scripts.back().first->PathW = path(PathFile).wstring();
		return Scripts.back().first;
		break;
	case FONTS:
		Fonts.push_back(make_pair(make_shared<File_system::File>(PathFile, File.extension().string(),
			Fname, (size_t)file_size(File), T), PathFile));
		Fonts.back().first->ExtW = File.extension().wstring();
		Fonts.back().first->FileW = File.filename().wstring();
		Fonts.back().first->PathW = File.wstring().c_str();
		return Fonts.back().first;
		break;
	}

	Engine::LogError("File System: ERROR_FILE_NOT_FOUND!\n",
		string(__FILE__) + ": " + to_string(__LINE__),
		"File: " + Fname + " not found\n");
	return shared_ptr<File_system::File>();
}

vector<wstring> File_system::getFilesInFolder(wstring Folder, bool Recursive, bool onlyFile)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder.c_str(), (WorkDirSourcesW).c_str()) != nullptr)
		ResPath = Folder;	// If found
	else
		ResPath = WorkDirSourcesW + Folder;	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().generic_wstring();
			if (is_directory(str))
				files.push_back(it->path().generic_wstring());
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().generic_wstring();
			if (!is_directory(str))
				files.push_back(it->path().generic_wstring());
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().generic_wstring();
			if (!is_directory(str))
				files.push_back(it->path().generic_wstring());
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().generic_wstring();
			if (is_directory(str))
				files.push_back(it->path().generic_wstring());
		}

	return files;
}
vector<wstring> File_system::getFilesInFolder(wstring Folder)
{
	vector<wstring> files;
	wstring ResPath;

	if (wcsstr(Folder.c_str(), (WorkDirSourcesW).c_str()) != nullptr)
		ResPath = Folder;	// If found
	else
		ResPath = WorkDirSourcesW + Folder;	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		files.push_back(it->path().generic_wstring());
	}

	return files;
}
vector<string> File_system::getFilesInFolder(string Folder, bool Recursive, bool onlyFile)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder.c_str(), (WorkDirSourcesA).c_str()) != nullptr)
		ResPath = Folder;	// If found
	else
		ResPath = WorkDirSourcesA + Folder;	// No!

	if (!Recursive && !onlyFile)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().generic_string();
			if (is_directory(str))
				files.push_back(str);
		}
	else if (Recursive && onlyFile)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().generic_string();
			if (!is_directory(str))
				files.push_back(str);
		}
	else if (onlyFile && !Recursive)
		for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
		{
			auto str = it->path().generic_string();
			if (!is_directory(str))
				files.push_back(str);
		}
	else if (!onlyFile && Recursive)
		for (recursive_directory_iterator it(ResPath); it != recursive_directory_iterator(); ++it)
		{
			auto str = it->path().generic_string();
			if (is_directory(str))
				files.push_back(str);
		}

	return files;
}
vector<string> File_system::getFilesInFolder(string Folder)
{
	vector<string> files;
	string ResPath;

	if (strstr(Folder.c_str(), (WorkDirSourcesA).c_str()) != nullptr)
		ResPath = Folder;	// If found
	else
		ResPath = WorkDirSourcesA + Folder;	// No!

	for (directory_iterator it(ResPath); it != directory_iterator(); ++it)
	{
		files.push_back(it->path().generic_string());
	}

	return files;
}

string File_system::getDataFromFile(string File, string start, string end)
{
	if (File.empty())
		return "";

	string Returned_val;
	std::ifstream streamObj = std::ifstream(File.c_str());
	streamObj >> noskipws;
	if (streamObj.is_open())
	{
		copy(istream_iterator<char>(streamObj), istream_iterator<char>(), std::back_inserter(Returned_val));

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
				string(__FILE__) + ": " + to_string(__LINE__),
				"File System: Something is wrong with File System Function (getDataFromFile)!");
			return "";
		}
	}
	else
	{
		Engine::LogError("File System::getDataFromFile Failed!",
			string(__FILE__) + ": " + to_string(__LINE__),
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
bool File_system::ReadFileMemory(LPCSTR filename, size_t &FileSize, vector<BYTE> &FilePtr)
{
	if (!exists(filename) || file_size(filename) == 0) return false;
	std::ifstream is(filename, ios::binary);
	if (is)
	{
		is.unsetf(std::ios::skipws);
		is.seekg(0, is.end);
		FileSize = static_cast<size_t>(is.tellg());
		is.seekg(0, is.beg);

		FilePtr.reserve(FileSize+1);

		// read the data:
		FilePtr.insert(FilePtr.begin(),
			std::istream_iterator<BYTE>(is),
			std::istream_iterator<BYTE>());
		FilePtr.push_back('\0');
	}
	else
		return false;

	return true;
}

ToDo("Add To Open Proj File Itself (From Recent)");
#include "Levels.h"
HRESULT File_system::ProjectFile::OpenFile(path File)
{
	vector<BYTE> Buf;
	size_t n = 0;
	if (Application->getFS()->ReadFileMemory(File.string().c_str(), n, Buf))
		EngineTrace(Application->getLevel()->Load(reinterpret_cast<char *>(Buf.data())));
	return S_OK;
}

#include "GameObjects.h"
void File_system::ProjectFile::SaveFile(path File)
{
	if (!File.empty() && File.extension().string().find(".*") != string::npos)
			File.replace_extension(".proj");

	SaveProj(File);
}

ToDo("Check It If We Use Dialog To Open File And Pass It")
void File_system::CreateProjectFile(string FName)
{
	path Fname;
	
	string BuffCmp;
	BuffCmp = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<scene>\n</scene>\n";
	// Create File
	*make_shared<boost::filesystem::ofstream>(Fname = (getPathFromType(_TypeOfFile::LEVELS) + (FName + ".proj")),
		std::ofstream::out) << BuffCmp;

	Project->SetCurProject(Fname);
}

void File_system::ProjectFile::SaveProj(path File)
{
	string Buff;
	auto MainChild = Application->getLevel()->getChild();
	auto Nodes = MainChild->GetNodes();

	shared_ptr<tinyxml2::XMLDocument> Doc = Application->getLevel()->getDocXMLFile();

	for (auto It: Nodes)
	{
		if (!File.empty())
		{
			if (!It->SaveInfo->IsRemoved)
			{
				It->SaveInfo->Pos = true;
				It->SaveInfo->Rot = true;
				It->SaveInfo->Scale = true;
				It->IsItChanged = true;
			}
		}

		if (Application->getLevel()->IsNotSaved() || (It->IsItChanged || It->SaveInfo->IsRemoved))
			Buff = Application->getLevel()->Save(Doc, It);
		if (It->SaveInfo->IsRemoved)
			Application->getLevel()->Remove(It->ID);
	}

	if (CurrentProj.empty())
	{
		Buff = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + Buff;
		Application->getFS()->CreateProjectFile("New File");
	}
	*make_shared<boost::filesystem::ofstream>(File.empty() ? CurrentProj : File, std::ofstream::out) << Buff;
}

void File_system::ProjectFile::SetCurProject(path File)
{
	//CheckForSameFile(file);
	RecentFiles.push_back({ (int)RecentFiles.size(), File });
	CurrentProj = File;
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

boost::property_tree::ptree File_system::LoadSettingsFile()
{
	if (!boost::filesystem::exists(GetCurrentPath() + "settings.cfg")) return boost::property_tree::ptree();

	bool IfRead = false;
	vector<BYTE> File; size_t Size = 0;
	boost::property_tree::ptree fData;
	if (IfRead = File_system::ReadFileMemory((GetCurrentPath() + "settings.cfg").c_str(), Size, File))
	{
		string Data = reinterpret_cast<char *>(File.data());
		to_lower(Data);

		std::istringstream ini(Data);
		boost::property_tree::ini_parser::read_ini(ini, fData);
	}

	return fData;
}

void File_system::SaveSettings(vector<pair<string, string>> ToFile)
{
	path p(GetCurrentPath() + "settings.cfg");
	remove(p);

	boost::property_tree::ptree fData;
	
	for (auto Auto: ToFile)
	{
		fData.add<string>(Auto.first, Auto.second);
	}

	boost::property_tree::ini_parser::write_ini(p.string(), fData);
}

bool File_system::compressFile(path data, string where)
{
	if (!where.empty())
		boost::filesystem::create_directories(where);
	try
	{
		std::ifstream file(data.string().c_str(), ios::binary | ios::in);
		if (file)
		{
			boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
			inbuf.push(gz_com(gzip_params(zlib::best_compression, zlib::deflated, zlib::default_window_bits,
				zlib::default_mem_level, zlib::default_strategy, data.string(), "Comment")));
			inbuf.push(file);

			auto output = (!where.empty() ? (where + "/") : "") + data.filename().string();

			if (gz_com *gz = inbuf.component<0, gz_com>())
			{
				OutputDebugStringA(("Writing " + output + "\n").c_str());
				std::ofstream ofs(output.c_str(), std::ios::binary | ios::out);
				ofs << 'Ÿ';
				boost::iostreams::copy(inbuf, ofs);
				rename(output, output + ".gz");
			}
		}
		else
			return false;

		return true;
	}
	catch (const gzip_error& e)
	{
		switch (e.error())
		{
		case gzip::bad_header: // 4
			throw exception("4");
			break;
		case gzip::bad_crc: // 2
			throw exception("2");
			break;
		case gzip::bad_footer: // 5
			throw exception("5");
			break;
		case gzip::bad_length: // 3
			throw exception("3");
			break;
		case gzip::bad_method: // 6
			throw exception("6");
			break;
		}
	}

	return false;
}

bool File_system::decompressFile(path data, string where)
{
	if (!where.empty())
		boost::filesystem::create_directories(where);
	try
	{
		std::ifstream file(data.string().c_str(), ios::binary | ios::in);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
		inbuf.push(gz_decom());
		inbuf.push(file);
		if (gz_decom *gz = inbuf.component<0, gz_decom>())
		{
			auto output = (!where.empty() ? (where + "/") : "") + data.filename().string();
			{
				OutputDebugStringA(("Writing " + output).c_str());
				std::ofstream ofs(output.c_str(), std::ios::binary);
				boost::iostreams::copy(inbuf, ofs);
			}

			OutputDebugStringA(("Original filename: " + gz->file_name() + "\n").c_str());
			OutputDebugStringA(("Original mtime: " + to_string(gz->mtime()) + "\n").c_str());
			OutputDebugStringA(("Zip comment: " + gz->comment() + "\n").c_str());
			if (!gz->file_name().empty())
				rename(output, where + "/" + path(gz->file_name()).filename().string());
		}

		return true;
	}
	catch (const gzip_error& e)
	{
		switch (e.error())
		{
		case gzip::bad_header: // 4
			throw exception("4");
			break;
		case gzip::bad_crc: // 2
			throw exception("2");
			break;
		case gzip::bad_footer: // 5
			throw exception("5");
			break;
		case gzip::bad_length: // 3
			throw exception("3");
			break;
		case gzip::bad_method: // 6
			throw exception("6");
			break;
		}
	}

	return false;
}
