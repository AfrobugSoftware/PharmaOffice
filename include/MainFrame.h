#pragma once

#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aboutdlg.h>
#include <wx/aui/dockart.h>
#include <wx/aui/auibar.h>
#include <wx/srchctrl.h>
#include <wx/frame.h>
#include <wx/progdlg.h>
#include <wx/infobar.h>
#include <wx/creddlg.h>
#include <wx/fontdlg.h>
#include <wx/dir.h>


#include "AuiTheme.h"
#include "LogSink.h"
#include "ArtProvider.h"
#include "Modules.h"
#include "Workspace.h"
#include "ProductView.h"
#include "SaleView.h"
#include "AuditView.h"
#include "PrescriptionView.h"
#include "PatientView.h"
#include "PoisonBookView.h"
#include "autocapture.h"

#include <Data.h>
#include <data_tuple.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <nlohmann/json.hpp>
#include <OpenXLSX.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <functional>
#include <unordered_map>


namespace excel = OpenXLSX;
namespace pof {
	class MainFrame : public wxFrame
	{
	public:
		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;
		constexpr static const std::array<std::string_view, 12> monthNames = { "Jaunary", "Febuary", "March", "April",
		"May", "June", "July", "August", "September", "October", "November", "December" };

		constexpr static const std::array<std::string_view, 7> dayNames = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Sarturday" };
		//pages
		enum : std::uint8_t {
			PAGE_WELCOME = 0,
			PAGE_WORKSPACE
		};

		//menu ID
		enum
		{
			ID_BACK_UP_DATA = wxID_HIGHEST + 100,
			ID_MODULE,
			ID_WORKSPACE,
			ID_USER_PROFILE,
			ID_USER_CREATE_ACCOUNT,
			ID_STATUS_BAR,
			ID_TOOL_BAR,
			ID_PRODUCT_VIEW,
			ID_SALE_VIEW,
			ID_PRESCRIPTION_VIEW,
			ID_AUDIT_VIEW,
			ID_PATIENT_VIEW,
			ID_POISONBOOK_VIEW,
			//MENUS
			ID_PAGER,
			ID_MENU_VIEW_SHOW_MODULES,
			ID_MENU_VIEW_LOG,
			ID_MENU_ACCOUNT_SIGN_OUT,
			ID_MENU_PRODUCT_IMPORT_JSON,
			ID_MENU_PRODUCT_IMPORT_EXCEL,
			ID_MENU_HELP_ABOUT,
			ID_MENU_HELP_SETTINGS,
			ID_MENU_PRODUCT_EXPORT,
			ID_MENU_EXPORT_JSON,
			ID_MENU_EXPORT_EXCEL,
			ID_MENU_EXPORT_CSV,
			ID_MENU_PRODUCT_SAVE,
			ID_MENU_PRODUCT_LOAD,
			ID_MENU_PRODUCT_MARKUP_SETTINGS,
			ID_MENU_PRODUCT_NOTIF_OS, //NOTIY OUT OF STOCK
			ID_MENU_PRODUCT_NOTIF_EXPIRE,
			ID_MENU_PRODUCT_SALE_ALERTS_CLASS,
			ID_MENU_PRODUCT_SALE_ALERTS_OS,
			ID_MENU_PRODUCT_SALE_ALERTS_EXPIRE,
			ID_MENU_PHARMACY_BACKUP,
			ID_MENU_PHARMACY_ROLLBACK,
			ID_IMPORT_FORMULARY,
			ID_EXPORT_FORMULARY,
			ID_CHANGE_FONT,
			ID_SCREENSHOT,
			ID_CAPTURE_CONTROLS,
		};

		MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
		virtual ~MainFrame();

		void ReloadFrame();
		std::string Perspective();
		void Perspective(const std::string& pers);
		void UpdateWelcomePage();
		void PinPatient(const pof::base::data::duuid_t& puid, const std::string& name);
		std::shared_ptr<pof::Account> mAccount;
	protected:
		void CreateMenuBar();
		void CreateStatusBar();
		void CreateTheAcceleratorTable();
		void CreateLogView();
		void CreateModules();
		void CreateWorkSpace();
		void CreateImageList();
		void CreateViews();
		void CreateWelcomePage();
		void CreateSelectList();

		void LoadPinnedPatients();
		void SetupAuiTheme();

		void OnAbout(wxCommandEvent& evt);
		void OnClose(wxCloseEvent& evt);
		void OnShowLog(wxCommandEvent& evt);
		void OnExportJson(wxCommandEvent& evt);
		void OnExportExcel(wxCommandEvent& evt);
		void OnExportCSV(wxCommandEvent& evt);
		void OnUpdateUI(wxUpdateUIEvent& evt);
		void OnShowModules(wxCommandEvent& evt);
		void OnSignOut(wxCommandEvent& evt);
		void OnImportExcel(wxCommandEvent& evt);
		void OnMarkupSettings(wxCommandEvent& evt);
		void OnNotif(wxCommandEvent& evt);
		void OnSaleAlerts(wxCommandEvent& evt);
		void OnShowSettings(wxCommandEvent& evt);
		void OnChangeFont(wxCommandEvent& evt);
		//test 
		void OnTestSave(wxCommandEvent& evt);
		void OnTestLoad(wxCommandEvent& evt);
		void OnIdle(wxIdleEvent& evt);
		void OnWelcomePageSelect(wxListEvent& evt);
		void OnMenuUpdateUI(wxUpdateUIEvent& evt);
		//menu evts
		void OnImportJson(wxCommandEvent& evt);
		void OnBackupData(wxCommandEvent& evt);
		void OnRollbackData(wxCommandEvent& evt);
		void OnImportFormulary(wxCommandEvent& evt);
		void OnExportFormulary(wxCommandEvent& evt);
		void OnScreenShot(wxCommandEvent& evt);
		void OnCaptureAllControls(wxCommandEvent& evt);
		//slots
		void OnModuleSlot(pof::Modules::const_iterator win, Modules::Evt notif);
		void OnModuleSlotReload(pof::Modules::const_iterator win, Modules::Evt notif);
		void OnAuiThemeChangeSlot();
		void OnCategoryAdded(const std::string& name);
		void OnProductModuleSlotReload(pof::Modules::const_iterator win, Modules::Evt notif);
		void OnWorkspaceNotif(pof::Workspace::Notif notif, size_t page);
	private:
		wxStaticText* time1 = nullptr;
		wxStaticText* date1 = nullptr;
		wxAuiManager mAuiManager;
		pof::Modules* mModules = nullptr;
		pof::Workspace* mWorkspace = nullptr;
		wxSimplebook* mPager = nullptr;
		wxPanel* mWelcomePage = nullptr;
		wxListCtrl* mSelectList = nullptr;
		std::unique_ptr<wxImageList> mImageList = nullptr;
		std::shared_ptr<wxTextCtrl> mLogView = nullptr;
		wxStaticText* pharmName = nullptr; //update the pharm name in the welcome page
		
		//views
		pof::ProductView* mProductView = nullptr;
		pof::SaleView* mSaleView = nullptr;
		pof::PrescriptionView* mPrescriptionView = nullptr;
		pof::AuditView* mAuditView = nullptr;
		pof::PatientView* mPatientView = nullptr;
		pof::PoisonBookView* mPoisonBookView = nullptr;

		std::chrono::system_clock::time_point mExpireWatchTime;
		DECLARE_EVENT_TABLE()
	};
};