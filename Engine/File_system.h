#pragma once
#if !defined(__FILE_SYSTEM_H__)
#define __FILE_SYSTEM_H__
#include "pch.h"

#include "boost/filesystem.hpp"
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

enum _TypeOfFile { MODELS = 1, TEXTURES, LEVELS, DIALOGS, SOUNDS, SHADERS, UIS, SCRIPTS, NONE };

class File_system
{
private:
	struct AllFile
	{
		struct File
		{
			File() {}
			File(wstring PathW, wstring ExtW, wstring FileW, size_t Size, _TypeOfFile TypeOfFile): PathW(PathW), ExtW(ExtW), FileW(FileW),
				Size(Size), TypeOfFile(TypeOfFile) {}

			File(string PathA, string ExtA, string FileA, size_t Size, _TypeOfFile TypeOfFile): PathA(PathA), ExtA(ExtA), FileA(FileA),
				Size(Size), TypeOfFile(TypeOfFile) {}

			wstring PathW = L"", ExtW = L"", FileW = L"";
			string PathA = "", ExtA = "", FileA = "";

			size_t Size = 0;

			_TypeOfFile TypeOfFile;
		};
		vector<shared_ptr<File>> Models;
		vector<shared_ptr<File>> Textures;
		vector<shared_ptr<File>> Levels;
		vector<shared_ptr<File>> Dialogs;
		vector<shared_ptr<File>> Sounds;
		vector<shared_ptr<File>> Shaders;
		vector<shared_ptr<File>> Uis;
		vector<shared_ptr<File>> Scripts;
		vector<shared_ptr<File>> None;

		AllFile() {}
	};
public:

	File_system();
	~File_system() {}

	void ScanFiles();

	shared_ptr<AllFile::File> GetFile(string file);
	shared_ptr<AllFile::File> GetFileByType(string file);
	vector<shared_ptr<AllFile::File>> GetFileByType(_TypeOfFile T);

	//********
	vector<wstring> getFilesInFolder(wstring Folder, _TypeOfFile type = NONE);
	vector<wstring> getFilesInFolder(wstring Folder, bool Recursive = false, bool onlyFile = false, _TypeOfFile type = NONE);

	vector<string> getFilesInFolder(string Folder, bool Recursive, bool onlyFile, _TypeOfFile type = NONE);
	vector<string> getFilesInFolder(string Folder, _TypeOfFile type = NONE);

	// Work With Files
	string getDataFromFile(string File, bool LineByline, string start, string end);

	vector<string> getDataFromFileVector(string File, bool LineByline);
	bool ReadFileMemory(LPCSTR filename, size_t *FileSize, UCHAR **FilePtr);

	_TypeOfFile GetTypeFile(string file);

	auto getAllFiles() { return Files; }
protected:
	path p;
	vector<shared_ptr<AllFile>> Files;
};
#endif // !__FILE_SYSTEM_H__
