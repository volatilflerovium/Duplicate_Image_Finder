/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class DataView                                  				         *
*                                                                    *
* Version: 1.0                                                       *
* Date:    20-03-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "data_visualization.h"
#include "settings_manager.h"
#include "event_definitions.h"

//====================================================================

DataView::DataView(wxWindow* parent, const wxSize& size)
:wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
, m_currentNode(0)
, m_nodeCounter(0)
, m_edgeCounter(0)
{
	m_webViewPtr=wxWebView::New(this, wxID_ANY, FileManager::dataVisualizationURL().c_str(), wxDefaultPosition, size);

	enableDebug();
	
	m_webViewPtr->AddScriptMessageHandler("wx_msg");
	
	m_webViewPtr->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, [this](wxWebViewEvent& evt) {
		wxCommandEvent event(wxEVT_CUSTOM_EVENT);
		if(evt.GetString()=="OK"){
			event.SetId(EvtID::ANIMATION_READY);
		}
		else{
			event.SetId(EvtID::FIND_IMG);
			event.SetString(evt.GetString());
		}
		//wxPostEvent(this, event);
		GetEventHandler()->AddPendingEvent(event);
	});
}

//--------------------------------------------------------------------

uint DataView::addFile(const std::string& fileName)
{
	const uint t=FileManager::findKey(fileName, 1+m_nodeCounter);
	m_isNew=(t==1+m_nodeCounter);
	m_nodeCounter+=m_isNew;

	wxCommandEvent event(wxEVT_CUSTOM_EVENT, EvtID::NODE_COUNT);
	event.SetString(wxString::Format(wxT("%d"), m_nodeCounter));
	GetEventHandler()->AddPendingEvent(event);

	return t;
}		

//--------------------------------------------------------------------

void DataView::loadSettings()
{
	wxString jsCode=wxString::Format("interfaceMod.setSettings(%d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\");", 
		SettingsManager::getNodeBorderWidth(),
		SettingsManager::getNodeBorderColor(),
		SettingsManager::getNodeBkgColor(),
		SettingsManager::getNodeSelectedColor(),
		SettingsManager::getNodeFontColor(), 
		SettingsManager::getEdgesColor(), 
		SettingsManager::getCanvasColor());
	 
	m_webViewPtr->RunScript(jsCode);
}
