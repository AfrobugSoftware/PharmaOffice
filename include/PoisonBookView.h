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
#include "SearchProduct.h"


namespace pof{
	class PoisonBookView : public wxPanel {
	public:
		enum {
			EMPTY = 0,
			THUMBNAIL_SELECT,
			BOOK_VIEW,
		};

		enum {
			ID_OPEN_BOOK = wxID_HIGHEST + 10,
			ID_CLEAR_TIMER,
			ID_BOOK,
			ID_THUMBNAIL,
			ID_BOOKLIST,
			ID_TOOLBAR,
			ID_ADD_PRODUCT,
			ID_SEARCH,
			ID_DATE_PICKER,
			ID_BOOKDATA,
			ID_VERIFY,
		};

		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		PoisonBookView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL | wxNO_BORDER);
		virtual ~PoisonBookView();

		void CreateToolBars();
		void CreateBookToolBar();
		void CreateViews();
		void CreateDataView();
		void CreateSpecialCols();
		void CreateEmptyBookPane();

		void LoadBooks();
		void LoadBookValues();
		void SetupAuiTheme();
		void SwitchToolBar();
		void LoadProductName(const std::string& name);
	private:
		void OnBookActivated(wxListEvent& evt);
		void OnBookSelected(ThumbnailEvent& evt);
		void OnAddProduct(wxCommandEvent& evt);
		void OnSearch(wxCommandEvent& evt);
		void OnSearchCleared(wxCommandEvent& evt);
		void OnAuiThemeChange();
		void OnBack(wxCommandEvent& evt);
		void OnDateChanged(wxDateEvent& evt);

		void OnContextMenu(wxDataViewEvent& evt);

		wxAuiManager mManager;
		
		wxPanel* mEmpty = nullptr;
		ThumbnailCtrl* mThumbCtrl = nullptr;
		wxListCtrl* mBookList = nullptr;
		wxDataViewCtrl* mBookData = nullptr;
		wxSimplebook* mBook = nullptr;
		wxTimer mClearTimer;
		wxAuiToolBar* mToolbar = nullptr;
		wxAuiToolBar* mBookbar = nullptr;
		wxSearchCtrl* mSearchbar = nullptr;
		wxDatePickerCtrl* mDateCtrl = nullptr;
		wxInfoBar* mInfoBar = nullptr;
		wxStaticText* mProductName = nullptr;
		wxPanel* mTextPanel = nullptr;

		DECLARE_EVENT_TABLE()
	};
}