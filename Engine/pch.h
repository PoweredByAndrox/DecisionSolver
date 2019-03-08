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

	// DXTK
#include <SimpleMath.h>
#include <DirectXMath.h>

	// Included for use USES_CONVERSION (or A2W, W2A)
#include <AtlConv.h>

#pragma warning( disable: 4995 )
#include <fstream>
#include <sstream>

#include <windows.h>

// *********
		/// DXTK
	#include "GeometricPrimitive.h"
		/// For ThrowIfFailed
	#include "../Src/PlatformHelpers.h"
// *********

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

#include "tinyxml2.h"
using namespace tinyxml2;

#if defined(Never)
	// Maybe this doesn't cause errors?
#define SPDLOG_TRACE_ON
#include "spdlog/spdlog.h"
using namespace spdlog;
#endif

#include "imgui.h"

#include "d3dx11effect.h"

#if defined(DEBUG) || defined(_DEBUG)
#include <d3d11sdklayers.h>
#endif

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "Effects11d.lib")
#else
#pragma comment(lib, "Effects11.lib")
#endif

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3dx9d.lib")

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

#pragma comment (lib, "DxErr.lib")
#pragma comment (lib, "Dxgi.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "D3DCSX.lib")

#pragma comment (lib, "assimp-vc140-mt.lib")

#pragma comment (lib, "DirectXTK.lib")
#pragma comment (lib, "DirectXTKAudioDX.lib")

	// For Debug!!!
#define MacroStr(x) #x
#define MacroStr2(x) MacroStr(x)
#define ToDo(desc) __pragma(message(__FILE__ "(" MacroStr2(__LINE__) ") :" #desc))

bool FindSubStr(wstring context, wstring const from);
bool FindSubStr(string context, string const from);

#include <foundation/PxVec3.h>
physx::PxVec3 ToPxVec3(Vector3 var);
Vector3 ToVec3(physx::PxVec3 var);

wstring &replaceAll(wstring &context, wstring const &from, wstring const &to);
wstring &replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also);

string &replaceAll(string &context, string const &from, string const &to);
string &replaceAll(string &context, string const &from, string const &to, string const &also);

void deleteWord(string &context, string const &what);
string deleteWord(string context, char const what, char const OnWhat);
string deleteWord(string context, string const start, string const end);

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

#include <boost/format.hpp>
wstring formatstr(const char *Buff, float X, float Y, float Z);