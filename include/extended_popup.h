/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class BasicPopup                                                   *
* class PopupWrapper                                                 *
* class WX_Popup                                                     *
* class ExtendedPopup                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _EXTENDED_POPUP_H
#define _EXTENDED_POPUP_H
#include <wx/sizer.h>
#include <wx/frame.h>

//====================================================================
/*
 * Basic popup frame: Popup(), Dismiss() also when it is close
 * it is Dismissed instead of destroyed. Additionally, it can be set an
 * function to be execute when the window is close.
 * */
class BasicPopup : public wxFrame
{
	public:
		BasicPopup(wxWindow* parent, const char* title, bool allowDestroy=false);

		virtual ~BasicPopup()=default;

		virtual void Popup()
		{
			DoPopupWindow();
			OnPopup();
			Raise();
			Show();
		}
		 
		virtual void Dismiss()
		{
			Hide();
			if(m_destroy){
				Close();
			}
		}

		void setOnClose(std::function<void(void)> onClose)
		{
			m_onClose=onClose;
		}

		template<typename Widget, typename... Args>
		Widget* builder(Args... args)
		{
			return new Widget(this, std::forward<Args>(args)...);
		}

		void setSizer(wxBoxSizer* box, bool deleteOld=true);

	protected:
		std::function<void(void)> m_onClose{nullptr};

		/* This is called when the window popup.
		 * */
		virtual void OnPopup()
		{}

	private:
		bool m_destroy;
		void DoPopupWindow(const wxPoint& pos);
		void DoPopupWindow();

};

//--------------------------------------------------------------------

inline void BasicPopup::setSizer(wxBoxSizer* box, bool deleteOld)
{
	wxBoxSizer* boxWrapper = new wxBoxSizer(wxHORIZONTAL);
	boxWrapper->Add(box, 0, wxALL, FromDIP(10));
	this->SetSizerAndFit(boxWrapper, deleteOld);
}

//====================================================================
/*
 * Replicate functionality of wxPopupTransientWindow except
 * wxPopupTransientWindow::ProcessLeftDown which is not implemented
 * */
class WX_Popup : public BasicPopup 
{
	public:
		WX_Popup(wxWindow *parent, const char* title, bool allowDestroy=false);

		virtual ~WX_Popup()=default;

	protected:
		/* This is called when the popup is disappeared because of anything
		 * else but direct call to Dismiss(). 
		 * */
		virtual void OnDismiss(){}

	private:
		bool isMouseHover();
};

//====================================================================
//====================================================================

class ExtendedPopup : public WX_Popup
{
	public:
		ExtendedPopup(wxWindow* parent, const char* title, bool allowDestroy=false)
		:WX_Popup(parent, title, allowDestroy)
		, m_onPopup(nullptr)
		, m_onDismiss(nullptr)
		{
		}

		virtual ~ExtendedPopup()=default;

		void setOnPopupCallback(std::function<void(wxWindow* popup)> onPopup)
		{
			m_onPopup=onPopup;
		}

		void setOnDismissCallback(std::function<void(wxWindow* popup)> onDismiss)
		{
			m_onDismiss=onDismiss;
		}

	protected:
		virtual void OnPopup() override;
		virtual void OnDismiss() override;

	private:
		std::function<void(wxWindow* popup)> m_onPopup;
		std::function<void(wxWindow* popup)> m_onDismiss;
};

//--------------------------------------------------------------------

inline void ExtendedPopup::OnPopup()
{
	if(m_onPopup){
		m_onPopup(this);
	}
}

//--------------------------------------------------------------------

inline void ExtendedPopup::OnDismiss()
{
	if(m_onDismiss){
		m_onDismiss(this);
	}
}

//====================================================================

#endif
