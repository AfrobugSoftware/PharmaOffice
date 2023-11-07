#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
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

#include <wx/collheaderctrl.h>
#include <wx/collpane.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/simplebook.h>
#include <wx/timer.h>
#include <wx/splitter.h>
#include <wx/statline.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>

#include "PackView.h"
#include "PatientManager.h"
#include "DropTarget.h"
#include "database.h"
#include "AddPatient.h"
#include <ranges>
#include <unordered_map>
#include <numeric>




namespace pof {
	class PatientView : public wxPanel
	{
	public:
		//pages
		constexpr const static size_t SELECTION_COL = 30456;
		enum : std::uint8_t {
			PATIENT_SELECT = 0,
			PATIENT_VIEW,
		};

		enum {
			ID_SEARCH_BAR = wxID_HIGHEST,
			ID_PATIENT_VIEW,
			ID_PATIENT_PANEL,
			ID_PATIENT_MEDS_VIEW,
			ID_PATIENT_HISTORY_VIEW,
			ID_MED_DETAILS,
			ID_ADD_PRODUCT,
			ID_REMOVE_PRODUCT,
			ID_ADD_PACK,
			ID_STOP_PRODUCT,
			ID_BOOK,
			ID_TOP_TOOLS,
			ID_PATIENT_TOOLS,
			ID_ADD_PATIENTS,
			ID_SHOW_PATIENT_DETAILS,
			ID_PATIENT_MED_DETAILS,
			ID_REMOVE_PATIENTS,
			ID_SELECT,
			ID_SELECT_MED,
			ID_SEARCH,
			ID_SEARCH_BY_NAME,
			ID_SEARCH_BY_LAST_NAME,
		};
		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		PatientView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL);
		~PatientView();
		void CreateToolBars();
		void CreateViews();
		void CreatePatientSummaryPopUp();
		void CreatePatientDetailsPane();
		void CreateSpecialCols();

		void SetupAuiTheme();

		void ShowPatientDetails();
		wxBitmap GetPatientBitMap();
		void SwitchToolBar();
		std::set<wxDataViewItem> mMedicationSelections;
		std::set<wxDataViewItem> mPatientSelections;
		size_t mSearchColumn = pof::PatientManager::PATIENT_NAME;
	protected:
		void OnPatientActivated(wxDataViewEvent& evt);
		void OnAddPatient(wxCommandEvent& evt);
		void OnRemovePatient(wxCommandEvent& evt);
		void OnAddMedication(wxCommandEvent& evt);
		void OnAddPacks(wxCommandEvent& evt);
		void OnRemoveMedication(wxCommandEvent& evt);
		void OnSelectCol(wxCommandEvent& evt);
		void OnSelectMedCol(wxCommandEvent& evt);
		void OnPatientsContextMenu(wxDataViewEvent& evt);
		void OnMedicationsContextMenu(wxDataViewEvent& evt);
		void OnMedicationsSelected(wxDataViewEvent& evt);
		void OnMedicationHistorySelected(wxDataViewEvent& evt);
		void OnAuiThemeChange();
		void OnSearchPatient(wxCommandEvent& evt);
		void OnSearchCleared(wxCommandEvent& evt);
		void OnStopProduct(wxCommandEvent& evt);
		void OnBack(wxCommandEvent& evt);
		void OnHidePatientMedicalDetails(wxCommandEvent& evt);

		//hide/show patient search column
		void ShowPatientsSelectCol();
		void ShowMedSelectCol();
		void HidePatientsSelectCol();
		void HideMedSelectCol();
		void OnPatientHeaderClicked(wxDataViewEvent& evt);
		void OnMedHeaderClicked(wxDataViewEvent& evt);
		void OnPatientDetailsChange(wxPropertyGridEvent& evt);
		//for the splitter
		void OnPositionChanged(wxSplitterEvent& event);
		void OnPositionChanging(wxSplitterEvent& event);
		void OnPositionResize(wxSplitterEvent& event);
		void OnDClick(wxSplitterEvent& event);
		void OnUnsplitEvent(wxSplitterEvent& event);
		void OnSpliterOnIdle(wxIdleEvent& evt);



		wxAuiManager mManager;
		wxPanel* mPanel = nullptr;
		wxTimer mUpdatePatientStockTimer;
		wxAuiToolBar* mTopTools = nullptr;
		wxAuiToolBar* mPatientTools = nullptr;
		wxSearchCtrl* mPaitentSearch = nullptr;
		wxSimplebook* mBook = nullptr;
		wxAuiToolBarItem* pd = nullptr;

		wxSplitterWindow * mPatientPanel;
		wxPanel* mSPanel = nullptr;
		wxCollapsiblePane* mCurrentMedicationPane = nullptr;
		wxCollapsiblePane* mMedsHistoryPane = nullptr;
		wxCollapsiblePane* mPane = nullptr;

		wxDataViewCtrl* mPatientSelect;
		wxDataViewColumn* mSelectCol = nullptr;
		wxDataViewCtrl* mCurrentMedicationView;
		wxDataViewColumn* mSelectMedCol = nullptr;
		wxStaticText* mPatientNameText = nullptr;
		wxStaticText* mDobText = nullptr;

		wxDataViewCtrl* mMedHistoryView;

		wxPropertyGridManager* mPatientDetails;
		wxPropertyGridManager* mMedicationDetails;
		std::optional<std::reference_wrapper<pof::base::data::row_t>> mCurrentPatient;
		wxSearchCtrl* mSearchbar = nullptr;

		DECLARE_EVENT_TABLE()
	};
};