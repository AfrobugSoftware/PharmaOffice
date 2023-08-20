#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <wx/aui/framemanager.h>

#include <filesystem>
#include <netmanager.h>
#include <database.h>
#include <chrono>
#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


#include <nlohmann/json.hpp>
#include "SignInDialog.h"
#include "Account.h"
#include "Pharmacy.h"
#include "MainFrame.h"
#include "ProductManager.h"
#include "SaleManager.h"
#include "AuditManager.h"
#include "PrintManager.h"


namespace js = nlohmann;
namespace fs = std::filesystem;
using namespace std::literals::string_literals;
namespace pof {
	class Application : public wxApp
	{
	public:
		using signoutsig_t = boost::signals2::signal<void(void)>;
		using clock_t = std::chrono::system_clock;

		Application();
		virtual ~Application() {}

		virtual bool OnInit() override;
		virtual int OnExit() override;


		static void SetUpColorTable();
		void SetupDatabaseExt();
		bool CreateMainFrame();
		bool CheckForUpdate();
		bool LunchWizard();
		bool OpenLocalDatabase();
		bool SetUpPaths();
		bool LoadSettings(const fs::path& fp);
		bool SaveSettings(const fs::path& fp);

		bool RegisterPharmacy();
		bool RegisterAccount();
		bool RegisterPharmacyLocal();
		bool RegisterAccountLocal();
		bool SignIn();
		bool SignOut();

		const fs::path& GetAssertsPath() const { return mAsserts; }
		void TestAccountAndPharmacy();
		void CreateTables();
		
		pof::base::net_manager mNetManager;
		pof::ProductManager mProductManager;
		pof::SaleManager mSaleManager;
		pof::AuditManager mAuditManager;
		std::shared_ptr<pof::PrintManager> mPrintManager;
		std::shared_ptr<pof::base::database> mLocalDatabase;

		//settings flags
		void ReadSettingsFlags();
		void SaveSettingsFlags();

		bool bUsingLocalDatabase = true;
		bool bHighlightLowStock = false;
		bool bGlobalCostMarkup = false;
		bool bKeepMeSignedIn = false;
		bool bUseMinStock = false;
		bool bPharamcistWarings = false;
		bool bCheckExpired = true;
		bool bCheckOutOfStock = true;
		bool bCheckPOM = true;
		bool bCheckExpirePeiodOnIdle = true;

		std::shared_ptr<pof::Pharmacy> MainPharmacy;
		std::shared_ptr<pof::Account> MainAccount;

		boost::property_tree::ptree& operator[](const std::string& path);

		//utility functions
		static clock_t::time_point FromDateTime(const wxDateTime& dt);
	private:
		boost::uuids::uuid mApplicationUUID; 
		pof::MainFrame* mMainFrame;
		std::string mVersion;
		fs::path mAsserts;
		fs::path mModules; //plugin 

		boost::property_tree::ptree mSettings;
	};
};
DECLARE_APP(pof::Application)