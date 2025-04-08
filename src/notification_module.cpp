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
#include "notification_module.h"
#include "wx_constants.h"

//----------------------------------------------------------------------

NotificationModule* NotificationModule::m_instance(nullptr);

//----------------------------------------------------------------------

NotificationModule::DataBlock::DataBlock(wxWindow* parent, const char* description, const wxSize& size, long align)
{		
	m_key=new wxStaticText(parent, wxID_ANY, wxString::Format(wxT("%s:"), description));
	m_val=new wxStaticText(parent, wxID_ANY, wxT("0"), wxDefaultPosition, size, align);
	m_sizer=new wxBoxSizer(wxHORIZONTAL);
	m_sizer->Add(m_key, 0, wxRIGHT, 10);
	m_sizer->Add(m_val, 1, wxEXPAND);
}

//----------------------------------------------------------------------

NotificationModule::DataBlock::~DataBlock()
{
	wxDELETE(m_key);
	wxDELETE(m_val);
}

//----------------------------------------------------------------------

NotificationModule::NotificationModule(wxWindow* parent)
: wxPanel(parent)
{
	const int lineHeight=20;

	m_rank=new DataBlock(this, "Rank", wxDefaultSize);
	m_nodes=new DataBlock(this, "Images", wxSize(50, lineHeight), wxALIGN_RIGHT);
	
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

	hbox->Add(m_rank->m_sizer, 1, wxEXPAND | wxRIGHT, 10);
	hbox->Add(1, 1, wxEXPAND);
	hbox->Add(m_nodes->m_sizer, 1, wxEXPAND);

	Bind(wxEVT_SIZE, &NotificationModule::onResize, this);

	SetSizerAndFit(hbox);
}

//----------------------------------------------------------------------

void NotificationModule::onResize(wxSizeEvent& event)
{
	Layout();
	event.Skip();
}
