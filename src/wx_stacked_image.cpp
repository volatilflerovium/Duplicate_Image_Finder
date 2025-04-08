/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* wxStackedImage class                               				      *                            				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021  (Reviewed 03/2025)                                              *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "wx_stacked_image.h"
#include "notification_module.h"
#include "wx_constants.h"
#include "picture_viewer.h"
#include "event_definitions.h"

#include "data_visualization.h"

extern DataView* s_dataViewPtr;

//====================================================================

wxStackedImage* wxStackedImage::m_last=nullptr;

BEGIN_EVENT_TABLE(wxStackedImage, wxPanel)
	EVT_COMMAND(EvtID::LEFT_CLICK_IMAGE, wxEVT_CUSTOM_EVENT, wxStackedImage::OnLeftClick)
	EVT_COMMAND(EvtID::RIGHT_CLICK_IMAGE, wxEVT_CUSTOM_EVENT, wxStackedImage::OnRightClick)

	EVT_MENU(WX::OPEN_IMG, wxStackedImage::OnOpenImg)
	EVT_MENU(WX::DELETE_IMG, wxStackedImage::OnDeleteImg)
END_EVENT_TABLE()

//----------------------------------------------------------------------

wxStackedImage::wxStackedImage(wxWindow* parent, const wxPoint& pos, int width, const char* file, float rank)
:wxPanel(parent, -1, pos)
, m_rank(rank)
{
	SetBackgroundColour(wxColour(wxT("white")));

	m_padding=c_picPadding;
	
	m_pic=new ImagePanel(this, file, wxPoint(m_padding, m_padding), width-2*m_padding);

	auto sz=m_pic->GetSize();
	SetMinSize(wxSize(width, sz.GetHeight()+2*m_padding));

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	vbox->SetMinSize(width, sz.GetHeight()+2*m_padding);	
	vbox->Add(m_pic, 1, wxEXPAND | wxALL, m_padding);

	SetSizerAndFit(vbox);
}

//----------------------------------------------------------------------

void wxStackedImage::OnLeftClick(wxCommandEvent& event)
{
	highlight();
	s_dataViewPtr->selectNode(std::string(m_pic->getFile().mb_str()));
	NotificationModule::setRank(m_rank);
}

//----------------------------------------------------------------------

void wxStackedImage::OnRightClick(wxCommandEvent& event)
{
	wxMenu popupMenu;
	popupMenu.Append(WX::OPEN_IMG, wxT("&Open"), wxT("Open"));
	popupMenu.Append(WX::DELETE_IMG, wxT("&Delete"), wxT("Delete"));	

	PopupMenu(&popupMenu);
}

//----------------------------------------------------------------------

void wxStackedImage::OnOpenImg(wxCommandEvent& event)
{
	std::string str("xdg-open ");
	str.append(m_pic->getFile().ToStdString());
	system(str.c_str());
}

//----------------------------------------------------------------------

void wxStackedImage::OnDeleteImg(wxCommandEvent& event)
{
	highlight("red");
	m_last=nullptr;
	wxMessageDialog removePicDialog(
		this,
		wxT("Do you want to delete this picture?"),
		wxT("Delete Image."),
		wxOK|wxCENTRE|wxICON_WARNING|wxCANCEL
	);
	
	if(wxID_OK==removePicDialog.ShowModal()){	
		wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::DELETE_IMG);
		event.SetString(m_pic->getFile());
		//GetEventHandler()->AddPendingEvent(event);
		wxPostEvent(this, event);
	}
}

//====================================================================
