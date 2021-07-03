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
		wxSlider* m_slider;
		wxStaticText* m_staticFiles;
		wxStaticText* m_totalFilesText;
		NotificationModule* m_board;
		ProgressBar* m_progressBar;
		wxButton* m_browseBtn;
		wxButton* m_startBtn;
		wxButton* m_resetBtn;
		wxButton* m_closeBtn;
		wxButton* m_cancelBtn;
		wxButton* m_clearBtn;
		int m_totalFiles;

		void btnEnaDis(wxButton* btn, bool enable);
		void setEnable(int mask);
		void OnChooseDir(wxCommandEvent& evt);
		void OnStart(wxCommandEvent& evt);
		void OnReset(wxCommandEvent& evt);
		void OnQuit(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnData(wxCommandEvent& event);
		void OnProgressBar(wxCommandEvent& event);
		void OnUpdateStatus(wxCommandEvent& event);
		void OnAddPic(wxCommandEvent& event);
		void OnClear(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnSliderChange(wxScrollEvent &event);

		struct
		{
			const int c_initial{32};//   | 100000 (32)
			const int c_ready{54};//     | 110110 (32+16+4+2+1= 32+20+3=54)
			const int c_running{1};//   | 000001 (1)
			const int c_cancelled{16};// | 010000 (16)
			const int c_cleared{32};//   | 100000 (32)
			const int c_finished{24};//  | 011000 (16+8=24)
			const int c_resetted{30};//  | 011010 (16+8+4+2=30)
		};

		DECLARE_EVENT_TABLE()		
};

//----------------------------------------------------------------------

inline void DuplicateImgGUI::setWorkerThread(WxWorker* wxWorker){
	m_worker=wxWorker;
}

#endif
