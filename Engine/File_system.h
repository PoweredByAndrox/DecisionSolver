/**
 * \file	File_system.h.
 *
 * \brief	Declares the file system class
 */

#pragma once
#if !defined(__FILE_SYSTEM_H__)
#define __FILE_SYSTEM_H__
#include "pch.h"

#include <fstream>
#include <Boost/filesystem.hpp>
#include <algorithm>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <Boost/iostreams/filtering_streambuf.hpp>
#include <Boost/iostreams/copy.hpp>
#include <Boost/iostreams/filter/gzip.hpp>
using gz_com = boost::iostreams::gzip_compressor;
using gz_decom = boost::iostreams::gzip_decompressor;

using namespace std;
using namespace boost::filesystem;
using namespace boost::iostreams;

/**
 * \enum	_TypeOfFile
 *
 * \brief	Values that represent type of files
 */

enum _TypeOfFile { MODELS = 1, TEXTURES, LEVELS, DIALOGS, SOUNDS, SHADERS, UIS, SCRIPTS, FONTS, NONE };

/**
 * \class	File_system
 *
 * \brief	A file system.
 *
 * \author	PBAX
 * \date	25.02.2020
 */

class File_system
{
private:
	struct ProjectFile;
	/**
	 * \struct	AllFile
	 *
	 * \brief	Struct Of Files.
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 */

	struct File
		{
			File() {}
			File(wstring PathW, wstring ExtW, wstring FileW, size_t Size, _TypeOfFile TypeOfFile): PathW(PathW), ExtW(ExtW), FileW(FileW),
				Size(Size), TypeOfFile(TypeOfFile) {}

			File(string PathA, string ExtA, string FileA, size_t Size, _TypeOfFile TypeOfFile): PathA(PathA), ExtA(ExtA), FileA(FileA),
				Size(Size), TypeOfFile(TypeOfFile) {}

			wstring PathW = L"", ExtW = L"", FileW = L"";
			// Full Path To Required File
			string PathA = "", ExtA = "", FileA = "";

			size_t Size = 0;

			_TypeOfFile TypeOfFile;
		};
	vector<pair<shared_ptr<File>, string/*IDPath*/>>
		Models,
		Textures,
		Levels,
		Dialogs,
		Sounds,
		Shaders,
		Uis,
		Scripts,
		Fonts,
		None;

	/** \brief	The Project For SDK */
	shared_ptr<ProjectFile> Project = make_shared<ProjectFile>();

	/**
	 * \struct	ProjectFile
	 *
	 * \brief	Struct Project File For SDK.
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 */

	struct ProjectFile
	{
		ProjectFile() {}
		~ProjectFile() {}

		/**
		 * \fn	ProjectFile(path CurrProj)
		 *
		 * \brief	Constructor (Path To Current Project)
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \param 	CurrProj	The curr project.
		 */

		ProjectFile(path CurrProj) { SetCurProject(CurrProj); }

		/**
		 * \fn	auto GetRecent()
		 *
		 * \brief	Get Recent Files
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \returns	Recent Files.
		 */

		auto GetRecent() { return RecentFiles; }

		/**
		 * \fn	auto GetCurrentProject()
		 *
		 * \brief	Gets current project
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \returns	The current project.
		 */

		auto GetCurrentProject() { return CurrentProj; }

		/**
		 * \fn	void OpenFile(path file);
		 *
		 * \brief	Open Project File
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \param 	file	The file.
		 */

		HRESULT OpenFile(path file);

		/**
		 * \fn	void SaveFile(path file);
		 *
		 * \brief	Save Project File
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \param 	file	The file.
		 */

		 // "Save As" Means Save In Another File Path
		void SaveFile(path file);

		/**
		 * \fn	void SaveCurrProj();
		 *
		 * \brief	Saves Current Project
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 */

		void SaveProj(path File = "");

		/**
		 * \fn	void SetCurProject(path File);
		 *
		 * \brief	Sets current project
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \param 	File	The file.
		 */

		void SetCurProject(path File);

		/**
		 * \fn	void CheckForSameFile(path Path);
		 *
		 * \brief	Check for same file For Recent Sorting Files (And Reorder Files)
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \param 	Path	Full pathname of the file.
		 */

