#pragma once
#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "boost/filesystem.hpp"
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

class File_system
{
public:
	File_system();
	~File_system() {}

		// Ansi (const char*)
	LPCSTR GetResPathA(string *File);
		// Wide (wchar_t*)
	LPCTSTR GetResPathW(wstring *File);
		// Get string return wstring
	wstring GetResPathW(string *File);
		// Getting massive vector<wstring>
	vector<wstring> GetResPathW(vector<wstring> *File[]);
		// Getting one array from wstring to vector<wstring>
	vector<wstring> GetResPathW(wstring File);

		//********
	vector<wstring> getFilesInFolder(wstring *File);
	vector<wstring> getFilesInFolder(wstring *File, bool Recursive = false, bool onlyFile = false);

private:
	path p;
};
#endif // !__FILE_SYSTEM_H__
