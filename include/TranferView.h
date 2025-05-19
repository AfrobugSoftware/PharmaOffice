#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/simplebook.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/statbmp.h>

#include "SearchProduct.h"
#include <memory>


namespace pof {
	class TransferView : public wxDialog
	{
	private:
		wxPanel* m_panel1;
		wxAuiToolBar* mTopTools;
		wxAuiToolBarItem* m_tool11;
		//wxPanel* m_panel2;
		wxDataViewCtrl* mOrderView;
		wxDataViewColumn* mProductCol;
		wxDataViewColumn* mQuanCol;
		wxDataViewColumn* m_dataViewColumn3;
		wxDataViewColumn* m_dataViewColumn4;
		wxDataViewColumn* m_dataViewColumn5;
		wxPanel* m_panel3;
		wxPanel* m_panel4;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxSimplebook* mBook;
		wxPanel* mEmpty;
		wxDataViewColumn* mSelectCol = nullptr;
		constexpr static size_t selcol = 1000;
		std::set<wxDataViewItem> mSelections;
		std::unique_ptr<pof::DataModel> model;
		wxDatePickerCtrl* mDatePicker;
		std::chrono::system_clock::time_point mSelectDay;
		pof::base::currency mTotalAmount;

	public:
		enum {
			ID_VIEW = wxID_HIGHEST + 50,
			ID_SELECT,
			ID_UPDATE_TRANSFER,
			ID_REMOVE_TRANSFER,
			ID_ADD_TRANSFER,
			ID_DOWNLOAD_EXCEL,
			ID_DATE_SELECT,
		};
		TransferView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Order List"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(878, 689), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
		virtual ~TransferView();
		void CreateEmptyPanel();
		void CreateSpeicalCol();
		void LoadTransfer();


		void OnAddTransfer(wxCommandEvent& evt);
		void OnRemoveTransfer(wxCommandEvent& evt);
		void OnUpdateTransfer(wxCommandEvent& evt);
		void OnDownloadExcel(wxCommandEvent& evt);
		void OnSelect(wxAuiToolBarEvent& evt);
		void OnDateChanged(wxDateEvent& evt);
		void OnUpdateUi(wxUpdateUIEvent& evt);
		void OnHeaderClick(wxDataViewEvent& evt);

		void ShowSelect();
		void HideSelect();

		void UpdateTexts();
		DECLARE_EVENT_TABLE()
	};
};