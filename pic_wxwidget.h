/*********************************************************************
* wxImageW class                                   				      *
* wxStackedImage class                               				      *
* PicWrapper struct                                 				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef WX_IMAGE_WIDGET_H
#define WX_IMAGE_WIDGET_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/string.h>

//######################################################################

class wxImageW : public wxPanel
{
	public:	
		wxImageW(wxPanel* parent, const wxPoint& pos);
		wxImageW(wxPanel* parent, wxString file, wxBitmapType format);
		wxImageW(wxPanel* parent, wxString file, wxBitmapType format, const wxPoint& pos);
		virtual ~wxImageW();

		int getHeight() const{
			int x, y;
			GetSize(&x, &y);
			return y;
		}
		
		void loadImage(wxString file, wxBitmapType format);
		void loadImage2(wxString file, wxBitmapType format);
		void loadImage3(wxString file, wxBitmapType format);

		void paintEvent(wxPaintEvent & evt);
		void paintNow();
		void scaleToWidth(int newW);
		void scaleToHeight(int newW);
		void scale();
		void render(wxDC& dc);

		DECLARE_EVENT_TABLE()

	protected:

		wxBitmap* m_image;	
		wxString m_file;
		wxBitmapType m_format;
};

//######################################################################
//######################################################################

class wxStackedImage : public wxImageW
{
	private:
		static wxImageW* m_viewerImg;

	public:
		wxStackedImage(wxPanel* parent, wxString file, wxBitmapType format)
		:wxImageW(parent, file, format)
		{}

		wxStackedImage(wxPanel* parent, wxString file, wxBitmapType format, const wxPoint& pos)
		:wxImageW(parent, file, format, pos)
		{}

		virtual ~wxStackedImage(){};

		static void setViewer(wxImageW* img);

		void OnOpenImg(wxCommandEvent& event);
		void OnDeleteImg(wxCommandEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);
		void OnMouseRightUp(wxMouseEvent& event);
		DECLARE_EVENT_TABLE()
};

//######################################################################

struct PicWrapper : public wxPanel
{
	PicWrapper(wxPanel* parent, wxString file, wxBitmapType format, const wxPoint& pos, int width);
	~PicWrapper(){
		wxDELETE(m_pic);
	}
	int getHeight() const{
		int x, y;
		GetSize(&x, &y);
		return y;
	}

	wxStackedImage* m_pic;

};

//######################################################################

#endif
