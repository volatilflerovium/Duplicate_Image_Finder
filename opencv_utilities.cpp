#include <iostream>

#include "opencv_utilities.h"
#include "data.h"

//######################################################################

std::vector<std::string> exCmd(const char* cmd) {
    int bufferSize=528;
    char buffer[bufferSize];
    std::vector<std::string> result;
    std::FILE* pipe=popen(cmd, "r");
 
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    int k;
    while(fgets(buffer, bufferSize, pipe) != nullptr) {
        k=result.size();
        result.push_back(buffer);
        auto r=result[k].end();
        --r;

        if(*r=='\n'){
            result[k]=result[k].substr(0, result[k].size()-1);
        }
    }
    return result;
}

//--------------------------------------------------------------------

void sorting(std::vector<int>& rLineNumber, std::vector<double>& ranks, int lineNumber, double rank){
	rLineNumber.push_back(lineNumber);
	ranks.push_back(rank);
	int t=ranks.size()-1;
	for(int i=0; i<ranks.size(); i++){
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

//--------------------------------------------------------------------

cv::Mat mkMask(const cv::Mat& img, int offsetX, int offsetY, int wide) {
	int w=img.size().width-2*offsetX;
	int h=img.size().height-2*offsetY;
	cv::Mat inner(cv::Size(w, h), CV_8UC1, cv::Scalar(255));
	cv::Mat outter(img.size(), CV_8UC1, cv::Scalar(0));
	inner.copyTo(outter(cv::Rect(offsetX, offsetY, w, h)));
	w-=2*wide;
	h-=2*wide;
	cv::Mat inner2(cv::Size(w, h), CV_8UC1, cv::Scalar(0));
	inner2.copyTo(outter(cv::Rect(offsetX+wide, offsetY+wide, w, h)));

	return outter;
}

//--------------------------------------------------------------------

cv::Mat mkMask2(const cv::Mat& img) {
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

cv::Mat getNormalizeHistogramG(const cv::Mat& img, const cv::Mat& mask) {
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

cv::Mat getNormalizeHistogram(const cv::Mat& img, const cv::Mat& mask) {
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

//--------------------------------------------------------------------

std::vector<cv::Mat> getNormalizeHistogram8(const char* image_name) {
	std::vector<cv::Mat> result;
	result.reserve(DIMGS::DATA_SIZE);
	cv::Mat image=cv::imread(image_name);
	
	int r=DIMGS::DATA_SIZE/2;

	int m=image.cols/(2*(2+r));
	int n=image.rows/(2*(2+r));
	
	int wide=60;
	if(wide>n/2 || wide>m/2){
		wide=std::min(m/3, n/3);
	}
	int offsetX=m;
	int offsetY=n;

	cv::Mat total_mask=mkMask2(image);
	
	for(int i=0; i<r; i++){
		cv::Mat mask=mkMask(image, offsetX, offsetY, wide);
		mask&=total_mask;
		result.push_back(getNormalizeHistogram(image, mask));
		result.push_back(getNormalizeHistogramG(image, mask));
		offsetX+=m;
		offsetY+=n;
	}

	return result;
}


//--------------------------------------------------------------------

cv::Mat getNormalizeHistogram(const char* image_name, const cv::Mat& mask) {
	return  getNormalizeHistogram(cv::imread(image_name), mask);
}

//--------------------------------------------------------------------

void loadHist(std::vector<cv::Mat>& hists, const std::string& dir, int j){
	static std::string file_name;
	for(int k=0; k<DIMGS::DATA_SIZE; k++){
		file_name=dir+std::string("img")+std::to_string(j+1)+std::string("_")+std::to_string(k)+std::string(".yml");
		cv::FileStorage rstorage(file_name, cv::FileStorage::READ);
		rstorage["img"] >> hists[k];
		rstorage.release();
	}
}

//--------------------------------------------------------------------

void mkHistograms(const std::string& picsPath, const std::string& img_list){
	std::ofstream fileList(img_list, std::ios::app);
	std::string cmd("ls ");
	cmd+=picsPath;
	std::vector<std::string> results=exCmd(cmd.c_str());
	int numb=1;
	for(int i=0; i<results.size(); i++){
		try
		{
			results[i]=picsPath+results[i];
			std::vector<cv::Mat> hist_base1=getNormalizeHistogram8(results[i].c_str());

			for(int k=0; k<8; k++){
				std::string file_name=picsPath+std::string("img")+std::to_string(numb)+std::string("_")+std::to_string(k)+std::string(".yml");
				cv::FileStorage storage(file_name, cv::FileStorage::WRITE);
				storage << "img" << hist_base1[k];
				storage.release();
			}
			fileList<<results[i]<<"\n";
			numb++;
		}
		catch(...){
			std::cout<<results[i]<<"\n";
		}
	}
	fileList.close();
}

//--------------------------------------------------------------------

int blockComp(int r){
	int result=0;
	while(r>0){
		result+=r--;
	}
	return result;
}

void mkDummy(const char* imagePath){
	cv::Mat img(cv::Size(30, 30), CV_8UC1, cv::Scalar(255));
	cv::imwrite(imagePath, img);	
}
//######################################################################
