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
#include "opencv_utilities.h"

#include <wx/dir.h>
#include <wx/icon.h>

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

//--------------------------------------------------------------------

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
	auto icon=wxIcon();
	icon.LoadFile(FileManager::iconsPath().append("/wxduplicatedimageapp.png"));
	SetIcon(icon);

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
		vBox->Add(m_directories, 0, wxEXPAND | wxBOTTOM, 10);
		vBox->Add(startBtn, 0, wxALIGN_RIGHT);

		m_directoriesPopup->setSizer(vBox);
	}

	m_addDirPopupBtn=new wxButton(this, wxID_ANY, wxT("Add Directories"));

	m_addDirPopupBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event){
		m_directoriesPopup->Popup();
	});

	//----------------------------------------------
	m_blurFilter=new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
										wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 1);

	ArrayStringType maskMode(4, "");
	maskMode[ImageProcessor::MaskMode::RectD]="mode 4";//"rect-D";
	maskMode[ImageProcessor::MaskMode::RectF]="mode 3";//"rect-F";
	maskMode[ImageProcessor::MaskMode::HashD]="mode 2";//"hash-D";
	maskMode[ImageProcessor::MaskMode::HashF]="mode 1";//"hash-F";

	m_maskMode=new wxChoice(this, wxID_ANY, wxDefaultPosition,
											wxDefaultSize, maskMode);
	m_maskMode->SetSelection(ImageProcessor::MaskMode::HashF);

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
		wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);
		vbox0->Add(m_totalFilesText, 1, wxALIGN_CENTER);
		
		wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	
		hbox->Add(settingsBtn, 0, wxRIGHT, 10);
		hbox->Add(m_playStopAnimationBtn, 0, wxRIGHT, 10);
		hbox->Add(-1, 1, wxEXPAND);
		hbox->Add(m_rebootBtn, 0, wxRIGHT, 10);
		hbox->Add(m_addDirPopupBtn, 0, wxRIGHT, 10);
		hbox->Add(m_staticFiles, 0, wxRIGHT | wxALIGN_CENTER, 10);		
		//hbox->Add(m_totalFilesText, 0, wxRIGHT | wxALIGN_CENTER, 10);
		hbox->Add(vbox0, 1, wxRIGHT | wxALIGN_BOTTOM, 10);

		hbox->Add(m_maskMode, 0, wxRIGHT, 10);
		hbox->Add(m_blurFilter, 0, wxRIGHT, 10);
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

	vBox->Add(m_board, 0, wxEXPAND | wxALL, c_windowPadding);

	Bind(wxEVT_CUSTOM_EVENT, [](wxCommandEvent& event){
		NotificationModule::setNodes(event.GetString());
	}, EvtID::NODE_COUNT);

	//-------------------------------------------------
	Bind(wxEVT_CUSTOM_EVENT, [this](wxCommandEvent& event){
		m_playStopAnimationBtn->Enable(true);
	}, EvtID::ANIMATION_READY);
	//-------------------------------------------------

	setEnable(STATUS::INITIAL);

	this->SetSizerAndFit(vBox);
	Maximize(true);
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
	if(p>0){
		m_dirDialog->SetPath(selectedDir.substr(0, p));
	}
	else{
		m_dirDialog->SetPath(selectedDir);
	}
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

