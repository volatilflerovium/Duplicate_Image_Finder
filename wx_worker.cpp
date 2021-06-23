#include "opencv_utilities.h"
#include "histogram_loader.h"
#include "scheduler.h"
#include "data_logger.h"
#include "file_manager.h"

#include "wx_worker.h"

//----------------------------------------------------------------------

WxWorker::WxWorker(wxFrame* parent)
:m_parent(parent),
m_fileList(nullptr), 
	m_running(true), m_ready(false), m_stopJob(false),
	m_jobCompleted(false), m_cancelled(false)
{
	m_ulock=std::unique_lock<std::mutex>(m_mtx);

	HistogramLoader::init(FileManager::c_DIR_HIST, FileManager::c_FILE_LIST);

	FileManager::loadSettings();
	if(FileManager::get("resume")!=1){
		FileManager::cleanUp();
	}
	else{
		HistogramLoader::setChunks(std::abs(FileManager::get("chunkL")), std::abs(FileManager::get("chunkR")));
	}
	
	Printer::init(FileManager::c_SAVE_SETTING);

	int num_cpus = std::thread::hardware_concurrency();

	Printer::logData("Total cpus available: ", num_cpus);

	Scheduler::init(num_cpus);
}

//----------------------------------------------------------------------

wxThread::ExitCode WxWorker::Entry(){
	while(m_running){
		m_ready=false;
		
		m_cv.wait(m_ulock, [this]{return this->m_ready;});
		
		if(m_fileList){
			
			if(mkHist()){
				m_ready=false;
				m_cv.wait(m_ulock, [this]{return this->m_ready;});

				if(!m_cancelled){
					doWork();
				}
			}

			m_fileList=nullptr;
		
			if(m_cancelled){
				sendEventStatus(WorkCancelled);
				Scheduler::unpause();
			}
			else if(m_jobCompleted){
				m_jobCompleted=false;
				sendEventStatus(SearchFinish);
			}
		}
	}
	return 0;
}

//----------------------------------------------------------------------

bool WxWorker::mkHist(){
	std::ofstream fileList(FileManager::c_FILE_LIST, std::ios::app);
	Logger::log("set histogram bands proportional to the actual picture size");
	int lineNumb=1;
	for(const std::string& imageName : (*m_fileList)) {
		if(m_stopJob){
			break;
		}

		try
		{
			std::vector<cv::Mat> hist_base1=getNormalizeHistogram8(imageName.c_str());

			for(int k=0; k<DIMGS::DATA_SIZE; k++){
				if(m_stopJob){
					break;
				}
				std::string file_name=FileManager::c_DIR_HIST+std::string("img")+std::to_string(lineNumb)+std::string("_")+std::to_string(k)+std::string(".yml");
				cv::FileStorage storage(file_name, cv::FileStorage::WRITE);
				storage << "img" << hist_base1[k];
				storage.release();
			}
			fileList<<imageName<<"\n";
			lineNumb++;
		}
		catch(...){
			Logger::log("imageName");
		}
		sendEventProgress();
	}

	fileList.close();

	if(!m_stopJob){
		sendEventData(c_histFinish, lineNumb-1);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------

void WxWorker::doWork(){
	bool newBlock=false;
	std::string* pic=nullptr;
	float rank=0.0;
	Data* data=nullptr;

	while(1){ 
		if(Scheduler::stopProcessing()){
			m_jobCompleted=false;
			break;
		}

		data=HistogramLoader::loadHistgrams();			
		if(!data){
			break;
		} 

		Scheduler::feedData(data);
		Scheduler::wait();
		while(Scheduler::getData(pic, newBlock, rank)){
			sendEventPicture(pic, newBlock, rank);	
		}
		sendEventProgress();
	}

	Scheduler::unpause();
}


//----------------------------------------------------------------------

void WxWorker::sendEventProgress(){
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_PROGRESS_BAR_ID);
	event.SetInt(1);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}

//----------------------------------------------------------------------

void WxWorker::sendEventData(const wxString& data, int val){
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_DATA_ID);
	event.SetInt(val);
	event.SetString(data);
	m_parent->GetEventHandler()->AddPendingEvent(event);
	
}

//----------------------------------------------------------------------

void WxWorker::sendEventStatus(int status){
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_STATUS_ID);
	event.SetInt(status);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}

//----------------------------------------------------------------------

void WxWorker::sendEventPicture(std::string* pic, bool newBlock, float rank){
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_ADD_PICTURE_ID);
	if(newBlock){
		rank*=-1.0;
	}
	int rk=rank*10000.0;
	
	event.SetInt(rk);
	event.SetString(wxString(*pic));
	m_parent->GetEventHandler()->AddPendingEvent(event);
}

//----------------------------------------------------------------------

