/*********************************************************************
* PictureViewer class                                				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef PICTURE_VIEWER_H
#define PICTURE_VIEWER_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/sizer.h>

#include "scroller.h"
#include "wx_imagew.h"
#include "wx_stacked_image.h"

//######################################################################

class PictureViewer : public wxPanel
{
	public:
		PictureViewer(wxPanel* parent);
		~PictureViewer();
		void loadPicture(std::string filePath, bool newBlock, float rank);
		void clear();

	private:
		ScrolledWidgetsPane* m_scroller;
};

#endif
