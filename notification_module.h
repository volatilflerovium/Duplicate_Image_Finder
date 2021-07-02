/*********************************************************************
* NotifyLabel class                                   			   *
*                                                                    *
* Version: 1.0                                                       *
* Date:    24-06-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef NOTIFICATION_MODULE_H
#define NOTIFICATION_MODULE_H


#include <wx/wx.h>

class NotificationModule : public wxPanel
{
	public:
		static NotificationModule* init(wxPanel* parent);
		static void displayData(const wxString& m_file, float rank);
		static void Clear();
		void clear();
		void setLabels(const wxString& m_file, float rank);
		~NotificationModule();
		
	private:
		static NotificationModule* m_instance;
		wxStaticText* m_filePathText;
		wxStaticText* m_rankText;
		NotificationModule(wxPanel* parent);
		void onResize(wxSizeEvent& evt);

};

#endif
