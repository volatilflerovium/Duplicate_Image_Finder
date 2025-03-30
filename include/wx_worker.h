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
#ifndef WX_WORKER_H
#define WX_WORKER_H
#include "histogram_loader.h"
#include "scheduler.h"

#include <mutex>
#include <condition_variable>

#include <wx/thread.h>
#include <wx/panel.h>

//====================================================================

enum EVENT_ID
{
	EVNT_ADD_PICTURE_ID = 100000,
	EVNT_PROGRESS_BAR,
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
const wxString c_histCancelled("histogramCancelled");

//====================================================================

class WxWorker : public wxThread
{
	public:
		WxWorker(wxFrame* parent);
		virtual ~WxWorker()=default;

		void processJob();
		void terminate();
		void cancel();
		void makeHistograms(bool skipHist=false);
		virtual ExitCode Entry();

	private:
		wxFrame* m_parent;
		std::condition_variable m_cv;
		std::unique_lock<std::mutex> m_ulock;
		std::mutex m_mtx;

		bool m_running;
		bool m_ready;
		bool m_stopJob;
		bool m_jobCompleted;
		bool m_cancelled;
		bool m_skipHist;

		void doWork();
		bool mkHist();
		
		void sendEventProgress();
		void sendHistogramStatus(int val);
		void sendEventStatus(int status);
		void sendEventPicture(std::string* pic, bool newBlock, int rank);
};

//----------------------------------------------------------------------

inline void WxWorker::processJob()
{
	m_stopJob=false;
	m_cancelled=false;
	m_jobCompleted=true;
	m_ready = true;
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void WxWorker::terminate()
{
	m_cancelled=true;
	m_stopJob=true;
	m_running=false;
	m_ready = true;
	Scheduler::terminate();
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void WxWorker::cancel()
{
	m_stopJob=true;
	m_cancelled=true;
	m_ready = true;
	HistogramLoader::setChunks(0, 0);
	Scheduler::pauseWorkers();
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void WxWorker::makeHistograms(bool mkHist)
{
	m_skipHist=!mkHist;
}

//====================================================================

#endif
