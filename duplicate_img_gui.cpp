#include <wx/dir.h>

#include "duplicate_img_gui.h"
#include "file_manager.h"
#include "pic_wxwidget.h"
#include "progress_bar.h"


DuplicateImgGUI::DuplicateImgGUI(const wxString& title)
: wxFrame(NULL, -1, title, wxPoint(-1, -1)), m_totalFiles(0)
{
	FileManager::init();

	const int padding=10;

	SetMinSize(wxSize(WX::MIN_WIDTH, WX::MIN_HEIGHT));

	m_bodyPanel = new wxPanel(this, -1, wxDefaultPosition);

	wxBoxSizer* vBox = new wxBoxSizer(wxVERTICAL);
	//m_bodyPanel->SetSizer(vBox);

	wxBoxSizer* hbox_1 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* st1 =  new wxStaticText(m_bodyPanel, wxID_ANY, wxT("Directory: "));

	hbox_1->Add(st1, 0, wxLEFT, 0);

	m_currentDir = new wxTextCtrl(m_bodyPanel, wxID_ANY);
	int w, h=20;
	m_currentDir->GetSize(&w, &h);
	m_currentDir->SetMinSize(wxSize(200, h));
	hbox_1->Add(m_currentDir, 1, wxLEFT);

	m_dirDialog = new wxDirDialog(this);

	m_browseBtn = new wxButton(m_bodyPanel, WX::BROWSE, wxT("Browse"));
	Connect(WX::BROWSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DuplicateImgGUI::OnChooseDir));

	hbox_1->Add(10, 1);
	hbox_1->Add(m_browseBtn, 0);

	vBox->Add(hbox_1, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, padding);
	vBox->Add(-1, 10);
	

	//------------------------------------------
	//Directories
	
	m_directories= new wxListView(m_bodyPanel, wxID_ANY, 
		wxDefaultPosition, wxSize(WX::MIN_WIDTH, 100));

	wxListItem col0;
	col0.SetId(0);
	col0.SetText( wxT("Directories") );
	col0.SetWidth(WX::MIN_WIDTH-3*padding);
	m_directories->InsertColumn(0, col0);

	vBox->Add(m_directories, 1, wxEXPAND| wxLEFT | wxRIGHT, padding);

	//----------------------------------------------

	wxBoxSizer* hbox_2 = new wxBoxSizer(wxHORIZONTAL);
	hbox_2->Add(10, -1, wxEXPAND);

	wxStaticText* staticFiles = new wxStaticText(m_bodyPanel, wxID_ANY, wxT("Total files: "));
	hbox_2->Add(staticFiles, 0, wxLEFT, 0);
	
	m_totalFilesText=new wxStaticText(m_bodyPanel, -1, wxT("0"), wxDefaultPosition, wxSize(120,60), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	hbox_2->Add(m_totalFilesText, 0, wxLEFT, 0);

	vBox->Add(hbox_2, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, padding);
	
	//-------------------------------------------------------
	
	wxBoxSizer* hbox_3 = new wxBoxSizer(wxHORIZONTAL);
	
	m_clearBtn = new wxButton(m_bodyPanel, WX::CLEAR, wxT("Clear"));
	Connect(WX::CLEAR, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnClear));

	hbox_3->Add(m_clearBtn, 0, wxLEFT, 0);

	hbox_3->Add(10, -1, wxEXPAND);
	
	m_startBtn = new wxButton(m_bodyPanel, WX::START, wxT("Start"));
	Connect(WX::START, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnStart));

	hbox_3->Add(m_startBtn, 0, wxLEFT, 0);

	vBox->Add(hbox_3, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, padding);
	
	//-------------------------------------------------
	//progress bar
	
	//wxBoxSizer* hbox42 = new wxBoxSizer(wxHORIZONTAL);
	
	m_progressBar=new ProgressBar(m_bodyPanel, WX::MIN_WIDTH-3*padding, 30);
	m_progressBar->setProgress(0);
	vBox->Add(m_progressBar, 1, wxEXPAND | wxTOP |  wxLEFT | wxRIGHT, padding);

	//-------------------------------------------------
	//picture viewer
	
	//wxBoxSizer* hbox4 = new wxBoxSizer(wxHORIZONTAL);
	
	mkDummy(FileManager::c_IMG_BACKGROUND.c_str());
	m_pictureViewer=new PictureViewer(m_bodyPanel);
	
	vBox->Add(m_pictureViewer, 1, wxEXPAND | wxTOP |  wxLEFT | wxRIGHT, padding);

	//-------------------------------------------------
	/*
	wxBoxSizer* hbox5 = new wxBoxSizer(wxHORIZONTAL);
	wxCheckBox* cb1 = new wxCheckBox(m_bodyPanel, wxID_ANY, wxT("Case Sensitive"));	
	int cbW=0, cbh=0;
	cb1->GetSize(&w, &h);
	cbW+=h;
	hbox5->Add(cb1);

	wxCheckBox* cb2 = new wxCheckBox(m_bodyPanel, wxID_ANY, wxT("Nested Classes"));
	cb2->GetSize(&w, &h);
	cbW+=w;
	hbox5->Add(cb2, 0, wxLEFT, 10);

	wxCheckBox* cb3 = new wxCheckBox(m_bodyPanel, wxID_ANY, wxT("Non-Project Classes"));	
	cb3->GetSize(&w, &h);
	cbW+=w;

	hbox5->Add(cb3, 0, wxLEFT, 10);
	
	vBox->Add(hbox5, 0, wxLEFT, 10);
	
	vBox->Add(-1, 25);
	//*/
	//-------------------------------------------------
	
	m_cancelBtn = new wxButton(m_bodyPanel, WX::CANCEL, wxT("Cancel"));
	Connect(WX::CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnCancel));

	m_closeBtn = new wxButton(m_bodyPanel, wxID_EXIT, wxT("Close"));
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnQuit));

	wxBoxSizer* hbox6 = new wxBoxSizer(wxHORIZONTAL);
	hbox6->Add(m_cancelBtn, 0);
	hbox6->Add(10, -1, wxEXPAND);
	hbox6->Add(m_closeBtn, 0);
	
	vBox->Add(hbox6, 0, wxEXPAND | wxALL, padding);
	



	setEnable(INITIAL_STATE);

	m_bodyPanel->SetSizerAndFit(vBox);

	Centre();
}

