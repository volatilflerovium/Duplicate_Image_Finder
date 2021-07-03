/*********************************************************************
* Worker class                                   				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef WORKER_H
#define WORKER_H

#include <mutex>
#include <thread>
#include <condition_variable>

#include "data.h"

//====================================================================

class Worker {

	public:
		Worker(int id, int position, int step)
		:
			m_ulock(std::unique_lock<std::mutex>(m_mtx)),
			m_data(nullptr), m_similarityThreshold(0.9), 
			m_wid(id), m_step(step), m_position(position), 
			m_running(true), m_ready(false), m_exitJob(false)
		{}

		void setRequest(const Data* data);

		void run();
		void terminate();
		void stopJob();
		bool isRunning() const;
		void notify();
		void makeReady();
		void setSimilarityThreshold(double threshold);

	private:
		std::condition_variable m_cv;
		static std::mutex m_bufferMtx;
		std::mutex m_mtx;
		std::unique_lock<std::mutex> m_ulock;
		const Data* m_data;
		double m_similarityThreshold;
		const int m_wid;
		const int m_step;
		const int m_position;
		bool m_running;
		bool m_ready;
		bool m_exitJob;
		void findSimilarImages();
};

//----------------------------------------------------------------------

inline bool Worker::isRunning() const {
	return m_running;
};

//----------------------------------------------------------------------

inline void Worker::notify(){
	m_ready = true;
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void Worker::makeReady(){
	m_exitJob=false;	
}

//----------------------------------------------------------------------

inline void Worker::setSimilarityThreshold(double threshold){
	m_similarityThreshold=threshold;
}
	
#endif
