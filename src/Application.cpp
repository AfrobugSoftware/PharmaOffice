//#include "Application.h"
#include "PofPch.h"
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
: gSessionLast(30) {
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
	gVersion = "0.0.1 pre beta";
	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);
	SetUpPaths();
	wxInitAllImageHandlers();
	wxArtProvider::Push(new pof::ArtProvider);
	SetUpColorTable();

	//do system settings here?
	wxSystemOptions::SetOption(wxT("msw.remap"), 0);
	wxSystemOptions::SetOption(wxT("msw.staticbox.optimized-paint"), 0);

	//need to change the font globally, how to do that ??


	//where do I lunch set up wizard?? 
	
	//load the settings
	auto path = fs::current_path() / "officeConfig.ini";
	bool ret = false;
	if (!fs::exists(path)) {
		LunchWizard();
	}
	LoadSettings();
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

	//check if time is increasing, very important that a changed time cause
	auto today = pof::base::data::clock_t::now();
	auto checkTime = mProductManager.GetLastCheckedTime();
	if (!checkTime.has_value()) {
		wxMessageBox("Database corrupted", "System Checks", wxICON_ERROR | wxOK);
		return false;
	}
	if (checkTime->time_since_epoch().count() == 0){
		//first time creating
		mProductManager.AddAction(ProductManager::CHECK_TIME);
	}
	else {
		if (checkTime > today){
			wxMessageBox("System time is less than last recorded time", "System checks", wxICON_ERROR | wxOK);
			return false;
		}
	}
	mProductManager.UpdateTimeCheck(today);
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

void pof::Application::SetupDatabaseExt()
{
	if (mLocalDatabase) {
		pof::base::func_aggregate costAgg;
		costAgg.name = "SumCost";
		costAgg.arg_count = 1;
		costAgg.fstep = pof::base::cost_step_func;
		costAgg.ffinal = pof::base::cost_final_func;

		pof::base::func_aggregate monthFunc;
		monthFunc.name = "Months";
		monthFunc.arg_count = 1;
		monthFunc.func = pof::base::month_func;
	
		pof::base::func_aggregate daysFunc;
		daysFunc.name = "Days";
		daysFunc.arg_count = 1;
		daysFunc.func = pof::base::day_func;

		pof::base::func_aggregate scaleCost;
		scaleCost.name = "ScaleCost";
		scaleCost.arg_count = 2;
		scaleCost.func = pof::base::cost_multi_add;

		mLocalDatabase->register_func(costAgg);
		mLocalDatabase->register_func(monthFunc);
		mLocalDatabase->register_func(daysFunc);
		mLocalDatabase->register_func(scaleCost);
	}
}

bool pof::Application::CreateMainFrame()
{
	mMainFrame = new pof::MainFrame(nullptr,
		wxID_ANY, wxDefaultPosition, wxSize(1122, 762));

	mMainFrame->mAccount = MainAccount;
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
	SetupDatabaseExt();

	mProductManager.mLocalDatabase = mLocalDatabase;
	mAuditManager.mLocalDatabase = mLocalDatabase;
	mSaleManager.mLocalDatabase = mLocalDatabase;
	MainAccount->mLocalDatabase = mLocalDatabase;

	return true;
}

bool pof::Application::SetUpPaths()
{
	mAsserts = std::filesystem::current_path() / "asserts";
	mModules = std::filesystem::current_path() / "plugins";
	return true;
}

bool pof::Application::LoadSettings()
{
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/application"));
	config->Write(wxT("UseLocalDatabase"), bUsingLocalDatabase);
	config->Write(wxT("HighlightLowStock"), bHighlightLowStock);
	config->Write(wxT("GlobalCostMarkup"), bGlobalCostMarkup);
	config->Write(wxT("KeepMeSignedIn"), bKeepMeSignedIn);
	config->Write(wxT("UseMinStock"), bUseMinStock);
	config->Write(wxT("PharmacistWarnings"), bPharamcistWarings);
	config->Write(wxT("CheckExpired"), bCheckExpired);
	config->Write(wxT("CheckOutOfStock"), bCheckOutOfStock);
	config->Write(wxT("CheckPOM"), bCheckPOM);
	config->Write(wxT("CheckExpirePeriodOnIdle"), bCheckExpirePeiodOnIdle);
	config->Write(wxT("AlertCriticalWarnings"), bAlertCriticalWarnings);
	config->Write(wxT("CheckExpiredOnUpdate"), bCheckExpiredOnUpdate);
	config->Write(wxT("CheckOutOfStockOnUpdate"), bCheckOutOfStockOnUpdate);
	config->Write(wxT("AutomacticBroughtForward"), bAutomaticBroughtForward);
	config->Write(wxT("Version"), wxString(gVersion));


	return true;
}

