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
#ifndef WX_IMAGEW_H
#define WX_IMAGEW_H
#include "event_definitions.h"

#include <wx/panel.h>
#include <wx/string.h>

//====================================================================

class ImagePanel : public wxPanel
{
	public:
		ImagePanel(wxWindow* parent, const char* file, const wxPoint& pos, int width);

		ImagePanel(wxWindow* parent, const wxSize& size=wxDefaultSize)
		:wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
		, m_bitmap(new wxBitmap)
		, m_width(0)
		, m_height(0)
		, m_isSupported(false)
		{
			if(size!=wxDefaultSize){
				m_width=size.GetWidth();
				m_height=size.GetHeight();
			}
		}

		virtual ~ImagePanel();

		void loadImage(const wxString& file, bool nativeSize=false);
		void replaceImage(const wxString& file);
		void loadAndFit(const wxString& file, uint width, uint height);
		void reSize(uint width, uint height);
		void fitToWidth(uint newWidth);
		void fitToHeight(uint newHeight);
		const wxString& getFile() const;

	protected:
		wxString m_file;
		wxBitmap* m_bitmap;	
		uint m_width;
		uint m_height;
		bool m_isSupported;

		void fitToSize(uint rWidth, uint rHeight);
		bool reloadImage();
		void paintNow();
		
		wxImage wxImageFromMat(const char* file);
		void render(wxDC& dc);

		void paintEvent(wxPaintEvent& evt);
		
		void OnMouseLeftUp(wxMouseEvent& event)
		{
			wxCommandEvent eventOut(wxEVT_CUSTOM_EVENT, EvtID::LEFT_CLICK_IMAGE);
			wxPostEvent(this, eventOut);
		}

		void OnMouseRightUp(wxMouseEvent& event)
		{
			wxCommandEvent eventOut(wxEVT_CUSTOM_EVENT, EvtID::RIGHT_CLICK_IMAGE);
			wxPostEvent(this, eventOut);
		}

		uint m_over{0};
		void OnMouseEnters(wxMouseEvent& event)
		{
			m_over++;
		}
		
		void OnMouseLeaves(wxMouseEvent& event)
		{
			m_over=0;
		}
		
		void OnMouseMove(wxMouseEvent& event)
		{
			if(m_over==1){
			}
			m_over++;
		}

		DECLARE_EVENT_TABLE()	
};

//----------------------------------------------------------------------

inline const wxString& ImagePanel::getFile() const
{
	return m_file;
}


//----------------------------------------------------------------------

inline void ImagePanel::fitToWidth(uint newWidth)
{
	reSize(newWidth, 0);
}

//----------------------------------------------------------------------

inline void ImagePanel::fitToHeight(uint newHeight)
{
	reSize(0, newHeight);
}

//----------------------------------------------------------------------

#endif
