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
	private:
		std::condition_variable m_cv;
		static std::mutex m_bufferMtx;
		std::mutex m_mtx;
		std::unique_lock<std::mutex> m_ulock;
		const Data* m_data;
		const int m_wid;
		const int m_step;
		const int m_position;
		bool m_running;
		bool m_ready;
		bool m_exitJob;
		void findSimilarImages();

	public:
		Worker(int id, int position, int step)
		:
			m_ulock(std::unique_lock<std::mutex>(m_mtx)),
			m_data(nullptr), m_wid(id), m_step(step), m_position(position), 
			m_running(true), m_ready(false), m_exitJob(false)
		{}

		void setRequest(const Data* data);

		void run();
		void terminate();
		void stopJob();
		bool isRunning() const {
			return m_running;
		};

		void notify(){
			m_ready = true;
			m_cv.notify_one();
		}
		
		void makeReady(){
			//Logger::log("Stopping worker job: ", m_wid);
			//m_ready=false;
			m_exitJob=false;	
		}
};


#endif