bool pof::Application::SaveSettings()
{
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/application"));
	config->Read(wxT("UseLocalDatabase"), &bUsingLocalDatabase);
	config->Read(wxT("HighlightLowStock"), &bHighlightLowStock);
	config->Read(wxT("GlobalCostMarkup"), &bGlobalCostMarkup);
	config->Read(wxT("KeepMeSignedIn"), &bKeepMeSignedIn);
	config->Read(wxT("UseMinStock"), &bUseMinStock);
	config->Read(wxT("PharmacistWarnings"), &bPharamcistWarings);
	config->Read(wxT("CheckExpired"), &bCheckExpired);
	config->Read(wxT("CheckOutOfStock"), &bCheckOutOfStock);
	config->Read(wxT("CheckPOM"), &bCheckPOM);
	config->Read(wxT("CheckExpirePeriodOnIdle"), &bCheckExpirePeiodOnIdle);
	config->Read(wxT("AlertCriticalWarnings"), &bAlertCriticalWarnings);
	config->Read(wxT("CheckExpiredOnUpdate"), &bCheckExpiredOnUpdate);
	config->Read(wxT("CheckOutOfStockOnUpdate"), &bCheckOutOfStockOnUpdate);
	config->Read(wxT("AutomacticBroughtForward"), &bAutomaticBroughtForward);
	wxString version;
	config->Read(wxT("Version"), &version);
	gVersion = version;
	
	config->SetPath(wxT("/"));
	config->SetPath(wxT("/mainframe"));
	int x, y, w, h;
	x = y = w = h = -1;
	config->Read(wxT("PosX"), &x);
	config->Read(wxT("PosY"), &y);
	config->Read(wxT("SizeW"), &w);
	config->Read(wxT("SizeH"), &h);



	config->SetPath(wxT("/"));
	config->SetPath(wxT("/pharmacy"));


	if (bKeepMeSignedIn) {
		//read and create pharmacy
		config->SetPath(wxT("/"));
		config->SetPath(wxT("/activeuser"));
		//save user id or session Id that is bound to a user, sessions have expiry dates 
	}


	config->SetPath(wxT("/"));
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

bool pof::Application::SignOut()
{
	mMainFrame->Hide();

	pof::SignInDialog Dialog(nullptr);
	while (1) {
		if (Dialog.ShowModal() == wxID_OK) {
			mMainFrame->ReloadFrame();
			mMainFrame->Show();
			return true;
		}
		else {
			spdlog::info("Sign in was cancelled");
			mMainFrame->Close();
			return false;
		}
	}
	return false;
}

void pof::Application::TestAccountAndPharmacy()
{
	MainPharmacy->name = "D-GLOPA NIGERIA LIMITED"s;
	MainPharmacy->addy.number = "16";
	MainPharmacy->addy.street = "Peterburg";
	MainPharmacy->addy.city = "Asaba";
	MainPharmacy->addy.state = "Delta State";
	MainPharmacy->addy.country = "Nigeria";
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
	mProductManager.CreateOrderListTable();
	mProductManager.CreateStockCheckTable();
	mProductManager.CreateWarningTable();
	mProductManager.CreateExpiredStockTable();
	mProductManager.CreateActionTable();
	mSaleManager.CreateSaleTable();
	MainAccount->CreateAccountInfoTable();
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

bool pof::Application::HasPrivilage(pof::Account::Privilage& priv)
{
	const int idx = static_cast<int>(priv);
	return (MainAccount->priv.test(idx));
}

void pof::Application::CreateSessionTable()
{
}
