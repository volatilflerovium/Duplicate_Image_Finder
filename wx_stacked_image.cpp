#include "wx_stacked_image.h"

#include "file_manager.h"
#include "notification_module.h"
#include "wx_constants.h"

//######################################################################
//######################################################################

PicWrapper2* wxStackedImage::m_viewerImg(nullptr);
wxStackedImage* wxStackedImage::m_last(nullptr);

BEGIN_EVENT_TABLE(wxStackedImage, wxImageW)
	EVT_LEFT_UP(wxStackedImage::OnMouseLeftUp)
	EVT_RIGHT_UP(wxStackedImage::OnMouseRightUp)
END_EVENT_TABLE()

//----------------------------------------------------------------------

wxStackedImage::wxStackedImage(wxPanel* parent, wxString file)
:wxImageW(parent, file)
{}

//----------------------------------------------------------------------

wxStackedImage::wxStackedImage(wxPanel* parent, const wxString& file, const wxPoint& pos)
:wxImageW(parent, file, pos)
{}

//----------------------------------------------------------------------

wxStackedImage::wxStackedImage(wxPanel* parent, const wxString& file, const wxPoint& pos, int width)
:wxImageW(parent, file, pos, width)
{}

//----------------------------------------------------------------------

void wxStackedImage::OnMouseLeftUp(wxMouseEvent& event){
	if(m_last){
		m_last->GetParent()->SetBackgroundColour(wxColour(wxT("white")));
	}
	GetParent()->SetBackgroundColour(wxColour(wxT("green")));
	m_last=this;
	if(m_viewerImg){
		m_viewerImg->wallPaper();		
		m_viewerImg->loadImage(m_file);
		
		NotificationModule::displayData(m_file, 0.12345);
	}
}

//----------------------------------------------------------------------

void wxStackedImage::OnMouseRightUp(wxMouseEvent& event){
	wxMenu popupMenu;
	popupMenu.Append(WX::OPEN_IMG, wxT("&Open"), wxT("Open"));
	Connect(WX::OPEN_IMG, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(wxStackedImage::OnOpenImg));

	popupMenu.Append(WX::DELETE_IMG, wxT("&Delete"), wxT("Delete"));
	Connect(WX::DELETE_IMG, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(wxStackedImage::OnDeleteImg));	

	PopupMenu(&popupMenu);
}

//----------------------------------------------------------------------

void wxStackedImage::OnOpenImg(wxCommandEvent& event){
	std::string str("xdg-open ");
	str.append(m_file.ToStdString());
	system(str.c_str());
}

//----------------------------------------------------------------------

void wxStackedImage::OnDeleteImg(wxCommandEvent& event){
	if(m_last){
		m_last->GetParent()->SetBackgroundColour(wxColour(wxT("white")));
	}
	GetParent()->SetBackgroundColour(wxColour(wxT("red")));
	m_last=nullptr;
	std::cout<<"Delete pic: "<<m_file<<"\n";
}

//----------------------------------------------------------------------

void wxStackedImage::setViewer(PicWrapper2* viewerImg){
	if(!m_viewerImg){
		m_viewerImg=viewerImg;
	}
}

//----------------------------------------------------------------------

void wxStackedImage::clearBackground(){
	m_viewerImg->wallPaper();
	m_last=nullptr;
}

//######################################################################
//######################################################################
//######################################################################

PicWrapper::PicWrapper(wxPanel* parent, const wxString& file, const wxPoint& pos, int width)
:wxPanel(parent, -1, pos)
{
	SetBackgroundColour(wxColour(wxT("white")));
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	SetSizer(vbox);
	
	m_padding=c_picPadding;
	m_pic=new wxStackedImage(this, file, wxPoint(m_padding, m_padding), width-2*m_padding);

	SetSize(width, m_pic->getHeight()+2*m_padding);
	vbox->Add(m_pic, 0, wxALL, m_padding);
}

//######################################################################

BEGIN_EVENT_TABLE(PicWrapper2, wxPanel)
	EVT_SIZE(PicWrapper2::resize)
END_EVENT_TABLE()

PicWrapper2::PicWrapper2(wxPanel* parent, const wxSize& size, int padding)
:wxPanel(parent, -1, wxDefaultPosition), m_pic(nullptr), m_padding(padding)
{
	wxColour bkgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	SetBackgroundColour(bkgColour);//wxColour(wxT("yellow")));
	SetMinSize(size);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	SetSizer(vbox);

	m_pic=new wxImageW(this, wxString(FileManager::c_IMG_BACKGROUND));

	vbox->Add(m_pic, 1, wxEXPAND | wxALL, m_padding);	
}

//----------------------------------------------------------------------

void PicWrapper2::wallPaper(){
	loadImage(wxString(FileManager::c_IMG_BACKGROUND.c_str()));
}

//----------------------------------------------------------------------

void PicWrapper2::loadImage(const wxString& file){
	int width, height;
	GetSize(&width, &height);
	m_pic->loadImage(file, width-2*m_padding, height-2*m_padding);
	int w=(width-m_pic->getWidth())/2;
	int h=(height-m_pic->getHeight())/2;
	m_pic->SetPosition(wxPoint(w, h));
}

//----------------------------------------------------------------------

void PicWrapper2::resize(wxSizeEvent& event){	
	static int w=0;
	static int h=0;
	//Layout();
	//event.Skip();

	int width, height;
	GetSize(&width, &height);
	if(w!=width || h!=height){	
		SetSize(width, height);

	}
	w=width;
	h=height;
}

//----------------------------------------------------------------------

void PicWrapper2::SetSize(int width, int height){
	if(m_pic){
		m_pic->SetSize(width-2*m_padding, height-2*m_padding);
		int w=(width-m_pic->getWidth())/2;
		int h=(height-m_pic->getHeight())/2;
		m_pic->SetPosition(wxPoint(w, h));
	}
	wxPanel::SetSize(width, height);
}

//----------------------------------------------------------------------
