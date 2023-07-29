#include "Application.h"
#include <net.h>
#include <errc.h>
#include <string>
#include <spdlog/spdlog.h>
#include <thread>

#include <fstream>

#include <random>

#include <data.h>
#include <data_tuple.h>
#include <packages.h>

#include "DataModel.h"

#include "datatree.h"

IMPLEMENT_APP(pof::Application)

int test_main(int argc, char** const argv)
{
	return 0;
}


pof::Application::Application()
{
	SetAppName("PharmaOffice");
	SetAppDisplayName("PharmaOffice");
	SetVendorName("D-GLOPA NIGERIA LIMITED");
	SetVendorDisplayName("D-GLOPA NIGERIA LIMITED");

	MainPharmacy = std::make_shared<pof::Pharmacy>();
	MainAccount = std::make_shared<pof::Account>();
	mPrintManager = std::make_shared<pof::PrintManager>();


}

bool pof::Application::OnInit()
{
	//read settings 
	//set up network
	//sign in
	// 
	//set up other things
	// check for updates
	//lunch mainframe
	if (!wxApp::OnInit()) return false;


	SetUpPaths();
	wxInitAllImageHandlers();
	wxArtProvider::Push(new pof::ArtProvider);
	SetUpColorTable();

	//where do I lunch set up wizard?? 
	
	//load the settings
	auto path = fs::current_path() / "officeConfig.ini";
	bool ret = false;
	if (!fs::exists(path)) {
		LunchWizard();
	}
	LoadSettings(path);
	if (bUsingLocalDatabase) {
		try {
			OpenLocalDatabase();
			CreateTables();
			mProductManager.bUsingLocalDatabase = bUsingLocalDatabase;
			
		}
		catch (std::exception& exp) {
			spdlog::critical("Cannot open databse: {}", exp.what());
			OnExit();
			return false;
		}
		if (!SignIn()) {
			//failed signed in from local database
			OnExit();
			return false;
		}
	}
	else {
		if (!SignIn()) {
			//failed signed in
			//return false;
		}
		//test	
	}

	TestAccountAndPharmacy();
	mAuditManager.mCurrentAccount = MainAccount;
	mSaleManager.mCurAccount = MainAccount;
	mSaleManager.mCurPharmacy = MainPharmacy;


	return CreateMainFrame();
}

int pof::Application::OnExit()
{
	mNetManager.stop();
	if (bUsingLocalDatabase) {
		sqlite3_shutdown();
	}
	return 0;
}

void pof::Application::SetUpColorTable()
{
	wxTheColourDatabase->AddColour("Aqua", wxColour(240, 255, 255));
	wxTheColourDatabase->AddColour("Navajo_white", wxColour(255, 222, 173));
	wxTheColourDatabase->AddColour("Tomato", wxColour(255, 99, 71));
	wxTheColourDatabase->AddColour("Papaya whip", wxColour(255, 239, 213));
}

bool pof::Application::CreateMainFrame()
{
	mMainFrame = new pof::MainFrame(nullptr,
		wxID_ANY, wxDefaultPosition, wxSize(1122, 762));

	mMainFrame->Center(wxBOTH);
	mMainFrame->Show();
	return true;
}

bool pof::Application::CheckForUpdate()
{
	//check everytime or just after some days
	return false;
}

bool pof::Application::LunchWizard()
{
	return false;
}

bool pof::Application::OpenLocalDatabase()
{
	auto dbPath = std::filesystem::current_path() / "localdatabase.db";
	sqlite3_initialize();
	mLocalDatabase = std::make_shared<pof::base::database>(dbPath);
	mProductManager.mLocalDatabase = mLocalDatabase;
	mAuditManager.mLocalDatabase = mLocalDatabase;
	mSaleManager.mLocalDatabase = mLocalDatabase;

	return true;
}

bool pof::Application::SetUpPaths()
{
	mAsserts = std::filesystem::current_path() / "asserts";
	mModules = std::filesystem::current_path() / "plugins";
	return true;
}

