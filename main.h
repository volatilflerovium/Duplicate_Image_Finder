#include <wx/wx.h>
#include <iostream>
#include "duplicate_img_gui.h"

class MyApp : public wxApp
{
	DuplicateImgGUI* m_simple;
  public:
		virtual bool OnInit();
		~MyApp(){
			//wxDELETE(m_simple);
		}
};
