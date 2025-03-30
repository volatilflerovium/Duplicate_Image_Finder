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
#include "file_manager.h"
#include "data_logger.h"
#include "settings_manager.h"

#include <algorithm>
#include <fstream>
#include <cstring>

#include <pwd.h>

#include "utilities/debug_utils.h"
#include "utilities/profiler.h"

//====================================================================

const std::string FileManager::c_APPLICATION_DIR(FileManager::mkPath("applicationDir"));
const std::string FileManager::c_IMG_BACKGROUND(FileManager::mkPath("imgBackground"));
const std::string FileManager::c_DIR_HIST(FileManager::mkPath("hist"));
const std::string FileManager::c_SAVE_SETTING(FileManager::mkPath("settings"));
const std::string FileManager::c_FILE_LIST(FileManager::mkPath("list"));
bool FileManager::m_saveSession(false);

std::map<std::string, uint> FileManager::m_fileMap;

std::map<wxString, bool> FileManager::m_opencvSupported
{
	{"jpeg", true},
	{"JPEG", true},
	{"jpg", true},
	{"JPG", true},
	{"jpe", true},
	{"JPE", true},
	{"png", true},
	{"PNG", true},
	{"webp", false},
	{"WEBP", false},
	{"bmp", true},
	{"BMP", true},
	{"tiff", true},
	{"TIFF", true},
	{"tif", false},
	{"TIF", false},
	{"jp2", false},
	{"JP2", false},
	{"pic", false},
	{"PIC", false},
	{"ppm", false},
	{"PPM", false},
	{"pxm", false},
	{"PXM", false},
	{"pnm", true},
	{"PNM", true},
	{"pfm", false},
	{"PFM", false},
	{"sr", false},
	{"SR", false},
	{"ras", false},
	{"RAS", false},
	{"exr", false},
	{"EXR", false},
	{"hdr", false},
	{"HDR", false},
	{"pbm", false},
	{"PBM", false},
	{"pgm", false},
	{"PGM", false},
	{"dip", false},
	{"DIP", false},
	{".jpeg", true},
	{".JPEG", true},
	{".jpg", true},
	{".JPG", true},
	{".jpe", true},
	{".JPE", true},
	{".png", true},
	{".PNG", true},
	{".webp", false},
	{".WEBP", false},
	{".bmp", true},
	{".BMP", true},
	{".tiff", true},
	{".TIFF", true},
	{".tif", false},
	{".TIF", false},
	{".jp2", false},
	{".JP2", false},
	{".pic", false},
	{".PIC", false},
	{".ppm", false},
	{".PPM", false},
	{".pxm", false},
	{".PXM", false},
	{".pnm", true},
	{".PNM", true},
	{".pfm", false},
	{".PFM", false},
	{".sr", false},
	{".SR", false},
	{".ras", false},
	{".RAS", false},
	{".exr", false},
	{".EXR", false},
	{".hdr", false},
	{".HDR", false},
	{".pbm", false},
	{".PBM", false},
	{".pgm", false},
	{".PGM", false},
	{".dip", false},
	{".DIP", false}
};

//----------------------------------------------------------------------

std::string FileManager::mkPath(const char* file)
{
	std::string homeDir(getenv("HOME"));

	if(homeDir == std::string("") || !fileExists(homeDir.c_str())) {
		homeDir = std::string(getpwuid(getuid())->pw_dir);
	
		if(!fileExists(homeDir.c_str())){
			homeDir=std::string("/tmp/data");
		}
	}
	
	std::string path=homeDir;
	path.append("/.DuplicateImgFinder_data/");

	if(std::strlen(file)==14 && std::memcmp(file, "applicationDir", 14)==0){
		return path;
	}

	if(std::strlen(file)==4 && std::memcmp(file, "list", 4)==0){
		path.append("list");
		return path;
	}
	
	if(std::strlen(file)==4 && std::memcmp(file, "hist", 4)==0){
		path.append("hists_data/");
		return path;
	}
	
	if(std::strlen(file)==8 && std::memcmp(file, "settings", 8)==0){
		path.append("settings");
		return path;
	}

	if(std::strlen(file)==13 && std::memcmp(file, "imgBackground", 13)==0){
		path.append("imgBackground.png");
		return path;
	}
	
	throw std::runtime_error("Path not found!");
	
	return "/tmp/data";
}

//----------------------------------------------------------------------

void FileManager::cleanUp(bool clearAll)
{
	if(fileExists(c_DIR_HIST.c_str())){
		remove_all(std::filesystem::path{c_DIR_HIST.c_str()});
		std::filesystem::create_directory(std::filesystem::path{c_DIR_HIST.c_str()});
	}
	
	if(fileExists(c_FILE_LIST.c_str())){
		remove(std::filesystem::path{c_FILE_LIST.c_str()});
	}
}

//----------------------------------------------------------------------

