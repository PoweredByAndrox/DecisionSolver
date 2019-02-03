#pragma once
#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__
#include "pch.h"

#include "boost/filesystem.hpp"
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

class File_system
{
public:
	File_system();
	~File_system() {}

	string *GetResPathA(string *File);
		// Wide (wchar_t*)
	wstring *GetResPathW(wstring *File);
		// Get string return wstring
	wstring GetResPathW(string *File);
		// Getting massive vector<wstring>
	vector<wstring> GetResPathW(vector<wstring> *File[]);
		// Getting one array from wstring to vector<wstring>
	vector<wstring> GetResPathW(wstring File);

		//********
	vector<wstring> getFilesInFolder(wstring *Folder);
	vector<wstring> getFilesInFolder(wstring *Folder, bool Recursive = false, bool onlyFile = false);

	vector<string> getFilesInFolder(string *Folder, bool Recursive, bool onlyFile);
	vector<string> getFilesInFolder(string *Folder);
	vector<string> getFilesInFolder(string *Folder, LPCSTR ext);

		// Work With Files
	string getDataFromFile(string *File, bool LineByline);
	vector<string> getDataFromFileVector(string *File, bool LineByline);
	bool ReadFileMemory(LPCSTR filename, size_t *FileSize, UCHAR **FilePtr);

protected:
	path p;
};
#endif // !__FILE_SYSTEM_H__
