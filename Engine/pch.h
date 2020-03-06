/**
 * \file	pch.h.
 *
 * \brief	Declares the pch class
 */

///< .
#pragma once
#include <vector>
#include <stdio.h> 
#include <direct.h>
#include <tchar.h>
#include <memory>
#include <thread>
#include <chrono>

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

#if defined (_DEBUG)
	#include <d3d11sdklayers.h>
#endif

	// For Debug!!!
#define MacroStr(x) #x
#define MacroStr2(x) MacroStr(x)

/**
 * \def	ToDo(desc)
 *
 * \brief	A macro that defines to do
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	desc	Text
 */

#define ToDo(desc) __pragma(message(__FILE__ "(" MacroStr2(__LINE__) "): " #desc))

#if !defined (SAFE_DELETE)

	/**
	 * \def	SAFE_DELETE(p)
	 *
	 * \brief	A macro that defines safe delete
	 *
	 * \author	PBAX
	 * \date	17.02.2020
	 *
	 * \param 	p	A void to process.
	 */

	#define SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#endif
#if !defined (SAFE_DELETE_ARRAY)

	/**
	 * \def	SAFE_DELETE_ARRAY(p)
	 *
	 * \brief	A macro that defines safe delete array
	 *
	 * \author	PBAX
	 * \date	17.02.2020
	 *
	 * \param 	p	A void to process.
	 */

	#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }
#endif
#if !defined (SAFE_RELEASE)

	/**
	 * \def	SAFE_RELEASE(p)
	 *
	 * \brief	A macro that defines safe Release
	 *
	 * \author	PBAX
	 * \date	17.02.2020
	 *
	 * \param 	p	A void to process.
	 */

	#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

#if !defined (SAFE_release)

/**
 * \def	SAFE_release(p)
 *
 * \brief	A macro that defines safe release
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	p	A void to process.
 */

	#define SAFE_release(p) { if (p) { (p)->release(); (p) = nullptr; } }
#endif

#if !defined (EngineTrace)

	/**
	 * \def	EngineTrace(x)
	 *
	 * \brief	A macro that defines engine trace
	 *
	 * \author	PBAX
	 * \date	17.02.2020
	 *
	 * \param 	x	A void to process.
	 */

	#define EngineTrace(x) \
	{ \
		HRESULT hr = (x); \
		if (FAILED(hr)) \
			return DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
	} 
#endif

/**
 * \fn	bool FindSubStr(wstring context, wstring const from);
 *
 * \brief	Searches for the first sub Wstring
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	context	The context.
 * \param 	from   	Source for the.
 *
 * \returns	True if it succeeds, false if it fails.
 */

bool FindSubStr(wstring context, wstring const from);

/**
 * \fn	bool FindSubStr(string context, string const from);
 *
 * \brief	Searches for the first sub Astring
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	context	The context.
 * \param 	from   	Source for the.
 *
 * \returns	True if it succeeds, false if it fails.
 */

bool FindSubStr(string context, string const from);

/**
 * \fn	void replaceAll(wstring &context, wstring const &from, wstring const &to, bool OneTime = false,
 * 		 bool FindInEnd = false, bool AlsoDeleteSpace = false);
 *
 * \brief	Replace all (Astring)
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	from		   	Source for the.
 * \param 		  	to			   	to.
 * \param 		  	OneTime		   	(Optional) True to one time.
 * \param 		  	FindInEnd	   	(Optional) True to find in end.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void replaceAll(wstring &context, wstring const &from, wstring const &to, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);

/**
 * \fn	void replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also,
 * 		 bool OneTime = false, bool FindInEnd = false, bool AlsoDeleteSpace = false);
 *
 * \brief	Replace all (Wstring)
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	from		   	Source for the.
 * \param 		  	to			   	to.
 * \param 		  	also		   	The also.
 * \param 		  	OneTime		   	(Optional) True to one time.
 * \param 		  	FindInEnd	   	(Optional) True to find in end.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void replaceAll(wstring &context, wstring const &from, wstring const &to, wstring const &also, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);

/**
 * \fn	void replaceAll(string &context, string const &from, string const &to, bool OneTime = false,
 * 		 bool FindInEnd = false, bool AlsoDeleteSpace = false);
 *
 * \brief	Replace all
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	from		   	Source for the.
 * \param 		  	to			   	to.
 * \param 		  	OneTime		   	(Optional) True to one time.
 * \param 		  	FindInEnd	   	(Optional) True to find in end.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void replaceAll(string &context, string const &from, string const &to, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);

/**
 * \fn	void replaceAll(string &context, string const &from, string const &to, string const &also,
 * 		 bool OneTime = false, bool FindInEnd = false, bool AlsoDeleteSpace = false);
 *
 * \brief	Replace all
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	from		   	Source for the.
 * \param 		  	to			   	to.
 * \param 		  	also		   	The also.
 * \param 		  	OneTime		   	(Optional) True to one time.
 * \param 		  	FindInEnd	   	(Optional) True to find in end.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void replaceAll(string &context, string const &from, string const &to, string const &also, bool OneTime = false,
	bool FindInEnd = false, bool AlsoDeleteSpace = false);

/**
 * \fn	void deleteWord(string &context, string const &what, bool OneTime = false,
 * 		 bool FindInEnd = false, bool AlsoDeleteSpace = false);
 *
 * \brief	Deletes the word
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	what		   	The what.
 * \param 		  	OneTime		   	(Optional) True to one time.
 * \param 		  	FindInEnd	   	(Optional) True to find in end.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void deleteWord(string &context, string const &what, bool OneTime = false, bool FindInEnd = false,
	bool AlsoDeleteSpace = false);

/**
 * \fn	void deleteWord(string &context, char const what, char const OnWhat, bool AlsoDeleteSpace = false);
 *
 * \brief	Deletes the word
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	what		   	The what.
 * \param 		  	OnWhat		   	The on what.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void deleteWord(string &context, char const what, char const OnWhat, bool AlsoDeleteSpace = false);

/**
 * \fn	void deleteWord(string &context, string const start, string const end,
 * 		 bool OneTime = false, bool AlsoDeleteSpace = true);
 *
 * \brief	Deletes the word
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	start		   	The start.
 * \param 		  	end			   	The end.
 * \param 		  	OneTime		   	(Optional) True to one time.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void deleteWord(string &context, string const start, string const end, bool OneTime = false,
	bool AlsoDeleteSpace = true);

/**
 * \enum	ModeProcessString
 *
 * \brief	Values that represent mode for deleting Word Function
 */

