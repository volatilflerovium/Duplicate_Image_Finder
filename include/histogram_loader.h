/*********************************************************************
* HistogramLoader class                           				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/

#ifndef HISTOGRAMLOADER_H
#define HISTOGRAMLOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <ctime>
#include <unistd.h>

#include "data_logger.h"
#include "data.h"

//####################################################################

class HistogramLoader
{
	public:

		static void init( 
			const std::string& directory, 
			const std::string& vdoList
		);

		static Data* loadHistgrams();
		
		static void setChunks(int chunkL, int chunkR){
			if(chunkL<=chunkR){
				m_chunkNumberL=chunkL;
				m_chunkNumberR=chunkR;
			}
		}
		
		static void saveCurrentState(){
			Printer::logData("chunkL: ",m_chunkNumberL);
			Printer::logData("chunkR: ",m_chunkNumberR);
		}

	private:
		static std::string m_directory;
		static std::string m_picList;
		static Data m_data;
		static const int m_limit;
		static int m_chunkNumberL;
		static int m_chunkNumberR;
		static int m_position;
		static int m_sentinelL;
		static int m_sentinelR;

		static bool loadHistgrams(	
			std::vector<cv::Mat>& histograms,
			std::vector<std::string>& pics, 
			int chunkNumber);
};

//#####################################################################

#endif
