#pragma once
#include <vector>
#include <stdio.h> 
#include <direct.h>
#include <tchar.h>
#include <memory>

#include "boost/thread.hpp"
#include "boost/thread/scoped_thread.hpp"

#include <d3dcompiler.h>

#include "d3dx11.h"
#include <dxgi1_2.h>

#include <d3d11_1.h>
#include "DxErr.h"

	// Included for use USES_CONVERSION (for A2W, W2A)
#include <AtlConv.h>

#pragma warning(disable:4995)
#include <fstream>
#include <sstream>

#include <windows.h>
#include <mmiscapi.h>

// *********
	/// DXTK
#include "inc/GeometricPrimitive.h"
#include <DirectXMath.h>

using namespace DirectX;

#include <inc/SimpleMath.h>
using namespace DirectX::SimpleMath;
using namespace std;

#include "tinyxml2.h"
using namespace tinyxml2;

#include "d3dx11effect.h"

#if defined (DEBUG)
	#include <d3d11sdklayers.h>
#endif

	// For Debug!!!
#define MacroStr(x) #x
#define MacroStr2(x) MacroStr(x)
#define ToDo(desc) __pragma(message(__FILE__ "(" MacroStr2(__LINE__) "): " #desc))

#if !defined (SAFE_DELETE)
	#define SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#endif
#if !defined (SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#endif
#if !defined (SAFE_RELEASE)
	#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
	#define SAFE_release(p) { if (p) { (p)->release(); (p) = nullptr; } }
#endif

#if !defined (EngineTrace)
	#define EngineTrace(x) \
{ \
	HRESULT hr = (x); \
	if (FAILED(hr)) \
		return DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
} 
#endif
#if !defined (V_RETURN)
	#define V_RETURN(x) \
{ \
	HRESULT hr = (x); \
	if (FAILED(hr)) \
		return DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
}
#endif

bool FindSubStr(wstring context, wstring const from);
bool FindSubStr(string context, string const from);

void replaceAll(wstring &context, wstring const &from, wstring const &to, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);
void replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);

void replaceAll(string &context, string const &from, string const &to, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);
void replaceAll(string &context, string const &from, string const &to, string const &also, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);

void deleteWord(string &context, string const &what, bool OneTime = false, bool FindInEnd = false,
	bool AlsoDeleteSpace = false);
void deleteWord(string &context, char const what, char const OnWhat, bool AlsoDeleteSpace = false);
void deleteWord(string &context, string const start, string const end, bool OneTime = false,
	bool AlsoDeleteSpace = true);

enum ModeProcessString { UntilTheBegin = 0, UntilTheEnd };
void deleteWord(string &context, string const start, ModeProcessString const mode, bool FindInEnd = false,
	bool AlsoDeleteSpace = true);

void getFloat3Text(string context, string Char2Split, vector<float> &Float3);
void getTextFloat3(string &context, string Char2Split, vector<float> Float3);

Vector3 ConstrainToBoundary(Vector3 Pos, Vector3 Min, Vector3 Max);

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

#include <boost/format.hpp>

enum Type
{
	Normal = 0,
	Information,
	Error
};

void ParseText(string &Text, Type type);

const vector<string> Rus = {
"à", "á", "â", "ã", "ä", "å", "¸", "æ", "ç", "è", "é",
"ê", "ë", "ì", "í", "î", "ï", "ð", "ñ", "ò", "ó", "ô", "õ",
"ö", "÷", "ø", "ù", "ú", "û", "ü", "ý", "þ", "ÿ"
};

const vector<string> Eng = {
"a", "b", "v", "g", "d", "e", "yo", "zh", "z", "i", "j", "k",
"l", "m", "n", "o", "p", "r", "s", "t", "u", "f", "h",
"c", "ch", "sh", "shh", "'", "y", "._", "e-", "yu", "ya"
};

string ReplaceSymbols(string In);