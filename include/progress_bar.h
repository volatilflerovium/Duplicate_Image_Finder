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
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <wx/panel.h>

//====================================================================

class wxStaticText;

class ProgressBar : public wxPanel
{
	public:
		ProgressBar(wxWindow* parent, int width, int height);
		virtual ~ProgressBar();
		void setProgress(int prog);
		void step();

		void setUp(int vRange);
		void complete();
		void reset(bool hard=true);

	private:
		wxStaticText* m_progressText;
		const int m_range;
		const int m_height;
		int m_position;
		int m_step;
		float m_delta;
		
		void OnPaint(wxPaintEvent& event);
};

//----------------------------------------------------------------------

#endif
