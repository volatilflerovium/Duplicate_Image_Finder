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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>

//====================================================================

class Worker;

class DataBuffer;
class Data;

class Scheduler 
{
	public:
		static void init(int numWorkers);
		static void terminate();
		static void feedData(Data* data);
		static void wait();
		static void pauseWorkers();
		static void workerReady();
		static void dumpData(DataBuffer& buffer);
		static void workerFinish();

		static void start();
		static void unpause();
		static bool getData(std::string*& str, bool& startBlock, int& rank);		
		static bool isProcessing();
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

		struct Record
		{
			Record(int r, int ln, bool b)
			:m_Rank(r)
			, m_LN(ln)
			, m_LNB(b)
			{}
			
			int m_Rank;
			int m_LN;
			bool m_LNB;
		};

		static std::vector<Record> m_records;
		
		static std::vector<Worker*> m_workers;
		
		static Data* m_currentData;
		static size_t m_pos;
		static size_t m_ready;
		static bool m_pausedWorkers;
		static bool m_noWork;		
		static bool m_stopJob;

		static void destroy();
		static void clearBuffers();
};

//--------------------------------------------------------------------

inline void Scheduler::start()
{
	m_ready=0;
}

//--------------------------------------------------------------------

inline bool Scheduler::isProcessing()
{
	return m_stopJob;
}

//--------------------------------------------------------------------
#if 0
template<typename T, typename Func>
void Scheduler::harvestData(Func cb, T* t)
{
	int k=-1;
	for(int i=0; i<m_records.size(); i++){
		if(m_records[i].m_LNB){
			(t->*cb)(((m_currentData->m_picsL)[m_records[i].m_LN]).c_str());
		}
		else{
			(t->*cb)(((m_currentData->m_picsR)[m_records[i].m_LN]).c_str());
		}
	}
	m_records.clear();
}
#endif

//====================================================================

#endif
