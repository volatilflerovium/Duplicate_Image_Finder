/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class BasicPopup                                                   *
* class PopupWrapper                                                 *
* class WX_Popup                                                     *
* class ExtendedPopup                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "extended_popup.h"

#include <wx/display.h>

//====================================================================

BasicPopup::BasicPopup(wxWindow* parent, const char* title, bool allowDestroy)
:wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize
, wxSTAY_ON_TOP | wxFRAME_NO_WINDOW_MENU | wxCLOSE_BOX)
, m_destroy(allowDestroy)
{
	Hide();

	Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event){
		if(m_onClose){
			m_onClose();
		}

		if(!m_destroy){
			Dismiss();
		}
		else{
			event.Skip();
		}
	});
}

//--------------------------------------------------------------------

void BasicPopup::DoPopupWindow(const wxPoint& pos)
{
	wxRect rect=wxDisplay(this).GetGeometry();
	wxSize sz=GetSize();
	wxPoint result=pos;

	int x=pos.x+sz.x-rect.GetWidth();
	if(x>0){
		result.x=pos.x-x;
	}

	int y=pos.y+sz.y-rect.GetHeight();
	if(y>0){
		result.y=pos.y-y;
	}

	SetPosition(FromDIP(result));
}

//--------------------------------------------------------------------

void BasicPopup::DoPopupWindow()
{
	wxRect rect=wxDisplay(this).GetGeometry();
	wxSize sz=GetSize();
	wxPoint result(
		(rect.GetWidth()-sz.x)/2,
		(rect.GetHeight()-sz.y)/2
	);

	SetPosition(FromDIP(result));
}

//====================================================================

WX_Popup::WX_Popup(wxWindow *parent, const char* title, bool allowDestroy)
:BasicPopup(parent, title, allowDestroy)
{
	Bind(wxEVT_ACTIVATE, [this](wxActivateEvent& event){
		if(!event.GetActive()){
			if(!isMouseHover()){
				OnDismiss();
				Dismiss();
			}
		}
	});	
}

//--------------------------------------------------------------------

bool WX_Popup::isMouseHover()
{
	wxPoint mousePosition=wxGetMousePosition();
	if(mousePosition.x<GetPosition().x+GetSize().x &&
			mousePosition.x>GetPosition().x){
		if(mousePosition.y<GetPosition().y+GetSize().y &&
			mousePosition.y>GetPosition().y){
			return true;
		}
	}
	return false;
}

//====================================================================
