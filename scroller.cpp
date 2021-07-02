#include <iostream>
#include "scroller.h"
#include "wx_stacked_image.h"
#include "wx_constants.h"

//######################################################################

ScrolledWidgetsPane::ScrolledWidgetsPane(wxWindow* parent, wxWindowID id, int width) 
: wxScrolledWindow(parent, id), m_width(width), m_height(0), m_newBlock(false)
{
	m_panel = new wxPanel(this, -1, wxDefaultPosition, wxSize(width, 0));			
}

//----------------------------------------------------------------------

ScrolledWidgetsPane::~ScrolledWidgetsPane(){
	clear();
	wxDELETE(m_panel);
}

//----------------------------------------------------------------------

void ScrolledWidgetsPane::addImage(wxString file, bool newBlock, float rank){
	int x, y;
	GetViewStart(&x, &y);
	
	if(newBlock){
		if(m_newBlock){
			int sHeight=10;
			wxPanel* separator=new wxPanel(m_panel, wxID_ANY, wxPoint(0, m_height), wxSize(m_width, sHeight));
			separator->SetPosition(wxPoint(0, m_height));
			separator->SetBackgroundColour(wxColour(c_separatorColour));
			m_height+=sHeight;
			m_panel->SetSize(m_width, m_height);
			m_pictureList.push_back(separator);
		}
		m_newBlock=true;
	}
		
	PicWrapper* newPic=new PicWrapper(m_panel, wxPoint(0, m_height), m_width, file, rank);
	
	m_height+=newPic->getHeight();

	m_panel->SetSize(m_width, m_height);
	SetScrollbars(1,1, 0, m_height, 0, y);
	Scroll(0, 0);
	Scroll(0, y);
	m_pictureList.push_back(newPic);
}

//----------------------------------------------------------------------

void ScrolledWidgetsPane::clear(){
	std::list<wxPanel*>::iterator it=m_pictureList.begin();
	m_height=0;
	wxPanel* wxTmp;
	while(it!=m_pictureList.end()){
		wxTmp=*it;
		++it;
		wxDELETE(wxTmp);
	}

	m_pictureList.clear();
	m_panel->SetSize(m_width, m_height);
	m_panel->SetPosition(wxPoint(0, 0));

	SetScrollbars(1,1, 0, 0, 0, 0);
	Scroll(0, 0);
	m_newBlock=false;

}

//----------------------------------------------------------------------

