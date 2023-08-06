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


#include "AuiTheme.h"
#include "LogSink.h"
#include "ArtProvider.h"
#include "Modules.h"
#include "Workspace.h"
#include "ProductView.h"
#include "SaleView.h"
#include "AuditView.h"
#include "PrescriptionView.h"

#include <Data.h>
#include <data_tuple.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <functional>
#include <unordered_map>


namespace pof {
	class MainFrame : public wxFrame
	{
	public:
		constexpr static long AUIMGRSTYLE = wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_LIVE_RESIZE;

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
			//MENUS
			ID_MENU_VIEW_SHOW_MODULES,
			ID_MENU_VIEW_LOG,
			ID_MENU_ACCOUNT_SIGN_OUT,
			ID_MENU_PRODUCT_IMPORT_JSON,
			ID_MENU_HELP_ABOUT,
			ID_MENU_PRODUCT_EXPORT,
			ID_MENU_EXPORT_JSON,
			ID_MENU_EXPORT_EXCEL,
			ID_MENU_EXPORT_CSV,
			ID_MENU_PRODUCT_SAVE,
			ID_MENU_PRODUCT_LOAD,
		

		};

		MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size);
		virtual ~MainFrame();

		void ReloadFrame();
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

		//test 
		void OnTestSave(wxCommandEvent& evt);
		void OnTestLoad(wxCommandEvent& evt);


		//menu evts
		void OnImportJson(wxCommandEvent& evt);

		//slots
		void OnModuleSlot(pof::Modules::const_iterator win, Modules::Evt notif);
		void OnModuleSlotReload(pof::Modules::const_iterator win, Modules::Evt notif);
		void OnAuiThemeChangeSlot();
		void OnCategoryAdded(const std::string& name);
		void OnProductModuleSlotReload(pof::Modules::const_iterator win, Modules::Evt notif);

	private:
		wxAuiManager mAuiManager;
		pof::Modules* mModules = nullptr;
		pof::Workspace* mWorkspace = nullptr;
		std::unique_ptr<wxImageList> mImageList = nullptr;
		std::shared_ptr<wxTextCtrl> mLogView = nullptr;

		//views
		pof::ProductView* mProductView = nullptr;
		pof::SaleView* mSaleView = nullptr;
		pof::PrescriptionView* mPrescriptionView = nullptr;
		pof::AuditView* mAuditView = nullptr;

		DECLARE_EVENT_TABLE()
	};
};