bool pof::Application::LoadSettings(const fs::path& fp)
{
	if (fp.empty() || !fs::exists(fp)) return false;
	std::fstream inifile(fp, std::ios::in);
	if (!inifile.is_open()) false;
	try {
		boost::property_tree::ini_parser::read_ini(inifile, mSettings);
		ReadSettingsFlags();
	}
	catch (const boost::property_tree::ini_parser_error& error) {
		//need to find where to log errors
		//cannot parse ini file,format error
		return false;
	}
	return true;
}

bool pof::Application::SaveSettings(const fs::path& fp)
{
	if (fp.empty() || !fs::exists(fp)) return false;
	std::fstream inifile(fp, std::ios::out);
	if (!inifile.is_open()) false;

	try {
		boost::property_tree::write_ini(inifile, mSettings);
	}
	catch (const boost::property_tree::ini_parser_error& error) {
		return false;
	}
	return true;
}

bool pof::Application::RegisterPharmacy()
{
	return false;
}

bool pof::Application::RegisterAccount()
{
	return false;
}

bool pof::Application::RegisterPharmacyLocal()
{
	return false;
}

bool pof::Application::RegisterAccountLocal()
{
	return false;
}

bool pof::Application::SignIn()
{
	pof::SignInDialog Dialog(nullptr);
	while (1) {
		if (Dialog.ShowModal() == wxID_OK) {
			return true;
		}
		else {
			spdlog::info("Sign in was cancelled");
			return false;
		}
	}
}

void pof::Application::TestAccountAndPharmacy()
{
	MainPharmacy->name = "D-GLOPA NIGERIA LIMITED"s;
}

void pof::Application::CreateTables()
{
	constexpr const std::string_view users_table = 
		"CREATE TABLE IF NOT EXISTS USERS (id integer primary key autoincrement, priv integer, name text, last_name text, email text, phonenumber text, regnumber text, username text, password text);";
	constexpr const std::string_view product_table =
		"CREATE TABLE IF NOT EXISTS products (uuid blob, serail_num integer, name text, generic_name text, class text, formulation text, strength text, strength_type text, usage_info text, descrip text, health_condition text, unit_price blob, cost_price blob, package_size integer, stock_count integer, side_effects text, barcode text, category integer, min_stock_count integer, expire_period text, expire_date integer);";
	constexpr const std::string_view inventory_table =
		"CREATE TABLE IF NOT EXISTS inventory (id integer, uuid blob, expire_date integer, input_date integer, stock_count integer, cost blob, manufacturer_name text, manufacturer_address_id integer, lot_number text);";
	constexpr const std::string_view category_table =
		"CREATE TABLE IF NOT EXISTS category (id integer primary key unique, name text unqiue);";
	auto stmt = mLocalDatabase->prepare(users_table);
	if (!stmt.has_value()) {
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	if (!mLocalDatabase->execute(*stmt))
	{
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	mLocalDatabase->finalise(*stmt);
	stmt = mLocalDatabase->prepare(product_table);
	if (!stmt.has_value()) {
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	if (!mLocalDatabase->execute(*stmt))
	{
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	mLocalDatabase->finalise(*stmt);
	stmt = mLocalDatabase->prepare(inventory_table);
	if (!stmt.has_value()) {
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	if (!mLocalDatabase->execute(*stmt))
	{
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	mLocalDatabase->finalise(*stmt);
	stmt = mLocalDatabase->prepare(category_table);
	if (!stmt.has_value()) {
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	if (!mLocalDatabase->execute(*stmt))
	{
		wxMessageBox(mLocalDatabase->err_msg().data(), "CREATE TABLE");
		return;
	}
	mLocalDatabase->finalise(*stmt);
	mAuditManager.CreateAuditTable();
	mProductManager.CreatePackTable();
	mSaleManager.CreateSaleTable();
}

void pof::Application::ReadSettingsFlags()
{
}

void pof::Application::SaveSettingsFlags()
{
}

boost::property_tree::ptree& pof::Application::operator[](const std::string& path)
{
	//path should point to an actual path to a setting
	if (path.empty()) throw std::logic_error("CANNOT HAVE AN EMPTY PATH TO SETTINGS");
	return mSettings.get_child(path);
}

pof::Application::clock_t::time_point pof::Application::FromDateTime(const wxDateTime& dt)
{
	return clock_t::from_time_t(dt.GetTicks());
}
