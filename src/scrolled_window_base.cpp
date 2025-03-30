/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* ScrolledWindowBase class                          				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021   (Reviewed 03/2025)                                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "scrolled_window_base.h"
#include <wx/panel.h>

#include "utilities/debug_utils.h"

//====================================================================

void ScrolledWindowBase::addPanel(wxPanel* panel)
{
	int posX, posY; 
	GetViewStart(&posX, &posY);

	Scroll(0, 0);

	panel->SetPosition(wxPoint(0, m_height));
	
	m_panelList.push_back(panel);

	updateScroll(panel->GetSize().GetHeight());

	if(posY>0){
		Scroll(0, posY);
	}
}

//----------------------------------------------------------------------

void ScrolledWindowBase::reduce(std::function<bool(wxPanel*)> cbk)
{
	if(m_panelList.size()>0){
		Scroll(0, 0);
		int height=0;
		int scrollPosition=-1;
		int hPosition=0;
		bool found=false;
		wxPanel* panelPtr=nullptr;
		std::list<wxPanel*>::iterator it=m_panelList.begin();
		while(it!=m_panelList.end()){
			panelPtr=*it;
			while(cbk(panelPtr)){
				if(!found){
					found=true;
					scrollPosition=hPosition;
				}

				height+=panelPtr->GetSize().GetHeight();
				it=m_panelList.erase(it);
				wxDELETE(panelPtr);
			}

			if(it==m_panelList.end()){
				break;
			}
			panelPtr=*it;
			if(found){
				panelPtr->SetPosition(wxPoint(0, hPosition));
			}
			hPosition+=panelPtr->GetSize().GetHeight();
			++it;
		}

		updateScroll(-1*height);
		Scroll(0, scrollPosition/10);
	}
}

//----------------------------------------------------------------------

void ScrolledWindowBase::deletePanel(wxPanel* targetPanelPtr)
{
	reduce([targetPanelPtr](wxPanel* panelPtr){
		return targetPanelPtr==panelPtr;
	});
}

//----------------------------------------------------------------------

void ScrolledWindowBase::clear()
{
	m_height=0;
	for(auto& panelPtr : m_panelList){
		wxDELETE(panelPtr);
	}

	m_panelList.clear();

	SetScrollbars(1, 1, 0, 0, 0, 0);
	Scroll(0, 0);
}

//----------------------------------------------------------------------

void ScrolledWindowBase::updateScroll(int height)
{
	m_height+=height;

	const int stepY=10;
	int hy=1+std::floor(m_height/stepY);

	SetScrollbars(1, stepY, 0, hy, 0, 0, true);
	//Scroll(0, hy);
	Scroll(0, 0);
}

//====================================================================
