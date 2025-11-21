/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class DirectoryPanel                                                    *
* class DirectoryScrolledWindow        	                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "directory_scrolled_window.h"
#include "wx_constants.h"

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>

//====================================================================

DirectoryPanel::DirectoryPanel(wxWindow* parent, uint posY, uint width, const wxString& fileName, const wxString& data)
:wxPanel(parent, wxID_ANY, wxPoint(0, posY), wxDefaultSize)
{
	SetWindowStyle(wxSIMPLE_BORDER);

	const int secondColWidth=100;
	auto path=new wxStaticText(this, wxID_ANY, fileName, wxDefaultPosition, 
	wxSize(width-(secondColWidth+10), -1), wxST_ELLIPSIZE_MIDDLE);

	auto numFiles=new wxStaticText(this, wxID_ANY, data, wxDefaultPosition, 
	wxSize(secondColWidth-2, -1), wxALIGN_RIGHT);
	
	wxBoxSizer* hbox=new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(path, 0,  wxTOP | wxBOTTOM, 5);
	
	hbox->Add(numFiles, 0, wxTOP | wxBOTTOM, 5);

	this->SetSizerAndFit(hbox);
}

//====================================================================

DirectoryScrolledWindow::DirectoryScrolledWindow(wxWindow* parent, int Id, const wxPoint& Point, const wxSize& wSize)
:ScrolledWindowBase(parent, wSize.GetWidth(), wSize.GetHeight())
{
	SetBackgroundColour(wxColour("#FFFFFF"));
}

//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(DirectoryScrolledWindow, wxScrolledWindow)
	EVT_MENU(WX::REMOVE_DIR, DirectoryScrolledWindow::RemoveDirectory)
END_EVENT_TABLE()


//----------------------------------------------------------------------

void DirectoryScrolledWindow::addDirectoryPanel(const wxString& fileName, const wxString& data)
{
	DirectoryPanel* filePanel=new DirectoryPanel(this, m_height, m_width, fileName, data);
	addPanel(filePanel);
}

void DirectoryScrolledWindow::addDirectoryPanel(const wxString& fileName, uint totalFiles)
{
	addDirectoryPanel(fileName, wxString::Format("%d", totalFiles));
}

//--------------------------------------------------------------------

void DirectoryScrolledWindow::RemoveDirectory(wxCommandEvent& event)
{
	wxMessageDialog confirm(
		this,
		wxT("Do you want to remove the selected directory?"),
		wxT(""),
		wxYES_NO|wxCENTRE|wxICON_WARNING
	);

	if(wxID_NO==confirm.ShowModal()){
		return;
	}
	/*
	if(m_filePanelList.size()>0){
		Scroll(0, 0);
		int height=0;
		int scrollPosition=0;
		int hPosition=0;
		bool found=false;
		for(size_t i=0; i<m_filePanelList.size(); i++){
			DirectoryPanel* filePanelPtr= m_filePanelList[i];
			if(filePanelPtr->isSelected()){
				DirectoryPanel::s_lastSelected=nullptr;
				height=-1*filePanelPtr->getHeight();

				std::error_code ec;
				std::filesystem::path filePath{getFilePath(filePanelPtr->getFileName().mb_str())};
				if(std::filesystem::exists(filePath, ec)){
					std::filesystem::remove(filePath);
					wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::REMOVE_FILE_FROM_DROPDOWN);
					event.SetString(filePanelPtr->getFileName());
					wxPostEvent(this, event);
				}

				found=true;
				scrollPosition=hPosition;
				delete(filePanelPtr);
				continue;
			}

			if(found){
				m_filePanelList[i-1]=m_filePanelList[i];
				filePanelPtr->SetPosition(wxPoint(0, hPosition));
			}
		
			hPosition+=filePanelPtr->getHeight();
		}

		updateScroll(height);
		Scroll(0, scrollPosition/10);

		m_filePanelList.pop_back();
	}
	// */
}


//--------------------------------------------------------------------

void DirectoryScrolledWindow::clear()
{
	ScrolledWindowBase::clear();

	addDirectoryPanel("Directories", "Images");
}

//====================================================================

