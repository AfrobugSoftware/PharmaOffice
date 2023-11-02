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

#include <wx/collheaderctrl.h>
#include <wx/collpane.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/simplebook.h>
#include <wx/timer.h>


#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>

#include "PackView.h"
#include "PatientManager.h"
#include "DropTarget.h"
#include "database.h"

#include <ranges>
#include <unordered_map>
#include <numeric>




namespace pof {
	class PatientView : public wxPanel
	{
	public:
		//pages
		enum : std::uint8_t {
			PATIENT_SELECT = 0,
			PATIENT_VIEW,
		};

		enum {
			ID_SEARCH_BAR = wxID_HIGHEST,
			ID_PATIENT_VIEW,
			ID_PATIENT_MEDS_VIEW,
			ID_PATIENT_HISTORY_VIEW,
			ID_ADD_PRODUCT,

		};

		PatientView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(917, 668), long style = wxTAB_TRAVERSAL);
		~PatientView();
		void CreateToolBars();
		void CreateViews();

		
	protected:
		void OnPatientSelected(wxDataViewEvent& evt);

		wxTimer mUpdatePatientStockTimer;
		wxAuiToolBar* mTopTools = nullptr;
		wxAuiToolBar* mBottomTools = nullptr;
		wxSearchCtrl* mPaitentSearch = nullptr;
		wxSimplebook* mBook = nullptr;
		
		wxPanel* mPatientPanel;
		wxCollapsiblePane* mCurrentMedicationPane = nullptr;
		wxCollapsiblePane* mMedsHistoryPane = nullptr;
		wxCollapsiblePane* mPane = nullptr;

		wxDataViewCtrl* mPatientSelect;
		wxDataViewCtrl* mCurrentMedicationView;
		wxDataViewCtrl* mMedHistoryView;

		wxPropertyGridManager* mPatientDetails;

		DECLARE_EVENT_TABLE()
	};
};