/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* NotifyLabel class                                       			   *
*                                                                    *
* Version: 1.0                                                       *
* Date:    24-06-2021   (Reviewed 03/2025)                           *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef NOTIFICATION_MODULE_H
#define NOTIFICATION_MODULE_H

#include <wx/wx.h>

//====================================================================

class NotificationModule : public wxPanel
{
	public:
		virtual ~NotificationModule();

		static NotificationModule* init(wxWindow* parent);
		static void Clear();
		static void setRank(float rank);
		static void setNodes(int rank);
		static void setNodes(const wxString& rank);
		
	private:
		static NotificationModule* m_instance;

		struct DataBlock
		{
			DataBlock(wxWindow* parent, const char* description, const wxSize& size, long align=wxALIGN_LEFT);
			~DataBlock();

			void setValue(const wxString& value)
			{
				m_val->SetLabel(value);
			}

			void setValue(int value)
			{
				setValue(wxString::Format(wxT("%d"), value));
			}
			
			void clear(const char* val="")
			{
				m_val->SetLabel(val);
			}

			wxStaticText* m_key;
			wxStaticText* m_val;
			wxBoxSizer* m_sizer;
		};
		
		DataBlock* m_nodes;
		DataBlock* m_rank;
		
		NotificationModule(wxWindow* parent);
		void onResize(wxSizeEvent& evt);
};

//----------------------------------------------------------------------

inline NotificationModule::~NotificationModule()
{
	wxDELETE(m_nodes);
	wxDELETE(m_rank);
}

//----------------------------------------------------------------------

inline NotificationModule* NotificationModule::init(wxWindow* parent)
{
	if(!m_instance){
		m_instance=new NotificationModule(parent);
	}
	return m_instance;
}

//----------------------------------------------------------------------

inline void NotificationModule::setNodes(const wxString& nodes)
{
	m_instance->m_nodes->setValue(nodes);
}
//----------------------------------------------------------------------

inline void NotificationModule::setNodes(int nodes)
{
	m_instance->m_nodes->setValue(nodes);
}

//----------------------------------------------------------------------

inline void NotificationModule::setRank(float rank)
{
	m_instance->m_rank->setValue(wxString::Format(wxT("%0.4f"), rank));
}

//----------------------------------------------------------------------

inline void NotificationModule::Clear()
{
	m_instance->m_rank->clear();
	m_instance->m_nodes->clear("0");
}

//====================================================================

#endif
