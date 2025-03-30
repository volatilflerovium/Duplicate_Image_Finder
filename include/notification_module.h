/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* NotifyLabel class                                       			   *
*                                                                    *
* Version: 1.0                                                       *
* Date:    24-06-2021   (Reviewed 03/2025)                           *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef NOTIFICATION_MODULE_H
#define NOTIFICATION_MODULE_H

#include <wx/wx.h>

//====================================================================

class NotificationModule : public wxPanel
{
	public:
		static NotificationModule* init(wxWindow* parent);
		static void displayData(const wxString& m_file, float rank);
		static void Clear();
		void clear();
		void setLabels(const wxString& m_file, float rank);
		~NotificationModule();
		
	private:
		static NotificationModule* m_instance;
		wxStaticText* m_filePathText;
		wxStaticText* m_rankText;
		NotificationModule(wxWindow* parent);
		void onResize(wxSizeEvent& evt);

};

//====================================================================

#endif
