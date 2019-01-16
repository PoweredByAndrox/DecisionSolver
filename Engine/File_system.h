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
	vector<wstring> getFilesInFolder(wstring *File);
	vector<wstring> getFilesInFolder(wstring *File, bool Recursive = false, bool onlyFile = false);

	vector<string> getFilesInFolder(string *File, bool Recursive, bool onlyFile);
	vector<string> getFilesInFolder(string *File);

	string &replaceAll(string &context, string const &from, string const &to);
	string &replaceAll(string &context, string const &from, string const &to, string const &also);

	wstring &replaceAll(wstring &context, wstring const &from, wstring const &to);
	wstring &replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also);

		// Work With Files
	//wstring *getDataFromFile(wstring *File);
	string getDataFromFile(string *File, bool LineByline);
	vector<string> getDataFromFileVector(string *File, bool LineByline);
private:
	path p;
};
#endif // !__FILE_SYSTEM_H__
