#pragma once
#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem;

class File_system
{
public:
	File_system();
	~File_system();
	void test_Func();

private:
	char Path[MAX_PATH];
	path p;
	void TestMsgBox(LPCSTR text);
};

