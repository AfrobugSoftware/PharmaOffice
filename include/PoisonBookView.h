#pragma once
#include <wx/artprov.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/srchctrl.h>
#include <wx/aui/auibar.h>
#include <wx/aui/aui.h>

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/simplebook.h>
#include <wx/timer.h>
#include <wx/splitter.h>
#include <wx/statline.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/timer.h>

#include "PoisonBookManager.h"
#include "controls/ThumbnailCtrl.h"


namespace pof{
	class PoisonBookView : wxPanel {
	public:
		enum {
			ID_OPEN_BOOK = wxID_HIGHEST + 10,

		};

		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		PoisonBookView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL | wxNO_BORDER);
		~PoisonBookView();

		void CreateToolBars();
		void CreateViews();
		void CreateSpecialCols();


	private:
		wxAuiManager mManager;
		ThumbnailCtrl* mThumbCtrl = nullptr;
		wxDataViewCtrl* mBookData = nullptr;
		wxSimplebook* mBook = nullptr;

	};
}