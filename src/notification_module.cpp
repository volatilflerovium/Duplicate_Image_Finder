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

NotificationModule* NotificationModule::m_instance(nullptr);

//----------------------------------------------------------------------

NotificationModule::NotificationModule(wxWindow* parent)
: wxPanel(parent)
{
	SetMinSize(wxSize(WX::MIN_WIDTH, 30));
	SetSize(wxSize(WX::MIN_WIDTH, 30));

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	const int lineHeight=30;
	wxStaticText* staticFilePath = new wxStaticText(this, wxID_ANY, wxT("File: "));
	hbox->Add(staticFilePath);
	
	int width=20+staticFilePath->GetSize().GetWidth();

	wxStaticText* staticRank = new wxStaticText(this, wxID_ANY, wxT("rank: "));
	width+=staticRank->GetSize().GetWidth();

	m_rankText=new wxStaticText(this, -1, wxT("     "), wxDefaultPosition,  wxSize(70, lineHeight), wxALIGN_RIGHT);
	width+=m_rankText->GetSize().GetWidth();
	
	m_filePathText=new wxStaticText(this, -1, wxT(""),  wxDefaultPosition, wxSize(WX::MIN_WIDTH-width, lineHeight), wxST_ELLIPSIZE_MIDDLE);
	
	hbox->Add(m_filePathText, 1, wxEXPAND, 10);
	hbox->Add(staticRank, 0);
	hbox->Add(m_rankText, 0, wxRIGHT, 10);

	Bind(wxEVT_SIZE, &NotificationModule::onResize, this);

	SetSizerAndFit(hbox);
}

//----------------------------------------------------------------------

NotificationModule::~NotificationModule(){
	wxDELETE(m_filePathText);
	wxDELETE(m_rankText);
}

//----------------------------------------------------------------------

NotificationModule* NotificationModule::init(wxWindow* parent){
	if(!m_instance){
		m_instance=new NotificationModule(parent);
	}

	return m_instance;
}

//----------------------------------------------------------------------

void NotificationModule::setLabels(const wxString& filePath, float rank){
	m_filePathText->SetLabel(filePath);
	m_rankText->SetLabel(wxString::Format(wxT("%0.4f"), rank));
	Layout();
}

//----------------------------------------------------------------------

void NotificationModule::Clear(){
	m_instance->clear();
}

//----------------------------------------------------------------------

void NotificationModule::clear(){
	m_filePathText->SetLabel(wxT(""));
	m_rankText->SetLabel(wxT(""));
	Layout();
}

//----------------------------------------------------------------------

void NotificationModule::displayData(const wxString& filePath, float rank){
	m_instance->setLabels(filePath, rank);
}

//----------------------------------------------------------------------

void NotificationModule::onResize(wxSizeEvent& event){
	Layout();
	event.Skip();
}
