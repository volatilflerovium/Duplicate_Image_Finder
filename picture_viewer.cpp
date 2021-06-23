#include "picture_viewer.h"

#include "data.h"
#include "data_logger.h"

//----------------------------------------------------------------------

PictureViewer::PictureViewer(wxPanel* parent)
:wxPanel(parent)
{
	wxBoxSizer* hBox = new wxBoxSizer(wxHORIZONTAL);
	//vbox->Add(hbox4, 1, wxLEFT | wxRIGHT | wxEXPAND, 10);
	SetSizer(hBox);//vbox);
	
	wxColour colour(wxT("#ffffff"));
	int minWidth=100;
	int minHeight=300;
	m_scroller=new ScrolledWidgetsPane(this, -1, minWidth);
	m_scroller->SetBackgroundColour(colour);
	m_scroller->SetMinSize(wxSize(minWidth, minHeight));
	hBox->Add(m_scroller, 0, wxEXPAND | wxTOP |  wxLEFT | wxRIGHT, 10);

	m_rightPanel=new wxImageW(this, wxString(FileManager::c_IMG_BACKGROUND), wxBITMAP_TYPE_ANY, wxDefaultPosition);//, wxSize(WX::MIN_WIDTH-minWidth, minHeight));

	m_rightPanel->SetBackgroundColour(colour);
	hBox->Add(m_rightPanel, 1, wxEXPAND | wxALL, 10);
	
	wxStackedImage::setViewer(m_rightPanel);

}

//----------------------------------------------------------------------

void PictureViewer::loadPicture(std::string filePath, bool newBlock, float rank){
	wxString wxStr(filePath);
	m_scroller->addImage(wxStr, FileManager::getBitmapType(wxStr), newBlock, rank);
}

//----------------------------------------------------------------------

void PictureViewer::clear(){
	m_scroller->clear();
	m_rightPanel->loadImage3(wxString(FileManager::c_IMG_BACKGROUND), wxBITMAP_TYPE_ANY);
	m_rightPanel->paintNow();
}

//----------------------------------------------------------------------



