#pragma once
#include <vector>
#include <stdio.h> 
#include <direct.h>
#include <tchar.h>
#include <memory>

#include <d3dcompiler.h>

#include <d3dx11.h>
#include <dxgi1_2.h>

#include <d3d11_1.h>
#include <d3d9.h>
#include <d3dx9.h>

	// Included for use USES_CONVERSION (for A2W, W2A)
#include <AtlConv.h>

#pragma warning(disable:4995)
#include <fstream>
#include <sstream>

#include <windows.h>

// *********
	/// DXTK
#include "GeometricPrimitive.h"
#include <DirectXMath.h>

	/// For ThrowIfFailed
#include "../Src/PlatformHelpers.h"
// *********

using namespace DirectX;

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;
using namespace std;

#include "tinyxml2.h"
using namespace tinyxml2;

#include "imgui.h"

#include "d3dx11effect.h"

#if defined(DEBUG)
	#include <d3d11sdklayers.h>
	#pragma comment(lib, "Effects11d.lib")
	#pragma comment (lib, "assimp-vc141-mtd.lib")
	#pragma comment (lib, "tinyxml2d.lib")
#else
	#pragma comment(lib, "Effects11.lib")
	#pragma comment (lib, "assimp-vc141-mt.lib")
	#pragma comment (lib, "tinyxml2.lib")
#endif

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

#pragma comment (lib, "DxErr.lib")
#pragma comment (lib, "Dxgi.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "D3DCSX.lib")
#pragma comment (lib, "DirectXTK.lib")

	// For Debug!!!
#define MacroStr(x) #x
#define MacroStr2(x) MacroStr(x)
#define ToDo(desc) __pragma(message("###TODO MESSAGE: " __FILE__ "(" MacroStr2(__LINE__) "): " #desc))

#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#endif
#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#endif
#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
	#define SAFE_release(p) { if (p) { (p)->release(); (p) = nullptr; } }
#endif

#if defined(DEBUG)
	#if !defined(EngineTrace)
		#define EngineTrace(x) { hr = (x); if (FAILED(hr)) { DXUTTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); } }
	#endif
	#if !defined(V_RETURN)
		#define V_RETURN(x) { hr = (x); if (FAILED(hr)) { return DXUTTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); } }
	#endif
#else
	#if !defined(EngineTrace)
		#define EngineTrace(x) { hr = (x); }
	#endif
	#if !defined(V_RETURN)
		#define V_RETURN(x) { hr = (x); if (FAILED(hr)) { return hr; } }
	#endif
#endif

bool FindSubStr(wstring context, wstring const from);
bool FindSubStr(string context, string const from);

void replaceAll(wstring &context, wstring const &from, wstring const &to, bool OneTime = false, bool FindInEnd = false);
void replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also, bool OneTime = false, bool FindInEnd = false);

void replaceAll(string &context, string const &from, string const &to, bool OneTime = false, bool FindInEnd = false);
void replaceAll(string &context, string const &from, string const &to, string const &also, bool OneTime = false, bool FindInEnd = false);

void deleteWord(string &context, string const &what, bool OneTime = false, bool FindInEnd = false);
void deleteWord(string &context, char const what, char const OnWhat);
void deleteWord(string &context, string const start, string const end, bool OneTime = false, bool AlsoDeleteSpace = true);

enum ModeProcessString { UntilTheBegin = 0, UntilTheEnd };
void deleteWord(string &context, string const start, ModeProcessString const mode, bool FindInEnd = false, bool AlsoDeleteSpace = true);

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

#include <boost/format.hpp>