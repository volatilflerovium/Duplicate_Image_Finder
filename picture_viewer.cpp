#include "picture_viewer.h"

#include "data.h"
#include "data_logger.h"
#include "wx_stacked_image.h"
#include "wx_constants.h"

//----------------------------------------------------------------------

PictureViewer::PictureViewer(wxPanel* parent)
:wxPanel(parent)
{
	wxBoxSizer* hBox = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(hBox);

	m_scroller=new ScrolledWidgetsPane(this, -1, c_scrollerWidth);
	m_scroller->SetBackgroundColour(*wxWHITE);
	m_scroller->SetMinSize(wxSize(c_scrollerWidth, c_scrollerMinHeight));

	wxBoxSizer* vBox2 = new wxBoxSizer(wxVERTICAL);

	vBox2->Add(m_scroller, 1, wxEXPAND, 0);
	hBox->Add(vBox2, 0, wxEXPAND | wxRIGHT, 10);

	m_rightPanel=new PicWrapper2(this, wxSize(MIN_WIDTH-c_scrollerWidth, c_scrollerMinHeight), 0);
	hBox->Add(m_rightPanel, 1, wxEXPAND, 0);
	hBox->Layout();
	
	wxStackedImage::setViewer(m_rightPanel);
}

//----------------------------------------------------------------------

void PictureViewer::loadPicture(std::string filePath, bool newBlock, float rank){
	m_scroller->addImage(wxString(filePath), newBlock, rank);
}

//----------------------------------------------------------------------

void PictureViewer::clear(){
	m_scroller->clear();
	wxStackedImage::clearBackground();	
	m_rightPanel->wallPaper();	
}

//----------------------------------------------------------------------
