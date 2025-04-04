#include "main.h"
#include "wx_worker.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	m_simple = new DuplicateImgGUI(wxT("Duplicated Image Finder"));
	m_simple->Show(true);

	// create the thread
	WxWorker* wxWorker = new WxWorker(m_simple);
	//wxThreadError err = wxWorker->Create();
	if(wxWorker->Create()!=wxTHREAD_NO_ERROR){
	
	//if(err != wxTHREAD_NO_ERROR){
		wxMessageBox( _("Couldn't create thread!") );
		return false;
	}
	
	if(wxWorker->Run() != wxTHREAD_NO_ERROR){
		wxMessageBox( _("Couldn't run thread!") );
		return false;
	}
	
	m_simple->setWorkerThread(wxWorker);

	return true;
}

void MyApp::OnClose(wxCloseEvent &event){
	//std::cout<<"closing app!\n";
}

