/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* ProgressBar class                                  				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "progress_bar.h"

#include <wx/wx.h>
#include <wx/event.h>
//----------------------------------------------------------------------

ProgressBar::ProgressBar(wxWindow* parent, int width, int height)
:wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(width, height)),
m_range(width), m_height(height), m_position(0),
m_step(0), m_delta(0.0)
{
	//SetMinSize();

	wxColour colour(wxT("#ffffff"));
	SetBackgroundColour(colour);

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(10, 1, wxEXPAND);
	int padding=2;
	m_progressText = new wxStaticText(this, wxID_ANY, wxT("0 %"), wxDefaultPosition,  wxSize(70, height-2*padding), wxALIGN_CENTER);
	hbox->Add(m_progressText, 0, wxALL | wxCENTER, padding);
	hbox->Add(10, 1, wxEXPAND);
	
	SetSizerAndFit(hbox);

	Connect(wxEVT_PAINT, wxPaintEventHandler(ProgressBar::OnPaint));
}

//----------------------------------------------------------------------

ProgressBar::~ProgressBar()
{
	wxDELETE(m_progressText);
}

//----------------------------------------------------------------------

void ProgressBar::setUp(int vRange)
{
	m_step=0;
	m_delta=(1.0*m_range)/(vRange*1.0);
	m_progressText->SetLabel(wxT("0 %"));
	Refresh();
	Update();
};

//----------------------------------------------------------------------

void ProgressBar::step()
{
	m_step++;
	m_position=(1.0*m_step)*m_delta;
	float p=(100.0*m_position)/(m_range*1.0);
	m_progressText->SetLabel(wxString::Format(wxT("%0.3f %%"), p));
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::complete()
{
	m_position=m_range;
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::reset(bool hard)
{
	m_position=0;
	m_step=0;
	if(hard){
		m_delta=0;
	}
	m_progressText->SetLabel(wxT("0 %"));
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::setProgress(int prog)
{
	m_position=m_range*((1.0*prog)/100.0);
	Refresh();
	Update();
}

//----------------------------------------------------------------------

void ProgressBar::OnPaint(wxPaintEvent& event)
{
	int newWidth, h=20;
	GetSize(&newWidth, &h);

	int newPosition=newWidth*(m_position/(m_range*1.0));

	wxPaintDC dc(this);

	wxPen pen(wxColour("#80b3ff"));
	dc.SetPen(pen);

	dc.DrawRectangle(wxRect(0, 0, newWidth, m_height));

	if(m_range-m_position<m_delta){
		wxBrush brush1(wxColour("#00e600"));
		dc.SetBrush(brush1);
		dc.DrawRectangle(wxRect(0, 0, newPosition, m_height));
		m_progressText->SetLabel(wxT("100 %"));
	}
	else{
		dc.GradientFillLinear(wxRect(0, 0, newPosition, m_height),
			wxColour("#00e600"), wxColour("#3399ff"), wxEAST);
	}
}

//----------------------------------------------------------------------
