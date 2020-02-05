#pragma once
#if !defined(__FILE_SYSTEM_H__)
#define __FILE_SYSTEM_H__
#include "pch.h"

#include <fstream>
#include "boost/filesystem.hpp"
#include <algorithm>

#include "zlib.h"

using namespace std;
using namespace boost::filesystem;

enum _TypeOfFile { MODELS = 1, TEXTURES, LEVELS, DIALOGS, SOUNDS, SHADERS, UIS, SCRIPTS, FONTS, NONE };

class File_system
{
private:
	struct ProjectFile;
	struct AllFile
	{
		struct File
		{
			File() {}
			File(wstring PathW, wstring ExtW, wstring FileW, size_t Size, _TypeOfFile TypeOfFile) : PathW(PathW), ExtW(ExtW), FileW(FileW),
				Size(Size), TypeOfFile(TypeOfFile) {}

			File(string PathA, string ExtA, string FileA, size_t Size, _TypeOfFile TypeOfFile) : PathA(PathA), ExtA(ExtA), FileA(FileA),
				Size(Size), TypeOfFile(TypeOfFile) {}

			wstring PathW = L"", ExtW = L"", FileW = L"";
			// Full Path To Required File
			string PathA = "",
				ExtA = "", FileA = "";

			size_t Size = 0;

			_TypeOfFile TypeOfFile;
		};
		vector<shared_ptr<File>>
			Models,
			Textures,
			Levels,
			Dialogs,
			Sounds,
			Shaders,
			Uis,
			Scripts,
			Fonts,
			None;
	};

	// For SDK!!!
	shared_ptr<ProjectFile> Projects = make_shared<ProjectFile>();

	struct ProjectFile
	{
		ProjectFile() {}
		~ProjectFile() {}

		ProjectFile(path CurrProj) { SetCurProject(CurrProj); }

		auto GetRecent() { return RecentFiles; }
		auto GetCurrentProject() { return CurrentProj; }

		ToDo("Add To Open Proj File Itself (From Recent)");
		void OpenFile(path file);

		// "Save As" Means Save In Another File Path
		void SaveFile(path file);

		void SaveCurrProj();
		void SetCurProject(path file);
		void CheckForSameFile(path Path);
	private:
		void Resort(bool Greater = true);
		shared_ptr<boost::filesystem::ofstream> ProjFile;
		shared_ptr<path> CurrentProj = make_shared<path>();
		list<pair<int, path>> RecentFiles = {};
	};
public:
	File_system();
	~File_system() {}

	void DecompressFile(string File, string &Buffer);
	void CompressFile(string File, string &Buffer);

	// Function reads all files from our Resource
	void ScanFiles();
	// Function if need to rescan files
	void RescanFilesByType(_TypeOfFile T);

	shared_ptr<AllFile::File> GetFile(string file);
	shared_ptr<AllFile::File> GetFileByType(string file);
	vector<shared_ptr<AllFile::File>> GetFileByType(_TypeOfFile T);

	//********
	vector<wstring> getFilesInFolder(wstring Folder);
	vector<wstring> getFilesInFolder(wstring Folder, bool Recursive = false, bool onlyFile = false);

	vector<string> getFilesInFolder(string Folder, bool Recursive, bool onlyFile);
	vector<string> getFilesInFolder(string Folder);

	// Work With Files
	string getDataFromFile(string File, bool LineByline, string start = "<!--", string end = "-->");

	vector<string> getDataFromFileVector(string File, bool LineByline);
	bool ReadFileMemory(LPCSTR filename, size_t *FileSize, void **FilePtr);

	_TypeOfFile GetTypeFile(string file);

	static void CreateLog();
	static void AddTextToLog(string Text, Type type);
	static void OpenLog();
	static void ClearLogs();

	void CreateProjectFile(string FName);

	auto getLogFName() { return LogFName; }

	auto static GetCurrentPath() { return p; }
	string static GetCurPath() { return p.string(); }

	string getPathFromType(_TypeOfFile T);

	auto getAllFiles() { return Files; }

	auto GetProjects() { return Projects; }
	void CreateProject(path CurrFile = "") { Projects = make_shared<ProjectFile>(CurrFile); }
protected:
	static path p;
	shared_ptr<AllFile> Files = make_shared<AllFile>();

	static path LogFName;
};
#endif // !__FILE_SYSTEM_H__
