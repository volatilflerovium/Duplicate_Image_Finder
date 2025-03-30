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
#ifndef WORKER_H
#define WORKER_H

#include <mutex>
#include <thread>
#include <condition_variable>

//====================================================================

struct Data;

class Worker {

	public:
		Worker(int position, int step);

		virtual ~Worker();

		void setRequest(const Data* data);

		void run();
		void terminate();
		void stopJob();
		bool isRunning() const;
		bool isAvailable() const;
		void notify();
		void makeReady();
		void setSimilarityThreshold(double threshold);

	private:
		std::condition_variable m_cv;
		static std::mutex m_bufferMtx;
		std::mutex m_mtx;
		std::unique_lock<std::mutex> m_ulock;
		std::thread* m_jobPtr;
		const Data* m_data;
		double m_similarityThreshold;
		const int m_step;
		const int m_position;
		bool m_running;
		bool m_ready;
		bool m_exitJob;
		bool m_available;
		void findSimilarImages();
};

//----------------------------------------------------------------------

inline bool Worker::isRunning() const
{
	return m_running;
};

//----------------------------------------------------------------------

inline bool Worker::isAvailable() const
{
	return m_available;
};

//----------------------------------------------------------------------

inline void Worker::notify()
{
	m_ready = true;
	m_cv.notify_one();
}

//----------------------------------------------------------------------

inline void Worker::makeReady()
{
	m_exitJob=false;	
}

//----------------------------------------------------------------------

inline void Worker::setSimilarityThreshold(double threshold)
{
	m_similarityThreshold=threshold;
}
	
#endif
