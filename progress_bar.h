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

//######################################################################

class ProgressBar : public wxPanel
{
	public:
		ProgressBar(wxPanel* parent, int width, int height);
		void setProgress(int prog);
		void step();

		void setUp(int vRange);
		void complete();
		void reset();

	private:
		const int m_range;
		const int m_height;
		int m_position;
		int m_step;
		float m_delta;
		
		void OnPaint(wxPaintEvent& event);
};

//----------------------------------------------------------------------

inline void ProgressBar::setUp(int vRange){
	m_step=0;
	m_delta=m_range/(vRange*1.0);
};


#endif