#include "notification_module.h"
#include "wx_constants.h"

NotificationModule* NotificationModule::instance(nullptr);

//----------------------------------------------------------------------

NotificationModule::NotificationModule(wxPanel* parent)
: wxPanel(parent)
{
	SetMinSize(wxSize(MIN_WIDTH, 30));
	SetSize(wxSize(MIN_WIDTH, 30));

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	const int lineHeight=30;
	wxStaticText* staticFilePath = new wxStaticText(this, wxID_ANY, wxT("File: "));
	hbox->Add(staticFilePath);
	
	int width=20+staticFilePath->GetSize().GetWidth();

	wxStaticText* staticRank = new wxStaticText(this, wxID_ANY, wxT("rank: "));
	width+=staticRank->GetSize().GetWidth();

	m_rankText=new wxStaticText(this, -1, wxT("     "), wxDefaultPosition,  wxSize(70, lineHeight), wxALIGN_RIGHT);
	width+=m_rankText->GetSize().GetWidth();
	
	m_filePathText=new wxStaticText(this, -1, wxT(""),  wxDefaultPosition, wxSize(MIN_WIDTH-width, lineHeight), wxST_ELLIPSIZE_MIDDLE);
	
	hbox->Add(m_filePathText, 1, wxEXPAND, 10);
	hbox->Add(staticRank, 0);
	hbox->Add(m_rankText, 0, wxRIGHT, 10);

	Bind(wxEVT_SIZE, &NotificationModule::onResize, this);

	SetSizerAndFit(hbox);
}

//----------------------------------------------------------------------

NotificationModule* NotificationModule::init(wxPanel* parent){
	if(!instance){
		instance=new NotificationModule(parent);
	}

	return instance;
}

//----------------------------------------------------------------------

void NotificationModule::setLabels(const wxString& filePath, float rank){
	m_filePathText->SetLabel(filePath);
	m_rankText->SetLabel(wxString::Format(wxT("%0.4f"), rank));
	Layout();
}

//----------------------------------------------------------------------

void NotificationModule::displayData(const wxString& filePath, float rank){
	instance->setLabels(filePath, rank);
}

//----------------------------------------------------------------------

void NotificationModule::onResize(wxSizeEvent& event){
	Layout();
	event.Skip();
}
