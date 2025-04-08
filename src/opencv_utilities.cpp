/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*                                                                    *
* class ImageProcessor                                               *
static cv::Mat mkMaskStable(const cv::Mat& img)
static cv::Mat getNormalizeHistogramG(const cv::Mat& img, const cv::Mat& mask)
static cv::Mat getNormalizeHistogram(const cv::Mat& img, const cv::Mat& mask)
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021   (Reviewed 03/2025)                           *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "opencv_utilities.h"
#include "file_manager.h"

#include <fstream>
#include <iostream>

#include "utilities/debug_utils.h"

//====================================================================

static cv::Mat mkMaskStable(const cv::Mat& img)
{
	static const int histSize[] = {256};
	static const float h_ranges[]={0, 256};
	static const float* ranges[] = {h_ranges};
	static const int channels[]={0};
	cv::Mat img_g;
	cv::cvtColor(img, img_g, cv::COLOR_BGR2GRAY);
	
	cv::Mat hist;
	calcHist(&img_g, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);
	int t=0;
	float tf=1;
	for(int i=0; i<256; i++){
		t+=hist.at<float>(i);
	}

	if(t>0){
		tf=1.0/t;
	}

	float b=(100.0*hist.at<float>(0))*tf;
	cv::Mat inner;
	bool a=false;
	if(b>20.0){
		 cv::threshold(img_g, inner, 1, 255, cv::THRESH_BINARY);
		 a=true;
	}

	b=(100.0*hist.at<float>(255))*tf;
	
	if(b>20.0){
		cv::Mat inner2;
		threshold(img_g, inner2, 254, 0, cv::THRESH_TOZERO_INV);
		threshold(inner2, inner2, 1, 255, cv::THRESH_BINARY);
		if(!a){
			return inner2;
		}
		
		return inner&inner2;
	}

	if(!a){
		return cv::Mat(img.size(), CV_8UC1, cv::Scalar(255));
	}
	
	return inner;
}

//--------------------------------------------------------------------

