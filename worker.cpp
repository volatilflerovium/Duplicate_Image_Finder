/*********************************************************************
* Worker class                                   				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/

#include<iostream>
#include "worker.h"
#include "scheduler.h"
#include "data_logger.h"

//####################################################################

std::mutex Worker::m_bufferMtx;


void Worker::terminate(){
	m_ready = true;
	m_running = false;
	m_exitJob=true;
	m_cv.notify_one();
}

//--------------------------------------------------------------------

void Worker::stopJob(){
	m_ready=false;
	m_exitJob=true;
}

//--------------------------------------------------------------------

void Worker::run() {
	while (m_running) {
		if(m_ready){
			m_ready = false;
			findSimilarImages();
			m_exitJob=false;
		}

		Scheduler::workerReady(m_wid);		
		m_cv.wait(m_ulock, [this]{return this->m_ready;});
	}
	Scheduler::workerFinish(m_wid);
}

//--------------------------------------------------------------------

void Worker::setRequest(const Data* data){
	m_data=data;
}

//--------------------------------------------------------------------

void Worker::findSimilarImages(){
	if(!m_data){
		return;
	}

	DataBuffer buffer(m_data->m_sentinelL);

	int line_number=1;
	std::string file_name, file_name2;
	std::vector<double> picRanks;
	std::vector<int> rLineNumber;

	double similarity;
	int chunk1=0;
	int chunk2=0;

	int i=m_position;
	int startAt=i+1;

	const float averageFactor=1.0/DIMGS::DATA_SIZE; 

	while(i<m_data->m_sentinelL){
		chunk1=i*DIMGS::DATA_SIZE;
		if((m_data->m_histogramsL)[chunk1].rows==0){
			i+=m_step;
			continue;
		}

		if(m_exitJob){
			Printer::logData("Worker: ",  m_wid, " Positions: i: ", i);
			break;
		}

		m_data->m_mode ? startAt=i+1: startAt=0;

		picRanks.clear();
		rLineNumber.clear();	
		
		for(int j=startAt; j<m_data->m_sentinelR; j++){
			if(m_exitJob){
				break;
			}
	
			chunk2=j*DIMGS::DATA_SIZE;
			if((m_data->m_histogramsR[chunk2]).rows==0){
				continue;
			}

			similarity=0;
			double ss=0;
			for(int k=0; k<DIMGS::DATA_SIZE; k++){
				similarity+=cv::compareHist((m_data->m_histogramsL)[chunk1+k], (m_data->m_histogramsR)[chunk2+k], cv::HISTCMP_BHATTACHARYYA);
			}
			similarity*=averageFactor;
			similarity=1-similarity;

			if(similarity>=m_similarityThreshold){
				sorting(rLineNumber, picRanks, j, similarity);				
			}

			if(j%20==0){
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		chunk1+=DIMGS::DATA_SIZE;
		if(rLineNumber.size()>0){
			buffer.insert(i, DIMGS::FLOAT_FACTOR, true);
			for(int k=0; k<rLineNumber.size(); k++){
				buffer.insert(rLineNumber[k], picRanks[k]*DIMGS::FLOAT_FACTOR, false);
				if(k>4){
					break;
				}
			}
		}
		i+=m_step;
	}
	Worker::m_bufferMtx.lock();
	Scheduler::dumpData(buffer);
	Worker::m_bufferMtx.unlock();
}

//--------------------------------------------------------------------

