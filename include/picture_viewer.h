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
#ifndef PICTURE_VIEWER_H
#define PICTURE_VIEWER_H

#include <wx/panel.h>

//====================================================================

class ImageScrolledPanel;
class DataView;

class PictureViewer : public wxPanel
{
	public:
		PictureViewer(wxWindow* parent);
		~PictureViewer();

		void loadPicture(const char* filePath, bool newBlock, float rank);
		void clear();

	private:
		ImageScrolledPanel* m_scroller;		
		DataView* m_dataViewPtr;

		void resize(wxSizeEvent& event);
		void OnDeleteImg(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

//====================================================================

#endif
