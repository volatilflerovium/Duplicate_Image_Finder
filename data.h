/*********************************************************************
* DIMGS namespace                                				         *
* Data struct                                                        *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/

#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>
#include "opencv_utilities.h"

namespace DIMGS
{
	const int DATA_SIZE=8;
	const float THRESH=0.98;//0.70; //0.98
	const int BLOCK_SIZE=50;
	const int STOP=1;
	const int PREPARE=0;
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
	std::vector<float> m_ranks;
	
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

namespace WX{
	enum 
	{
		BROWSE=101,
		PICTURE_SCROLLER,
		PICTURE_VIWER,
		ADD_PIC,
		CLEAR,
		START,
		CANCEL,
		DELETE_IMG,
		OPEN_IMG,
	};
	const int MIN_WIDTH=800;
	const int MIN_HEIGHT=800;
};

//======================================================================

enum class SUBDIR
{
	IS_NEW=0,
	IS_SUB,
	IS_SUP,
	
};

//======================================================================


#endif