//----------------------------------------------------------------------

DuplicateImgGUI::~DuplicateImgGUI(){
	wxDELETE(m_bodyPanel);
	
	/*wxDELETE(m_dirDialog);
	wxDELETE(m_currentDir);
	wxDELETE(m_directories);
	wxDELETE(m_pictureViewer);
	
	//WxWorker* m_worker;
	//wxDELETE(m_browseBtn);
	//wxDELETE(m_startBtn);
	//wxDELETE(m_closeBtn);
	//wxDELETE(m_cancelBtn);
	//wxDELETE(m_clearBtn);//*/
	//std::cout<<"destroying simple!\n";
};

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(DuplicateImgGUI, wxFrame )
	EVT_CLOSE(DuplicateImgGUI::OnClose)
	EVT_COMMAND(EVNT_DATA_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnData)
	EVT_COMMAND(EVNT_ADD_PICTURE_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnAddPic)
	EVT_COMMAND(EVNT_PROGRESS_BAR_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnProgressBar)
	EVT_COMMAND(EVNT_STATUS_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnUpdateStatus)
	//EVT_SIZE(DuplicateImgGUI::hi)
END_EVENT_TABLE()

//----------------------------------------------------------------------

void DuplicateImgGUI::OnChooseDir(wxCommandEvent& evt){
	if (m_dirDialog->ShowModal() == wxID_OK){
		wxString selectedDir = m_dirDialog->GetPath();
		
		SUBDIR test=FileManager::reduce(selectedDir.ToStdString(), m_dirList);
		if(test==SUBDIR::IS_SUB){
			return;
		}
		else if(test==SUBDIR::IS_NEW){
			m_dirList.push_back(selectedDir.ToStdString());
		}

		m_currentDir->SetValue(selectedDir);
		long index=m_directories->InsertItem(0, selectedDir);
		
		wxArrayString files;
		int total=wxDir::GetAllFiles(selectedDir, &files, wxT(""),  wxDIR_FILES | wxDIR_DIRS);
	 	for(int i=0; i<total; i++){
			if(FileManager::isSuported(files[i])){
				m_fileList.push_back(std::string(files[i]));
			}
		}

		FileManager::removeDuplicates(m_fileList);
		if(m_fileList.size()>0){
			setEnable(READY);
		}
		int n=m_fileList.size();
		m_totalFilesText->SetLabel(wxString::Format(wxT("%d"), n));
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::btnEnaDis(wxButton* btn, bool enable){
	if(enable){
		btn->Enable();
		return;
	}
	btn->Disable();
}

//----------------------------------------------------------------------

void DuplicateImgGUI::setEnable(int mask){
	btnEnaDis(m_browseBtn, static_cast<bool>(mask & 1));
	btnEnaDis(m_clearBtn, static_cast<bool>(mask & 2));
	btnEnaDis(m_startBtn, static_cast<bool>(mask & 4));
	btnEnaDis(m_cancelBtn, static_cast<bool>(mask & 8));
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnAddPic(wxCommandEvent& event){
	bool newBlock=false;
	float rank=event.GetInt()/10000;
	if(event.GetInt()<0){
		newBlock=true;
		rank*=-1.0;
	}
	m_pictureViewer->loadPicture(event.GetString().ToStdString(), newBlock, rank);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnData(wxCommandEvent& event){
	
	if(event.GetString()==c_histFinish){
		int m_totalFiles=event.GetInt();
		int x=(m_totalFiles+DIMGS::BLOCK_SIZE-1)/DIMGS::BLOCK_SIZE;
	
		int totalComp=blockComp(x);
		if(totalComp>0){
			//m_progressBar->complete();
			m_worker->processJob();
		}

		m_progressBar->reset();
		m_progressBar->setUp(totalComp);
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnProgressBar(wxCommandEvent& event){
	m_progressBar->step();
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnUpdateStatus(wxCommandEvent& event){
	int statusCode=event.GetInt();
	if(statusCode==SearchFinish){
		m_progressBar->complete();
		setEnable(FINISHED);
	}
	else if(statusCode==WorkCancelled){
		setEnable(CANCELLED);
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnStart(wxCommandEvent & event){
	if(m_fileList.size()>0){
		m_progressBar->setUp(m_fileList.size());
		setEnable(FIND_DUPLICATES);
		m_worker->setFileList(&m_fileList);
		m_worker->processJob();
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnClear(wxCommandEvent & event){
	FileManager::cleanUp(false);
	Printer::reset();
	HistogramLoader::setChunks(0, 0);
	m_totalFilesText->SetLabel(wxT("0"));
	m_totalFiles=0;	
	m_directories->DeleteAllItems();
	m_pictureViewer->clear();
	m_fileList.clear();
	m_dirList.clear();
	m_progressBar->reset();
	setEnable(CLEAR);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnCancel(wxCommandEvent & event){
	m_worker->cancel();
	setEnable(INITIAL_STATE);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnQuit(wxCommandEvent & event){

	if(!FileManager::isSessionSaved()){
		FileManager::cleanUp();
	}

	Close(true);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnClose(wxCloseEvent &event){
	m_worker->terminate();
	event.Skip();
}

//----------------------------------------------------------------------