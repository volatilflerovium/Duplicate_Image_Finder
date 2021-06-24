#include "notification_module.h"

NotificationModule* NotificationModule::instance(nullptr);

NotificationModule::NotificationModule(wxPanel* parent)
: wxPanel(parent)
{
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	const int lineHeight=30;
	wxStaticText* staticFilePath = new wxStaticText(this, wxID_ANY, wxT("File: "));
	hbox->Add(staticFilePath);

	m_filePathText=new wxStaticText(this, -1, wxT(""));
	hbox->Add(m_filePathText, 1, wxEXPAND, 10);

	wxStaticText* staticRank = new wxStaticText(this, wxID_ANY, wxT("rank: "));
	hbox->Add(staticRank, 0);

	m_rankText=new wxStaticText(this, -1, wxT("     "), wxDefaultPosition,  wxSize(70, lineHeight), wxALIGN_RIGHT);
	hbox->Add(m_rankText, 0, wxRIGHT, 10);
	
	SetSizerAndFit(hbox);
}

NotificationModule* NotificationModule::init(wxPanel* parent){
	if(!instance){
		instance=new NotificationModule(parent);
	}

	return instance;
}

void NotificationModule::setLabels(const wxString& filePath, float rank){
	m_filePathText->SetLabel(filePath);
	m_rankText->SetLabel(wxString::Format(wxT("%0.4f"), rank));
}

void NotificationModule::displayData(const wxString& filePath, float rank){
	instance->setLabels(filePath, rank);
}
