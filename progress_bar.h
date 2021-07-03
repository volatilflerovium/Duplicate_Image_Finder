/*********************************************************************
* ProgressBar class                                  				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/event.h>

//######################################################################

class ProgressBar : public wxPanel
{
	public:
		ProgressBar(wxPanel* parent, int width, int height);
		virtual ~ProgressBar();
		void setProgress(int prog);
		void step();

		void setUp(int vRange);
		void complete();
		void reset(bool hard=true);

	private:
		wxStaticText* m_progressText;
		const int m_range;
		const int m_height;
		int m_position;
		int m_step;
		float m_delta;
		
		void OnPaint(wxPaintEvent& event);
};

//----------------------------------------------------------------------




#endif
