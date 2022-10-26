/*********************************************************************
* ScrolledWidgetsPane class                          				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef SCROLLER_H
#define SCROLLER_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <list>


//######################################################################

class ScrolledWidgetsPane : public wxScrolledWindow
{
	public:
		ScrolledWidgetsPane(wxWindow* parent, wxWindowID id, int width);
		~ScrolledWidgetsPane();
		void addImage(wxString file, bool newBlock, float rank);
		void clear();

	private:
		std::list<wxPanel*> m_pictureList;
		wxPanel* m_panel;
		const int m_width;
		int m_height;
		bool m_newBlock;
};



#endif
