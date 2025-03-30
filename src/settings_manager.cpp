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

bool SettingsManager::updateNetwork()
{
	std::fstream fileData("/tmp/.DuplicatedImgNetworkData.js", std::ios::out | std::ios::trunc);
	if(fileData.is_open()){
		fileData<<"function loadSettings()\n\
		{\n\
			interfaceMod.setSettings("; 
			fileData<<getNodeBorderWidth()<<", ";
			fileData<<"\""<<getNodeBorderColor()<<"\", ";
			fileData<<"\""<<getNodeBkgColor()<<"\", ";
			fileData<<"\""<<getNodeSelectedColor()<<"\", ";
			fileData<<"\""<<getNodeFontColor()<<"\", ";
			fileData<<"\""<<getEdgesColor()<<"\", ";
			fileData<<"\""<<getCanvasColor()<<"\");\n";
			fileData<<"}\n";
		fileData.close();
		return true;
	}
	
	return false;
}

bool SettingsManager::loadSettings()
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
		
		return updateNetwork();
	}
	
	return false;
}

//--------------------------------------------------------------------

void SettingsManager::save()
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
		<<m_resume<<"::::\n";

		fileData.close();
	}
	else{
		Logger::log("Save settings to file failed.");
	}
}

//====================================================================

