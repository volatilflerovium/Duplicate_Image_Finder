/*********************************************************************
* Scheduler class                                				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include "data.h"

//####################################################################

class Worker;

class Scheduler {

	public:
		static void init(int numWorkers);
		static void terminate();
		static void feedData(Data* data);
		static void wait();
		static void pauseWorkers();
		static void workerReady(int workerID);
		static void dumpData(DataBuffer& buffer);
		static void workerFinish(int workerID);

		template<typename T, typename Func>
		static void harvestData(Func cb, T* t);
	
		static void start();
		static void unpause();
		static bool getData(std::string*& str, bool& startBlock, int& rank);		
		static bool stopProcessing();
		static void setSimilarityThreshold(int val);

	private:
		static std::mutex m_workersMutex;
		static std::mutex m_stop_workersMutex;
		static std::mutex m_controlMutex;
		static std::mutex m_stopMutex;
		static std::condition_variable m_cv;
		static std::condition_variable m_stop_cv;
		static std::unique_lock<std::mutex> m_ulock;
		static std::unique_lock<std::mutex> m_stop_ulock;
		
		static std::vector<int> m_bufferRank;
		static std::vector<int> m_bufferLN;
		static std::vector<bool> m_bufferLNB;
		
		static std::vector<Worker*> m_workers;
		static std::vector<std::thread*> m_threads;
		static std::vector<bool> m_availableWorkers;
		static std::vector<bool> m_stoppedWorkers;
		
		static Data* m_currentData;
		static int m_pos;
		static bool m_ready;
		static bool m_pausedWorkers;
		static bool m_noWork;		
		static bool m_stopJob;

		static void spanThreads();
		static void destroy();
		static void clearBuffers();
		
		static void terminateWorkers();
};

//--------------------------------------------------------------------

inline void Scheduler::start(){
	m_ready=false;
}

//--------------------------------------------------------------------

inline bool Scheduler::stopProcessing(){
	return m_stopJob;
}

//--------------------------------------------------------------------

template<typename T, typename Func>
void Scheduler::harvestData(Func cb, T* t){
	int k=-1;
	for(int i=0; i<m_bufferLN.size(); i++){
		if(m_bufferLNB[i]){
			(t->*cb)(((m_currentData->m_picsL)[m_bufferLN[i]]).c_str());
		}
		else{
			(t->*cb)(((m_currentData->m_picsR)[m_bufferLN[i]]).c_str());
		}
	}
	m_bufferLN.clear();
	m_bufferLNB.clear();
}

//####################################################################

#endif