static cv::Mat getNormalizeHistogramG(const cv::Mat& img, const cv::Mat& mask)
{
	static const int histSize[] = {256};
	static const float h_ranges[]={0, 256};
	static const float* ranges[] = {h_ranges};
	static const int channels[]={0};

	cv::Mat img_hsv;
	cv::cvtColor(img, img_hsv, cv::COLOR_BGR2GRAY);
	
	cv::Mat hist_base;
	cv::calcHist(&img_hsv, 1, channels, mask, hist_base, 1, histSize, ranges, true, false);
	
	normalize(hist_base, hist_base, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

	return hist_base;
}

//--------------------------------------------------------------------

static cv::Mat getNormalizeHistogram(const cv::Mat& img, const cv::Mat& mask)
{
	//static int h_bins = 50, 
	//static s_bins = 60;
	static const int histSize[] = {50, 60};
	static const float h_ranges[]={0, 180};// hue varies from 0 to 179, 
	static const float s_ranges[]={0, 256};// saturation varies from 0 to 255
	static const float* ranges[] = {h_ranges, s_ranges};
	static const int channels[]={0, 1};// Use the 0-th and 1-st channels

	cv::Mat img_hsv;
	cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);

	cv::Mat hist_base;
	calcHist(&img_hsv, 1, channels, mask, hist_base, 2, histSize, ranges, true, false );
	normalize(hist_base, hist_base, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	return hist_base;
}

//====================================================================

void ImageProcessor::makeNormalizeHistogram(const char* image_name)
{
	cv::Mat image=cv::imread(image_name, cv::IMREAD_COLOR);

	//image=sharpImage(image);

	if(m_filterSize>0){
		cv::blur(image, image, cv::Size(m_filterSize*2+1, m_filterSize*2+1));
	}

	const int paddingW=image.cols/(2*(DIMGS::DATA_SIZE+1));
	const int paddingH=image.rows/(2*(DIMGS::DATA_SIZE+1));

	int wideW=paddingW/2;
	int wideH=paddingH/2;

	int offsetX=paddingW;
	int offsetY=paddingH;
	
	const float kpX=(image.cols*1.0)/(1.0*wideW);
	const float kpY=(image.rows*1.0)/(1.0*wideH);
	const cv::Mat total_mask=mkMaskStable(image);
	for(int i=0; i<DIMGS::DATA_SIZE; i++){
		
		if(m_useHashMask){
			mkMaskHash(image, offsetX, offsetY, wideW, wideH);
		}
		else{
			mkMaskRect(image, offsetX, offsetY, wideW, wideH);
		}

		m_mask&=total_mask;

		if(i%2==0){
			m_histograms[i]=::getNormalizeHistogram(image, m_mask);
		}
		else{
			m_histograms[i]=::getNormalizeHistogramG(image, m_mask);
		}

		offsetX+=paddingW;
		offsetY+=paddingH;
		
		if(m_dynamicMask){
			wideW=(1.0*(image.cols-2*offsetX))/kpX;
			wideH=(1.0*(image.rows-2*offsetY))/kpY;
		}
	}
}

//--------------------------------------------------------------------

void ImageProcessor::mkMaskRect(const cv::Mat& img, int offsetX, int offsetY, int wideX, int wideY)
{
	resetMask(img.size());

	const int width=m_mask.cols;
	const int heigh=m_mask.rows;
	for(int x=0; x<width; x++){
		for(int y=0; y<heigh; y++){
			if((y>=offsetY && y<=offsetY+wideY) || (y>=heigh-offsetY-wideY && y<=heigh-offsetY)){
				if((x>=offsetX && x<=width-offsetX)){
					m_mask.at<uchar>(y, x) = 255;
				}
			}

			if((x>=offsetX && x<=offsetX+wideX) || (x>=width-offsetX-wideX && x<=width-offsetX)){
				if((y>=offsetY && y<=heigh-offsetY)){
					m_mask.at<uchar>(y, x) = 255;
				}
			}
		}
	}
}

//--------------------------------------------------------------------

void ImageProcessor::mkMaskHash(const cv::Mat& img, int offsetX, int offsetY, int wideX, int wideY)
{
	resetMask(img.size());

	const int width=m_mask.cols;
	const int heigh=m_mask.rows;
	
	for(int i=0; i<m_mask.cols; i++){
		for(int j=0; j<m_mask.rows; j++){
			if((j>=offsetY && j<=offsetY+wideY) || (j>=heigh-offsetY-wideY && j<=heigh-offsetY)){
				m_mask.at<uchar>(j, i) = 255;
			}

			if((i>=offsetX && i<=offsetX+wideX) || (i>=width-offsetX-wideX && i<=width-offsetX)){
				m_mask.at<uchar>(j, i) = 255;
			}
		}
	}
}

//--------------------------------------------------------------------

void ImageProcessor::resetMask(const cv::Size& imgSize)
{
	if(m_mask.size()!=imgSize){
		m_mask=cv::Mat(imgSize, CV_8UC1, cv::Scalar(0));
	}
	else{
		m_mask.setTo(cv::Scalar(0));
	}
}

//--------------------------------------------------------------------

bool ImageProcessor::setupHistograms(const char* image_name, int lineNumb)
{
	getInstance().makeNormalizeHistogram(image_name);

	for(int k=0; k<DIMGS::DATA_SIZE; k++){
		std::string file_name=FileManager::c_DIR_HIST+std::string("img")+std::to_string(lineNumb)+std::string("_")+std::to_string(k)+std::string(".yml");
		cv::FileStorage storage(file_name, cv::FileStorage::WRITE);
		storage << "img" << getInstance().m_histograms[k];
		storage.release();
	}
	return true;
}

//--------------------------------------------------------------------

bool ImageProcessor::settinsChanged()
{
	static uint filterSize=m_filterSize;
	static bool dynamicMask=m_dynamicMask;
	static bool useHashMask=m_useHashMask;
	
	bool result= filterSize!=m_filterSize || dynamicMask!=m_dynamicMask || useHashMask!=m_useHashMask;
	filterSize=m_filterSize;
	dynamicMask=m_dynamicMask;
	useHashMask=m_useHashMask;
	return result;
}

//--------------------------------------------------------------------

void ImageProcessor::printMat(const cv::Mat& img, const char* filePath)
{
	if(std::strlen(filePath)>0){
		cv::imwrite(filePath, img);
		return;
	}

	static int d1=0;
	std::string testImgName="/tmp/sample";
	testImgName.append(std::to_string(d1++));
	testImgName.append(".png");
	cv::imwrite(testImgName, img);
}

//====================================================================
