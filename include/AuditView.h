#pragma once
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/aui/aui.h>
#include <wx/infobar.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>


#include "Application.h"

#include <set>
#include <algorithm>
#include <ranges>
#include <utility>
#include <queue>

namespace pof
{
	class AuditView : public wxPanel {
		enum {
			ID_DATA_VIEW = wxID_HIGHEST + 10,
			ID_INFOBAR,
			ID_FILTER_TYPE,
			ID_COLOUR_TYPE,
		};
		using range_t = std::pair<size_t, size_t>;
		AuditView(wxWindow* parent, wxWindowID id, const wxPoint& position = wxDefaultPosition,
			const wxSize& size = wxDefaultSize);

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

		void ShowAdditionalInfoPopup();
	private:
		std::set<wxDataViewItem> mSelections;
		std::queue<range_t> mPageRanges;


		wxAuiManager mAuiManager;
		
		wxComboBox* mFilterType = nullptr;
		wxStaticText* mPageNoInfo = nullptr;
		wxAuiToolBar* mToolBar = nullptr;
		wxAuiToolBarItem* mNext = nullptr;
		wxAuiToolBarItem* mBack = nullptr;
		wxInfoBar* mInfoBar = nullptr;
		wxPopupTransientWindow* mAdditionalInfoPopup = nullptr;
		wxDataViewCtrl* mDataView = nullptr;

		DECLARE_EVENT_TABLE()
	};
};