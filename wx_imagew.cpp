#include <wx/image.h>
#include <iostream>

#include "wx_imagew.h"
#include "file_manager.h"
#include "data.h"

//####################################################

BEGIN_EVENT_TABLE(wxPanel2, wxPanel)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(wxImageW, wxPanel)
	EVT_PAINT(wxImageW::paintEvent)
END_EVENT_TABLE()

//----------------------------------------------------------------------

wxImageW::wxImageW(wxPanel* parent, const wxString& file, const wxPoint& pos)
: wxPanel2(parent, pos), 
	m_file(file), m_isSupported(FileManager::isWXsuported(file))
{
	m_image=new wxBitmap();
	reLoadImage();
}

//----------------------------------------------------------------------

wxImageW::wxImageW(wxPanel* parent, const wxString& file, const wxPoint& pos, int width)
: wxPanel2(parent, pos), 
	m_file(file), m_isSupported(FileManager::isWXsuported(file))
{
	m_image=new wxBitmap();
	reLoadImage();
	scaleToWidth(width);
}

//----------------------------------------------------------------------

wxImageW::wxImageW(wxPanel* parent, const wxString& file)
: wxImageW(parent, file, wxDefaultPosition)
{}

//----------------------------------------------------------------------

wxImageW::~wxImageW(){
	wxDELETE(m_image);
}

//----------------------------------------------------------------------

void wxImageW::reLoadImage(){
	if(m_isSupported){
		m_image->LoadFile(m_file, FileManager::getBitmapType(m_file));	
	}
	else{
		*m_image=wxBitmap(wxImageFromMat(m_file.mb_str()));
	}
	
	m_imageWidth=m_image->GetWidth();
	m_imageHeight=m_image->GetHeight();
}

//----------------------------------------------------------------------

void wxImageW::loadImage(const wxString& file){
	m_file=file;
	m_isSupported=true;
	reLoadImage();
	resizeImage();
}

//----------------------------------------------------------------------

void wxImageW::loadImage(const wxString& file, int width, int height){
	m_file=file;
	m_isSupported=true;
	reLoadImage();
	
	SetSize(width, height);
	paintNow();
}

//----------------------------------------------------------------------

void wxImageW::wallPaper(const wxString& file){
	m_file=file;
	m_isSupported=true;

	reLoadImage();

	int newW, newH;
	GetSize(&newW, &newH);
	
	*m_image=wxBitmap(m_image->ConvertToImage().Scale(newW, newH /*, wxIMAGE_QUALITY_HIGH*/ ) );
}

//----------------------------------------------------------------------

void wxImageW::resizeImage(){
	int width, height;
	GetSize(&width, &height);
	SetSize(width, height);
}

//----------------------------------------------------------------------

void wxImageW::scaleToWidth(int newWidth){
	int width=newWidth; 
	int height=-1;
	getProportions(width, height);

	sizeImage(width, height);
}

//----------------------------------------------------------------------

void wxImageW::scaleToHeight(int newHeight){
	int width=-1; 
	int height=newHeight;
	getProportions(width, height);
	sizeImage(width, height);
}

//----------------------------------------------------------------------

void wxImageW::getProportions(int& width, int& height){
	int cW=m_imageWidth;
	int cH=m_imageHeight;
	
	if(width<0){
		width=height*((cW*1.0)/(1.0*cH));
	}
	else if(height<0){
		height=(1.0*width)*((1.0*cH)/(1.0*cW));
	}
}

//----------------------------------------------------------------------

void wxImageW::sizeImage(int width, int height){	
	*m_image=wxBitmap(m_image->ConvertToImage().Scale(width, height/*, wxIMAGE_QUALITY_HIGH*/ ) );
	wxPanel::SetSize(width, height);
}

//----------------------------------------------------------------------

void wxImageW::SetSize(int width, int height){
	if(m_image->GetWidth()<m_imageWidth){
		reLoadImage();
	}
	else if(m_image->GetWidth()==width && m_image->GetHeight()==height){
		return;
	}

	int W=width;
	int H=-1;
	getProportions(W, H);
	
	if(H>height){
		H=height;
		W=-1;
		getProportions(W, H);
	}
	sizeImage(W, H);
}

//----------------------------------------------------------------------

wxImage wxImageW::wxImageFromMat(const char* file){
	cv::Mat img=cv::imread(file);
	cv::Mat im2;
	if(img.channels()==1){
		cv::cvtColor(img,im2,cv::COLOR_GRAY2RGB);
	}
	else if(img.channels()==4){
		cv::cvtColor(img, im2, cv::COLOR_BGRA2RGB);
	}
	else{
		cv::cvtColor(img, im2, cv::COLOR_BGR2RGB);
	}

	long imsize = im2.rows*im2.cols*im2.channels();
	unsigned char* storage=static_cast<unsigned char*>(operator new(imsize));
	//wxImage wx(im2.cols, im2.rows,(unsigned char*)malloc(imsize), false);
	wxImage wx(im2.cols, im2.rows, storage, false);
	unsigned char* s=im2.data;
	unsigned char* d=wx.GetData();
	for(long i = 0; i < imsize; i++) 
	{
		d[i]=s[i];
	}
   return wx;
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


/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
