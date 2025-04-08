/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* ScrolledWidgetsPane class                          				      *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021     (Reviewed 03/2025)                         *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef IMAGE_SCROLLED_PANEL_H
#define IMAGE_SCROLLED_PANEL_H
#include "scrolled_window_base.h"

//====================================================================

class ImageScrolledPanel : public ScrolledWindowBase
{
	public:
		ImageScrolledPanel(wxWindow* parent, wxWindowID id, int width);
		virtual ~ImageScrolledPanel()=default;
		void addImage(const char* file, bool newBlock, float rank);
		
		virtual void clear();

	private:
		bool m_newBlock;
};

//----------------------------------------------------------------------

inline ImageScrolledPanel::ImageScrolledPanel(wxWindow* parent, wxWindowID id, int width) 
: ScrolledWindowBase(parent, width)
, m_newBlock(false)
{
}

//----------------------------------------------------------------------

inline void ImageScrolledPanel::clear()
{
	ScrolledWindowBase::clear();
	m_newBlock=false;
}

//====================================================================

#endif
