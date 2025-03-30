/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* DIMGS namespace                                				         *
* Data struct                                                        *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021  (Reviewed 03-2025)                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef DATA_H
#define DATA_H
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

//======================================================================

namespace DIMGS
{
	const int DATA_SIZE=8;
	const int BLOCK_SIZE=50;
	const int FLOAT_FACTOR=10000;
	const float INV_FLOAT_FACTOR=1.0/(1.0*FLOAT_FACTOR);
}

//======================================================================

struct Data
{
	std::vector<cv::Mat> m_histogramsL;
	std::vector<cv::Mat> m_histogramsR;
	std::vector<std::string> m_picsL;
	std::vector<std::string> m_picsR;
	int m_sentinelL{0};
	int m_sentinelR{0};
	bool m_mode{true};

	Data()
	:m_histogramsL(DIMGS::BLOCK_SIZE*DIMGS::DATA_SIZE),
	m_histogramsR(DIMGS::BLOCK_SIZE*DIMGS::DATA_SIZE),
	m_picsL(DIMGS::BLOCK_SIZE),
	m_picsR(DIMGS::BLOCK_SIZE)
	{}
};

//======================================================================

struct DataBuffer
{
	std::vector<int> m_lines;
	std::vector<bool> m_blocks;
	std::vector<int> m_ranks;
	
	DataBuffer(int n){
		m_lines.reserve(n);
		m_blocks.reserve(n);
		m_ranks.reserve(n);
	}
	
	void insert(int line, float rk, bool block){
		m_lines.push_back(line);
		m_blocks.push_back(block);
		m_ranks.push_back(rk);
	}
	
	int size() const{
		return m_lines.size();
	}
};



//======================================================================


#endif
