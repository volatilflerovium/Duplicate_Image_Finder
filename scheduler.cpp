#include<iostream>
#include "scheduler.h"
#include "worker.h"
#include "data_logger.h"

//####################################################################
//####################################################################

std::mutex Scheduler::m_workersMutex;
std::mutex Scheduler::m_stop_workersMutex;
std::mutex Scheduler::m_controlMutex;
std::mutex Scheduler::m_stopMutex;
std::condition_variable Scheduler::m_cv;
std::condition_variable Scheduler::m_stop_cv;
std::unique_lock<std::mutex> Scheduler::m_ulock;
std::unique_lock<std::mutex> Scheduler::m_stop_ulock;

std::vector<int> Scheduler::m_bufferRank;
std::vector<int> Scheduler::m_bufferLN;
std::vector<bool> Scheduler::m_bufferLNB;

std::vector<Worker*> Scheduler::m_workers;
std::vector<std::thread*> Scheduler::m_threads;
std::vector<bool> Scheduler::m_availableWorkers;
std::vector<bool> Scheduler::m_stoppedWorkers;
Data* Scheduler::m_currentData(nullptr);

bool Scheduler::m_ready;
bool Scheduler::m_stopJob;
bool Scheduler::m_pausedWorkers;
bool Scheduler::m_noWork(false);
int Scheduler::m_pos(0);

//----------------------------------------------------------------------

void Scheduler::init(int workers) {
	m_ulock=std::unique_lock<std::mutex>(m_controlMutex);
	m_stop_ulock=std::unique_lock<std::mutex>(m_stopMutex);
	m_ready=false;
	m_pausedWorkers=false;
	m_noWork=true;
	m_stopJob=false;
	std::thread* thd = nullptr;
	Worker* worker = nullptr;
	for(int i=0; i<workers; ++i) {
		worker=new Worker(i, i, workers);
		m_workers.push_back(worker);
		thd = new std::thread(&Worker::run, worker);
		m_threads.push_back(thd);
		m_availableWorkers.push_back(true);
		m_stoppedWorkers.push_back(false);
	}
	
	m_bufferLN.reserve(100);
	m_bufferLNB.reserve(100);
	m_pos=0;
}

//----------------------------------------------------------------------

void Scheduler::spanThreads(){
	for(int i=0; i<m_threads.size(); i++){
		delete m_threads[i];
	}
	m_threads.clear();

	std::thread* tmpThd = nullptr;
	for(int i=0; i<m_workers.size(); ++i) {
		tmpThd = new std::thread(&Worker::run, m_workers[i]);
		m_threads.push_back(tmpThd);
		m_availableWorkers[i]=true;
	}
}

//----------------------------------------------------------------------

void Scheduler::destroy() {
	Logger::log("Destroying workers.");
	for (int i = 0; i < m_workers.size(); ++i) {
		delete m_workers[i];
		delete m_threads[i];
	}
}

//----------------------------------------------------------------------

void Scheduler::workerFinish(int workerID) {
	bool stop;
	m_stop_workersMutex.lock();
	stop=true;
	m_stoppedWorkers[workerID]=true;
	for(int i=0; i<m_stoppedWorkers.size(); i++){
		stop=stop && m_stoppedWorkers[i];
	}
	
	if(stop){
		m_noWork=true;
		m_stop_cv.notify_one();
	}
	m_stop_workersMutex.unlock();
}

//----------------------------------------------------------------------

void Scheduler::terminateWorkers() {
	m_noWork=false;
	bool noWork=true;

	for (int i = 0; i < m_workers.size(); ++i) {
		if(m_workers[i]->isRunning()){
			m_stoppedWorkers[i]=false;
		}
		else{
			m_stoppedWorkers[i]=true;
		}
		noWork=noWork && m_stoppedWorkers[i];
		m_workers[i]->terminate();
	}

	if(noWork){
		m_noWork=true;
		m_stop_cv.notify_one();
	}
	
	m_stop_cv.wait(m_stop_ulock, []{return Scheduler::m_noWork;});

	if(!m_ready){// in case wait() is waiting
		m_ready=true;
	}
	m_cv.notify_one();
	
	clearBuffers();
}

//----------------------------------------------------------------------

void Scheduler::terminate() {
	m_stopJob=true;
	terminateWorkers();
	for (int i = 0; i <m_threads.size(); ++i) {
		delete m_workers[i];
		m_threads[i]->detach();
		delete m_threads[i];
	}
}

//----------------------------------------------------------------------

void Scheduler::pauseWorkers(){
	m_stopJob=true;
	m_pausedWorkers=true;
	for(int i=0; i<m_availableWorkers.size(); i++){
		m_workers[i]->stopJob();
	}
}

//----------------------------------------------------------------------

void Scheduler::unpause(){
	m_stopJob=false;
	m_pausedWorkers=false;
	for(int i=0; i<m_availableWorkers.size(); i++){
		m_workers[i]->makeReady();
	}
}

//----------------------------------------------------------------------

void Scheduler::workerReady(int workerID) {
	bool ready;
	m_workersMutex.lock();
	ready=true;
	m_availableWorkers[workerID]=true;
	for(int i=0; i<m_availableWorkers.size(); i++){
		ready=ready && m_availableWorkers[i];
	}
	
	if(ready){
		m_ready=true;
		m_cv.notify_one();
	}
	m_workersMutex.unlock();
}

//----------------------------------------------------------------------

void Scheduler::feedData(Data* data){
	if(m_pausedWorkers){
		return;
	}
	m_currentData=data;
	for(int i=0; i<m_availableWorkers.size(); i++){
		m_workers[i]->setRequest(data);
		m_availableWorkers[i]=false;
		m_workers[i]->notify();
	}
}

//----------------------------------------------------------------------

void Scheduler::wait(){
	m_ready=false;
	m_cv.wait(m_ulock, []{return Scheduler::m_ready;});
}

//----------------------------------------------------------------------

void Scheduler::dumpData(DataBuffer& buffer){
	for(int i=0; i<buffer.size(); i++){
		m_bufferLN.push_back(buffer.m_lines[i]);
		m_bufferRank.push_back(buffer.m_ranks[i]);		
		m_bufferLNB.push_back(buffer.m_blocks[i]);
	}
}

//----------------------------------------------------------------------

bool Scheduler::getData(std::string*& str, bool& startBlock, int& rank){
	if(m_pos==m_bufferLN.size()){
		clearBuffers();
		return false;
	}

	if(m_bufferLNB[m_pos]){
		str=&(m_currentData->m_picsL)[m_bufferLN[m_pos]];	
	}
	else{
		str=&(m_currentData->m_picsR)[m_bufferLN[m_pos]];
	}
	startBlock=m_bufferLNB[m_pos];
	rank=m_bufferRank[m_pos];
	m_pos++;
	return true;
}

//----------------------------------------------------------------------

void Scheduler::clearBuffers(){
	m_pos=0;
	m_bufferLN.clear();
	m_bufferLNB.clear();
	m_bufferRank.clear();
}

//----------------------------------------------------------------------

void Scheduler::setSimilarityThreshold(int val){
	double threshold=0.9;
	if(val>0 && val<11){
		threshold=(1.0*val)/10.0;
	}
	for(int i=0; i<m_availableWorkers.size(); i++){
		m_workers[i]->setSimilarityThreshold(threshold);
	}
}

//----------------------------------------------------------------------

