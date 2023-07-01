#pragma once
#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/notebook.h>
#include <wx/progdlg.h>
#include <wx/dataview.h>


#include <wx/aui/aui.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibar.h>

#include "ArtProvider.h"

#include <spdlog/spdlog.h>
#include <fmt/format.h>


#include <memory>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <chrono>

#include "DataModel.h"
#include "AuiTheme.h"

namespace pof {
	class DispensaryView : public wxPanel
	{
	public:

		enum {
			DATA_VIEW = wxID_HIGHEST + 500,
			PROPERTY_MANAGER
		};

		enum : std::uint8_t {
			DISPENSE_ID,
			DISPENSE_MEDIACTION,
			DISPENSE_DOSAGE_FORM,
			DISPENSE_STRENGTH,
			DISPENSE_STRENGTH_TYPE,
			DISPENSE_DIR_FOR_USE,
			DISPENSE_QUANTITY,
			DISPENSE_STATUS,
			DISPENSE_MAX
		};


		enum : std::uint8_t {
			STATUS_DISPENSED,
			STATUS_PEND, //because STATUS_PENDING is used by microsoft
			STATUS_REJECTED
		};

		DispensaryView(wxWindow* parent, wxWindowID id);
		~DispensaryView();

		void OnAuiThemeChange();
		void SetupAuiTheme();

		void Load(const pof::base::data::row_t& prescription);
		void LoadPropertyGrid(const pof::base::data::row_t& prescription);
		void CreateToolBar();
		void CreateDataView();
		void SetSpecialCol();
		void CreatePropertyGrid();
		void CreatePropertyGridToolBar();
		void CreatePropertyGridArt();
		void CreateEditProperty();
		void ResetViewData();
		void ResetModifiedStatus();
		void Dispense();
		void PreviewLabel();
		bool CheckDispensed();
		void UpdatePrescription();
	
		std::set<wxDataViewItem> mSelections;
	private:
		void OnDataViewItemSelected(wxDataViewEvent& evt);
		void OnPropertyChanged(wxPropertyGridEvent& evt);
		void OnPropertyChanging(wxPropertyGridEvent& evt);
	private:
		wxAuiManager mPanelManager;

		std::unique_ptr<wxDataViewCtrl> mDataView;
		std::unique_ptr<wxPropertyGridManager> mPropertyManager;
		std::unique_ptr<pof::DataModel> mDataModel;
		//edit text
		std::string EditText(const std::string& text);

		DECLARE_EVENT_TABLE()

	};
}

