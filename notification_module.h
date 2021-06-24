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
	private:
		static NotificationModule* instance;
		wxBoxSizer* m_hbox;
		wxStaticText* m_filePathText;
		wxStaticText* m_rankText;
		NotificationModule(wxPanel* parent);

	public:
		static NotificationModule* init(wxPanel* parent);
		static void displayData(const wxString& m_file, float rank);
		void setLabels(const wxString& m_file, float rank);

};

#endif
