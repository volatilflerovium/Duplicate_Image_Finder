/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* Some custom event definitions                                      *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef EVENT_DEFINITIONS_H
#define EVENT_DEFINITIONS_H

#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_CUSTOM_EVENT, wxCommandEvent);


enum EvtID
{
	ID=2500,
	LEFT_CLICK_IMAGE,
	RIGHT_CLICK_IMAGE,
	DELETE_IMG,
	REBUILD_HIST,
};

inline void postEvent(wxEvtHandler* h, wxEventType commandEventType, int id)
{
	wxCommandEvent event(commandEventType, id);
	wxPostEvent(h, event);
}

#endif
