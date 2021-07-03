#include "wx_stacked_image.h"

#include "file_manager.h"
#include "notification_module.h"
#include "wx_constants.h"

//######################################################################
//######################################################################

BEGIN_EVENT_TABLE(PicWrapper2, wxPanel2)
	EVT_SIZE(PicWrapper2::resize)
END_EVENT_TABLE()

PicWrapper2* PicWrapper2::m_instance(nullptr);

//----------------------------------------------------------------------

PicWrapper2* PicWrapper2::getInstance(wxPanel* parent, const wxSize& size, int padding){
	if(!m_instance){
		m_instance=new PicWrapper2(parent, size, padding);
	}
	return m_instance;
}

//----------------------------------------------------------------------

PicWrapper2::PicWrapper2(wxPanel* parent, const wxSize& size, int padding)
:wxPanel2(parent, wxDefaultPosition), m_pic(nullptr), 
m_padding(padding)//, m_xx(false)
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

PicWrapper2::~PicWrapper2(){
	wxDELETE(m_pic);
}

//----------------------------------------------------------------------

void PicWrapper2::WallPaper(){
	m_instance->loadImage(wxString(FileManager::c_IMG_BACKGROUND.c_str()));
}

//----------------------------------------------------------------------

void PicWrapper2::LoadImage(const wxString& file){
	m_instance->loadImage(file);
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
	wxPanel2::SetSize(width, height);
}

//######################################################################
//######################################################################

wxStackedImage* wxStackedImage::m_last(nullptr);

BEGIN_EVENT_TABLE(wxStackedImage, wxImageW)
	EVT_LEFT_UP(wxStackedImage::OnMouseLeftUp)
	EVT_RIGHT_UP(wxStackedImage::OnMouseRightUp)
END_EVENT_TABLE()

//----------------------------------------------------------------------

wxStackedImage::wxStackedImage(wxPanel* parent, const wxPoint& pos, int width, const wxString& file, float rank)
:wxImageW(parent, file, pos, width), m_rank(rank)
{}

//----------------------------------------------------------------------

void wxStackedImage::OnMouseLeftUp(wxMouseEvent& event){
	if(m_last){
		m_last->GetParent()->SetBackgroundColour(wxColour(wxT("white")));
	}
	GetParent()->SetBackgroundColour(wxColour(wxT("green")));
	m_last=this;
	PicWrapper2::LoadImage(m_file);		
	NotificationModule::displayData(m_file, m_rank);
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

void wxStackedImage::clearBackground(){
	m_last=nullptr;
}

//######################################################################
//######################################################################
//######################################################################

PicWrapper::PicWrapper(wxPanel* parent, const wxPoint& pos, int width, const wxString& file, float rank)
:wxPanel2(parent, pos)
{
	SetBackgroundColour(wxColour(wxT("white")));
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	SetSizer(vbox);
	
	m_padding=c_picPadding;
	m_pic=new wxStackedImage(this, wxPoint(m_padding, m_padding), width-2*m_padding, file, rank);

	SetSize(width, m_pic->getHeight()+2*m_padding);
	vbox->Add(m_pic, 0, wxALL, m_padding);
}

//######################################################################
//######################################################################
//######################################################################

