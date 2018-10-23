#include "pch.h"
#include "DXUT.h"
#include <windows.h>
#include "File_system.h"

#include <direct.h>

File_system::File_system()
{
	if (GetCurrentDirectoryA(sizeof(Path), Path) == 0)
		exit(-1);
	p = Path;
	ZeroMemory(Path, sizeof(Path));
}


File_system::~File_system()
{
}

void File_system::test_Func()
{
	TestMsgBox(p.root_path().generic_string().c_str());
}

void File_system::TestMsgBox(LPCSTR text)
{
	MessageBoxA(DXUTGetHWND(), text, "Test", MB_OK);
}
