/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* HistogramLoader class                           				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "histogram_loader.h"
#include "data_logger.h"
#include "opencv_utilities.h"
#include "data_logger.h"

//####################################################################

std::string HistogramLoader::m_directory;
std::string HistogramLoader::m_picList;
const int HistogramLoader::m_limit(DIMGS::BLOCK_SIZE);
int HistogramLoader::m_chunkNumberL;
int HistogramLoader::m_chunkNumberR;
int HistogramLoader::m_position;
Data HistogramLoader::m_data;

//--------------------------------------------------------------------

void HistogramLoader::init( 
	const std::string& directory, 
	const std::string& vdoList
){
	m_chunkNumberL=0;
	m_chunkNumberR=0;
	m_position=0;
	m_directory=directory; 
	m_picList=vdoList;
}

//--------------------------------------------------------------------

Data* HistogramLoader::loadHistgrams()
{
	if(m_chunkNumberL<0){
		return nullptr;
	}	
	
	if(!loadHistgrams(m_data.m_histogramsR, m_data.m_picsR, m_chunkNumberR)){
		m_chunkNumberR=0;
	}
	m_data.m_sentinelR=m_position;

	if(m_chunkNumberR==0){
		if(loadHistgrams(m_data.m_histogramsL, m_data.m_picsL, m_chunkNumberL)){
			m_data.m_sentinelL=m_position;
			m_chunkNumberR=m_chunkNumberL;
			loadHistgrams(m_data.m_histogramsR, m_data.m_picsR, m_chunkNumberR);
			m_data.m_sentinelR=m_position;
			m_chunkNumberL++;
		}
		else{
			m_chunkNumberL=-1;
		}
	}

	m_chunkNumberR++;
	m_data.m_mode=m_chunkNumberR==m_chunkNumberL;

	if(m_chunkNumberL<0){
		return nullptr;
	}
	return &m_data;
}

//--------------------------------------------------------------------

bool HistogramLoader::loadHistgrams(
	std::vector<cv::Mat>& histograms,
	std::vector<std::string>& pics, 
	int chunkNumber
)
{
	std::ifstream file;
	file.open(m_picList);
	if(!file.is_open()){
		Logger::log("Failed to open pictures list file.");
		return false;
	}
	
	bool valid=false;
	int lineNumber=-1, i=0;
	m_position=0;
	std::string line, file_name;		
	while(getline(file, line) && m_position<m_limit){
		lineNumber++;
		//Logger::log("Lines: ", lineNumber, " chunkN: ", chunkNumber);
		if(lineNumber<chunkNumber*m_limit || line.length()==0){
			continue;
		}
		//Logger::log("Lines: ", lineNumber, " chunkN: ", chunkNumber, "===");
		
		valid=true;
		pics[m_position]=line;

		for(int k=0; k<DIMGS::DATA_SIZE; k++){
			file_name=m_directory+std::string("img")+std::to_string(lineNumber+1)+std::string("_")+std::to_string(k)+std::string(".yml");
			cv::FileStorage rstorage(file_name, cv::FileStorage::READ);
			rstorage["img"] >> histograms[i++];
			rstorage.release();
		}
		m_position++;
	}

	return valid;
}

//--------------------------------------------------------------------

