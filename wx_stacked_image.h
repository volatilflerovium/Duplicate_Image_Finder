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


class PicWrapper2 : public wxPanel2
{
	public:
		static PicWrapper2* m_instance;
		static PicWrapper2* getInstance(wxPanel* parent, const wxSize& size, int padding);
		
		virtual ~PicWrapper2();

		static void WallPaper();
		static void LoadImage(const wxString& file);
		
		void SetSize(int width, int height);

	private:
		wxImageW* m_pic;
		int m_padding;
		
		void loadImage(const wxString& file);

		PicWrapper2(wxPanel* parent, const wxSize& size, int padding);
		
		PicWrapper2(const PicWrapper2&) = delete;
		PicWrapper2() = delete;
		PicWrapper2& operator=(const PicWrapper2&) = delete;

		void resize(wxSizeEvent& event);
		DECLARE_EVENT_TABLE()	
};


//######################################################################
//######################################################################

class wxStackedImage : public wxImageW
{
	public:
		wxStackedImage(wxPanel* parent, const wxPoint& pos, int width, const wxString& file, float rank);
		virtual ~wxStackedImage(){};
		static void clearBackground();

	private:
		static wxStackedImage* m_last;
		float m_rank;

		void OnOpenImg(wxCommandEvent& event);
		void OnDeleteImg(wxCommandEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);
		void OnMouseRightUp(wxMouseEvent& event);
		DECLARE_EVENT_TABLE()	

};

//######################################################################
//######################################################################
//######################################################################

class PicWrapper : public wxPanel2
{
	public:
		PicWrapper(wxPanel* parent, const wxPoint& pos, int width, const wxString& file, float rank);

		virtual ~PicWrapper(){
			wxDELETE(m_pic);
		}

	protected:
		wxStackedImage* m_pic;
		int m_padding;
};

//######################################################################
//######################################################################
//######################################################################

#endif