		void CheckForSameFile(path Path);
	private:

		/**
		 * \fn	void Resort(bool Greater = true);
		 *
		 * \brief	Resorts the given greater
		 *
		 * \author	PBAX
		 * \date	29.02.2020
		 *
		 * \param 	Greater	(Optional) True to greater.
		 */

		void Resort(bool Greater = true);
		shared_ptr<boost::filesystem::ofstream> ProjFile;
		path CurrentProj = path();
		list<pair<int, path>> RecentFiles = {};
	};
public:
	/**
	 * \fn	File_system::File_system();
	 *
	 * \brief	Default Ctor Which Scan Files And Get Main Path
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 */

	File_system();
	~File_system() {}

	/**
	 * \fn	void File_system::ScanFiles();
	 *
	 * \brief	Scan Files And Add To Engine
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 */

	void ScanFiles();

	/**
	 * \fn	void File_system::RescanFilesByType(_TypeOfFile T);
	 *
	 * \brief	Rescan Files By Type
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 *
	 * \param 	T	A _TypeOfFile to process.
	 */

	void RescanFilesByType(_TypeOfFile T);

	/**
	 * \fn	shared_ptr<AllFile::File> File_system::GetFile(path File);
	 *
	 * \brief	Gets a file
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 *
	 * \param 	File	The file.
	 *
	 * \returns	The file.
	 */

	shared_ptr<File> GetFile(path File);

	/**
	 * \fn	shared_ptr<AllFile::File> File_system::AddFile(path File);
	 *
	 * \brief	Adds a file
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 *
	 * \param 	File	The file.
	 *
	 * \returns	A shared_ptr&lt;AllFile::File&gt;
	 */

	shared_ptr<File> AddFile(path File);

	/**
	 * \fn	vector<pair<shared_ptr<AllFile::File>, string>> File_system::GetFileByType(_TypeOfFile T);
	 *
	 * \brief	Get File By Type
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 *
	 * \param 	T	A _TypeOfFile to process.
	 *
	 * \returns	The file by type.
	 */

	vector<pair<shared_ptr<File>, string>> GetFileByType(_TypeOfFile T);

	/**
	 * \fn	vector<wstring> File_system::getFilesInFolderW(wstring Folder);
	 *
	 * \brief	Get Massive Files In Resource Folder
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	Folder	Pathname of the folder. WSTRING (Wide)
	 *
	 * \returns	Massive Files Of Needed Folder.
	 */

	vector<wstring> getFilesInFolder(wstring Folder);

	/**
	 * \fn	vector<wstring> File_system::getFilesInFolder(wstring Folder, bool Recursive = false, bool onlyFile = false);
	 *
	 * \brief	Get Massive Files In Resource Folder
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	Folder   	Pathname of the folder. WSTRING (Wide)
	 * \param 	Recursive	(Optional) True to process recursively, false to process locally only.
	 * \param 	onlyFile 	(Optional) True to only file.
	 *
	 * \returns	Massive Files Of Needed Folder.
	 */

	vector<wstring> getFilesInFolder(wstring Folder, bool Recursive = false, bool onlyFile = false);

	/**
	 * \fn	vector<string> File_system::getFilesInFolder(string Folder, bool Recursive, bool onlyFile);
	 *
	 * \brief	Gets files in folder
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	Folder   	Pathname of the folder. STRING (Ansi)
	 * \param 	Recursive	True to process recursively, false to process locally only.
	 * \param 	onlyFile 	True to only file.
	 *
	 * \returns	The files in folder.
	 */

	vector<string> getFilesInFolder(string Folder, bool Recursive, bool onlyFile);

	/**
	 * \fn	vector<string> File_system::getFilesInFolder(string Folder);
	 *
	 * \brief	Get Massive Files In Resource Folder
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	Folder	Pathname of the folder.	STRING (Ansi)
	 *
	 * \returns	Massive Files Of Needed Folder.
	 */

	vector<string> getFilesInFolder(string Folder);

	/**
	 * \fn	string File_system::getDataFromFile(string File, string start = "<!--", string end = "-->");
	 *
	 * \brief	Get Data (In String) From File
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	File 	Needed File.
	 * \param 	start	(Optional) The start.
	 * \param 	end  	(Optional) The end.
	 *
	 * \returns	String Data File.
	 *
	 * ### param 	LineByline	True to line byline.
	 */

