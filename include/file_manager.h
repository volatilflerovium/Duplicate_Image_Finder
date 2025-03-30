/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* FileManager class                                  				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <map>
#include <filesystem>
#include <functional>

#include <wx/string.h>
#include <wx/image.h>

//====================================================================

typedef std::function<bool(const std::string&, uint&)> CBK;

enum class SUBDIR
{
	IS_NEW=0,
	IS_SUB,
	IS_SUP,
	
};

//======================================================================

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
		static std::string setPath(const char* filePath);
		static bool fileExists(const char* filePath);
		static SUBDIR isSubdirectory(const std::string& dirTest, std::vector<std::string>& directories);
		static SUBDIR reduce(const std::string& dirTest, std::vector<std::string>& directories);

		static bool isFileSuported(const char* extension);

		static void saveSession(bool save){
			m_saveSession=save;
		}

		static bool isSessionSaved(){
			return m_saveSession;
		}
		
		static std::string iconsPath();

		static bool existPath(const char* fileName);
		static bool deleteFile(const char* filePath);
		static std::string dataVisualizationURL();
		static std::string dataVisualizationJson();
		
		static int crawler(const char* directoryPath);

		static size_t totalFiles();
		static void clearFilesMap();
		static void resetFilesMap();
		static void loopFilesMap(CBK cbk);
		static uint findKey(const std::string& file, uint id);
		static uint findKey(const std::string& file);

	private:
		static std::map<std::string, uint> m_fileMap;

		static std::map<wxString, bool> m_opencvSupported;
		static constexpr const char* c_separator=":"; 
		static bool m_saveSession;

		static std::string trim(const std::string& str);
		static std::string mkPath(const char* file);
};

//----------------------------------------------------------------------

inline size_t FileManager::totalFiles()
{
	return m_fileMap.size();
}

//----------------------------------------------------------------------

inline void FileManager::clearFilesMap()
{
	return m_fileMap.clear();
}

//----------------------------------------------------------------------

inline void FileManager::resetFilesMap()
{
	for(auto& [key, val] : m_fileMap){
		val=0;
	}
}

//----------------------------------------------------------------------

inline void FileManager::loopFilesMap(CBK cbk)
{
	for(auto& [key, val] : m_fileMap){
		if(!cbk(key, val)){
			break;
		}
	}
}


//----------------------------------------------------------------------
 
inline uint FileManager::findKey(const std::string& file)
{
	auto it=m_fileMap.find(file);
	if(it!=m_fileMap.end()){
		return it->second;
	}
	return 0;
}

//----------------------------------------------------------------------

inline uint FileManager::findKey(const std::string& file, uint id)
{
	auto it=m_fileMap.find(file);
	if(it!=m_fileMap.end()){
		if(it->second==0){
			it->second=id;
		}
		return it->second;
	}
	return 0;
}

//----------------------------------------------------------------------

inline bool FileManager::existPath(const char* fileName)
{
	std::error_code ec;
	return std::filesystem::exists(fileName, ec);
}

//----------------------------------------------------------------------

#endif
