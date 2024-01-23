#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <wx/aui/framemanager.h>
#include <wx/sysopt.h>
#include <wx/config.h>
#include <wx/stdpaths.h>
#include <wx/propdlg.h>
#include <wx/snglinst.h>
#include <wx/debugrpt.h>
#include <wx/file.h>
#include <wx/ffile.h>
#include <wx/notifmsg.h>
#include <wx/splash.h>


#include <filesystem>
#include <netmanager.h>
#include <database.h>
#include <databasemysql.h>
#include <chrono>
#include <sstream>
#include <random>
#include <regex>

#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>


#include <nlohmann/json.hpp>
#include "SignInDialog.h"
#include "Account.h"
#include "Pharmacy.h"
#include "MainFrame.h"
#include "ProductManager.h"
#include "SaleManager.h"
#include "AuditManager.h"
#include "PrintManager.h"
#include "PatientManager.h"
#include "PoisonBookManager.h"
#include "PharmacySetupWizard.h"

namespace js = nlohmann;
namespace fs = std::filesystem;
using namespace std::literals::string_literals;
namespace pof {
	template<typename T = std::uint64_t, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	auto GenRandomId() -> T
	{
		static std::mt19937_64 engine(std::random_device{}());
		static std::uniform_int_distribution<T> dist(1000000, 100000000);
		static auto random = std::bind(dist, engine);

		return random();
	}
	extern wxArrayString SplitIntoArrayString(const std::string& string);

	class RegexValidator : public wxTextValidator
	{
	public:
		RegexValidator(std::regex&& reg, const std::string& errorstr = {});
		virtual wxObject* Clone() const override;
		virtual bool TransferFromWindow() override;
		virtual bool TransferToWindow() override;
		virtual bool Validate(wxWindow* parent) override;
		virtual wxString IsValid(const wxString& val) const override;

	private:
		std::string estr;
		std::regex pattern;

	};


	class Application : public wxApp
	{
	public:
		enum {
			ID_DELETE_ACCOUNT = wxID_HIGHEST + 2,
			ID_RESET_ACCOUNT,
		};

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
		bool LoadSettings();
		bool SaveSettings();

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
		pof::PatientManager mPatientManager;
		pof::PoisonBookManager mPoisonBookManager;
		std::shared_ptr<pof::PrintManager> mPrintManager;
		std::shared_ptr<pof::base::database> mLocalDatabase;
		std::shared_ptr<pof::base::databasemysql> mMysqlDatabase;
		wxArrayString FormulationChoices;

		void CreateMysqlDatabase();
		void CreateMysqlTables();
		//settings flags
		void ReadSettingsFlags();
		void SaveSettingsFlags();

		void GenerateReport(wxDebugReport::Context ctx);
		virtual void OnFatalException() override;
		virtual void OnUnhandledException() override;

		void DecorateSplashScreen(wxBitmap& bmp);

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
		bool bAlertCriticalWarnings = true;
		bool bCheckExpiredOnUpdate = true;
		bool bCheckOutOfStockOnUpdate = true;
		bool bAutomaticBroughtForward = true;
		bool bAllowOtherUsersInventoryPermission = false;
		bool bMaximizeOnLoad = true;
		bool bShowPreviewOnSale = false;
		bool bShowPrintPrompt = true;
		bool bAutomaticBatchNumber = true;
		bool bNotifyStockCheckInComplete = true;
		bool bAllowSellControlledMed = true;
		bool bAlwaysCreateEntryIntoRegister = false;
		bool bShowPageSetup = false;

		wxPaperSize sPaperSize; //may not need it
		date::days gSessionLast;


		std::shared_ptr<pof::Pharmacy> MainPharmacy;
		std::shared_ptr<pof::Account> MainAccount;
		std::string gVersion;
		int mPaperType = 0;

		//utility functions
		static clock_t::time_point FromDateTime(const wxDateTime& dt);
		bool HasPrivilage(pof::Account::Privilage priv);
		void ShowSettings();
		void ShowGeneralSettings(wxPropertySheetDialog& sd);
		void ShowPharmacySettings(wxPropertySheetDialog& sd);
		void ShowAccountSettings(wxPropertySheetDialog& sd);
		void ShowSaleSettings(wxPropertySheetDialog& sd);

		//formulation choices
		void LoadFormulationChoices();
		void AddFormulationChoices(const std::string& choice);
		void SaveFormulationChoices();
		void UpdateFormulationChoices(const wxArrayString& choices);

	
		//settings callback
		void OnResetAccount(wxCommandEvent& evt);
		void OnDeleteAccount(wxCommandEvent& evt);

		//allow main frame to be visable through out the system
		pof::MainFrame* mMainFrame;
	private:
		std::array<wxPropertyGridManager*, 4> mSettingProperties;
		boost::uuids::uuid mApplicationUUID; 
		std::unique_ptr<wxSingleInstanceChecker> mChecker; //only one instance of the application 

		fs::path mAsserts;
		fs::path mModules; //plugin 
		pof::PharmacySetupWizard* wizard = nullptr;

	};
};
DECLARE_APP(pof::Application)