/*********************************************************************
* DuplicateImgGUI class                                   			   *
*                                                                    *
* Version: 1.0                                                       *
* Date:    22-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef DUPLICATE_IMG_GUI_H
#define DUPLICATE_IMG_GUI_H

#include <wx/wx.h>
#include <wx/dirdlg.h>
#include <wx/listctrl.h>

#include <map>

#include "picture_viewer.h"
#include "wx_worker.h"
#include "progress_bar.h"

#include "notification_module.h"

//######################################################################

class DuplicateImgGUI : public wxFrame
{
	public:
		DuplicateImgGUI(const wxString& title);
		virtual ~DuplicateImgGUI();
		void setWorkerThread(WxWorker* wxWorker);

	private:
		std::map<wxString, bool> m_filter;
		std::vector<std::string> m_dirList;
		std::vector<std::string> m_fileList;
		wxDirDialog* m_dirDialog;
		wxTextCtrl* m_currentDir;
		wxListView* m_directories;
		PictureViewer* m_pictureViewer;
		wxPanel* m_bodyPanel;
		WxWorker* m_worker;
		wxStaticText* m_staticFiles;
		wxStaticText* m_totalFilesText;
		NotificationModule* m_board;
		ProgressBar* m_progressBar;
		wxButton* m_browseBtn;
		wxButton* m_startBtn;
		wxButton* m_closeBtn;
		wxButton* m_cancelBtn;
		wxButton* m_clearBtn;
		int m_totalFiles;

		void btnEnaDis(wxButton* btn, bool enable);
		void setEnable(int mask);
		void OnChooseDir(wxCommandEvent& evt);
		void OnStart(wxCommandEvent& evt);
		void OnQuit(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnData(wxCommandEvent& event);
		void OnProgressBar(wxCommandEvent& event);
		void OnUpdateStatus(wxCommandEvent& event);
		void OnAddPic(wxCommandEvent& event);
		void OnClear(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);

		struct
		{
			const int CANCELLED=2;//cancel: 0010 =2
			const int FINISHED=2;
			const int INITIAL_STATE=1;//initial state:  0001 = 1
			const int CLEAR=1;//clear= initial state
			const int READY=7;//when files are added: 0111 = 7
			const int FIND_DUPLICATES=8;//find ducplicates: 1000 = 10	
			const int RESET=4;
		};

		DECLARE_EVENT_TABLE()		
};

//----------------------------------------------------------------------

inline void DuplicateImgGUI::setWorkerThread(WxWorker* wxWorker){
	m_worker=wxWorker;
}

#endif
