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
#ifndef _DATA_VISUALZATION_H
#define _DATA_VISUALZATION_H
#include "file_manager.h"

#include <wx/wx.h>
#include <wx/webview.h>

//====================================================================

class DataView : public wxPanel
{
	public:
		DataView(wxWindow* parent, const wxSize& size=wxDefaultSize);

		virtual ~DataView();

		void SetSize(int w, int h);
		void updateData(const char* fileName, bool newNode);
		void reset();
		void loadSettings();
		void selectNode(const std::string& filePath);
		void removeNode(const std::string& filePath);
		void unselectNode();
		void transferCompleted();
		void startAnimation();

	private:
		wxWebView* m_webViewPtr;
		size_t m_currentNode;
		uint m_nodeCounter;
		uint m_edgeCounter;
		bool m_isNew;
		
		void enableDebug();
		
		uint addFile(const std::string& fileName);

		void addNode(int id, const char* path);
		void addEdge(int from, int to);
		void addEdge(int id1, int id2, bool a);
};

//--------------------------------------------------------------------

inline DataView::~DataView()
{
	wxDELETE(m_webViewPtr);
}

//--------------------------------------------------------------------

inline void DataView::enableDebug()
{
	#ifdef DEBUG
	m_webViewPtr->EnableAccessToDevTools();
	#endif
}

//--------------------------------------------------------------------

inline void DataView::SetSize(int w, int h)
{
	wxPanel::SetMinSize(wxSize(w, h));
	wxPanel::SetSize(wxSize(w, h));
	m_webViewPtr->SetMinSize(wxSize(w, h));
	m_webViewPtr->SetSize(wxSize(w, h));			
}

//--------------------------------------------------------------------

inline void DataView::reset()
{
	m_webViewPtr->RunScript("interfaceMod.reset();");
	FileManager::resetFilesMap();
	m_currentNode=0;
	m_nodeCounter=0;
	m_edgeCounter=0;
}

//--------------------------------------------------------------------

inline void DataView::selectNode(const std::string& filePath)
{
	uint t=FileManager::findKey(filePath);
	if(t>0){
		wxString jsCode=wxString::Format("interfaceMod.focusOnNode(%d);", t);
		m_webViewPtr->RunScript(jsCode);
	}
}

//--------------------------------------------------------------------

inline void DataView::removeNode(const std::string& filePath)
{
	uint t=FileManager::findKey(filePath);
	if(t>0){
		wxString jsCode=wxString::Format("interfaceMod.removeNode(%d);", t);
		m_webViewPtr->RunScript(jsCode);
	}
}

//--------------------------------------------------------------------

inline void DataView::transferCompleted()
{
	wxString jsCode=wxString::Format("interfaceMod.transferDone(%d);", m_nodeCounter);
	m_webViewPtr->RunScript(jsCode);
}

//--------------------------------------------------------------------

inline void DataView::startAnimation()
{
	wxString jsCode="AnimatedShow.animatedShow();";
	m_webViewPtr->RunScript(jsCode);
}

//--------------------------------------------------------------------

inline void DataView::unselectNode()
{
	wxString jsCode="interfaceMod.unselectNode();";
	m_webViewPtr->RunScript(jsCode);
}

//--------------------------------------------------------------------

inline void DataView::updateData(const char* fileName, bool newNode)
{
	size_t id=addFile(fileName);
	if(m_isNew){
		addNode(id, fileName);
	}

	if(newNode){
		m_currentNode=id;
	}
	else{
		addEdge(m_currentNode, id, m_currentNode>id);
	}
}

//--------------------------------------------------------------------

inline void DataView::addNode(int id, const char* path)
{
	wxString jsCode=wxString::Format("interfaceMod.addNode({id:%d, shape:\"image\", image:\"%s\", title:\"%s\"});", id, path, path);
	m_webViewPtr->RunScript(jsCode);
}

//--------------------------------------------------------------------

inline void DataView::addEdge(int from, int to)
{
	wxString jsCode=wxString::Format("interfaceMod.addEdge({id: %d, from:%d, to:%d});", ++m_edgeCounter, from, to);
	m_webViewPtr->RunScript(jsCode);
}

//--------------------------------------------------------------------

inline void DataView::addEdge(int id1, int id2, bool a)
{
	if(a){
		addEdge(id1, id2);
		return;
	}

	addEdge(id2, id1);
}

//====================================================================

#endif
