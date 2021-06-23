#include "progress_bar.h"

//----------------------------------------------------------------------

ProgressBar::ProgressBar(wxPanel* parent, int width, int height)
:wxPanel(parent, -1, wxDefaultPosition, wxSize(width, height)), 
m_range(width), m_height(height), m_position(0)
{
	Connect(wxEVT_PAINT, wxPaintEventHandler(ProgressBar::OnPaint));
}

//----------------------------------------------------------------------

void ProgressBar::step(){
	m_step++;
	m_position=(1.0*m_step)*m_delta;
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::complete(){
	m_position=m_range;
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::reset(){
	m_position=0;
	m_delta=0;
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::setProgress(int prog){
	m_position=m_range*((1.0*prog)/100.0);
}

//----------------------------------------------------------------------

void ProgressBar::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	wxPen pen(wxColour("#80b3ff"));
	dc.SetPen(pen);

	dc.DrawRectangle(wxRect(0, 0, m_range, m_height));

	if(m_range-m_position<m_delta){
		wxBrush brush1(wxColour("#00e600"));
		dc.SetBrush(brush1);
		dc.DrawRectangle(wxRect(0, 0, m_position, m_height));
	}
	else{
		dc.GradientFillLinear(wxRect(0, 0, m_position, m_height),
			wxColour("#00e600"), wxColour("#3399ff"), wxEAST);
	}
}

//----------------------------------------------------------------------