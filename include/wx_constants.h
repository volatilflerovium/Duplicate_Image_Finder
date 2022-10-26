#ifndef WX_CONSTANTS_H
#define WX_CONSTANTS_H
#include <wx/wx.h>
#include <wx/settings.h>


namespace WX{
	enum 
	{
		BROWSE=101,
		PICTURE_SCROLLER,
		PICTURE_VIWER,
		ADD_PIC,
		CLEAR,
		START,
		CANCEL,
		DELETE_IMG,
		OPEN_IMG,
		SLIDER_ID,
		RESET,
	};

	const int MIN_WIDTH=800;
	const int MIN_HEIGHT=800;
};

const int c_windowPadding=10;
const int c_picPadding=5;
const int c_scrollerWidth=130;
const int c_scrollerMinHeight=300;


const wxString c_separatorColour("#678fff");

#endif
