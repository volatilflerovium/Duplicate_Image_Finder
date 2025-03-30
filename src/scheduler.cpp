/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* Scheduler class                                				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "scheduler.h"
#include "worker.h"
#include "data_logger.h"
#include "data.h"

//====================================================================

std::mutex Scheduler::m_workersMutex;
std::mutex Scheduler::m_stop_workersMutex;
std::mutex Scheduler::m_controlMutex;
std::mutex Scheduler::m_stopMutex;
std::condition_variable Scheduler::m_cv;
std::condition_variable Scheduler::m_stop_cv;
std::unique_lock<std::mutex> Scheduler::m_ulock;
std::unique_lock<std::mutex> Scheduler::m_stop_ulock;

std::vector<Scheduler::Record> Scheduler::m_records;

std::vector<Worker*> Scheduler::m_workers;
Data* Scheduler::m_currentData(nullptr);

size_t Scheduler::m_ready=0;
bool Scheduler::m_stopJob;
bool Scheduler::m_pausedWorkers;
bool Scheduler::m_noWork(false);
size_t Scheduler::m_pos(0);

//----------------------------------------------------------------------

void Scheduler::init(int workers)
{
	m_ulock=std::unique_lock<std::mutex>(m_controlMutex);
	m_stop_ulock=std::unique_lock<std::mutex>(m_stopMutex);
	m_ready=0;
	m_pausedWorkers=false;
	m_noWork=true;
	m_stopJob=false;

	for(int i=0; i<workers; ++i) {
		m_workers.push_back(new Worker(i, workers));
	}
	
	m_records.reserve(100);
	m_pos=0;
}

//----------------------------------------------------------------------

void Scheduler::destroy()
{
	Logger::log("Destroying workers.");
	for (size_t i = 0; i < m_workers.size(); ++i) {
		delete m_workers[i];
	}
}

//----------------------------------------------------------------------

void Scheduler::workerFinish()
{
	m_stop_workersMutex.lock();
	m_noWork=true;
	m_stop_cv.notify_one();	
	m_stop_workersMutex.unlock();
}

//----------------------------------------------------------------------

void Scheduler::terminate()
{
	m_stopJob=true;
	
	for(size_t i = 0; i < m_workers.size(); ++i) {
		m_noWork=false;
		if(m_workers[i]->isRunning()){
			m_workers[i]->terminate();
			m_stop_cv.wait(m_stop_ulock, []{return Scheduler::m_noWork;});
		}
	}

	m_ready=m_workers.size();
	m_cv.notify_one();
	
	clearBuffers();
}

//----------------------------------------------------------------------

void Scheduler::pauseWorkers()
{
	m_stopJob=true;
	m_pausedWorkers=true;
	for(size_t i=0; i<m_workers.size(); i++){
		m_workers[i]->stopJob();
	}
}

//----------------------------------------------------------------------

void Scheduler::unpause()
{
	m_stopJob=false;
	m_pausedWorkers=false;
	for(size_t i=0; i<m_workers.size(); i++){
		m_workers[i]->makeReady();
	}
}

//----------------------------------------------------------------------

void Scheduler::workerReady()
{
	m_workersMutex.lock();
	m_ready++;

	if(m_ready==m_workers.size()){// to avoid spurious awakenings
		m_cv.notify_one();
	}
		
	m_workersMutex.unlock();
}

//----------------------------------------------------------------------

void Scheduler::feedData(Data* data)
{
	if(m_pausedWorkers){
		Logger::log("workers paused");
		return;
	}

	m_ready=0;
	m_currentData=data;
	for(size_t i=0; i<m_workers.size(); i++){
		m_workers[i]->setRequest(data);
		m_workers[i]->notify();
	}	
}

//----------------------------------------------------------------------

void Scheduler::wait()
{
	m_cv.wait(m_ulock, []{return Scheduler::m_ready==m_workers.size();});
}

//----------------------------------------------------------------------

void Scheduler::dumpData(DataBuffer& buffer)
{
	for(int i=0; i<buffer.size(); i++){
		m_records.emplace_back(buffer.m_ranks[i], buffer.m_lines[i], buffer.m_blocks[i]);
	}
}

//----------------------------------------------------------------------

bool Scheduler::getData(std::string*& str, bool& startBlock, int& rank)
{
	if(m_pos==m_records.size()){
		clearBuffers();
		return false;
	}
	
	const Record& record=m_records[m_pos];
	
	if(record.m_LNB){
		str=&(m_currentData->m_picsL)[record.m_LN];	
	}
	else{
		str=&(m_currentData->m_picsR)[record.m_LN];
	}
	startBlock=record.m_LNB;
	rank=record.m_Rank;

	m_pos++;
	return true;
}

//----------------------------------------------------------------------

void Scheduler::clearBuffers()
{
	m_pos=0;
	m_records.clear();
}

//----------------------------------------------------------------------

void Scheduler::setSimilarityThreshold(int val)
{
	double threshold=0.9;
	if(val>0 && val<11){
		threshold=(1.0*val)/10.0;
	}

	for(size_t i=0; i<m_workers.size(); i++){
		m_workers[i]->setSimilarityThreshold(threshold);
	}
}

//====================================================================
