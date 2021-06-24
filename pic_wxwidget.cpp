#include <wx/image.h>
#include <iostream>
#include "pic_wxwidget.h"
#include "data.h"
#include "file_manager.h"
#include "notification_module.h"
//####################################################

BEGIN_EVENT_TABLE(wxImageW, wxPanel)
	EVT_PAINT(wxImageW::paintEvent)
END_EVENT_TABLE()


//----------------------------------------------------------------------

wxImageW::wxImageW(wxPanel* parent, wxString file, wxBitmapType format, const wxPoint& pos)
: wxPanel(parent, wxID_ANY, pos), 
	m_file(file), m_format(format)
{
	m_image=new wxBitmap();
	m_image->LoadFile(file, format);	
}

//----------------------------------------------------------------------

wxImageW::~wxImageW(){
	wxDELETE(m_image);
}

//----------------------------------------------------------------------

wxImageW::wxImageW(wxPanel* parent, wxString file, wxBitmapType format)
: wxImageW(parent, file, format, wxDefaultPosition)
{
}

//----------------------------------------------------------------------

void wxImageW::scaleToWidth(int newW){
	int cW=(*m_image).GetWidth();
	int cH=(*m_image).GetHeight();
	int newH=(newW*cH)/cW;

	*m_image=wxBitmap((*m_image).ConvertToImage().Scale(newW, newH /*, wxIMAGE_QUALITY_HIGH*/ ) );

	SetSize(newW, newH);
}

//----------------------------------------------------------------------

void wxImageW::scaleToHeight(int newH){
	int cW=(*m_image).GetWidth();
	int cH=(*m_image).GetHeight();
	int newW=(newH*cW)/cH;

	*m_image=wxBitmap((*m_image).ConvertToImage().Scale(newW, newH /*, wxIMAGE_QUALITY_HIGH*/ ) );

	SetSize(newW, newH);
}

//----------------------------------------------------------------------

void wxImageW::loadImage2(wxString file, wxBitmapType format){
	int newW, newH;
	GetSize(&newW, &newH);
	wxBitmap tmpImage;
	tmpImage.LoadFile(file, format);
	
	int cW=(tmpImage).GetWidth();
	int cH=(tmpImage).GetHeight();
	int W=newW;
	int H=W*((cH*1.0)/(1.0*cW));
	if(H>newH){
		H=newH;
		W=(H*cW)/cH;
	}

	*m_image=wxBitmap(tmpImage.ConvertToImage().Scale(W, H /*, wxIMAGE_QUALITY_HIGH*/ ) );
}

//----------------------------------------------------------------------

void wxImageW::loadImage3(wxString file, wxBitmapType format){
	int newW, newH;
	GetSize(&newW, &newH);
	wxBitmap tmpImage;
	tmpImage.LoadFile(file, format);
	
	*m_image=wxBitmap(tmpImage.ConvertToImage().Scale(newW, newH /*, wxIMAGE_QUALITY_HIGH*/ ) );
}
//----------------------------------------------------------------------

void wxImageW::loadImage(wxString file, wxBitmapType format){
	int w, h;
	GetSize(&w, &h);
	wxBitmap tmpImage;
	tmpImage.LoadFile(file, format);
	*m_image=wxBitmap(tmpImage.ConvertToImage().Scale(w, h /*, wxIMAGE_QUALITY_HIGH*/ ) );
}

//----------------------------------------------------------------------

void wxImageW::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
	render(dc);
}

//----------------------------------------------------------------------

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void wxImageW::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
	wxClientDC dc(this);
	render(dc);
}

//----------------------------------------------------------------------

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void wxImageW::render(wxDC&  dc)
{
	dc.DrawBitmap(*m_image, 0, 0, false );
}


// some useful events
/*
void wxImagePanel::mouseMoved(wxMouseEvent& event) {}
void wxImagePanel::mouseDown(wxMouseEvent& event) {}
void wxImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
void wxImagePanel::mouseReleased(wxMouseEvent& event) {}
void wxImagePanel::rightClick(wxMouseEvent& event) {}
void wxImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
void wxImagePanel::keyPressed(wxKeyEvent& event) {}
void wxImagePanel::keyReleased(wxKeyEvent& event) {}
//*/

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

//######################################################################
//######################################################################

wxImageW* wxStackedImage::m_viewerImg(nullptr);

BEGIN_EVENT_TABLE(wxStackedImage, wxImageW)
	EVT_LEFT_UP(wxStackedImage::OnMouseLeftUp)
	EVT_RIGHT_UP(wxStackedImage::OnMouseRightUp)
END_EVENT_TABLE()

//----------------------------------------------------------------------

void wxStackedImage::OnMouseLeftUp(wxMouseEvent& event){
	if(m_viewerImg){
		m_viewerImg->loadImage3(wxString(FileManager::c_IMG_BACKGROUND.c_str()), wxBITMAP_TYPE_ANY);
		m_viewerImg->paintNow();
		
		m_viewerImg->loadImage2(m_file, m_format);
		m_viewerImg->paintNow();
		
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
	std::cout<<"Delete pic: "<<m_file<<"\n";
}

//----------------------------------------------------------------------

void wxStackedImage::setViewer(wxImageW* img){
	if(!m_viewerImg){
		m_viewerImg=img;
	}
}

//######################################################################

PicWrapper::PicWrapper(wxPanel* parent, wxString file, wxBitmapType format, const wxPoint& pos, int width)
:wxPanel(parent, -1, pos)
{
	int padding=3;
	m_pic=new wxStackedImage(this, file, format, wxPoint(padding, padding));

	m_pic->scaleToWidth(width-2*padding);
	m_pic->paintNow();
	wxColour col1;
	col1.Set(wxT("green"));

	this->SetBackgroundColour(col1);
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	SetSize(width, m_pic->getHeight()+2*padding);
	vbox->Add(m_pic, 0, wxALL, padding);
	this->SetSizer(vbox);
	
}

//######################################################################
//######################################################################
