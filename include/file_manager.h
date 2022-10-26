/*********************************************************************
* FileManager class                                  				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef PATH_MANAGER
#define PATH_MANAGER
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

#include <wx/string.h>
#include <wx/image.h>

#include "data.h"

class FileManager
{
	public:
		static const std::string c_APPLICATION_DIR;
		static const std::string c_IMG_BACKGROUND;
		static const std::string c_DIR_HIST;
		static const std::string c_SAVE_SETTING;
		static const std::string c_FILE_LIST;

		static void init();
		static void cleanUp(bool clearAll=true);
		static bool loadSettings();
		static int get(const char* setting);
		static std::string setPath(const char* filePath);
		static bool fileExists(const std::string& filePath);
		static SUBDIR isSubdirectory(const std::string& dirTest, std::vector<std::string>& directories);
		static void removeDuplicates(std::vector<std::string>& directories);
		static SUBDIR reduce(const std::string& dirTest, std::vector<std::string>& directories);
		static bool isSuported(const wxString& wxStr);
		static bool isWXsuported(const wxString& wxStr);
		static wxBitmapType getBitmapType(const wxString& wxStr);
		static void saveSession(bool save){
			m_saveSession=save;
		}

		static bool isSessionSaved(){
			return m_saveSession;
		}
		
		struct CnstChar
		{
			const char* m_cstr;
			CnstChar(const char* data)
			:m_cstr(std::move(data))
			{}
		};


	private:	
		static constexpr const char* c_separator=":"; 
		static std::map<CnstChar, int> m_settings;
		static std::string trim(const std::string& str);
		static bool m_saveSession;
		static std::string mkPath(const char* file);
		static std::map<wxString, wxBitmapType> m_filter;
		static std::map<wxString, bool> m_wxSupported;
};

//----------------------------------------------------------------------

inline bool operator<(const FileManager::CnstChar& data, const FileManager::CnstChar& other){
	return std::strcmp(data.m_cstr, other.m_cstr)<0;
}

//----------------------------------------------------------------------

inline bool operator>(const FileManager::CnstChar& data, const FileManager::CnstChar& other){
	return std::strcmp(data.m_cstr, other.m_cstr)>0;
}

//----------------------------------------------------------------------

inline bool operator==(const FileManager::CnstChar& data, const FileManager::CnstChar& other){
	return std::strcmp(other.m_cstr, data.m_cstr)==0;
}

#endif
