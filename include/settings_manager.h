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

class SettingsManager final
{
	public:
		~SettingsManager()
		{
			save();
		}

		static SettingsManager& getSettingManager()
		{
			static SettingsManager manager;
			return manager;
		}

		void save();
		
		bool updateNetwork();

		const char* getNodeBorderColor() const
		{
			return m_nodesColorBorder.c_str();
		}

		void setNodeBorderColor(const char* color)
		{
			m_nodesColorBorder=color;
		}

		template<typename FUNC>
		wxTextCtrl* NodeBorderColor(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, getNodeBorderColor(), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setNodeBorderColor(val.mb_str());
				cbk();	
			});
		}

		const char* getNodeBkgColor() const
		{
			return m_nodesBkgColor.c_str();
		}

		void setNodeBkgColor(const char* color)
		{
			m_nodesBkgColor=color;
		}

		template<typename FUNC>
		wxTextCtrl* NodeBkgColor(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, getNodeBkgColor(), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setNodeBkgColor(val.mb_str());
				cbk();
			});
		}

		const char* getNodeSelectedColor() const
		{
			return m_nodesSelectedColor.c_str();
		}

		void setNodeSelectedColor(const char* color)
		{
			m_nodesSelectedColor=color;
		}

		template<typename FUNC>
		wxTextCtrl* NodeSelectedColor(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, getNodeSelectedColor(), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setNodeSelectedColor(val.mb_str());
				cbk();
			});
		}

		const char* getNodeFontColor() const
		{
			return m_nodesFontColor.c_str();
		}

		void setNodeFontColor(const char* color)
		{
			m_nodesFontColor=color;
		}

		template<typename FUNC>
		wxTextCtrl* NodeFontColor(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, getNodeFontColor(), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setNodeBorderColor(val.mb_str());
				cbk();
			});
		}

		const char* getEdgesColor() const
		{
			return m_edgesColor.c_str();
		}
		
		void setEdgesColor(const char* color)
		{
			m_edgesColor=color;
		}

		template<typename FUNC>
		wxTextCtrl* EdgesColor(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, getEdgesColor(), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setEdgesColor(val.mb_str());
				cbk();
			});
		}

		const char* getCanvasColor() const
		{
			return m_canvasColor.c_str();
		}

		void setCanvasColor(const char* color)
		{
			m_canvasColor=color;
		}

		template<typename FUNC>
		wxTextCtrl* CanvasColor(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, getCanvasColor(), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setCanvasColor(val.mb_str());
				cbk();	
			});
		}

		uint getNodeBorderWidth() const
		{
			return m_borderWidth;
		}

		void setNodeBorderWidth(uint width)
		{
			m_borderWidth=width;
		}

		template<typename FUNC>
		wxTextCtrl* NodeBorderWidth(wxWindow* parent, FUNC cbk)
		{
			return builder(parent, wxString::Format("%d", getNodeBorderWidth()), [cbk](const wxString& val){
				SettingsManager::getSettingManager().setNodeBorderWidth(wxAtoi(val));
				cbk();	
			});
		}

		int getChunkL() const
		{
			return m_chunkL;
		}

		void setChunkL(int chunk)
		{
			m_chunkL=chunk;
		}

		int getChunkR() const
		{
			return m_chunkR;
		}

		void setChunkR(int chunk)
		{
			m_chunkR=chunk;
		}

		int getResume() const
		{
			return m_resume;
		}

		void setResume(int resume)
		{
			m_resume=resume;
		}

	private:
		std::string m_nodesColorBorder{"#406897"};
		std::string m_nodesBkgColor{"#FFFFAA"};
		std::string m_nodesSelectedColor{"#FFFF00"};
		std::string m_nodesFontColor{"#eeeeee"};
		std::string m_edgesColor{"lightgray"};
		std::string m_canvasColor{"#ebf5fc"};
		
		uint m_borderWidth{2};
		int m_chunkL{-1};
		int m_chunkR{-1};
		int m_resume{-1};

		SettingsManager()
		{
			loadSettings();
		}

		bool loadSettings();

		template<typename FUNC>
		wxTextCtrl* builder(wxWindow* parent, const char* val, FUNC cbk)
		{
			wxTextCtrl* ctrl=new wxTextCtrl(parent, wxID_ANY, 
			val, 
			wxDefaultPosition,
			//wxDefaultSize, 
			wxSize(120, -1), 
			wxTE_PROCESS_ENTER);

			ctrl->Bind(wxEVT_TEXT_ENTER, [cbk, ctrl](wxCommandEvent& event){			
				cbk(ctrl->GetValue());
			});
			
			return ctrl;
		}
};

//====================================================================

#endif
