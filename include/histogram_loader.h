/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* HistogramLoader class                           				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    29-05-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef HISTOGRAMLOADER_H
#define HISTOGRAMLOADER_H
#include "data.h"
#include "settings_manager.h"

#include <vector>
#include <string>
#include <fstream>
#include <thread>

//====================================================================

class HistogramLoader
{
	public:

		static void init( 
			const std::string& directory, 
			const std::string& vdoList
		);

		static Data* loadHistgrams();
		
		static void setChunks(int chunkL, int chunkR){
			if(chunkL<=chunkR){
				m_chunkNumberL=chunkL;
				m_chunkNumberR=chunkR;
			}
		}
		
		static void saveCurrentState()
		{
			SettingsManager::getSettingManager().setChunkL(m_chunkNumberL);
			SettingsManager::getSettingManager().setChunkR(m_chunkNumberR);
		}

	private:
		static std::string m_directory;
		static std::string m_picList;
		static Data m_data;
		static const int m_limit;
		static int m_chunkNumberL;
		static int m_chunkNumberR;
		static int m_position;
		static int m_sentinelL;
		static int m_sentinelR;

		static bool loadHistgrams(	
			std::vector<cv::Mat>& histograms,
			std::vector<std::string>& pics, 
			int chunkNumber);
};

//#####################################################################

#endif
