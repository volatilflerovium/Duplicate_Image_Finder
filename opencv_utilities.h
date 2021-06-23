/*********************************************************************
*                                                                    *
* std::vector<std::string> exCmd(const char* cmd);                   *
* void sorting(std::vector<int>&, std::vector<double>&, int, double);*
* cv::Mat mkMask2(const cv::Mat&);                                   *
* cv::Mat mkMask(const cv::Mat&, int, int, int);                     *
* cv::Mat getNormalizeHistogramG(const cv::Mat&, const cv::Mat&);    *
* cv::Mat getNormalizeHistogram(const cv::Mat&, const cv::Mat&);     *
* std::vector<cv::Mat> getNormalizeHistogram8(const char*);          *
* void loadHist(std::vector<cv::Mat>&, const std::string&, int;      *
* void mkHistograms(const std::string&, const std::string&);         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/

#ifndef OPENCV_UTILITIES_H
#define OPENCV_UTILITIES_H

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <mutex>
#include <thread>
#include <ctime>
#include <map>

#include <unistd.h>


//####################################################################

std::vector<std::string> exCmd(const char* cmd);

void sorting(std::vector<int>& rLineNumber, std::vector<double>& ranks, int lineNumber, double rank);

cv::Mat mkMask2(const cv::Mat& img);

cv::Mat mkMask(const cv::Mat& img, int offsetX, int offsetY, int wide);

cv::Mat getNormalizeHistogramG(const cv::Mat& img, const cv::Mat& mask);

cv::Mat getNormalizeHistogram(const cv::Mat& img, const cv::Mat& mask);

std::vector<cv::Mat> getNormalizeHistogram8(const char* image_name);

void loadHist(std::vector<cv::Mat>& hists, const std::string& dir, int j);

void mkHistograms(const std::string& picsPath, const std::string& video_list);

int blockComp(int x);

void mkDummy(const char* path);
//######################################################################


#endif
