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
#include "image_scrolled_panel.h"
#include "wx_stacked_image.h"

const wxString c_separatorColour("#678fff");

//====================================================================

ImageScrolledPanel::ImageScrolledPanel(wxWindow* parent, wxWindowID id, int width) 
: ScrolledWindowBase(parent, width)
, m_newBlock(false)
{
}

//----------------------------------------------------------------------

ImageScrolledPanel::~ImageScrolledPanel()
{
	//clear();
}

//----------------------------------------------------------------------

void ImageScrolledPanel::addImage(const char* file, bool newBlock, float rank)
{
	const int sHeight=10;

	if(newBlock){
		if(m_newBlock){
			wxPanel* separator=new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(m_width, sHeight));
			separator->SetBackgroundColour(wxColour(c_separatorColour));
			addPanel(separator);			
		}
		m_newBlock=true;
	}
	wxStackedImage* newPic=new wxStackedImage(this, wxDefaultPosition, m_width, file, rank);

	addPanel(newPic);
}

//----------------------------------------------------------------------

void ImageScrolledPanel::clear()
{
	ScrolledWindowBase::clear();
	m_newBlock=false;
}

//----------------------------------------------------------------------

