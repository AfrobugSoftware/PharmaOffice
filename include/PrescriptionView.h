#pragma once
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/artprov.h>
#include <wx/srchctrl.h>
#include <wx/combobox.h>
#include <wx/choice.h>
#include <wx/datectrl.h>

#include <nlohmann/json.hpp>
#include "DataModel.h"
#include "net.h"
#include "AuiTheme.h"


namespace js = nlohmann;
namespace pof {
	class PrescriptionView : public wxPanel
	{
	public:
		enum class PrescriptionState {
				PENDING,
				COMPLETED,
				REJECTED,
				INVALID,
				PARTIAL_COMPLETED,
				MAX
		};

		enum {
			PRESCRIPTION_ID,
			PRESCRIPTION_UUID,
			PRESCRIPTION_DATE,
			PRESCRIPTION_MEDICATIONS, //JSON STRING
			PRESCRIPTION_PATIENT_NAME,
			PRESCRIPTION_PATIENT_ADDRESS,
			PRESCRIPTION_PATIENT_AGE,
			PRESCRIPTION_PATIENT_WEIGHT,

			PRESCRIPTION_PRESCRIBER_NAME,
			PRESCRIPTION_PRESCRIBER_ADDRESS,
			PRESCRIPTION_PRESCRIBER_CONTACTS, //JSON STRING
			PRESCRIPTION_STATE,
			PRESCRIPTION_MAX
		};

		enum {
			ID_DATA_VIEW = wxID_HIGHEST + 2000,
			ID_ADD_PRESCRIPTION,
			ID_DISPENSARY,
			ID_DISPENSARY_TOOL_BAR,
			ID_DISPENSE,
			ID_SHOW_PATIENT_FILE,
			ID_MAKE_INTERVENTION,
			ID_REPORT_INTERACTION,
			ID_ADD_DRUG_TO_PRESCRIPTION,
			ID_PREVIEW,
			ID_SEARCH,
			ID_SEARCH_FOR_PRESCRIPTION,
			ID_SUBSCRIBE,
			ID_REFRESH,
			ID_PSCHOICE,
			ID_PRESCRIPTION_SOURCE,
			ID_PRESCRIPTION_DATE,
			ID_GET_PRESCRIPTION_SOURCES,
			ID_BACK
		};

		PrescriptionView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style);

		void CreateToolBar();
		void LoadPrescriptions();
		void CreateDispensaryView();
		void CreateDispensaryToolBar();
		void CreatePrescriptionSourceChoice();
		void InitDataView();
		void SetSpecialColumns();
		void SetDefaultAuiArt();
		virtual ~PrescriptionView();


	private:

		//test
		void GenerateFakePrescription();
		void OnAddPrescription(wxCommandEvent& evt);
		void OnDispense(wxCommandEvent& evt);
		void OnShowPaitientFile(wxCommandEvent& evt);
		void OnMakeIntervention(wxCommandEvent& evt);
		void OnReportInteraction(wxCommandEvent& evt);
		void OnAddDrugToPrescription(wxCommandEvent& evt);
		void OnLabelPreview(wxCommandEvent& evt);
		void OnRefresh(wxCommandEvent& evt);
		void OnPrescriptionChoiceChange(wxCommandEvent& evt);
		void OnSearchforPrescription(wxCommandEvent& evt);
		void OnGetPrescriptionSources(wxCommandEvent& evt);
		//evts 
		void OnPrescriptionActivated(wxDataViewEvent& evt);
		void OnBack(wxCommandEvent& evt);
		void OnAuiThemeChangeSlot();
	private:
		void OnPrscriptionSource();
		void OnError(const std::string& what);

	private:
		std::unique_ptr<wxAuiManager> mPanelManager;
		wxDatePickerCtrl* mPrescriptionDate;


		wxDataViewCtrl* mDataView;
		//pof::DispensaryView* mDispensaryView;
		std::unique_ptr<pof::DataModel> mModel;


		wxChoice* mPrescriptionSourceChoiceBox;

		//holds the source target name for the prescriptions 
		std::string mPrescriptionSource;

		//
		DECLARE_EVENT_TABLE()


	};
};


