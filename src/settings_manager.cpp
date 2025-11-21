/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class SettingsManager                                              *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    20-03-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "settings_manager.h"
#include "data_logger.h"
#include "file_manager.h"
#include "cstr_split.h"

#include <fstream>

//====================================================================

SettingsManager* SettingsManager::m_settingsPtr=SettingsManager::init();

//--------------------------------------------------------------------

SettingsManager::SettingsManager()
{
	std::ifstream settingsFile;
	settingsFile.open(FileManager::c_SAVE_SETTING.c_str(), std::ifstream::in);
	if(settingsFile.is_open()){
		std::string infoLine;
		while(std::getline(settingsFile, infoLine)){
			if(infoLine.length()==0){
				continue;
			}
			CstrSplit<12> parts(infoLine.c_str(), ":");
			try{
					m_nodesColorBorder=parts[0];
					m_nodesBkgColor=parts[1];
					m_nodesSelectedColor=parts[2];
					m_nodesFontColor=parts[3];
					m_edgesColor=parts[4];
					m_canvasColor=parts[5];
					
					m_borderWidth=std::atoi(parts[6]);
					m_chunkL=std::atoi(parts[7]);
					m_chunkR=std::atoi(parts[8]);
					m_resume=std::atoi(parts[9]);
					m_isInstalled=static_cast<bool>(std::atoi(parts[10]));
				break;
			}
			catch(...)
			{
				continue;
			}
		}
		settingsFile.close();
		
		if(m_chunkL+m_chunkR>0){
			m_resume=1;
		}
		
		updateNetwork();
	}
}

//--------------------------------------------------------------------

SettingsManager::~SettingsManager()
{
	std::fstream fileData(FileManager::c_SAVE_SETTING.c_str(), std::ios::out | std::ios::trunc);
	if(fileData.is_open()){
		fileData\
		<<m_nodesColorBorder<<":"
		<<m_nodesBkgColor<<":"
		<<m_nodesSelectedColor<<":"
		<<m_nodesFontColor<<":"
		<<m_edgesColor<<":"
		<<m_canvasColor<<":"
		<<m_borderWidth<<":"
		<<m_chunkL<<":"
		<<m_chunkR<<":"
		<<m_resume<<":"
		<<static_cast<int>(m_isInstalled)<<":::\n";

		fileData.close();
	}
	else{
		Logger::log("Save settings to file failed.");
	}
}

//--------------------------------------------------------------------

bool SettingsManager::updateNetwork()
{
	std::fstream fileData("/tmp/.DuplicatedImgNetworkData.js", std::ios::out | std::ios::trunc);
	if(fileData.is_open()){
		fileData<<"function loadSettings()\n\
		{\n\
			interfaceMod.setSettings("; 
			fileData<<m_borderWidth<<", ";
			fileData<<"\""<<m_nodesColorBorder<<"\", ";
			fileData<<"\""<<m_nodesBkgColor<<"\", ";
			fileData<<"\""<<m_nodesSelectedColor<<"\", ";
			fileData<<"\""<<m_nodesFontColor<<"\", ";
			fileData<<"\""<<m_edgesColor<<"\", ";
			fileData<<"\""<<m_canvasColor<<"\");\n";
			fileData<<"}\n";
		fileData.close();
		return true;
	}

	return false;
}

//====================================================================

