#pragma once
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/aui/aui.h>
#include <wx/infobar.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>
#include <wx/combo.h>
#include <wx/button.h>
#include <wx/itemattr.h>
#include <wx/choice.h>


#include <set>
#include <algorithm>
#include <ranges>
#include <utility>
#include <queue>
#include <stack>

namespace pof
{
	class AuditView : public wxPanel {
	public:
		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		enum {
			ID_DATA_VIEW = wxID_HIGHEST + 10,
			ID_INFOBAR,
			ID_FILTER_TYPE,
			ID_COLOUR_TYPE,
			ID_DOWNLOAD_EXCEL,
		};
		using range_t = std::pair<size_t, size_t>;
		AuditView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
			const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxTAB_TRAVERSAL);
		~AuditView();

		void CreateToolBar();
		void CreateDataView();
		void CreatePopupMenu();
		void CreatePopupAdditionalInformation();

		void OnCacheHint(wxDataViewEvent& evt);
		void OnItemActivated(wxDataViewEvent& evt);

		void OnBackPage(wxCommandEvent& evt);
		void OnNextPage(wxCommandEvent& evt);
		void OnFilterSelected(wxCommandEvent& evt);
		void OnApplyFilter(wxCommandEvent& evt);
		void OnColourAuditType(wxCommandEvent& evt);
		void OnDownloadExcel(wxCommandEvent& evt);
		void OnAuiThemeChange();


		void ShowAdditionalInfoPopup();

		void SetupAuiTheme();
		void SetupDataViewStyle();

		inline void SetHeaderAttr(const wxItemAttr& attr) { mHeaderAttr = attr; }
		inline void SetDataViewRowHeight(size_t rowHeight) { mRowHeights = rowHeight; }
		inline void SetDataViewFont(const wxFont& font) { mDataViewFont = font; }

	private:
		std::set<wxDataViewItem> mSelections;
		
		size_t mRowHeights;
		wxItemAttr mHeaderAttr;
		wxFont mDataViewFont;

		//vector and current position into the range
		std::stack<range_t> mPageRanges;


		wxAuiManager mAuiManager;
		
		wxStaticText* mPageNoInfo = nullptr;
		wxChoice* mFilterType = nullptr;
		wxAuiToolBar* mToolBar = nullptr;
		wxAuiToolBarItem* mNext = nullptr;
		wxAuiToolBarItem* mBack = nullptr;
		wxInfoBar* mInfoBar = nullptr;
		wxPopupTransientWindow* mAdditionalInfoPopup = nullptr;
		wxDataViewCtrl* mDataView = nullptr;
		bool bFilterType = false;
		pof::AuditManager::auditType mCurType;
		DECLARE_EVENT_TABLE()
	};
};