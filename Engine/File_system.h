#pragma once
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
	wstring File_system::GetResPathW(string *File);
		// Getting massive vector<wstring>
	vector<wstring> File_system::GetResPathW(vector<wstring> *File[]);
		// Getting one array from wstring to vector<wstring>
	vector<wstring> File_system::GetResPathW(wstring File);
		// Getting a resources file location
	LPCTSTR File_system::GetResPathW(LPCTSTR File);

private:
	path p;
	void TestMsgBox(LPCSTR text);
	void TestMsgBox(LPCSTR Text, LPCSTR CaptionText, int MButtons = MB_OK);
};

