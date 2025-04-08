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
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021   (Reviewed 03/2025)                           *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef OPENCV_UTILITIES_H
#define OPENCV_UTILITIES_H
#include "data.h"

#include "utilities/debug_utils.h"

//====================================================================

class ImageProcessor final
{
	public:
		enum MaskMode
		{
			RectD=0,
			RectF,
			HashD,
			HashF,
		};

	public:
		~ImageProcessor()=default;

		static void setFilterSize(uint filterSize);
		static void setMaskMode(const MaskMode maskMode);

		static bool setupHistograms(const char* image_name, int lineNumb);
		static bool rebuildHistograms();
		static void printMat(const cv::Mat& img, const char* filePath="");

	private:
		cv::Mat m_histograms[DIMGS::DATA_SIZE];
		cv::Mat m_mask;
		unsigned char m_lastData;
		uint m_filterSize;
		bool m_dynamicMask;
		bool m_useHashMask;

		void resetMask(const cv::Size& imgSize);
		void mkMaskRect(const cv::Mat& img, int offsetX, int offsetY, int wideX, int wideY);
		void mkMaskHash(const cv::Mat& img, int offsetX, int offsetY, int wideX, int wideY);

		bool settinsChanged();		

		ImageProcessor();
		static ImageProcessor& getInstance();

		void makeNormalizeHistogram(const char* image_name);
};

//--------------------------------------------------------------------

inline ImageProcessor::ImageProcessor()
:m_filterSize(0)
, m_dynamicMask(true)
, m_useHashMask(false)
{
	settinsChanged();
}

//--------------------------------------------------------------------

inline ImageProcessor& ImageProcessor::getInstance()
{
	static ImageProcessor instance;
	return instance;
}

//--------------------------------------------------------------------

inline void ImageProcessor::setFilterSize(uint filterSize)
{
	getInstance().m_filterSize=filterSize;
}

//--------------------------------------------------------------------

inline void ImageProcessor::setMaskMode(const MaskMode maskMode)
{
	getInstance().m_dynamicMask=maskMode==MaskMode::RectD || maskMode==MaskMode::HashD;
	getInstance().m_useHashMask=maskMode==MaskMode::HashF || maskMode==MaskMode::HashD;
}

//--------------------------------------------------------------------

inline bool ImageProcessor::rebuildHistograms()
{
	return getInstance().settinsChanged();
}
		
//====================================================================

#endif
