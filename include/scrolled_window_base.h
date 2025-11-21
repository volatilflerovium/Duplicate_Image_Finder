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
#ifndef SCROLLED_WINDOW_BASE_H
#define SCROLLED_WINDOW_BASE_H
#include <list>
#include <functional>

#include <wx/scrolwin.h>

//====================================================================

class wxPanel;

class ScrolledWindowBase : public wxScrolledWindow
{
	public:
		ScrolledWindowBase(wxWindow* parent, int width, int minHeight=-1);
		virtual ~ScrolledWindowBase();

		void addPanel(wxPanel* panel);

		virtual void clear();
		void reduce(std::function<bool(wxPanel* panel)> cbk);
		void apply(std::function<bool(wxPanel*)> cbk);

	protected:
		std::list<wxPanel*> m_panelList;

		const int m_width;
		int m_height;
		
		void deletePanel(wxPanel* panel);
		void updateScroll(int height);
};

//----------------------------------------------------------------------

inline ScrolledWindowBase::ScrolledWindowBase(wxWindow* parent, int width, int minHeight) 
: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(width, minHeight))
, m_width(width)
, m_height(0)
{
}

//----------------------------------------------------------------------

inline ScrolledWindowBase::~ScrolledWindowBase()
{
	clear();
}

//----------------------------------------------------------------------

inline void ScrolledWindowBase::apply(std::function<bool(wxPanel*)> cbk)
{
	for(wxPanel* panelPtr : m_panelList){
		if(cbk(panelPtr)){
			break;
		}
	}
}

//====================================================================

#endif
