/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* WxWorker class                                   				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "wx_worker.h"
#include "opencv_utilities.h"
#include "data.h"
#include "data_logger.h"
#include "file_manager.h"
#include "settings_manager.h"

#include "data_visualization.h"

extern DataView* s_dataViewPtr;

//----------------------------------------------------------------------

WxWorker::WxWorker(wxFrame* parent)
:wxThread(wxTHREAD_DETACHED)
, m_parent(parent)
, m_running(true)
, m_ready(false)
, m_stopJob(false)
, m_jobCompleted(false)
, m_cancelled(false)
, m_skipHist(false)
{
	m_ulock=std::unique_lock<std::mutex>(m_mtx);

	HistogramLoader::init(FileManager::c_DIR_HIST, FileManager::c_FILE_LIST);

	if(SettingsManager::getResume()!=1){
		FileManager::cleanUp();
	}
	else{
		HistogramLoader::setChunks(std::abs(SettingsManager::getChunkL()), std::abs(SettingsManager::getChunkR()));
	}

	int num_cpus =3*std::thread::hardware_concurrency();

	Scheduler::init(num_cpus);
}

//----------------------------------------------------------------------

wxThread::ExitCode WxWorker::Entry()
{
	while(m_running){
		m_ready=false;
		m_cv.wait(m_ulock, [this]{return this->m_ready;});
		m_ready=false;
		if(FileManager::totalFiles()>0 && m_running){
			if(mkHist()){
				m_cv.wait(m_ulock, [this]{return this->m_ready;});
				if(!m_cancelled && m_running){
					doWork();
				}
			}
			
			if(!m_running){
				break;
			}
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

bool WxWorker::mkHist()
{
	static int lineNumb=1;

	// Order is important to allow the evaluation for the function!
	if(!ImageProcessor::rebuildHistograms() && m_skipHist){
		sendHistogramStatus(lineNumb-1);
		return true;
	}

	lineNumb=1;

	std::ios_base::openmode mode=std::ios::trunc;
	std::ofstream fileList(FileManager::c_FILE_LIST, mode);
	
	FileManager::loopFilesMap([&](const std::string& imageName, uint& val){
		val=0;

		if(m_stopJob){
			return false;
		}

		try
		{
			ImageProcessor::setupHistograms(imageName.c_str(), lineNumb);
			if(m_stopJob){
				return false;
			}

			fileList<<imageName<<"\n";
			lineNumb++;
		}
		catch(...){
			Logger::log(imageName);
		}
		sendEventProgress();
		return true;
	});

	fileList.close();

	m_skipHist=!m_stopJob;
	if(!m_stopJob){
		sendHistogramStatus(lineNumb-1);
	}
	else{
		sendHistogramStatus(-1);
	}
	return m_skipHist;
}

//----------------------------------------------------------------------

void WxWorker::doWork()
{
	s_dataViewPtr->reset();
	bool newBlock=false;
	std::string* pic=nullptr;
	int rank=0;
	Data* data=nullptr;
	while(1){ 
		if(Scheduler::isProcessing()){
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
			s_dataViewPtr->updateData(pic->c_str(), newBlock);
			sendEventPicture(pic, newBlock, rank);
			
		}
		sendEventProgress();
	}
	Scheduler::unpause();
}


//----------------------------------------------------------------------

void WxWorker::sendEventProgress()
{
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_PROGRESS_BAR);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}

//----------------------------------------------------------------------

void WxWorker::sendHistogramStatus(int val)
{
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_DATA_ID);
	event.SetInt(val);
	if(val==-1){
		event.SetString(c_histCancelled);
	}
	else{
		event.SetString(c_histFinish);
	}

	m_parent->GetEventHandler()->AddPendingEvent(event);
	
}

//----------------------------------------------------------------------

void WxWorker::sendEventStatus(int status)
{
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_STATUS_ID);
	event.SetInt(status);
	m_parent->GetEventHandler()->AddPendingEvent(event);
}

//----------------------------------------------------------------------

void WxWorker::sendEventPicture(std::string* pic, bool newBlock, int rank)
{
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVNT_ADD_PICTURE_ID);
	if(newBlock){
		rank*=-1;
	}

	event.SetInt(rank);
	event.SetString(pic->c_str());
	m_parent->GetEventHandler()->AddPendingEvent(event);
}

//----------------------------------------------------------------------