void DuplicateImgGUI::setEnable(int status)
{
	m_rebootBtn->Enable(BTNS_ID::REBOOT  & status);
	m_addDirPopupBtn->Enable(BTNS_ID::ADD_DIRECTORY & status);
	
	m_blurFilter->Enable(BTNS_ID::SENSITIVITY_LEVEL & status);
	m_maskMode->Enable(BTNS_ID::SENSITIVITY_LEVEL & status);
	m_sensitivityChoice->Enable(BTNS_ID::SENSITIVITY_LEVEL & status);
	m_undoBtn->Enable(BTNS_ID::UNDO & status);
	m_runCancelBtn->Enable(BTNS_ID::CANCEL & status);
	m_playStopAnimationBtn->Enable(BTNS_ID::PLAY_STOP & status);
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

	setEnable(static_cast<int>(m_status));

	if(m_status==STATUS::INITIAL){
		m_runCancelBtn->SetBitmap(m_runBitmapBundle);
	}
	else if(m_status==STATUS::LOADED){
		m_runCancelBtn->SetBitmap(m_runBitmapBundle);
	}
	else if(m_status==STATUS::PROCESSING){
		m_runCancelBtn->SetBitmap(m_cancellBitmapBundle);
	}
	else if(m_status==STATUS::READY){ //processing has finished
		m_runCancelBtn->SetBitmap(m_runBitmapBundle);
		s_dataViewPtr->transferCompleted();
	}
	else if(m_status==STATUS::CANCELLED){
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
		ImageProcessor::setFilterSize(m_blurFilter->GetValue());
		ImageProcessor::setMaskMode(static_cast<ImageProcessor::MaskMode>(m_maskMode->GetSelection()));
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
	auto textCtrlBuilder=[](wxWindow* parent, const wxString& val, std::function<void(const wxString&)> cbk)
	{
		wxTextCtrl* ctrl=new wxTextCtrl(parent, wxID_ANY, val, 
								wxDefaultPosition, wxSize(120, -1), wxTE_PROCESS_ENTER);

		ctrl->Bind(wxEVT_TEXT_ENTER, [cbk, ctrl](wxCommandEvent& event){			
			cbk(ctrl->GetValue());
			s_dataViewPtr->loadSettings();
		});
		
		return ctrl;
	};
	
	auto settingsPopup=new ExtendedPopup(this, "Settings", true);
	{
		wxStaticText* st1 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node border color: "));
		auto textCtrl1=textCtrlBuilder(settingsPopup, SettingsManager::getNodeBorderColor(), SettingsManager::setNodeBorderColor);

		wxStaticText* st2 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node font color: "));
		auto textCtrl2=textCtrlBuilder(settingsPopup, SettingsManager::getNodeFontColor(), SettingsManager::setNodeFontColor);

		wxStaticText* st3 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Edge color: "));
		auto textCtrl3=textCtrlBuilder(settingsPopup, SettingsManager::getEdgesColor(), SettingsManager::setEdgesColor);

		//canvas color
		wxStaticText* st4 =settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Background color: "));
		auto textCtrl4=textCtrlBuilder(settingsPopup, SettingsManager::getCanvasColor(), SettingsManager::setCanvasColor);

		wxStaticText* st5=settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node background color: "));
		auto textCtrl5=textCtrlBuilder(settingsPopup, SettingsManager::getNodeBkgColor(), SettingsManager::setNodeBkgColor);

		wxStaticText* st6=settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node selected color: "));
		auto textCtrl6=textCtrlBuilder(settingsPopup, SettingsManager::getNodeSelectedColor(), SettingsManager::setNodeSelectedColor);

		wxStaticText* st7=settingsPopup->builder<wxStaticText>(wxID_ANY, wxT("Node width: "));
		auto textCtrl7=textCtrlBuilder(settingsPopup, wxString::Format("%d", SettingsManager::getNodeBorderWidth()), [](const wxString& val){
			SettingsManager::setNodeBorderWidth(wxAtoi(val));
		});

		wxCheckBox* checkBox=nullptr;

		if(isAppInstalled()){
			checkBox=settingsPopup->builder<wxCheckBox>(wxID_ANY, wxT("Installed"),
					wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			checkBox->SetValue(true);
			checkBox->Disable();
		}
		else{
			checkBox=settingsPopup->builder<wxCheckBox>(wxID_ANY, wxT("Install"),
					wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			checkBox->Bind(wxEVT_CHECKBOX, [this, settingsPopup](wxCommandEvent& event){
				settingsPopup->Dismiss();
				autoInstall();
			});
		}

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

		wxBoxSizer* hbox8 = new wxBoxSizer(wxHORIZONTAL);
		hbox8->Add(checkBox, 0, wxCENTER | wxLEFT, 10);

		wxBoxSizer* vBox = new wxBoxSizer(wxVERTICAL);
		//vBox->SetMinSize(400, 10);
		vBox->Add(hbox7, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox1, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox5, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox6, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox2, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox3, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox4, 0, wxALIGN_RIGHT | wxBOTTOM, 10);
		vBox->Add(hbox8, 0, wxALIGN_RIGHT);

		settingsPopup->setSizer(vBox);
	}
	settingsPopup->Popup();
}

//----------------------------------------------------------------------

void DuplicateImgGUI::autoInstall()
{
	#ifndef DEBUG
		const wxString desktopEntryFile=wxString::Format("%s/.local/share/applications/wxDuplicatedImageApp.desktop", getenv("HOME")); 
		const wxString appInstallationDir=wxString::Format("%s/bin/appimages/wxDuplicatedImageApp", getenv("HOME")); 
	#else
		const wxString desktopEntryFile="/tmp/wxduplicatedimageapp.desktop";
		const wxString appInstallationDir="/tmp/bin/wxDuplicatedImageApp";
	#endif

	#ifndef DEBUG
	
	auto saveDataDialog=wxMessageDialog(
		this,
		wxT("Do you want to create a desktop file entry?"),
		wxT("Install"),
		wxYES_NO|wxCENTRE|wxICON_WARNING
	);

	int response=saveDataDialog.ShowModal();
	if(wxID_YES==response){
		std::string appImagePath=getenv("APPIMAGE");
		std::string appimageName=appImagePath.substr(appImagePath.find_last_of("/")+1);

		std::error_code ec;
		if(!std::filesystem::exists(std::string(appInstallationDir), ec)){
			std::filesystem::create_directories(std::string(appInstallationDir), ec);
		}

		const char* command="[Desktop Entry]\n\
Name=wxDuplicated Image Finder\n\
Comment=Graphical tool for find duplicated and simillar images in a collection.\n\
Terminal=false\n\
Type=Application\n\
Exec=%s/%s\n\
Icon=%s/wxduplicatedimageapp.png\n\
Categories=Graphics;";

		wxString fileContent=wxString::Format(command, appInstallationDir, appimageName, appInstallationDir);

		std::fstream fileStream(desktopEntryFile.mb_str(), std::ios::out | std::ios::trunc);
		if(fileStream.is_open()){
			fileStream<<fileContent;
			fileStream.close();
		}

		std::string srcFilePath=std::string(wxString::Format("%s/%s", getenv("OWD"), appimageName).mb_str());
		std::string dstFilePath=std::string(wxString::Format("%s/%s", appInstallationDir, appimageName).mb_str());

		std::filesystem::rename(srcFilePath, dstFilePath, ec);
		
		srcFilePath=std::string(wxString::Format("%s/wxduplicatedimageapp.png", getenv("APPDIR")).mb_str());
		dstFilePath=std::string(wxString::Format("%s/wxduplicatedimageapp.png", appInstallationDir).mb_str());
		std::filesystem::copy_file(srcFilePath, dstFilePath, std::filesystem::copy_options::overwrite_existing, ec);
	}

	#endif
}

//----------------------------------------------------------------------

bool DuplicateImgGUI::isAppInstalled()
{
	#ifndef DEBUG
		const std::string desktopEntryFile=std::string(wxString::Format("%s/.local/share/applications/wxDuplicatedImageApp.desktop", getenv("HOME")).mb_str()); 

		std::error_code ec;
		if(std::filesystem::exists(desktopEntryFile, ec)){
			const wxString appInstallationDir=wxString::Format("%s/bin/appimages/wxDuplicatedImageApp", getenv("HOME")); 

			std::string appImagePath=getenv("APPIMAGE");
			std::string appimageName=appImagePath.substr(appImagePath.find_last_of("/")+1);

			std::string appFile=std::string(wxString::Format("%s/%s", appInstallationDir, appimageName).mb_str());
			if(std::filesystem::exists(appFile, ec)){
				return true;
			}
		}
		return false;

	#endif
	return true;
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
