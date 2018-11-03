#pragma once
#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem;

class File_system
{
public:
	File_system();
	~File_system();

	void GetPath(); // Test Func

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

private:
	path p;
	void TestMsgBox(LPCSTR text);
	void TestMsgBox(LPCSTR Text, LPCSTR CaptionText, int MButtons = MB_OK);
};
#endif // !__FILE_SYSTEM_H__