	string getDataFromFile(string File, string start = "<!--", string end = "-->");

	/**
	 * \fn	vector<string> File_system::getDataFromFileVector(string File, bool LineByline);
	 *
	 * \brief	Get Data (In Massive String) From File
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	File	  	Needed File.
	 * \param 	LineByline	True To Read Line By Line.
	 *
	 * \returns	Massive Strings File.
	 */

	vector<string> getDataFromFileVector(string File, bool LineByline);

	/**
	 * \fn	bool File_system::ReadFileMemory(LPCSTR filename, size_t *FileSize, void **FilePtr);
	 *
	 * \brief	Reads file memory
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 		  	filename	Filename of the file. Ansi
	 * \param [in,out]	FileSize	If non-null, size of the file.
	 * \param [in,out]	FilePtr 	If non-null, the file pointer.
	 *
	 * \returns	True if it succeeds, false if it fails.
	 */

	static bool ReadFileMemory(LPCSTR filename, size_t &FileSize, vector<BYTE> &FilePtr);

	/**
	 * \fn	_TypeOfFile File_system::GetTypeFileByExt(path File);
	 *
	 * \brief	Constructor
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	File	The file.
	 */

	_TypeOfFile GetTypeFileByExt(path File);

	/**
	 * \fn	shared_ptr<File> File_system::GetFileByPath(path File);
	 *
	 * \brief	Gets file by path
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	File	The file.
	 *
	 * \returns	The file by path.
	 */

	shared_ptr<File> GetFileByPath(path File);

	// ********************************
		// Functions For Logging File
	static void CreateLog();
	static void AddTextToLog(string Text, Type type);
	static void OpenLog();
	static void ClearLogs();
	// ********************************

	/**
	 * \fn	void File_system::CreateProjectFile(string FName);
	 *
	 * \brief	Creates Project File (SDK)
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	FName	The name.
	 */

	void CreateProjectFile(string FName);

	auto getLogFName() { return LogFName; }

	/**
	 * \fn	auto static File_system::GetCurrentPath()
	 *
	 * \brief	Gets current path
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \returns	The current path.
	 */

	auto static GetCurrentPath() { return WorkDir.generic_string() + 
		((WorkDir.generic_string().back() == '/') ? "" : "/"); }

	/**
	 * \fn	string File_system::getPathFromType(_TypeOfFile T);
	 *
	 * \brief	Gets path from type
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	T	A _TypeOfFile to process.
	 *
	 * \returns	The path from type.
	 */

	string getPathFromType(_TypeOfFile T);

	/**
	 * \fn	auto File_system::GetProject()
	 *
	 * \brief	Get The Project File
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \returns	Project File
	 */

	auto GetProject() { return Project; }

	/**
	 * \fn	void File_system::CreateProject(path CurrFile)
	 *
	 * \brief	Create Project
	 *
	 * \author	PBAX
	 * \date	29.02.2020
	 *
	 * \param 	CurrFile The Current Tile.
	 */

	void CreateProject(path CurrFile) { Project = make_shared<ProjectFile>(CurrFile); }
	boost::property_tree::ptree LoadSettingsFile();
	void SaveSettings(vector<pair<string, string>> ToFile);

	bool compressFile(path data, string where = "");
	bool decompressFile(path data, string where = "");
protected:
	/** \brief	The work dir */
	static path WorkDir;
	/** \brief	The work dir resources Ansi */
	string WorkDirSourcesA;
	/** \brief	The work dir resources Wide */
	wstring WorkDirSourcesW;

	/** \brief	Name of the log file */
	static path LogFName;

	/**
	 * \fn	shared_ptr<File_system::AllFile::File> File_system::Find(path File);
	 *
	 * \brief	Searches for the first match for the given path
	 *
	 * \author	PBAX
	 * \date	25.02.2020
	 *
	 * \param 	File	The file.
	 *
	 * \returns	A shared_ptr&lt;File_system::AllFile::File&gt;
	 */

	shared_ptr<File_system::File> Find(path File);
};
#endif // !__FILE_SYSTEM_H__
