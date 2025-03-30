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
#ifndef WX_STACKED_IMAGE_H
#define WX_STACKED_IMAGE_H

#include "image_panel.h"

//######################################################################

class wxStackedImage : public wxPanel
{
	public:
		wxStackedImage(wxWindow* parent, const wxPoint& pos, int width, const char* file, float rank);
		virtual ~wxStackedImage(){};
		static void clearBackground();
		const wxString& getFileName() const;

	private:
		static wxStackedImage* m_last;
		ImagePanel* m_pic;
		int m_padding;
		float m_rank;

		void OnOpenImg(wxCommandEvent& event);
		void OnDeleteImg(wxCommandEvent& event);

		void OnLeftClick(wxCommandEvent& event);
		void OnRightClick(wxCommandEvent& event);
		
		DECLARE_EVENT_TABLE()
};

inline const wxString& wxStackedImage::getFileName() const
{
	return m_pic->getFile();
}

//====================================================================

#endif
