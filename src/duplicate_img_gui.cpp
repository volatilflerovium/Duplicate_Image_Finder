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
#include "duplicate_img_gui.h"
#include "file_manager.h"
#include "progress_bar.h"
#include "wx_constants.h"
#include "wxstring_array.h"
#include "extended_popup.h"
#include "event_definitions.h"

#include "directory_scrolled_window.h"
#include "data_visualization.h"

#include <wx/dir.h>
#include "utilities/profiler.h"
#include "utilities/debug_utils.h"

Profiler<std::chrono::milliseconds> s_profiler;

extern DataView* s_dataViewPtr;

//====================================================================

static int blockComp(int r)
{
	int result=0;
	while(r>0){
		result+=r--;
	}
	return result;
}

static wxBitmapBundle mkBitmapBundle(const char* icon)
{
	wxVector<wxBitmap> bitmaps;
	auto iconDirMk=[&bitmaps](const char* relPath, const char* iconName){
		std::string iconDir=FileManager::iconsPath();
		iconDir.append(relPath);
		iconDir.append(iconName);
		bitmaps.push_back(wxBitmap(iconDir, wxBITMAP_TYPE_PNG));
	};
	iconDirMk("16x16/", icon);
	iconDirMk("24x24/", icon);
	iconDirMk("32x32/", icon);
	iconDirMk("48x48/", icon);
	iconDirMk("64x64/", icon);

	return wxBitmapBundle::FromBitmaps(bitmaps);
}

static wxBitmapButton* makeButton(wxWindow* parent, const char* icon, int id=wxID_ANY)
{
	return new wxBitmapButton(parent, id, mkBitmapBundle(icon), wxDefaultPosition, wxSize(-1, -1));
}

//====================================================================

