/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class SettingsManager                                              *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    20-03-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _SETTINGS_MANAGER_H
#define _SETTINGS_MANAGER_H

#include <wx/textctrl.h>

//====================================================================

class SettingsManager 
{
	public:
		virtual ~SettingsManager();

		static const wxString& getNodeBorderColor()
		{
			return m_settingsPtr->m_nodesColorBorder;
		}

		static void setNodeBorderColor(const wxString& color)
		{
			m_settingsPtr->m_nodesColorBorder=color;
		}

		static const char* getNodeBkgColor()
		{
			return m_settingsPtr->m_nodesBkgColor.c_str();
		}

		static void setNodeBkgColor(const wxString& color)
		{
			m_settingsPtr->m_nodesBkgColor=color;
		}

		static const wxString& getNodeSelectedColor()
		{
			return m_settingsPtr->m_nodesSelectedColor;
		}

		static void setNodeSelectedColor(const wxString& color)
		{
			m_settingsPtr->m_nodesSelectedColor=color;
		}

		static const wxString& getNodeFontColor()
		{
			return m_settingsPtr->m_nodesFontColor;
		}

		static void setNodeFontColor(const wxString& color)
		{
			m_settingsPtr->m_nodesFontColor=color;
		}

		static const wxString& getEdgesColor()
		{
			return m_settingsPtr->m_edgesColor;
		}
		
		static void setEdgesColor(const wxString& color)
		{
			m_settingsPtr->m_edgesColor=color;
		}

		static const wxString& getCanvasColor()
		{
			return m_settingsPtr->m_canvasColor;
		}

		static void setCanvasColor(const wxString& color)
		{
			m_settingsPtr->m_canvasColor=color;
		}

		static uint getNodeBorderWidth()
		{
			return m_settingsPtr->m_borderWidth;
		}

		static void setNodeBorderWidth(uint width)
		{
			m_settingsPtr->m_borderWidth=width;
		}

		static int getChunkL()
		{
			return m_settingsPtr->m_chunkL;
		}

		static void setChunkL(int chunk)
		{
			m_settingsPtr->m_chunkL=chunk;
		}

		static int getChunkR()
		{
			return m_settingsPtr->m_chunkR;
		}

		static void setChunkR(int chunk)
		{
			m_settingsPtr->m_chunkR=chunk;
		}

		static int getResume() 
		{
			return m_settingsPtr->m_resume;
		}

		static void setResume(int resume)
		{
			m_settingsPtr->m_resume=resume;
		}

	private:
		wxString m_nodesColorBorder{"#406897"};
		wxString m_nodesBkgColor{"#FFFFAA"};
		wxString m_nodesSelectedColor{"#FFFF00"};
		wxString m_nodesFontColor{"#eeeeee"};
		wxString m_edgesColor{"lightgray"};
		wxString m_canvasColor{"#ebf5fc"};
		static SettingsManager* m_settingsPtr;
		
		uint m_borderWidth{2};
		int m_chunkL{-1};
		int m_chunkR{-1};
		int m_resume{-1};
		
		SettingsManager();

		bool updateNetwork();

		struct Deleter
		{
			Deleter()
			:m_settingsPtr(nullptr)
			{}
			
			~Deleter()
			{
				if(m_settingsPtr){
					delete m_settingsPtr;
				}
			}

			SettingsManager* m_settingsPtr;			
		};	

		static SettingsManager* init()
		{
			static Deleter deleter;
			if(!deleter.m_settingsPtr){
				deleter.m_settingsPtr=new SettingsManager;
			}
			return deleter.m_settingsPtr;
		}
};

//====================================================================

#endif