bool FileManager::deleteFile(const char* filePath)
{
	std::string cmd="gio trash \"";
	cmd.append(filePath);
	cmd.append("\"");
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (!pipe) {
		return false;
	}
	return true;
}

//----------------------------------------------------------------------

bool FileManager::fileExists(const char* filePath)
{	
	std::ifstream file;
	file.open(filePath);
	if(!file.is_open()){
		return false;
	}
	file.close();
	return true;
}

//----------------------------------------------------------------------
	
void FileManager::init()
{
	if(!fileExists(c_APPLICATION_DIR.c_str())){
		setPath(c_APPLICATION_DIR.c_str());
	}

	if(!fileExists(c_DIR_HIST.c_str())){
		setPath(c_DIR_HIST.c_str());
	}

}

//----------------------------------------------------------------------

bool FileManager::isFileSuported(const char* extension)
{
	auto const& support=m_opencvSupported.find(extension);
	if(support!=m_opencvSupported.end()){
		return true;
	}
	return false;
}

//----------------------------------------------------------------------

std::string FileManager::trim(const std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if(std::string::npos == first){
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

//----------------------------------------------------------------------

std::string FileManager::setPath(const char* filePath)
{
	char buffer[254];
	int t=strlen(filePath);
	std::memcpy(buffer, filePath, t*sizeof(char));
	buffer[t]='\0';
	char pos = '/';
	char* ptr = strrchr(buffer, pos); 
	if(ptr){
		int r=ptr-buffer+1;
		char path[r];
		std::memcpy(path, buffer, r-1);
		path[r-1]='\0';
		
		std::filesystem::create_directory(std::filesystem::path{path});
	}
	return filePath;
}

//----------------------------------------------------------------------

std::string FileManager::dataVisualizationJson()
{
	std::string filePath="/tmp";
	filePath.append("/.DuplicatedImgNetworkData.js");
	return filePath;
}

//----------------------------------------------------------------------

std::string FileManager::dataVisualizationURL()
{
	std::string filePath="file:";
	 
	#ifdef DEBUG
	filePath.append(DEBUG_DIR); // define in CMakeLists
	#else
	if(getenv("APPDIR")){
		filePath.append(getenv("APPDIR"));
		filePath.append("/usr/share");
	}
	else{
		filePath.append("/tmp/usr/share");
	}
	#endif
	filePath.append("/data_visualization/Vis_Network.html");

	return filePath;
}

//----------------------------------------------------------------------

std::string FileManager::iconsPath()
{
	std::string filePath="";
	#ifdef DEBUG
		filePath=DEBUG_DIR; // define in CMakeLists
	#else
		if(getenv("APPDIR")){
			filePath=getenv("APPDIR");
		}
		else{
			filePath="/tmp/";
		}
		filePath.append("/usr/share");
	#endif
	
	filePath.append("/icons/");
	return filePath;
}

//----------------------------------------------------------------------

SUBDIR FileManager::isSubdirectory(const std::string& dirTest, std::vector<std::string>& directories)
{
	for(std::string& directory : directories){
		if(directory.find(dirTest)==0){
			if(directory.find_last_of("/")==dirTest.length()){
				return SUBDIR::IS_SUP;
			}
		}
		else if(dirTest.find(directory)==0){
			if(dirTest.find_last_of("/")==directory.length()){
				return SUBDIR::IS_SUB;
			}
		}
	}

	return SUBDIR::IS_NEW;
}

//----------------------------------------------------------------------

SUBDIR FileManager::reduce(const std::string& dirTest, std::vector<std::string>& directories)
{
	bool a=false;
	for(std::string& directory : directories){
		if(directory==dirTest){
			return SUBDIR::IS_SUB;
		}
		if(directory.find(dirTest)==0){
			if(directory.find_last_of("/")==dirTest.length()){
				directory=dirTest;
				a=true;
			}
		}
		else if(dirTest.find(directory)==0){
			if(dirTest.find_last_of("/")==directory.length()){
				return SUBDIR::IS_SUB;
			}
		}
	}

	if(a){
		std::sort(directories.begin(), directories.end()); 
		auto last = std::unique(directories.begin(), directories.end());
		directories.erase(last, directories.end());
		
		return SUBDIR::IS_SUP;
	}

	return SUBDIR::IS_NEW;
}

//----------------------------------------------------------------------

int FileManager::crawler(const char* directoryPath)//, std::map<std::string, uint>& files)
{
	std::error_code ec;
	int count=0;
	std::filesystem::current_path(directoryPath, ec);
	for(auto const& dirEntry : std::filesystem::recursive_directory_iterator(directoryPath, ec)){
		if(dirEntry.is_regular_file()){
			if(FileManager::isFileSuported(dirEntry.path().extension().c_str())){
				if(m_fileMap.end()==m_fileMap.find(dirEntry.path().c_str())){
					m_fileMap.emplace(dirEntry.path().c_str(), 0);
					count++;
				}
			}
		}
	}
	return count;
}

//----------------------------------------------------------------------
