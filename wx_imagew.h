/*********************************************************************
* wxImageW class                                   				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
* Note: partially taken from https://wiki.wxwidgets.org/An_image_panel *
**********************************************************************/
#ifndef WX_IMAGEW_H
#define WX_IMAGEW_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/string.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

//######################################################################

class wxPanel2 : public wxPanel
{
	public:
		wxPanel2(wxPanel* parent, const wxPoint& pos)
		: wxPanel(parent, wxID_ANY, pos)
		{}
		
		
		int getHeight() const{
			int x, y;
			GetSize(&x, &y);
			return y;
		}
		
		int getWidth() const{
			int x, y;
			GetSize(&x, &y);
			return x;
		}

	private:
		DECLARE_EVENT_TABLE()	
};

//######################################################################


class wxImageW : public wxPanel2
{
	public:
		wxImageW(wxPanel* parent, const wxString& file, const wxPoint& pos);
		wxImageW(wxPanel* parent, const wxString& file, const wxPoint& pos, int width);
		wxImageW(wxPanel* parent, const wxString& file);
		virtual ~wxImageW();
	
		void loadImage(const wxString& file);
		void loadImage(const wxString& file, int width, int height);
		void wallPaper(const wxString& file);
		
		void scaleToWidth(int newW);
		void scaleToHeight(int newW);
		void SetSize(int width, int height);
		void paintNow();
		void render(wxDC& dc);

	protected:
		wxBitmap* m_image;	
		wxString m_file;
		int m_imageWidth;
		int m_imageHeight;
		bool m_isSupported;
		wxImage wxImageFromMat(const char* file);
		void reLoadImage();
		void resizeImage();
		void getProportions(int& width, int& height);
		void sizeImage(int width, int height);
		void paintEvent(wxPaintEvent& evt);
		
		DECLARE_EVENT_TABLE()	
};


//----------------------------------------------------------------------

#endif
