/*********************************************************************
* WxWorker class                                   				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef WX_WORKER_H
#define WX_WORKER_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <wx/wx.h>
#include <wx/panel.h>


#include "histogram_loader.h"
#include "scheduler.h"
#include "data_logger.h"
#include "data.h"

// the ID we'll use to identify our event
enum EVENT_ID
{
	EVNT_ADD_PICTURE_ID = 100000,
	EVNT_PROGRESS_BAR_ID,
	EVNT_STATUS_ID,
	EVNT_DATA_ID,
};

enum StatusCode
{
	HistogramsDone,
	SearchFinish,
	SearchCancelled,
	HistogramsCancelled,
	WorkCancelled,
};

const wxString c_histFinish("histogramFinished");

//######################################################################

class WxWorker : public wxThread
{
	public:
		WxWorker(wxFrame* parent);

		void setFileList(std::vector<std::string>* fileList);
		void processJob();
		void terminate();
		void cancel();
	
		virtual ExitCode Entry();

	private:
		wxFrame* m_parent;
		std::condition_variable m_cv;
		std::unique_lock<std::mutex> m_ulock;
		std::mutex m_mtx;
		std::vector<std::string>* m_fileList;

		bool m_running;
		bool m_ready;
		bool m_stopJob;
		bool m_jobCompleted;
		bool m_cancelled;

		void doWork();
		bool mkHist();
		void sendEventProgress();
		void sendEventData(const wxString& data, int val);
		void sendEventStatus(int status);
		void sendEventPicture(std::string* pic, bool newBlock, float rank);
};

//----------------------------------------------------------------------

inline void WxWorker::setFileList(std::vector<std::string>* fileList){
	m_fileList=fileList;
}

//----------------------------------------------------------------------

inline void WxWorker::processJob(){
	m_stopJob=false;
	m_cancelled=false;
	m_jobCompleted=true;
	m_ready = true;
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void WxWorker::terminate(){
	m_stopJob=true;
	m_running=false;
	m_ready = true;
	Scheduler::terminate();
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void WxWorker::cancel(){
	m_stopJob=true;
	m_cancelled=true;
	m_ready = true;
	HistogramLoader::setChunks(0, 0);
	Scheduler::pauseWorkers();
	m_cv.notify_one();
}
//----------------------------------------------------------------------


#endif
