/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class DirectoryPanel                                               *
* class DirectoryScrolledWindow        	                           *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    20-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _DIRECTORY_SCROLLED_WINDOW_H
#define _DIRECTORY_SCROLLED_WINDOW_H

#include "scrolled_window_base.h"

//====================================================================

class DirectoryPanel : public wxPanel
{
	public:
		DirectoryPanel(wxWindow* parent, uint posY, uint width, const wxString& fileName, const wxString& data);

		DirectoryPanel(wxWindow* parent, uint posY, uint width, const wxString& fileName, uint totalFiles)
		:DirectoryPanel(parent, posY, width, fileName, wxString::Format("%d", totalFiles))
		{}

		virtual ~DirectoryPanel()=default;
		
		int getHeight()
		{
			return GetSize().GetHeight();
		}

	private:
		friend class DirectoryScrolledWindow;
};

//====================================================================

class DirectoryScrolledWindow : public ScrolledWindowBase
{
	public:
		DirectoryScrolledWindow(wxWindow* parent, int Id, const wxPoint& Point, const wxSize& wSize);
		virtual ~DirectoryScrolledWindow()=default;

		void addDirectoryPanel(const wxString& fileName, const wxString& data);
		void addDirectoryPanel(const wxString& fileName, uint totalFiles);
		virtual void clear();

	private:
		void RemoveDirectory(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

//====================================================================

#endif