DuplicateImgGUI::DuplicateImgGUI(const wxString& title)
: wxFrame(NULL, -1, title, wxPoint(-1, -1))
, m_runBitmapBundle(mkBitmapBundle("actions/system-run-symbolic.symbolic.png"))
, m_cancellBitmapBundle(mkBitmapBundle("actions/process-stop-symbolic.symbolic.png"))
, m_cancelledBitmapBundle(mkBitmapBundle("actions/action-unavailable-symbolic.symbolic.png"))
, m_startAnimationBitmapBundle(mkBitmapBundle("actions/media-playback-start-symbolic.symbolic.png"))
, m_stopAnimationBitmapBundle(mkBitmapBundle("actions/media-playback-stop-symbolic.symbolic.png"))
, m_status(STATUS::INITIAL)
, m_initialSensitivity(SENSITIVITY::MEDIUM)
, m_animationRunning(false)
{
	wxInitAllImageHandlers();

	FileManager::init();

	SetMinSize(wxSize(WX::MIN_WIDTH, WX::MIN_HEIGHT));

	wxBoxSizer* vBox = new wxBoxSizer(wxVERTICAL);

	m_directoriesPopup=new ExtendedPopup(this, "Directories");
	{
		wxStaticText* st1 =m_directoriesPopup->builder<wxStaticText>(wxID_ANY, wxT("Directory: "));

		m_currentDir = m_directoriesPopup->builder<wxTextCtrl>(wxID_ANY, 
			wxEmptyString, wxDefaultPosition, 
			wxDefaultSize, wxTE_PROCESS_ENTER);

		m_currentDir->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& event){
			getImages(m_currentDir->GetValue());
		});

		m_dirDialog = m_directoriesPopup->builder<wxDirDialog>();

		m_browseBtn = m_directoriesPopup->builder<wxButton>(WX::BROWSE, wxT("Browse"));
		Connect(WX::BROWSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DuplicateImgGUI::OnChooseDir));

		wxBoxSizer* vBox = new wxBoxSizer(wxVERTICAL);
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

			hbox->Add(st1, 0, wxRIGHT, 10);
			hbox->Add(m_currentDir, 1, wxRIGHT, 10);
			hbox->Add(m_browseBtn, 0);
			vBox->Add(hbox, 0, wxEXPAND | wxBOTTOM, 10);
		}
		//------------------------------------------
		//Directories
		
		m_directories= m_directoriesPopup->builder<DirectoryScrolledWindow>(wxID_ANY, 
		wxDefaultPosition, wxSize(WX::MIN_WIDTH-3*c_windowPadding, 200));

		m_directories->addDirectoryPanel("Directories", "Images");

		auto startBtn = m_directoriesPopup->builder<wxButton>(wxID_ANY, wxT("OK"));

		startBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){			
			m_directoriesPopup->Dismiss();
		});
		/* 
		 * auto startBtn = m_directoriesPopup->builder<wxButton>(WX::START_FROM_POPUP, wxT("Start"));
		 * Connect(WX::START_FROM_POPUP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DuplicateImgGUI::OnStart));
		*/
		vBox->Add(m_directories, 1, wxEXPAND | wxBOTTOM, 10);
		vBox->Add(startBtn, 0, wxALIGN_RIGHT);

		m_directoriesPopup->setSizer(vBox);
	}

	m_addDirPopupBtn=new wxButton(this, wxID_ANY, wxT("Add Directories"));

	m_addDirPopupBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		m_directoriesPopup->Popup();
	});

	//----------------------------------------------
	// Static Text
	const int lineHeight=30;

	ArrayStringType sensitivity(3, "");

	sensitivity[0]="High";
	sensitivity[1]="Medium";
	sensitivity[2]="Low";

	m_sensitivityChoice=new wxChoice(this, WX::SENSITIVITY_MODE, wxDefaultPosition,
											wxDefaultSize, sensitivity);
	m_sensitivityChoice->SetSelection(1);

	m_staticFiles = new wxStaticText(this, wxID_ANY, wxT("Total files: "));

	m_totalFilesText=new wxStaticText(this, -1, wxT("0"), wxDefaultPosition, wxSize(60,lineHeight), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);

	//-------------------------------------------------------
	
	m_playStopAnimationBtn=makeButton(this, "actions/media-playback-start-symbolic.symbolic.png");
	m_playStopAnimationBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		s_dataViewPtr->startAnimation();
		m_animationRunning=!m_animationRunning;
		if(m_animationRunning){
			m_playStopAnimationBtn->SetBitmap(m_stopAnimationBitmapBundle);
		}
		else{
			m_playStopAnimationBtn->SetBitmap(m_startAnimationBitmapBundle);
		}
		
	});

	m_rebootBtn=makeButton(this, "actions/system-reboot-symbolic.symbolic.png", WX::CLEAR);
	Connect(WX::CLEAR, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnClear));

	m_undoBtn=makeButton(this, "actions/edit-undo-symbolic.symbolic.png", WX::RESET);
	Connect(WX::RESET, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnReset));

	m_runCancelBtn=makeButton(this, "actions/system-run-symbolic.symbolic.png", WX::START);	
	Connect(WX::START, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(DuplicateImgGUI::OnStart));

	wxBitmapButton* settingsBtn=makeButton(this, "categories/preferences-system-symbolic.symbolic.png");

	settingsBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		OnSettings(event);
	});

	// Layout
	{
		wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	
		hbox->Add(settingsBtn, 0, wxRIGHT, 10);
		hbox->Add(m_playStopAnimationBtn, 0, wxRIGHT, 10);
		hbox->Add(-1, 1, wxEXPAND);
		hbox->Add(m_rebootBtn, 0, wxRIGHT, 10);
		hbox->Add(m_addDirPopupBtn, 0, wxRIGHT, 10);
		hbox->Add(m_staticFiles, 0, wxRIGHT | wxALIGN_CENTER, 10);		
		hbox->Add(m_totalFilesText, 0, wxRIGHT | wxALIGN_BOTTOM, 10);
		hbox->Add(m_sensitivityChoice, 0, wxRIGHT, 10);
		hbox->Add(m_undoBtn, 0, wxRIGHT, 10);
		hbox->Add(m_runCancelBtn, 0);

		vBox->Add(hbox, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, c_windowPadding);
	}
	//-------------------------------------------------
	//progress bar

	m_progressBar=new ProgressBar(this, WX::MIN_WIDTH-3*c_windowPadding, 30);
	m_progressBar->setProgress(0);

	vBox->Add(m_progressBar, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, c_windowPadding);// element will expand...and it will pass extra space to its childs
	
	//-------------------------------------------------
	//picture viewer

	m_pictureViewer=new PictureViewer(this);
	
	vBox->Add(m_pictureViewer, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, c_windowPadding);

	//-------------------------------------------------
	// file and rank labels
	NotificationModule* m_board=NotificationModule::init(this);

	vBox->Add(m_board, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, c_windowPadding);
	
	//-------------------------------------------------

	setEnable(STATUS::INITIAL);

	this->SetSizerAndFit(vBox);
	Maximize(true);
	//Centre();
}

