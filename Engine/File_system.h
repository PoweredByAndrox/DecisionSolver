#pragma once
#if !defined(__FILE_SYSTEM_H__)
#define __FILE_SYSTEM_H__
#include "pch.h"

#include "boost/filesystem.hpp"
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

class File_system
{
private:
	enum TYPE { MODELS = 1, TEXTURES, LEVELS, DIALOGS, SOUNDS, SHADERS, UIS, NONE };
	struct File
	{
		File(wstring PathW, wstring ExtW, wstring FileW, size_t Size, TYPE TypeOfFile) : PathW(PathW), ExtW(ExtW), FileW(FileW),
			Size(Size), TypeOfFile(TypeOfFile) {}

		File(string PathA, string ExtA, string FileA, size_t Size, TYPE TypeOfFile) : PathA(PathA), ExtA(ExtA), FileA(FileA),
			Size(Size), TypeOfFile(TypeOfFile) {}

		wstring PathW = L"", ExtW = L"", FileW = L"";
		string PathA = "", ExtA = "", FileA = "";

		size_t Size = 0;

		TYPE TypeOfFile;
	};
	vector<File *> Files;
public:

	File_system();
	~File_system() {}

	void ScanFilesInRes();

	File *GetFile(string file);

	//********
	vector<wstring> getFilesInFolder(wstring *Folder);
	vector<wstring> getFilesInFolder(wstring *Folder, bool Recursive = false, bool onlyFile = false);

	vector<string> getFilesInFolder(string *Folder, bool Recursive, bool onlyFile);
	vector<string> getFilesInFolder(string *Folder);
	vector<string> getFilesInFolder(LPCSTR ext);

	// Work With Files
	string getDataFromFile(string *File, bool LineByline, string start, string end);

	vector<string> getDataFromFileVector(string *File, bool LineByline);
	bool ReadFileMemory(LPCSTR filename, size_t *FileSize, UCHAR **FilePtr);

	auto getAllFiles() { return Files; }
protected:
	path p;
};
#endif // !__FILE_SYSTEM_H__
