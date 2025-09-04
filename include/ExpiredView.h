#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/dialog.h>


#include "DataModel.h"
#include <set>

namespace pof {
	class ExpiredView : public wxDialog {
		wxAuiToolBar* ToolBar;
		wxSearchCtrl* SearchCtrl;
		wxAuiToolBarItem* m_tool1;
		wxPanel* m_panel1;
		wxDataViewCtrl* SearchData;
		wxDataViewColumn* ProductIdCol;
		wxDataViewColumn* ProductName;
		wxDataViewColumn* PriceCol;
		wxDataViewColumn* expiredDateCol;
		wxPanel* mEmpty = nullptr;
		wxStaticText* mEmptyStr = nullptr;
		wxStaticText* mInfoStr = nullptr;
		wxSimplebook* mBook = nullptr;
		wxDatePickerCtrl* mDatePicker;
		std::shared_ptr<pof::base::data> mdata;
		pof::DataModel* mModel;
		wxAuiToolBarItem* mInfoItem;
	public:
		enum {
			ID_DATE,
			ID_RESET,
			ID_VIEW,
			ID_MOVE_EXPIRED,
		};

		ExpiredView(wxWindow* parent, wxWindowID id = wxID_ANY,
			std::shared_ptr<pof::base::data> data = nullptr,
			const wxString& title = wxT("Expired Products"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1013, 523), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
		virtual ~ExpiredView();
		wxAuiManager m_mgr;
		void CreateToolBar();
		void CreateView();
		void CreateViewManagerDefaultArt();
		void CreateEmptyPanel(const std::string& text = {});
		void ShowEmpty(const std::string& text);

		void LoadExpiredProducts();
		void OnDateChanged(wxDateEvent& evt);
		void OnReset(wxCommandEvent& evt);
		void OnMarkAsExpired(wxCommandEvent& evt);
		void OnContextMenu(wxDataViewEvent& evt);
		void UpdateText(const std::string& str);

		DECLARE_EVENT_TABLE()
	};
};
