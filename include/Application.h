#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <wx/aui/framemanager.h>

#include <filesystem>
#include <netmanager.h>
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

		bool CreateMainFrame();
		bool CheckForUpdate();
		bool SetUpPaths();
		bool LoadSettings(const fs::path& fp);
		bool SaveSettings(const fs::path& fp);
		bool RegisterPharmacy();
		bool RegisterAccount();
		bool SignIn();

		const fs::path& GetAssertsPath() const { return mAsserts; }
		void TestAccountAndPharmacy();
		
		pof::base::net_manager mNetManager;
		pof::ProductManager mProductManager;
		pof::Pharmacy MainPharamcy;
		pof::Account MainAccount;

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