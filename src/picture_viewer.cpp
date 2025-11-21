/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* PictureViewer class                                				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "picture_viewer.h"
#include "wx_constants.h"
#include "wx_stacked_image.h"
#include "image_scrolled_panel.h"
#include "file_manager.h"
#include "data_visualization.h"
#include "event_definitions.h"

#include <wx/sizer.h>

DataView* s_dataViewPtr=nullptr;

//----------------------------------------------------------------------

PictureViewer::PictureViewer(wxWindow* parent)
:wxPanel(parent)
{
	SetMinSize(wxSize(WX::MIN_WIDTH, c_scrollerMinHeight));

	m_dataViewPtr=new DataView(this, wxSize(WX::MIN_WIDTH-(10+c_scrollerWidth), c_scrollerMinHeight));
	s_dataViewPtr=m_dataViewPtr;
	
	m_scroller=new ImageScrolledPanel(this, -1, c_scrollerWidth);
	m_scroller->SetBackgroundColour(*wxWHITE);
	m_scroller->SetMinSize(wxSize(c_scrollerWidth, c_scrollerMinHeight));

	Bind(wxEVT_CUSTOM_EVENT, [this](wxCommandEvent& event){
		int hPosition=0;
		m_scroller->apply([this, &event, &hPosition](wxPanel* panelPtr){
			hPosition+=panelPtr->GetSize().GetHeight();
			wxStackedImage* ptr=dynamic_cast<wxStackedImage*>(panelPtr);
			
			if(ptr){
				if(ptr->getFileName()==event.GetString()){
					ptr->highlight();
					hPosition-=panelPtr->GetSize().GetHeight();
					m_scroller->Scroll(0, hPosition/10);
					return true;
				}
			}
			return false;
		});
	}, EvtID::FIND_IMG);

	auto dockPtr = new wxBoxSizer(wxVERTICAL);
	dockPtr->Add(m_dataViewPtr, 1, wxALIGN_CENTER_HORIZONTAL);
	
	wxBoxSizer* hBox = new wxBoxSizer(wxHORIZONTAL);
	hBox->Add(m_scroller, 0, wxEXPAND | wxRIGHT, 10);
	hBox->Add(dockPtr, 1, wxEXPAND);

	SetSizerAndFit(hBox);
}

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(PictureViewer, wxPanel)
	EVT_SIZE(PictureViewer::resize)

	EVT_COMMAND(EvtID::DELETE_IMG, wxEVT_CUSTOM_EVENT, PictureViewer::OnDeleteImg)
	
	EVT_MENU(WX::DELETE_IMG, PictureViewer::OnDeleteImg)
	
END_EVENT_TABLE()

//----------------------------------------------------------------------

PictureViewer::~PictureViewer()
{
	wxDELETE(m_scroller);
}

//----------------------------------------------------------------------

void PictureViewer::loadPicture(const char* filePath, bool newBlock, float rank)
{
	m_scroller->addImage(filePath, newBlock, rank);
}

//----------------------------------------------------------------------

void PictureViewer::OnDeleteImg(wxCommandEvent& event)
{
	m_scroller->reduce([&event](wxPanel* panelPtr){
		wxStackedImage* ptr=dynamic_cast<wxStackedImage*>(panelPtr);
		if(ptr){
			return ptr->getFileName()==event.GetString();
		}
		return false;
	});

	m_dataViewPtr->removeNode(std::string(event.GetString().mb_str()));
	if(FileManager::deleteFile(event.GetString().mb_str())){
		wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::REBUILD_HIST);
		GetEventHandler()->AddPendingEvent(event);
	}
}

//----------------------------------------------------------------------

void PictureViewer::clear()
{
	m_dataViewPtr->reset();
	m_scroller->clear();
	wxStackedImage::clearBackground();
}

//----------------------------------------------------------------------

void PictureViewer::resize(wxSizeEvent& event)
{
	int width, height;
	GetSize(&width, &height);
	m_dataViewPtr->SetSize(width-(10+c_scrollerWidth), height);
	Layout();
	//event.Skip(); 
}

//----------------------------------------------------------------------
