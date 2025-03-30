/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* ImagePanel class                                   				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021   (Reviewed 03/2015)                           *
* Author:  Dan Machado                                               *
* Note: partially taken from https://wiki.wxwidgets.org/An_image_panel *
**********************************************************************/
#include "image_panel.h"
#include "data.h"

#include <map>
#include <cassert>

#include <wx/wx.h>
#include <wx/image.h>

//====================================================================

static std::map<wxString, wxBitmapType> wxBitmapTypes
{
	{"jpeg", wxBITMAP_TYPE_JPEG},
	{"JPEG", wxBITMAP_TYPE_JPEG},
	{"jpg", wxBITMAP_TYPE_JPEG},
	{"JPG", wxBITMAP_TYPE_JPEG},
	{"jpe", wxBITMAP_TYPE_JPEG},
	{"JPE", wxBITMAP_TYPE_JPEG},
	{"png", wxBITMAP_TYPE_PNG},
	{"PNG", wxBITMAP_TYPE_PNG},
	{"webp", wxBITMAP_TYPE_ANY},
	{"WEBP", wxBITMAP_TYPE_ANY},
	{"bmp", wxBITMAP_TYPE_BMP},
	{"BMP", wxBITMAP_TYPE_BMP},
	{"tiff", wxBITMAP_TYPE_TIFF},
	{"TIFF", wxBITMAP_TYPE_TIFF},
	{"tif", wxBITMAP_TYPE_ANY},
	{"TIF", wxBITMAP_TYPE_ANY},
	{"jp2", wxBITMAP_TYPE_ANY},
	{"JP2", wxBITMAP_TYPE_ANY},
	{"pic", wxBITMAP_TYPE_ANY},
	{"PIC", wxBITMAP_TYPE_ANY},
	{"ppm", wxBITMAP_TYPE_ANY},
	{"PPM", wxBITMAP_TYPE_ANY},
	{"pxm", wxBITMAP_TYPE_ANY},
	{"PXM", wxBITMAP_TYPE_ANY},
	{"pnm", wxBITMAP_TYPE_PNM},
	{"PNM", wxBITMAP_TYPE_PNM},
	{"pfm", wxBITMAP_TYPE_ANY},
	{"PFM", wxBITMAP_TYPE_ANY},
	{"sr", wxBITMAP_TYPE_ANY},
	{"SR", wxBITMAP_TYPE_ANY},
	{"ras", wxBITMAP_TYPE_ANY},
	{"RAS", wxBITMAP_TYPE_ANY},
	{"exr", wxBITMAP_TYPE_ANY},
	{"EXR", wxBITMAP_TYPE_ANY},
	{"hdr", wxBITMAP_TYPE_ANY},
	{"HDR", wxBITMAP_TYPE_ANY},
	{"pbm", wxBITMAP_TYPE_ANY},
	{"PBM", wxBITMAP_TYPE_ANY},
	{"pgm", wxBITMAP_TYPE_ANY},
	{"PGM", wxBITMAP_TYPE_ANY},
	{"dip", wxBITMAP_TYPE_ANY},
	{"DIP", wxBITMAP_TYPE_ANY}
};

//----------------------------------------------------------------------

static bool isWXsuported(const wxString& wxStr)
{
	std::size_t n=wxStr.find_last_of('.');
	if(n!= std::string::npos){
		auto it=wxBitmapTypes.find(wxStr.substr(n+1));
		if(it!=wxBitmapTypes.end()){
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------

static wxBitmapType getBitmapType(const wxString& wxStr)
{
	std::size_t n=wxStr.find_last_of('.');
	if(n!= std::string::npos){
		auto it=wxBitmapTypes.find(wxStr.substr(n+1));
		if(it!=wxBitmapTypes.end()){
			return it->second;
		}
	}
	return wxBITMAP_TYPE_ANY;
}

//====================================================================

ImagePanel::ImagePanel(wxWindow* parent, const char* file, const wxPoint& pos, int width)
: wxPanel(parent, wxID_ANY, pos)
, m_file(file)
, m_bitmap(new wxBitmap)
, m_width(width)
, m_height(0)
, m_isSupported(isWXsuported(file))
{
	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
	EVT_PAINT(ImagePanel::paintEvent)
	EVT_LEFT_UP(ImagePanel::OnMouseLeftUp)
	EVT_RIGHT_UP(ImagePanel::OnMouseRightUp)
	
	EVT_ENTER_WINDOW(ImagePanel::OnMouseEnters)
	EVT_LEAVE_WINDOW(ImagePanel::OnMouseLeaves)
	EVT_MOTION(ImagePanel::OnMouseMove)
END_EVENT_TABLE()

//----------------------------------------------------------------------

ImagePanel::~ImagePanel()
{
	wxDELETE(m_bitmap);
}

//----------------------------------------------------------------------

bool ImagePanel::reloadImage()
{
	if(m_file.Length()>0){
		if(m_isSupported){
			m_bitmap->LoadFile(m_file, getBitmapType(m_file));
		}
		else{
			*m_bitmap=wxBitmap(wxImageFromMat(m_file.mb_str()));
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------

void ImagePanel::replaceImage(const wxString& file)
{
	assert(m_width+m_height!=0);

	m_file=file;
	m_isSupported=isWXsuported(file);

	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//--------------------------------------------------------------------

void ImagePanel::loadImage(const wxString& file, bool nativeSize)
{
	m_file=file;
	m_isSupported=isWXsuported(file);
	if(reloadImage()){
		if(nativeSize){
			m_width=m_bitmap->GetWidth();
			m_height=m_bitmap->GetHeight();
		}
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

void ImagePanel::loadAndFit(const wxString& file, uint width, uint height)
{
	m_file=file;
	m_isSupported=isWXsuported(file);
	m_width=width;
	m_height=height;
	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

void ImagePanel::reSize(uint width, uint height)
{
	if(m_width==width && m_height==height){
		return;
	}

	m_width=width;
	m_height=height;
	if(reloadImage()){
		fitToSize(m_width, m_height);
	}
}

//----------------------------------------------------------------------

void ImagePanel::fitToSize(uint rWidth, uint rHeight)
{
	if(rWidth+rHeight==0){
		return;
	}

	int width=m_bitmap->GetWidth();
	int height=m_bitmap->GetHeight();

	float t=(1.0*rWidth)/(1.0*width);
	float r=(1.0*rHeight)/(1.0*height);

	if(t==0 || (r>0 && r<t)){
		t=r;
	}

	*m_bitmap=wxBitmap(m_bitmap->ConvertToImage().Scale(t*width, t*height, wxIMAGE_QUALITY_HIGH));

	SetMinSize(FromDIP(wxSize(t*width, t*height)));
	SetSize(FromDIP(wxSize(t*width, t*height)));
	Refresh();
}

//----------------------------------------------------------------------

wxImage ImagePanel::wxImageFromMat(const char* file)
{
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

void ImagePanel::paintEvent(wxPaintEvent & evt)
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
void ImagePanel::paintNow()
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
void ImagePanel::render(wxDC&  dc)
{
	if(m_file.Length()>0){
		dc.DrawBitmap(*m_bitmap, 0, 0, false );
	}
}


/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
 
//----------------------------------------------------------------------
