/*********************************************************************
* wxStackedImage class                               				      *
* PicWrapper struct                                 				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef WX_STACKED_IMAGE_H
#define WX_STACKED_IMAGE_H

#include "wx_imagew.h"

//######################################################################
//######################################################################

class PicWrapper2;

class wxStackedImage : public wxImageW
{
	private:
		static PicWrapper2* m_viewerImg;
		static wxStackedImage* m_last;

	public:
		wxStackedImage(wxPanel* parent, wxString file);
		wxStackedImage(wxPanel* parent, const wxString& file, const wxPoint& pos);
		wxStackedImage(wxPanel* parent, const wxString& file, const wxPoint& pos, int width);

		wxStackedImage(wxPanel* parent, wxString file, const wxPoint& pos, int width, int height);

		virtual ~wxStackedImage(){};

		static void setViewer(PicWrapper2* viewerImg);
		static void clearBackground();

		void OnOpenImg(wxCommandEvent& event);
		void OnDeleteImg(wxCommandEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);
		void OnMouseRightUp(wxMouseEvent& event);
		DECLARE_EVENT_TABLE()
};

//######################################################################
//######################################################################
//######################################################################

class PicWrapper : public wxPanel
{
	public:
		PicWrapper(wxPanel* parent, const wxString& file, const wxPoint& pos, int width);

		virtual ~PicWrapper(){
			wxDELETE(m_pic);
		}

		int getHeight() const;

	protected:
		wxStackedImage* m_pic;
		int m_padding;
};

inline int PicWrapper::getHeight() const{
	int x, y;
	GetSize(&x, &y);
	return y;
}

//######################################################################


class PicWrapper2 : public wxPanel
{
	public:
		PicWrapper2(wxPanel* parent, const wxSize& size, int padding);
		virtual ~PicWrapper2(){
			wxDELETE(m_pic);
		}

		void wallPaper();
		void loadImage(const wxString& file);
		void SetSize(int width, int height);
		int getHeight() const;
	
	protected:
		wxImageW* m_pic;
		int m_padding;
		
		void resize(wxSizeEvent& event);
		DECLARE_EVENT_TABLE()	
};

//----------------------------------------------------------------------

inline int PicWrapper2::getHeight() const{
	int x, y;
	GetSize(&x, &y);
	return y;
}


//######################################################################

#endif