//----------------------------------------------------------------------

DuplicateImgGUI::~DuplicateImgGUI()
{
	wxDELETE(m_staticFiles);
	wxDELETE(m_totalFilesText);
	wxDELETE(m_progressBar);
	wxDELETE(m_board);
};

//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(DuplicateImgGUI, wxFrame )
	EVT_CLOSE(DuplicateImgGUI::OnClose)
	EVT_COMMAND(EVNT_DATA_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnHistogramStatus)
	EVT_COMMAND(EVNT_ADD_PICTURE_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnAddPic)
	EVT_COMMAND(EVNT_PROGRESS_BAR, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnProgressBar)
	EVT_COMMAND(EVNT_STATUS_ID, wxEVT_COMMAND_TEXT_UPDATED, DuplicateImgGUI::OnUpdateStatus)

	EVT_COMMAND(EvtID::REBUILD_HIST, wxEVT_CUSTOM_EVENT, DuplicateImgGUI::ResetHist)

	EVT_CHOICE(WX::SENSITIVITY_MODE, DuplicateImgGUI::OnModeSelection)
END_EVENT_TABLE()

//----------------------------------------------------------------------

void DuplicateImgGUI::ResetHist(wxCommandEvent& event)
{
	m_worker->makeHistograms(true);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnModeSelection(wxCommandEvent& event)
{
	int val = m_sensitivityChoice->GetSelection();
	if(val==0){
		Scheduler::setSimilarityThreshold(SENSITIVITY::HIGH);
	}
	else if(val==1){
		Scheduler::setSimilarityThreshold(SENSITIVITY::MEDIUM);
	}
	else{// val==2
		Scheduler::setSimilarityThreshold(SENSITIVITY::LOW);
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::getImages(const wxString& selectedDir)
{
	SUBDIR test=FileManager::reduce(selectedDir.ToStdString(), m_dirList);
	if(test==SUBDIR::IS_SUB){
		return;
	}
	else if(test==SUBDIR::IS_NEW){
		m_dirList.push_back(selectedDir.ToStdString());
	}

	m_currentDir->SetValue(selectedDir);

	int filesInDir=FileManager::crawler(selectedDir.mb_str());

	m_directories->addDirectoryPanel(selectedDir, filesInDir);

	if(FileManager::totalFiles()>0){
		setEnable(STATUS::LOADED);
	}

	m_totalFilesText->SetLabel(wxString::Format(wxT("%d"), (int)FileManager::totalFiles()));
	
	size_t p=selectedDir.find_last_of("/");
	m_dirDialog->SetPath(selectedDir.substr(0, p));

	m_worker->makeHistograms(filesInDir>0);	
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnChooseDir(wxCommandEvent& evt)
{
	if (m_dirDialog->ShowModal() == wxID_OK){
		getImages(m_dirDialog->GetPath());
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::btnEnaDis(wxButton* btn, bool enable)
{
	if(enable){
		btn->Enable();
		return;
	}
	btn->Disable();
}

//----------------------------------------------------------------------

void DuplicateImgGUI::setEnable(int stateFlag)
{
	/*
	m_rebootBtn->Enable(REBOOOT  & stateFlag);
	m_addDirPopupBtn->Enable(ADD_DIRECTORY & stateFlag);
	m_sensitivityChoice->Enable(SENSITIVITY & stateFlag);
	m_undoBtn->Enable(UNDO & stateFlag);
	m_runCancelBtn->Enable(RUN_CANCEL & stateFlag);
	m_playStopAnimationBtn->Enable(PLAY_STOP & stateFlag);
	// */
}

void DuplicateImgGUI::setEnable(STATUS status)
{
	m_status=status;

	m_playStopAnimationBtn->Enable(false);
	if(m_animationRunning){
		m_animationRunning=false;
		s_dataViewPtr->startAnimation();
		m_playStopAnimationBtn->SetBitmap(m_startAnimationBitmapBundle);
	}

	if(m_status==STATUS::INITIAL){
		m_rebootBtn->Enable(false);
		m_addDirPopupBtn->Enable(true);
		m_sensitivityChoice->Enable(true);
		m_undoBtn->Enable(false);
		m_runCancelBtn->Enable(false);
			m_runCancelBtn->SetBitmap(m_runBitmapBundle);
	}
	else if(m_status==STATUS::READY){ //processing has finished
		m_rebootBtn->Enable(true);
		m_addDirPopupBtn->Enable(false);
		m_sensitivityChoice->Enable(false);
		m_undoBtn->Enable(true);
		m_runCancelBtn->Enable(false);
			m_runCancelBtn->SetBitmap(m_runBitmapBundle);
		m_playStopAnimationBtn->Enable(true);
	}
	else if(m_status==STATUS::LOADED){
		m_rebootBtn->Enable(true);
		m_addDirPopupBtn->Enable(true);
		m_sensitivityChoice->Enable(true);
		m_undoBtn->Enable(false);
		m_runCancelBtn->Enable(true);
			m_runCancelBtn->SetBitmap(m_runBitmapBundle);
	}
	else if(m_status==STATUS::PROCESSING){
		m_rebootBtn->Enable(false);
		m_addDirPopupBtn->Enable(false);
		m_sensitivityChoice->Enable(false);
		m_undoBtn->Enable(false);
		m_runCancelBtn->Enable(true);// cancell
			m_runCancelBtn->SetBitmap(m_cancellBitmapBundle);
	}
	else if(m_status==STATUS::CANCELLED){
		m_rebootBtn->Enable(true);
		m_addDirPopupBtn->Enable(false);
		m_sensitivityChoice->Enable(false);
		m_undoBtn->Enable(true);
		m_runCancelBtn->Enable(false);
			m_runCancelBtn->SetBitmap(m_cancelledBitmapBundle);
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnAddPic(wxCommandEvent& event)
{
	bool newBlock=false;
	float rank=(1.0*event.GetInt())*DIMGS::INV_FLOAT_FACTOR;
	if(event.GetInt()<0){
		newBlock=true;
		rank*=-1.0;
	}

	m_pictureViewer->loadPicture(event.GetString().mb_str(), newBlock, rank);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnHistogramStatus(wxCommandEvent& event)
{
	if(event.GetString()==c_histFinish){
		int totalFiles=event.GetInt();
		int x=(totalFiles+DIMGS::BLOCK_SIZE-1)/DIMGS::BLOCK_SIZE;
		int totalComp=blockComp(x);
		if(totalComp>0){
			HistogramLoader::setChunks(0, 0);
			m_worker->processJob();
		}
		
		m_progressBar->reset();
		m_progressBar->setUp(totalComp);
		m_worker->makeHistograms(false);
	}
	else{//c_histCancelled
		m_worker->makeHistograms(true);
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnProgressBar(wxCommandEvent& event)
{
	m_progressBar->step();
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnUpdateStatus(wxCommandEvent& event)
{
	int statusCode=event.GetInt();
	if(statusCode==SearchFinish){
		m_progressBar->complete();
		setEnable(STATUS::READY);
	}
	else if(statusCode==WorkCancelled){
		setEnable(STATUS::CANCELLED);
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnStart(wxCommandEvent & event)
{
	if(m_status==STATUS::LOADED){
		setEnable(STATUS::PROCESSING);
		m_pictureViewer->clear();
		m_progressBar->reset(false);
		NotificationModule::Clear();

		m_progressBar->setUp(FileManager::totalFiles());
		m_worker->processJob();
	}
	else	if(m_status==STATUS::PROCESSING){
		m_worker->cancel();
	}
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnReset(wxCommandEvent & event)
{
	HistogramLoader::setChunks(0, 0);
	m_pictureViewer->clear();
	m_progressBar->reset(false);
	NotificationModule::Clear();

	setEnable(STATUS::LOADED);	
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnClear(wxCommandEvent & event)
{
	
	m_directories->clear();
	m_totalFilesText->SetLabel(wxT("0"));
	FileManager::cleanUp(false);
	FileManager::clearFilesMap();
	m_dirList.clear();
	HistogramLoader::setChunks(0, 0);	
	m_pictureViewer->clear();
	m_progressBar->reset();
	NotificationModule::Clear();

	setEnable(STATUS::INITIAL);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnSettings(wxCommandEvent& event)
{
	auto settings=SettingsManager::getSettingManager();
	auto settingsPopup=new ExtendedPopup(this, "Settings", true);
	{
		wxStaticText* st1 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node border color: "));
		auto textCtrl1=settings.NodeBorderColor(settingsPopup, [](){s_dataViewPtr->loadSettings();});
				
		wxStaticText* st2 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node font color: "));
		auto textCtrl2=settings.NodeFontColor(settingsPopup, [](){s_dataViewPtr->loadSettings();});

		wxStaticText* st3 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Edge color: "));
		auto textCtrl3=settings.EdgesColor(settingsPopup, [](){s_dataViewPtr->loadSettings();});
		
		//canvas color
		wxStaticText* st4 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Background color: "));
		auto textCtrl4=settings.CanvasColor(settingsPopup, [](){s_dataViewPtr->loadSettings();});

		wxStaticText* st5=settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node background color: "));
		auto textCtrl5=settings.NodeBkgColor(settingsPopup, [](){s_dataViewPtr->loadSettings();});

		wxStaticText* st6=settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node selected color: "));
		auto textCtrl6=settings.NodeSelectedColor(settingsPopup, [](){s_dataViewPtr->loadSettings();});

		wxStaticText* st7=settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node width: "));
		auto textCtrl7=settings.NodeBorderWidth(settingsPopup, [](){s_dataViewPtr->loadSettings();});


		wxBoxSizer* hbox7 = new wxBoxSizer(wxHORIZONTAL);
		hbox7->Add(st7, 0, wxCENTER | wxRIGHT, 10);
		hbox7->Add(textCtrl7, 0, wxEXPAND);

		wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
		hbox1->Add(st1, 0, wxCENTER | wxRIGHT, 10);
		hbox1->Add(textCtrl1, 0, wxEXPAND);
		
		wxBoxSizer* hbox5 = new wxBoxSizer(wxHORIZONTAL);
		hbox5->Add(st5, 0, wxCENTER | wxRIGHT, 10);
		hbox5->Add(textCtrl5, 0);
		
		wxBoxSizer* hbox6 = new wxBoxSizer(wxHORIZONTAL);
		hbox6->Add(st6, 0, wxCENTER | wxRIGHT, 10);
		hbox6->Add(textCtrl6, 0);

		wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
		hbox2->Add(st2, 0, wxCENTER | wxRIGHT, 10);
		hbox2->Add(textCtrl2, 0);
		
		wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
		hbox3->Add(st3, 0, wxCENTER | wxRIGHT, 10);
		hbox3->Add(textCtrl3, 0);

		wxBoxSizer* hbox4 = new wxBoxSizer(wxHORIZONTAL);
		hbox4->Add(st4, 0, wxCENTER | wxRIGHT, 10);
		hbox4->Add(textCtrl4, 0);

		wxBoxSizer* vBox = new wxBoxSizer(wxVERTICAL);
		//vBox->SetMinSize(400, 10);
		vBox->Add(hbox7, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox1, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox5, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox6, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox2, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox3, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox4, 0, wxALIGN_RIGHT);

		settingsPopup->setSizer(vBox);
	}
	settingsPopup->Popup();
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnQuit(wxCommandEvent & event)
{
	if(!FileManager::isSessionSaved()){
		FileManager::cleanUp();
	}

	Close(true);
}

//----------------------------------------------------------------------

void DuplicateImgGUI::OnClose(wxCloseEvent &event)
{
	m_worker->terminate();
	event.Skip();
}

//====================================================================
