/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* Worker class                                   				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021   (Reviewed 03/2025)                           *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "worker.h"
#include "data.h"
#include "scheduler.h"
#include "data_logger.h"
#include "opencv_utilities.h"

//====================================================================

static void sorting(std::vector<int>& rLineNumber, std::vector<double>& ranks, int lineNumber, double rank)
{
	rLineNumber.push_back(lineNumber);
	ranks.push_back(rank);
	int t=ranks.size()-1;
	for(size_t i=0; i<ranks.size(); i++){
		if(ranks[i]<rank){
			for(int j=i; j<t; j++){
				ranks[t]=ranks[j];
				ranks[j]=rank;
				rank=ranks[t];
				rLineNumber[t]=rLineNumber[j];
				rLineNumber[j]=lineNumber;
				lineNumber=rLineNumber[t];
			}
			break;
		}
	}
}

//====================================================================

std::mutex Worker::m_bufferMtx;

//----------------------------------------------------------------------

Worker::Worker(int position, int step)
:m_ulock(std::unique_lock<std::mutex>(m_mtx))
, m_jobPtr(nullptr)
, m_data(nullptr)
, m_similarityThreshold(0.9)
, m_step(step)
, m_position(position)
, m_running(true)
, m_ready(false)
, m_exitJob(false)
, m_available(true)
{
	m_jobPtr=new std::thread(&Worker::run, this);
}

//----------------------------------------------------------------------

Worker::~Worker()
{
	if(m_jobPtr){
		terminate();
		if(m_jobPtr->joinable()){
			m_jobPtr->join();
		}
		delete m_jobPtr;
		m_jobPtr=nullptr;
	}
}

//----------------------------------------------------------------------

void Worker::terminate()
{
	m_ready = true;
	m_running = false;
	m_exitJob=true;
	m_cv.notify_one();
}

//----------------------------------------------------------------------

void Worker::stopJob()
{
	m_ready=false;
	m_exitJob=true;
}

//----------------------------------------------------------------------

void Worker::run() 
{
	while(m_running){
		if(m_ready){
			m_ready = false;
			m_available=false;
			findSimilarImages();
			m_available=true;
		}

		Scheduler::workerReady();		
		m_cv.wait(m_ulock, [this]{return this->m_ready;});
	}
	Scheduler::workerFinish();
}

//----------------------------------------------------------------------

void Worker::setRequest(const Data* data)
{
	m_available=false;
	m_data=data;
}

//----------------------------------------------------------------------

void Worker::findSimilarImages()
{
	if(!m_data){
		return;
	}

	DataBuffer buffer(m_data->m_sentinelL);

	//int line_number=1;
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
	
			for(int k=0; k<DIMGS::DATA_SIZE; k++){
				similarity+=cv::compareHist((m_data->m_histogramsL)[chunk1+k], (m_data->m_histogramsR)[chunk2+k], cv::HISTCMP_BHATTACHARYYA);
				//similarity+=cv::compareHist((m_data->m_histogramsL)[chunk1+k], (m_data->m_histogramsR)[chunk2+k], cv::HISTCMP_CORREL);
			}
			similarity*=averageFactor;
			similarity=1-similarity;

			//Logger::log(similarity, " ", (m_data->m_picsL)[i], " :: ", (m_data->m_picsR)[j], "\n");
			
			//"Testing: ", (m_data->m_picsL)[i], " wk: ", m_wid);
			if(similarity>=m_similarityThreshold){//Scheduler::getSimilarityThreshold()){//DIMGS::THRESH){
				sorting(rLineNumber, picRanks, j, similarity);	
			}

			if(j%20==0){
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		chunk1+=DIMGS::DATA_SIZE;
		if(rLineNumber.size()>0){
			//Logger::log("Testing: ", (m_data->m_picsL)[i], " wk: ", m_wid);
			buffer.insert(i, DIMGS::FLOAT_FACTOR, true);
			for(size_t k=0; k<rLineNumber.size(); k++){
				//Logger::log((m_data->m_picsR)[rLineNumber[k]]," ", picRanks[k]);			
				buffer.insert(rLineNumber[k], picRanks[k]*DIMGS::FLOAT_FACTOR, false);
				if(k>4){
					break;
				}
			}
		}
		i+=m_step;
	}
	//dbg("++++++++++++++++++++++++++++ ", m_position, " ", m_data->m_sentinelL);
	Worker::m_bufferMtx.lock();
	Scheduler::dumpData(buffer);
	Worker::m_bufferMtx.unlock();
}

//====================================================================
