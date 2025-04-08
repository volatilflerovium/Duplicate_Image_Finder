/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*  
* DuplicateImgGUI class                                   			   *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021  (Reviewed 03/2025)                            *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef DUPLICATE_IMG_GUI_H
#define DUPLICATE_IMG_GUI_H
#include "picture_viewer.h"
#include "wx_worker.h"
#include "progress_bar.h"
#include "notification_module.h"

#include <wx/wx.h>
#include <wx/dirdlg.h>
#include <wx/spinctrl.h>

#if 0
#define REBOOOT           1     // m_rebootBtn
#define ADD_DIRECTORY     1<<1 // m_addDirPopupBtn
#define SENSITIVITY_LEVEL 1<<2 // m_sensitivityChoice
#define UNDO              1<<3 // m_undoBtn
#define CANCEL            1<<4 // m_runCancelBtn
#define PLAY_STOP         1<<5 // m_playStopAnimationBtn

//states
#define INITIAL    ADD_DIRECTORY | SENSITIVITY_LEVEL
#define READY      REBOOOT | UNDO | PLAY_STOP
#define LOADED     REBOOOT | ADD_DIRECTORY | SENSITIVITY_LEVEL | RUN_CANCEL
#define PROCESSING  RUN_CANCEL
#define CANCELLED   REBOOOT | UNDO
#endif

//====================================================================

enum SENSITIVITY
{
	LOW=4,
	MEDIUM=6,
	HIGH=8,
};

class ExtendedPopup;
class DirectoryScrolledWindow;
class DuplicateImgGUI : public wxFrame
{
	public:
		DuplicateImgGUI(const wxString& title);
		virtual ~DuplicateImgGUI();
		void setWorkerThread(WxWorker* wxWorker);

	private:		
		std::vector<std::string> m_dirList;
		wxBitmapBundle m_runBitmapBundle;
		wxBitmapBundle m_cancellBitmapBundle;
		wxBitmapBundle m_cancelledBitmapBundle;
		wxBitmapBundle m_startAnimationBitmapBundle;
		wxBitmapBundle m_stopAnimationBitmapBundle;

		wxDirDialog* m_dirDialog;
		wxTextCtrl* m_currentDir;
		DirectoryScrolledWindow* m_directories;
		PictureViewer* m_pictureViewer;
		WxWorker* m_worker;
		ExtendedPopup* m_directoriesPopup;
		wxSpinCtrl* m_blurFilter;

		wxChoice* m_sensitivityChoice; 
		wxChoice* m_maskType; 
		wxChoice* m_maskMode; 

		wxStaticText* m_staticFiles;
		wxStaticText* m_totalFilesText;
		NotificationModule* m_board;
		ProgressBar* m_progressBar;
		
		wxBitmapButton* m_runCancelBtn;
		wxBitmapButton* m_undoBtn;
		wxBitmapButton* m_rebootBtn;
		wxBitmapButton* m_playStopAnimationBtn;
		
		wxButton* m_browseBtn;
		wxButton* m_addDirPopupBtn;

		enum class STATUS
		{
			INITIAL,
			READY,
			LOADED,
			PROCESSING,
			CANCELLED,
		} m_status;

		const SENSITIVITY m_initialSensitivity;
		bool m_animationRunning;

		void btnEnaDis(wxButton* btn, bool enable);

		void setEnable(STATUS status);
		void setEnable(int stateFlag);
		
		void getImages(const wxString& selectedDir);
		void OnChooseDir(wxCommandEvent& evt);
		void OnStart(wxCommandEvent& evt);
		void OnReset(wxCommandEvent& evt);
		void OnQuit(wxCommandEvent& event);

		void OnSettings(wxCommandEvent& event);
		void ResetHist(wxCommandEvent& event);
		void OnHistogramStatus(wxCommandEvent& event);
		void OnProgressBar(wxCommandEvent& event);
		void OnUpdateStatus(wxCommandEvent& event);
		void OnAddPic(wxCommandEvent& event);
		void OnClear(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnModeSelection(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()		
};

//----------------------------------------------------------------------

inline void DuplicateImgGUI::setWorkerThread(WxWorker* wxWorker)
{
	Scheduler::setSimilarityThreshold(m_initialSensitivity);
	m_worker=wxWorker;
}

//====================================================================

#endif
