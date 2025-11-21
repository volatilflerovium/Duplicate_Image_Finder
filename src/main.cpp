#include "main.h"
#include "wx_worker.h"
#include <csignal>

sig_atomic_t signal_caught = 0;

static DuplicateImgGUI* duplicateImgGUI=nullptr;

void sigint_handler(int sig)
{	
	if(duplicateImgGUI){
		wxDELETE(duplicateImgGUI);
	}
	exit(sig);
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	duplicateImgGUI = new DuplicateImgGUI(wxT("Duplicated Image Finder"));
	duplicateImgGUI->Show(true);

	// create the thread
	WxWorker* wxWorker = new WxWorker(duplicateImgGUI);

	if(wxWorker->Create()!=wxTHREAD_NO_ERROR){
		wxMessageBox( _("Couldn't create thread!") );
		return false;
	}
	
	if(wxWorker->Run() != wxTHREAD_NO_ERROR){
		wxMessageBox( _("Couldn't run thread!") );
		return false;
	}
	
	duplicateImgGUI->setWorkerThread(wxWorker);

	return true;
}

void MyApp::OnClose(wxCloseEvent &event)
{
}

