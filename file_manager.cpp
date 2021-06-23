#include <cstdio>
#include <filesystem>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "file_manager.h"
#include "data_logger.h"

const std::string FileManager::c_APPLICATION_DIR(FileManager::mkPath("applicationDir"));
const std::string FileManager::c_IMG_BACKGROUND(FileManager::mkPath("imgBackground"));
const std::string FileManager::c_DIR_HIST(FileManager::mkPath("hist"));
const std::string FileManager::c_SAVE_SETTING(FileManager::mkPath("settings"));
const std::string FileManager::c_FILE_LIST(FileManager::mkPath("list"));
bool FileManager::m_saveSession(false);
std::map<wxString, wxBitmapType> FileManager::m_filter;
std::map<FileManager::CnstChar, int> FileManager::m_settings;
//----------------------------------------------------------------------

std::string FileManager::mkPath(const char* file){
	std::string homeDir(getenv("HOME"));

	if(homeDir == std::string("") || !fileExists(homeDir)) 
	{
		homeDir = std::string(getpwuid(getuid())->pw_dir);
	
		if(!fileExists(homeDir)){
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

void FileManager::cleanUp(bool clearAll){
	if(fileExists(c_DIR_HIST)){
		remove_all(std::filesystem::path{c_DIR_HIST.c_str()});
		std::filesystem::create_directory(std::filesystem::path{c_DIR_HIST.c_str()});//, 0777);
	}
	
	if(fileExists(c_FILE_LIST)){
		remove(std::filesystem::path{c_FILE_LIST.c_str()});
	}

	if(clearAll){
		if(fileExists(c_SAVE_SETTING)){
			remove(std::filesystem::path{c_SAVE_SETTING.c_str()});
		}
	}
}

//----------------------------------------------------------------------

bool FileManager::fileExists(const std::string& filePath){
	
	std::ifstream file;
	file.open(filePath);
	if(!file.is_open()){
		return false;
	}
	file.close();
	return true;
}

//----------------------------------------------------------------------
	
void FileManager::init(){
	
	m_filter.emplace("jpeg", wxBITMAP_TYPE_JPEG);
	m_filter.emplace("JPEG", wxBITMAP_TYPE_JPEG);
	m_filter.emplace("jpg", wxBITMAP_TYPE_JPEG);
	m_filter.emplace("JPG", wxBITMAP_TYPE_JPEG);
	m_filter.emplace("jpe", wxBITMAP_TYPE_JPEG);
	m_filter.emplace("JPE", wxBITMAP_TYPE_JPEG);
	m_filter.emplace("png", wxBITMAP_TYPE_PNG);
	m_filter.emplace("PNG", wxBITMAP_TYPE_PNG);
	m_filter.emplace("webp", wxBITMAP_TYPE_ANY);
	m_filter.emplace("WEBP", wxBITMAP_TYPE_ANY);
	m_filter.emplace("bmp", wxBITMAP_TYPE_BMP);
	m_filter.emplace("BMP", wxBITMAP_TYPE_BMP);
	m_filter.emplace("tiff", wxBITMAP_TYPE_TIFF);
	m_filter.emplace("TIFF", wxBITMAP_TYPE_TIFF);
	m_filter.emplace("tif", wxBITMAP_TYPE_ANY);
	m_filter.emplace("TIF", wxBITMAP_TYPE_ANY);
	m_filter.emplace("jp2", wxBITMAP_TYPE_ANY);
	m_filter.emplace("JP2", wxBITMAP_TYPE_ANY);
	m_filter.emplace("pic", wxBITMAP_TYPE_ANY);
	m_filter.emplace("PIC", wxBITMAP_TYPE_ANY);
	m_filter.emplace("ppm", wxBITMAP_TYPE_ANY);
	m_filter.emplace("PPM", wxBITMAP_TYPE_ANY);
	m_filter.emplace("pxm", wxBITMAP_TYPE_ANY);
	m_filter.emplace("PXM", wxBITMAP_TYPE_ANY);
	m_filter.emplace("pnm", wxBITMAP_TYPE_PNM);
	m_filter.emplace("PNM", wxBITMAP_TYPE_PNM);
	m_filter.emplace("pfm", wxBITMAP_TYPE_ANY);
	m_filter.emplace("PFM", wxBITMAP_TYPE_ANY);
	m_filter.emplace("sr", wxBITMAP_TYPE_ANY);
	m_filter.emplace("SR", wxBITMAP_TYPE_ANY);
	m_filter.emplace("ras", wxBITMAP_TYPE_ANY);
	m_filter.emplace("RAS", wxBITMAP_TYPE_ANY);
	m_filter.emplace("exr", wxBITMAP_TYPE_ANY);
	m_filter.emplace("EXR", wxBITMAP_TYPE_ANY);
	m_filter.emplace("hdr", wxBITMAP_TYPE_ANY);
	m_filter.emplace("HDR", wxBITMAP_TYPE_ANY);
	m_filter.emplace("pbm", wxBITMAP_TYPE_ANY);
	m_filter.emplace("PBM", wxBITMAP_TYPE_ANY);
	m_filter.emplace("pgm", wxBITMAP_TYPE_ANY);
	m_filter.emplace("PGM", wxBITMAP_TYPE_ANY);
	m_filter.emplace("dib", wxBITMAP_TYPE_ANY);
	m_filter.emplace("DIP", wxBITMAP_TYPE_ANY);

	//In Linux everything is a file :-)

	if(!fileExists(c_APPLICATION_DIR)){
		setPath(c_APPLICATION_DIR.c_str());
	}

	if(!fileExists(c_DIR_HIST)){
		setPath(c_DIR_HIST.c_str());
	}
	
	m_settings["chunkL"]=-1;
	m_settings["chunkR"]=-1;
	m_settings["resume"]=-1;
}

//----------------------------------------------------------------------

wxBitmapType FileManager::getBitmapType(const wxString& wxStr){
	if(isSuported(wxStr)){
		std::size_t n=wxStr.find_last_of('.');
		return m_filter[wxStr.substr(n+1)];
	}
	return wxBITMAP_TYPE_ANY;
}

//----------------------------------------------------------------------

bool FileManager::isSuported(const wxString& wxStr){
	std::size_t n=wxStr.find_last_of('.');
	if(n!= std::string::npos){
		return m_filter.find(wxStr.substr(n+1))!=m_filter.end();
	}
	return false;
}
//----------------------------------------------------------------------

bool FileManager::loadSettings(){
	std::ifstream file;
	file.open(c_SAVE_SETTING);
	if(!file.is_open()){
		return false;
	}

	std::size_t pos=std::string::npos;
	std::string line, setting, val;
	int n;
	std::map<FileManager::CnstChar, int>::iterator it;
	while(getline(file, line)){
		pos=line.find_last_of(c_separator);
		if(pos!=std::string::npos && pos>0){
			setting=line.substr(0, pos);
			
			it=m_settings.find(CnstChar(setting.c_str()));
			if(it==m_settings.end()){
				continue;
			}
			
			val=trim(line.substr(pos+1));
			try
			{
				n=std::stoi(val);
			}
			catch(...){
				n=0;
			}
			m_settings[setting.c_str()]=n; 
		}
	}
	file.close();

	if(m_settings["chunkL"]>0 && m_settings["chunkR"]>0){
		m_settings["resume"]=1;
	}
	
	return true;
}

//----------------------------------------------------------------------

std::string FileManager::trim(const std::string& str){
	size_t first = str.find_first_not_of(' ');
	if(std::string::npos == first){
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

//----------------------------------------------------------------------

int FileManager::get(const char* setting){
	std::map<CnstChar, int>::iterator it=m_settings.find(CnstChar(setting));
	if(it!=m_settings.end()){
		return it->second;
	}
	return -1;
}

//----------------------------------------------------------------------

std::string FileManager::setPath(const char* filePath) {
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
		//mkdir(path, 0777);//*/
		
		std::filesystem::create_directory(std::filesystem::path{path});//, 0777);
	}
	return filePath;
}

//----------------------------------------------------------------------

SUBDIR FileManager::isSubdirectory(const std::string& dirTest, std::vector<std::string>& directories){
	bool a=false;
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

void FileManager::removeDuplicates(std::vector<std::string>& data){
	sort(data.begin(), data.end());

	int k=0;
	int i=0;
	int n=data.size();
	while(i<n){
		k=i+1;
		if(k==n){
			break;
		}
		
		while(k<n && data[k]==data[i]){
			data[k++]="";
		}

		i=k;
	}

	sort(data.begin(), data.end(), [](const std::string& str1, const std::string& str2){
		return str1>str2;
	});
	
	for(int j=n-1; j>-1; j--){
		if(data[j]==""){
			data.pop_back();
		}
	}
}

//----------------------------------------------------------------------

SUBDIR FileManager::reduce(const std::string& dirTest, std::vector<std::string>& directories){
	bool a=false;
	for(std::string& directory : directories){
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
		removeDuplicates(directories);
		return SUBDIR::IS_SUP;
	}

	return SUBDIR::IS_NEW;
}


//----------------------------------------------------------------------