enum ModeProcessString { UntilTheBegin = 0, UntilTheEnd };

/**
 * \fn	void deleteWord(string &context, string const start, ModeProcessString const mode,
 * 		 bool FindInEnd = false, bool AlsoDeleteSpace = true);
 *
 * \brief	Deletes the word
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context		   	The context.
 * \param 		  	start		   	The start.
 * \param 		  	mode		   	The mode.
 * \param 		  	FindInEnd	   	(Optional) True to find in end.
 * \param 		  	AlsoDeleteSpace	(Optional) True to also delete space.
 */

void deleteWord(string &context, string const start, ModeProcessString const mode, bool FindInEnd = false,
	bool AlsoDeleteSpace = true);

/**
 * \fn	void getFloat3Text(string context, string Char2Split, vector<float> &Float3);
 *
 * \brief	Function That Converts From String To Vector3 (Massive)
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 		  	context   	Source string
 * \param 		  	Char2Split	Char that will using in split
 * \param [in,out]	Float3	  	Return MASSIVE converted coords
 */

void getFloat3Text(string context, string Char2Split, vector<float> &Float3);

/**
 * \fn	void getTextFloat3(string &context, string Char2Split, vector<float> Float3);
 *
 * \brief	Function That Converts Vector3 (Massive) To String
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	context   	Return STRING that converted coords.
 * \param 		  	Char2Split	Char that will using in split.
 * \param 		  	Float3	  	Massive that will using for convert coords
 */

void getTextFloat3(string &context, string Char2Split, vector<float> Float3);

/**
 * \fn	Vector3 ConstrainToBoundary(Vector3 Pos, Vector3 Min, Vector3 Max);
 *
 * \brief	Constrain to boundary Vector3 Pos
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	Pos	Position that will be Constrained
 * \param 	Min	Vector3 minimum pos
 * \param 	Max	Vector3 maximum pos
 *
 * \returns	Constrained To Boundary Vector3
 */

Vector3 ConstrainToBoundary(Vector3 Pos, Vector3 Min, Vector3 Max);

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

#include <boost/format.hpp>

/**
 * \enum	Type
 *
 * \brief	 Values that using in Loggin File And UI Console Text Colored
 */

enum Type
{
	Normal = 0,
	Information,
	Error
};

/**
 * \fn	void ParseText(string &Text, Type type);
 *
 * \brief	Parse text
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param [in,out]	Text	The text.
 * \param 		  	type	The type.
 */

void ParseText(string &Text, Type type);

/** \brief	Massive Cyrillic Characters For Converted */
const vector<string> Rus = {
"а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й",
"к", "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф", "х",
"ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"
};

/** \brief	Massive English Characters For Converted */
const vector<string> Eng = {
"a", "b", "v", "g", "d", "e", "yo", "zh", "z", "i", "j", "k",
"l", "m", "n", "o", "p", "r", "s", "t", "u", "f", "h",
"c", "ch", "sh", "shh", "'", "y", "._", "e-", "yu", "ya"
};

/**
 * \fn	string ReplaceSymbols(string In);
 *
 * \brief	Replace Cyrillic Characters To English
 *
 * \author	PBAX
 * \date	17.02.2020
 *
 * \param 	In	Source String.
 *
 * \returns	Converted String.
 */

string ReplaceSymbols(string In);

/**
// End of pch.h
 */