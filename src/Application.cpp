//#include "Application.h"
#include "PofPch.h"
#include <net.h>
#include <errc.h>
#include <string>
#include <spdlog/spdlog.h>
#include <thread>

#include <fstream>

#include "RegisterPharmacy.h"

IMPLEMENT_APP(pof::Application)


int test_main(int argc, char** const argv)
{
	return 0;
}

wxArrayString pof::SplitIntoArrayString(const std::string& string)
{
	wxArrayString Split;
	auto pos = string.find_first_of(",");
	size_t pos2 = 0;
	if (pos == std::string::npos) {
		if (!string.empty()) Split.push_back(string);
		return Split;
	}

	Split.push_back(std::move(string.substr(0, pos)));
	while ((pos2 = string.find_first_of(",", pos + 1)) != std::string::npos) {
		Split.push_back(std::move(string.substr(pos + 1, pos2 - (pos + 1))));
		pos = pos2;
	}
	Split.push_back(std::move(string.substr(pos + 1, string.size() - (pos + 1))));
	return Split;
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

	wxHandleFatalExceptions(true);

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
try{
	if (!wxApp::OnInit()) return false;
	mChecker = std::make_unique<wxSingleInstanceChecker>();
	if (mChecker->IsAnotherRunning()) {
		//..find out how to pop up the current running application
		mChecker.reset(nullptr);
		OnExit();
		return false;
	}




	SetUseBestVisual(true, true);


	//load the settings
	LoadSettings();

	gVersion = "1.1.2 - pre beta";
	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);
	SetUpPaths();
	wxInitAllImageHandlers();
	wxArtProvider::Push(new pof::ArtProvider);

	wxBitmap bitmap = wxArtProvider::GetBitmap("splash");

	SetUpColorTable();
	//do system settings here?
	wxSystemOptions::SetOption(wxT("msw.remap"), 1);
	wxSystemOptions::SetOption(wxT("msw.staticbox.optimized-paint"), 1);

	//need to change the font globally, how to do that ??


	//where do I lunch set up wizard?? 
	pof::Account::CreateSecurityQuestions();
	LoadFormulationChoices();
	LoadStrengthChoices();

	//TestAccountAndPharmacy();
	mAuditManager.mCurrentAccount = MainAccount;
	mSaleManager.mCurAccount = MainAccount;
	mSaleManager.mCurPharmacy = MainPharmacy;

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

		//lunch register pharmacy
		if (MainPharmacy->name.empty()) {
			bool registerd = LunchWizard();
			if (!registerd) {
				OnExit();
				return false;
			}
		}

		//check if in active session, if we are asked to keep the signin
		bool ssin = false;
		try {
			if (bKeepMeSignedIn) {
				ssin = MainAccount->SignInFromSession();
			}
			if (!ssin) {
				if (!SignIn()) {
					//failed signed in from local database
					OnExit();
					return false;
				}
			}
		}
		catch (const std::exception& exp) {
			wxMessageBox(exp.what(), "FATAL ERROR", wxICON_ERROR | wxOK);
			OnExit();
			return false;
		}
	}
	else {
		if (!SignIn()) {
			//failed signed in
			OnExit();
			return false;
		}
		//test	
	}

	//check if time is increasing, very important that a changed time cause
	//check that we are in West central africa
	auto curzone = std::chrono::current_zone();
	auto info = curzone->get_info(std::chrono::system_clock::now());
	const std::string zz = std::format("{:%z}", info);
	if (zz != "+0100") {
		wxMessageBox("Please change time zone in settings to (UTC + 01:00)", "PharamOffice System checks", wxICON_ERROR | wxOK);
		return false;
	}

	auto today = pof::base::data::clock_t::now();
	auto checkTime = mProductManager.GetLastCheckedTime();
	if (!checkTime.has_value()) {
		wxMessageBox("Database corrupted", "PharmaOffice System Checks", wxICON_ERROR | wxOK);
		OnExit();
		return false;
	}
	if (checkTime->time_since_epoch().count() == static_cast<pof::base::data::clock_t::duration::rep>(0)) {
		//first time creating
		mProductManager.AddAction(ProductManager::CHECK_TIME);
	}
	else {
		if (checkTime > today) {
			wxMessageBox("System time is less than last recorded time", "PharmaOffice System checks", wxICON_ERROR | wxOK);
			OnExit();
			return false;
		}
	}

	mProductManager.UpdateTimeCheck(today);
	bool status = CreateMainFrame();

	SaveSettings();
	return status;
	}
	catch (const std::exception& exp)
	{
		wxMessageBox(exp.what(), "FATAL ERROR", wxICON_ERROR | wxOK);
		OnExit();
		return false;
	}
}

int pof::Application::OnExit()
{
	wxApp::OnExit();
	if (wizard){
		wizard->Destroy();
		delete wizard;
		wizard = nullptr;
	}
	mNetManager.stop();
	if (bUsingLocalDatabase && mLocalDatabase) {
		mLocalDatabase->flush_db();
		sqlite3_shutdown();
	}
	return 0;
}

void pof::Application::SetUpColorTable()
{
	//wxTheColourDatabase->AddColour("module", wxColour(0xFF, 0xCA, 0x28, 50));
	wxTheColourDatabase->AddColour("module", wxColour(0xFF, 0xFF, 0xFF, 50));
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

		pof::base::func_aggregate CostMulti;
		CostMulti.name = "CostMulti";
		CostMulti.arg_count = 2;
		CostMulti.func = pof::base::cost_multi;

		//pof::base::func_aggregate WeekFunc;
		//WeekFunc.name = "Weeks"s;
		//WeekFunc.arg_count = 1;
		//WeekFunc.func = pof::Application::DBFuncWeek;

		mLocalDatabase->register_func(costAgg);
		mLocalDatabase->register_func(monthFunc);
		mLocalDatabase->register_func(daysFunc);
		mLocalDatabase->register_func(scaleCost);
		mLocalDatabase->register_func(CostMulti);
		//mLocalDatabase->register_func(WeekFunc);
	}
}

bool pof::Application::CreateMainFrame()
{
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/pharmacy"));
	int x, y, w, h;
	x = y = w = h = -1;
	config->Read(wxT("PosX"), &x);
	config->Read(wxT("PosY"), &y);
	config->Read(wxT("SizeW"), &w);
	config->Read(wxT("SizeH"), &h);
	if (x == -1) {
		mMainFrame = new pof::MainFrame(nullptr,
			wxID_ANY, wxDefaultPosition, wxDefaultSize);
		mMainFrame->SetSize(mMainFrame->FromDIP(wxSize(822, 762)));
	}
	else {
		mMainFrame = new pof::MainFrame(nullptr,
			wxID_ANY, wxDefaultPosition, wxDefaultSize);
		mMainFrame->SetPosition(mMainFrame->FromDIP(wxPoint(x, y)));
		mMainFrame->SetSize(mMainFrame->FromDIP(wxSize(w, h)));
	}



	mMainFrame->mAccount = MainAccount;
	mMainFrame->Center(wxBOTH);
	if (bMaximizeOnLoad) mMainFrame->Maximize();

	wxString readData;
	config->Read(wxT("Perspective"), &readData);
	spdlog::info(readData.ToStdString());
	mMainFrame->Perspective(readData.ToStdString());

	CreateMysqlDatabase(); //for testing
	auto font = mFontSettings.GetChosenFont();
	if(font.IsOk()) mfontSignal(font); //send the chosen font
	SetTopWindow(mMainFrame);

	return mMainFrame->Show();
}

bool pof::Application::CheckForUpdate()
{
	auto sess = std::make_shared<pof::base::ssl::session<http::string_body>>(wxGetApp().mNetManager.io(), wxGetApp().mNetManager.ssl());
	auto fut = sess->req<http::verb::get>("localhost", "/update/info", "80");


	//check everytime or just after some days
	return false;
}

bool pof::Application::LunchWizard()
{
	wizard = new pof::PharmacySetupWizard(nullptr);
	wizard->mp = MainPharmacy;
	wizard->RunWizard(wizard->GetFirstPage());
	bool state = wizard->CheckState();
	
	wizard->Destroy();
	delete wizard;
	wizard = nullptr;

	if(state) SaveSettings();
	return state;
}

bool pof::Application::OpenLocalDatabase()
{
	//hide database
	auto dbFolder = std::filesystem::current_path() / ".data";
	if (!fs::is_directory(dbFolder)) {
		//have not create the folder yet
		fs::create_directory(dbFolder);
	}
	auto dbPath = dbFolder / "localdatabase.db";


	sqlite3_initialize();
	mLocalDatabase = std::make_shared<pof::base::database>(dbPath);
	SetupDatabaseExt();

	mProductManager.mLocalDatabase = mLocalDatabase;
	mAuditManager.mLocalDatabase = mLocalDatabase;
	mSaleManager.mLocalDatabase = mLocalDatabase;
	mPatientManager.mLocalDatabase = mLocalDatabase;
	MainAccount->mLocalDatabase = mLocalDatabase;
	mPoisonBookManager.mLocalDatabase = mLocalDatabase;

	//db extensions for the different modules
	mPatientManager.CreateDatabaseFunctions();
	mSaleManager.CreateDatabaseFunctions();

	return true;
}

bool pof::Application::SetUpPaths()
{
	mAsserts = std::filesystem::current_path() / "asserts";
	mModules = std::filesystem::current_path() / "plugins";
	return true;
}

bool pof::Application::SaveSettings()
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
	config->Write(wxT("AllowOtherUsersInventoryPermission"), bAllowOtherUsersInventoryPermission);
	config->Write(wxT("MaximizeOnLoad"), bMaximizeOnLoad);
	config->Write(wxT("Version"), wxString(gVersion));
	config->Write(wxT("ShowPreviewOnSale"), bShowPreviewOnSale);
	config->Write(wxT("ShowPrintPrompt"), bShowPrintPrompt);
	config->Write(wxT("AutomaticBatchNumber"), bAutomaticBatchNumber);
	config->Write(wxT("NotifyStockCheckInComplete"), bNotifyStockCheckInComplete);
	config->Write(wxT("ShowPageSetup"), bShowPageSetup);
	config->Write(wxT("UseSavedCost"), bUseSavedCost);
	config->Write(wxT("PaperType"), mPaperType);
	config->Write(wxT("HighlightOutOfStockInCategory"), bHighlightOutOfStockInCategory);
	config->Write(wxT("Markup"), mProductManager.gMarkup);
	//pharmacy
	config->SetPath(wxT("/pharamcy"));
	config->Write(wxT("Name"), wxString(MainPharmacy->name));
	config->Write(wxT("Type"), MainPharmacy->GetPharmacyType().to_ullong());

	//addy
	config->Write(wxT("Addy.country"), wxString(MainPharmacy->addy.country));
	config->Write(wxT("Addy.state"), wxString(MainPharmacy->addy.state));
	config->Write(wxT("Addy.lga"), wxString(MainPharmacy->addy.lga));
	config->Write(wxT("Addy.city"), wxString(MainPharmacy->addy.city));
	config->Write(wxT("Addy.street"), wxString(MainPharmacy->addy.street));
	config->Write(wxT("Addy.number"), wxString(MainPharmacy->addy.number));
	config->Write(wxT("Addy.postcode"), wxString(MainPharmacy->addy.postcode));

	//contact info
	config->Write(wxT("Contact.phonenumber"), wxString(MainPharmacy->contact.phoneNumber));
	config->Write(wxT("Contact.email"), wxString(MainPharmacy->contact.email));
	config->Write(wxT("Contact.website"), wxString(MainPharmacy->contact.website));

	//for the applicaion size
	config->SetPath("/pharmacy");
	if (mMainFrame) {
		if (!mMainFrame->IsMaximized()) {
			wxPoint pos = mMainFrame->ToDIP(mMainFrame->GetPosition());
			wxSize size = mMainFrame->ToDIP(mMainFrame->GetSize());
			config->Write(wxT("PosX"), pos.x);
			config->Write(wxT("PosY"), pos.y);
			config->Write(wxT("SizeW"), size.x);
			config->Write(wxT("SizeH"), size.y);
		}
		config->Write(wxT("Perspective"), wxString(mMainFrame->Perspective()));
	}
	SaveFont(); //save the font
	SaveReceiptPageSettings();
	return true;
}

bool pof::Application::LoadSettings()
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
	config->Read(wxT("AllowOtherUsersInventoryPermission"), &bAllowOtherUsersInventoryPermission);
	config->Read(wxT("MaximizeOnLoad"), &bMaximizeOnLoad);
	config->Read(wxT("ShowPreviewOnSale"), &bShowPreviewOnSale);
	config->Read(wxT("ShowPrintPromt"), &bShowPrintPrompt);
	config->Read(wxT("AutomaticBatchNumber"), &bAutomaticBatchNumber);
	config->Read(wxT("NotifyStockCheckInComplete"), &bNotifyStockCheckInComplete);
	config->Read(wxT("ShowPageSetup"), &bShowPageSetup);
	config->Read(wxT("UseSavedCost"), &bUseSavedCost);
	config->Read(wxT("PaperType"), &mPaperType);
	config->Read(wxT("HighlightOutOfStockInCategory"), &bHighlightOutOfStockInCategory);
	config->Read(wxT("Markup"), &(mProductManager.gMarkup));

	wxString version;
	config->Read(wxT("Version"), &version);
	gVersion = version;
	
	config->SetPath(wxT("/pharamcy"));

	//load in the pharamcy data
	wxString readData;
	config->Read(wxT("Name"), &readData);
	MainPharmacy->name = readData;

	std::uint64_t type;
	config->Read(wxT("Type"), &type);
	MainPharmacy->SetPharmacyType({ type });
	//brach ID and pharmacy ID


	//addy
	config->Read(wxT("Addy.country"), &readData);
	MainPharmacy->addy.country = readData;

	config->Read(wxT("Addy.state"), &readData);
	MainPharmacy->addy.state = readData;

	config->Read(wxT("Addy.lga"), &readData);
	MainPharmacy->addy.lga = readData;

	config->Read(wxT("Addy.city"), &readData);
	MainPharmacy->addy.city = readData;

	config->Read(wxT("Addy.street"), &readData);
	MainPharmacy->addy.street = readData;

	config->Read(wxT("Addy.number"), &readData);
	MainPharmacy->addy.number = readData;

	config->Read(wxT("Addy.postcode"), &readData);
	MainPharmacy->addy.postcode = readData;


	//contact info
	config->Read(wxT("Contact.phonenumber"), &readData);
	MainPharmacy->contact.phoneNumber = readData;

	config->Read(wxT("Contact.email"), &readData);
	MainPharmacy->contact.email = readData;

	config->Read(wxT("Contact.website"), &readData);
	MainPharmacy->contact.website = readData;

	LoadFont(); //loads the saved font
	LoadReceiptPageSettings();
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
	pof::PharmacyRegistration pr(nullptr, wxID_ANY);
	pr.mp = MainPharmacy;
	return (pr.ShowModal() == wxID_OK);
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
			//check for keepsignin
			bKeepMeSignedIn = Dialog.GetKeepSignedIn();
			if (bKeepMeSignedIn){
				MainAccount->InsertSession(); //starts new session
			}
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, fmt::format("{} signed in at {:%d/%m/%Y %H:%M:%S}", MainAccount->username, MainAccount->signintime));
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
			bKeepMeSignedIn = Dialog.GetKeepSignedIn();
			if (bKeepMeSignedIn) {
				MainAccount->InsertSession(); //starts new session
			}
			mMainFrame->ReloadFrame();
			mMainFrame->Show();
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, fmt::format("{} signed in at {:%d/%M/%Y %H:%M:%S}", MainAccount->username, MainAccount->signintime));
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
	MainPharmacy->contact.website = "www.zino.com";
	MainPharmacy->contact.email = "ferife_zino@yahoo.co.uk";
	MainPharmacy->contact.phoneNumber = "09131861793";
}

void pof::Application::CreateTables()
{
	if (bUsingLocalDatabase) {
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
	}
	mAuditManager.CreateAuditTable();
	mProductManager.CreatePackTable();
	mProductManager.CreateOrderListTable();
	mProductManager.CreateStockCheckTable();
	mProductManager.CreateWarningTable();
	mProductManager.CreateExpiredStockTable();
	mProductManager.CreateActionTable();
	mProductManager.CreateSupplierInvoiceTable();
	mProductManager.CreateHideTable();
	
	mSaleManager.CreateSaleTable();
	mSaleManager.CreateSaveSaleTable();
	mSaleManager.CreateSaleLabelTable();
	mSaleManager.CreateSaleInfoTable();
	mSaleManager.CreateSaleCostTable();

	MainAccount->CreateAccountInfoTable();
	MainAccount->CreateSessionTable();

	mPatientManager.CreatePatientMedicationTable();
	mPatientManager.CreatePatientTable();
	mPatientManager.CreatePatientAddInfoTable();

	mPoisonBookManager.CreatePoisonBookTable();
}

void pof::Application::CreateMysqlDatabase()
{
	mMysqlDatabase = std::make_shared<pof::base::databasemysql>(mNetManager.io(),
		mNetManager.ssl());
	boost::asio::co_spawn(mNetManager.io(),
	mMysqlDatabase->connect("localhost"s, "3306"s, "root"s, "Topdollar123"s), [&](std::exception_ptr ptr, std::error_code ec) {
		spdlog::info("{:ec}", ec);
		if (ec) return;

		//open the database
		auto qptr = std::make_shared<pof::base::query<pof::base::databasemysql>>(mMysqlDatabase);
		auto qptr2 = std::make_shared<pof::base::query<pof::base::databasemysql>>(mMysqlDatabase);
		
			qptr->m_sql = R"(CREATE DATABASE IF NOT EXISTS pharmaoffice;)";
			qptr2->m_sql = R"(USE pharmaoffice;)";
			qptr->m_sig.connect([&](std::error_code ec, auto sqpr) {
				spdlog::info("{:ec}", ec);
			});
			qptr2->m_sig.connect([&](std::error_code ec, auto sqpr) {
				spdlog::info("{:ec}", ec);
				CreateMysqlTables();
			});
			mMysqlDatabase->push(qptr);
			mMysqlDatabase->push(qptr2);
	});
}

void pof::Application::CreateMysqlTables()
{
	auto query = std::make_shared<pof::base::query<pof::base::databasemysql>>(mMysqlDatabase);
	query->m_sql = R"(CREATE TABLE IF NOT EXISTS products (
		uuid blob, 
		serail_num integer, 
		name text, 
		generic_name text, 
		class text, 
		formulation text, 
		strength text, 
		strength_type text, 
		usage_info text, 
		descrip text, 
		health_condition text, 
		unit_price blob, 
		cost_price blob, 
		package_size integer, 
		stock_count integer, 
		side_effects text, 
		barcode text, 
		category integer, 
		min_stock_count integer, 
		expire_period text, 
		expire_date integer);)"s;
	query->m_sig.connect([&](std::error_code ec, std::shared_ptr<pof::base::query<pof::base::databasemysql>> ptr) {
		spdlog::info("Created table with: {:ec}", ec);
	});
	mMysqlDatabase->push(query);
}

void pof::Application::ReadSettingsFlags()
{

}

void pof::Application::SaveSettingsFlags()
{

}

void pof::Application::GenerateReport(wxDebugReport::Context ctx)
{
	wxDebugReport* report = new wxDebugReport;
	report->AddAll(ctx);
	wxFileName fn(report->GetDirectory(), "timestamp.my");
	wxFFile file(fn.GetFullPath(), "w");
	if (file.IsOpened())
	{
		wxDateTime dt = wxDateTime::Now();
		file.Write(dt.FormatISODate() + ' ' + dt.FormatISOTime());
		file.Close();
	}

	report->AddFile(fn.GetFullName(), "timestamp of this report");
	report->AddFile((fs::current_path() / "log.txt").c_str(), "Application log");

#ifdef __WXMSW__
	wxString windir;
	if (!wxGetEnv("WINDIR", &windir))
		windir = "C:\\Windows";
	fn.AssignDir(windir);
	fn.AppendDir("system32");
	fn.AppendDir("drivers");
	fn.AppendDir("etc");
#else // !__WXMSW__
	fn.AssignDir("/etc");
#endif // __WXMSW__/!__WXMSW__
	fn.SetFullName("hosts");

	if (fn.FileExists())
		report->AddFile(fn.GetFullPath(), "Local hosts file");
	if (wxDebugReportPreviewStd().Show(*report))
	{
		if (report->Process())
		{
			report->Reset();
		}
	}
	delete report;
}

void pof::Application::OnFatalException()
{
	GenerateReport(wxDebugReport::Context_Exception);
}

void pof::Application::OnUnhandledException()
{
	wxApp::OnUnhandledException();
	auto exp = std::current_exception();
	if (exp) {
		try {
			std::rethrow_exception(exp);
		}
		catch (const std::exception& e) {
			spdlog::critical(e.what());
		}
	}
}

void pof::Application::DecorateSplashScreen(wxBitmap& bmp)
{
	// use a memory DC to draw directly onto the bitmap
	wxMemoryDC memDc(bmp);

	// draw an orange box (with black outline) at the bottom of the splashscreen.
	// this box will be 10% of the height of the bitmap, and be at the bottom.
	const wxRect bannerRect(wxPoint(0, (bmp.GetHeight() / 10) * 9),
		wxPoint(bmp.GetWidth(), bmp.GetHeight()));
	wxDCBrushChanger bc(memDc, wxBrush(wxColour(255, 102, 0)));
	memDc.DrawRectangle(bannerRect);
	memDc.DrawLine(bannerRect.GetTopLeft(), bannerRect.GetTopRight());

	// dynamically get the wxWidgets version to display
	wxString description = wxString::Format("PharmaOffice %s", gVersion);
	// create a copyright notice that uses the year that this file was compiled
	wxString year(__DATE__);
	wxString copyrightLabel = wxString::Format("%s%s D-glopa Nigeria limited. %s",
		wxString::FromUTF8("\xc2\xa9"), year.Mid(year.length() - 4),
		"All rights reserved.");

	// draw the (white) labels inside of our orange box (at the bottom of the splashscreen)
	memDc.SetTextForeground(*wxWHITE);
	// draw the "wxWidget" label on the left side, vertically centered.
	// note that we deflate the banner rect a little bit horizontally
	// so that the text has some padding to its left.
	memDc.DrawLabel(description, bannerRect.Deflate(5, 0), wxALIGN_CENTRE_VERTICAL | wxALIGN_LEFT);

	// draw the copyright label on the right side
	memDc.SetFont(wxFontInfo(8));
	memDc.DrawLabel(copyrightLabel, bannerRect.Deflate(5, 0), wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT);
}

pof::Application::clock_t::time_point pof::Application::FromDateTime(const wxDateTime& dt)
{
	return clock_t::from_time_t(dt.GetTicks());
}

bool pof::Application::HasPrivilage(pof::Account::Privilage priv)
{
	const int idx = static_cast<int>(priv);
	return (MainAccount->priv.test(idx));
}

void pof::Application::ShowSettings()
{
	wxPropertySheetDialog sd;
	sd.SetSheetStyle(wxPROPSHEET_LISTBOOK);
	sd.Create(mMainFrame, wxID_ANY, "Settings", wxDefaultPosition, sd.FromDIP(wxSize(980, 659)));
	sd.Center(wxBOTH);
	sd.CreateButtons(wxOK | wxCANCEL);
	sd.SetBackgroundColour(wxColour(255, 255, 255));
	sd.GetBookCtrl()->SetInternalBorder(5);
	sd.SetSizeHints(sd.FromDIP(wxSize(980, 659)), sd.FromDIP(wxSize(980, 659)));

	//application 
	wxPanel* gpanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* ppanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* apanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* spanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* epanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	//General
	sd.GetBookCtrl()->AddPage(gpanel, "General");
	sd.GetBookCtrl()->AddPage(ppanel, "Pharmacy");
	sd.GetBookCtrl()->AddPage(apanel, "Account");
	sd.GetBookCtrl()->AddPage(spanel, "Sale");
	sd.GetBookCtrl()->AddPage(epanel, "Employees");
	
	
	ShowGeneralSettings(sd);
	ShowPharmacySettings(sd);
	ShowAccountSettings(sd);
	ShowSaleSettings(sd);
	ShowEmployeeSettings(sd);

	//sd.LayoutDialog();
	if (sd.ShowModal() == wxID_OK){
		
		SaveSettings();
	}

}

void pof::Application::ShowGeneralSettings(wxPropertySheetDialog& sd)
{
	auto panel = sd.GetBookCtrl()->GetPage(0);
	if (!panel) return;
	mSettingProperties[0] = new wxPropertyGridManager(panel, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION |
		wxPG_SPLITTER_AUTO_CENTER |
		wxNO_BORDER |
		wxPGMAN_DEFAULT_STYLE);
	mSettingProperties[0]->SetBackgroundColour(*wxWHITE);
	mSettingProperties[0]->SetExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MODE_BUTTONS);
	auto grid = mSettingProperties[0]->AddPage("General", wxNullBitmap);
	grid->GetGrid()->SetBackgroundColour(*wxWHITE);
	grid->GetGrid()->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->GetGrid()->SetCaptionTextColour(*wxBLACK);
	grid->GetGrid()->SetMarginColour(wxTheColourDatabase->Find("Aqua"));

	auto tool = mSettingProperties[0]->GetToolBar();
	if (tool) {
		tool->SetBackgroundColour(*wxWHITE);
	}

	//the grid settings
	auto ct0 = grid->Append(new wxPropertyCategory("Application settings"));
	auto pp0 = grid->Append(new wxBoolProperty("Use local database", "0", bUsingLocalDatabase));
	auto pp1 = grid->Append(new wxBoolProperty("Highlight low stock", "1", bHighlightLowStock));
	auto pp2 = grid->Append(new wxBoolProperty("Use minimum stock count", "2", bUseMinStock));
	auto pp3 = grid->Append(new wxBoolProperty("Show product warnings", "3", bPharamcistWarings));
	auto pp4 = grid->Append(new wxBoolProperty("Check expired on sale", "4", bCheckExpired));
	auto pp5 = grid->Append(new wxBoolProperty("Check out of stock on sale", "5", bCheckOutOfStock));
	auto pp6 = grid->Append(new wxBoolProperty("Check product class on sale", "6", bCheckPOM));
	auto pp7 = grid->Append(new wxBoolProperty("Allow other users to add inventory", "7", bAllowOtherUsersInventoryPermission));
	auto pp8 = grid->Append(new wxBoolProperty("Maximise application", "8", bMaximizeOnLoad));
	auto pp9 = grid->Append(new wxBoolProperty("Brought forward", "9", bAutomaticBroughtForward));
	auto pp10 = grid->Append(new wxBoolProperty("Alert critical warnings", "10", bAlertCriticalWarnings));	
	auto pp11 = grid->Append(new wxBoolProperty("Automatic batch number generation", "11", bAutomaticBatchNumber));	
	auto pp12 = grid->Append(new wxBoolProperty("Notify stock check before end of month", "12", bNotifyStockCheckInComplete));	
	auto pp13 = grid->Append(new wxBoolProperty("Allow sale of controlled medication", "13", bAllowSellControlledMed));	
	auto pp14 = grid->Append(new wxBoolProperty("Create poison book entry for each controlled drug sale", "14", bAlwaysCreateEntryIntoRegister));	
	auto pp17 = grid->Append(new wxBoolProperty("Highlight out of stock in category", "17", bHighlightOutOfStockInCategory));
	auto ct1 = grid->Append(new wxPropertyCategory("Product settings"));
	auto pp15 = grid->Append(new wxArrayStringProperty("Product formulation", "15", FormulationChoices));
	auto pp16 = grid->Append(new wxArrayStringProperty("Product Strength", "16", StrengthChoices));

	
	if (mLocalDatabase){
		pp0->Enable(false);
	}
	grid->SetPropertyHelpString(pp0, "Use localdatabase for data storage");
	grid->SetPropertyHelpString(pp1, "Highlight low stock count on products");
	grid->SetPropertyHelpString(pp2, "Use minimum stock count as an indication of low stock");
	grid->SetPropertyHelpString(pp3, "Show Product warnings on sale");
	grid->SetPropertyHelpString(pp4, "Alert if product as expired on sale");
	grid->SetPropertyHelpString(pp5, "Alert if product is out of stock on sale");
	grid->SetPropertyHelpString(pp6, "Check product class on sale");
	grid->SetPropertyHelpString(pp7, "Allow other users than the sperindent pharmacist to add inventory to stock");
	grid->SetPropertyHelpString(pp8, "Maximize the application on start up");
	grid->SetPropertyHelpString(pp9, "Allow application to do automatic brough forward on inventory");
	grid->SetPropertyHelpString(pp10, "Alert critical warnings on sale");
	grid->SetPropertyHelpString(pp11, "Allow PharmaOffice to generate batch number for inventory entry");
	grid->SetPropertyHelpString(pp12, "Notify the user of stock check incomplete 5 days before end of month");
	grid->SetPropertyHelpString(pp13, "Allow users that are not pharmacist to sell controlled medications and enter in to the register");
	grid->SetPropertyHelpString(pp14, "Create posion book entry for the medications that are controlled");
	grid->SetPropertyHelpString(pp15, "Add or remove product formulations");
	grid->SetPropertyHelpString(pp16, "Add or remove product strength");
	grid->SetPropertyHelpString(pp17, "Highlight out of stock products in category");

	pp0->SetBackgroundColour(*wxWHITE);
	mSettingProperties[0]->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& evt) {
		if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
			wxMessageBox("User account cannot perform this function", "Settings", wxICON_INFORMATION | wxOK);
			evt.Veto();
			return;
		}
	});


	mSettingProperties[0]->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& evt) {
		wxPGProperty* props = evt.GetProperty();
		if (!props || props->IsCategory()) return;
		try {
			size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
			wxVariant v = evt.GetPropertyValue();
			switch (n)
			{
			case 0:
				bUsingLocalDatabase = v.GetBool();
				break;
			case 1:
				bHighlightLowStock = v.GetBool();
				break;
			case 2:
				bUseMinStock = v.GetBool();
				break;
			case 3:
				bPharamcistWarings = v.GetBool();
				break;
			case 4:
				bCheckExpired = v.GetBool();
				break;
			case 5:
				bCheckOutOfStock = v.GetBool();
				break;
			case 6:
				bCheckPOM = v.GetBool();
				break;
			case 7:
				bAllowOtherUsersInventoryPermission = v.GetBool();
				break;
			case 8:
				bMaximizeOnLoad = v.GetBool();
				break;
			case 9:
				bAutomaticBroughtForward = v.GetBool();
				break;
			case 10:
				bAlertCriticalWarnings = v.GetBool();
				break;
			case 11:
				bAutomaticBatchNumber = v.GetBool();
				break;
			case 12:
				bNotifyStockCheckInComplete = v.GetBool();
				break;
			case 13:
				bAllowSellControlledMed = v.GetBool();
				break;
			case 14:
				bAlwaysCreateEntryIntoRegister = v.GetBool();
				break;
			case 15:
			{
				FormulationChoices = v.GetArrayString();
				SaveFormulationChoices();
				mUpdateChoices();
			}
				break;
			case 16:
			{
				StrengthChoices = v.GetArrayString();
				SaveStrengthChoices();
				mUpdateChoices();
			}
			case 17:
				bHighlightOutOfStockInCategory = v.GetBool();
				break;
			default:
				evt.Skip();
				return;
			}
		}
		catch (std::exception& exp){
			spdlog::error(exp.what());
			return;
		}
		evt.Veto();

	});

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	bSizer5->Add(mSettingProperties[0], wxSizerFlags().Expand().Proportion(1).Border(wxALL, 3));
	bSizer5->Fit(mSettingProperties[0]);
	panel->SetSizer(bSizer5);
	panel->Layout();
}

void pof::Application::ShowPharmacySettings(wxPropertySheetDialog& sd)
{
	auto panel = sd.GetBookCtrl()->GetPage(1);
	if (!panel) return;
	mSettingProperties[1] = new wxPropertyGridManager(panel, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION |
		wxPGMAN_DEFAULT_STYLE);
	mSettingProperties[1]->SetExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MODE_BUTTONS);
	mSettingProperties[1]->SetBackgroundColour(*wxWHITE);

	auto grid = mSettingProperties[1]->AddPage("Pharmacy", wxNullBitmap);
	grid->GetGrid()->SetBackgroundColour(*wxWHITE);
	grid->GetGrid()->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->GetGrid()->SetCaptionTextColour(*wxBLACK);
	grid->GetGrid()->SetMarginColour(wxTheColourDatabase->Find("Aqua"));

	auto tool = mSettingProperties[1]->GetToolBar();
	if (tool){
		tool->SetBackgroundColour(*wxWHITE);
	}
	//the grid settings
	auto ct0 = grid->Append(new wxPropertyCategory("Pharmacy details"));
	wxString mPharamcyTypeValueChoices[] = { wxT("COMMUNITY"), wxT("HOSPITAL"), wxT("INDUSTRY"), wxT("WHOLESALE"), wxT("EDUCATIONAL") };
	wxPGChoices ptypes;
	ptypes.Add(5, mPharamcyTypeValueChoices);
	auto pp0 = grid->Append(new wxEnumProperty("Type", "0" , ptypes));
	pp0->SetValue(MainPharmacy->GetPharmacyTypeAsString());
	auto pp1 = grid->Append(new wxStringProperty("Name", "1", MainPharmacy->name));
	pp1->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	auto ct1 = grid->Append(new wxPropertyCategory("Pharmacy Contact"));
	auto pp2 = grid->Append(new wxStringProperty("Phone Number", "2", MainPharmacy->contact.phoneNumber));
	pp2->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	auto pp3 = grid->Append(new wxStringProperty("Email", "3", MainPharmacy->contact.email));
	auto pp4 = grid->Append(new wxStringProperty("Website", "4", MainPharmacy->contact.website));

	auto ct2 = grid->Append(new wxPropertyCategory("Pharmacy Address"));

	auto pp5 = grid->Append(new wxStringProperty("No", "5", MainPharmacy->addy.number));
	pp5->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	auto pp6 = grid->Append(new wxStringProperty("Street", "6", MainPharmacy->addy.street));
	auto pp7 = grid->Append(new wxStringProperty("City", "7", MainPharmacy->addy.city));
	auto pp8 = grid->Append(new wxStringProperty("L.G.A", "8", MainPharmacy->addy.lga));
	auto pp9 = grid->Append(new wxStringProperty("State", "9", MainPharmacy->addy.state));
	auto pp10 = grid->Append(new wxStringProperty("Country", "10", MainPharmacy->addy.country));


	grid->SetPropertyHelpString(pp0, "Change the Type of the pharmacy");
	grid->SetPropertyHelpString(pp1, "Change the Name of The pharmacy");
	grid->SetPropertyHelpString(pp2, "Change the Phone Number of the pharmacy");
	grid->SetPropertyHelpString(pp3, "Change the Email of the pharmacy");
	grid->SetPropertyHelpString(pp4, "Change the Website of the pharmacy");
	
	grid->SetPropertyHelpString(pp5, "Change the House no address of the pharmacy");
	grid->SetPropertyHelpString(pp6, "Change the Street address of the pharmacy");
	grid->SetPropertyHelpString(pp7, "Change the City address of the pharmacy");
	grid->SetPropertyHelpString(pp8, "Change the L.G.A address of the pharmacy");
	grid->SetPropertyHelpString(pp9, "Change the State address of the pharmacy");
	grid->SetPropertyHelpString(pp10, "Change the Country address of the pharmacy");


	
	grid->SetPropertyBackgroundColour(pp0, *wxWHITE);
	mSettingProperties[1]->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& evt) {
		if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
			wxMessageBox("User account cannot perform this function", "Settings", wxICON_INFORMATION | wxOK);
			evt.Veto();
			return;
		}
		wxPGProperty* props = evt.GetProperty();
		if (!props || props->IsCategory()) return;
		try {
			size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
			wxVariant v = evt.GetPropertyValue();
			switch (n)
			{
			case 2: {
				auto phonenumer = v.GetString().ToStdString();
				if (phonenumer.size() != 11){
					wxMessageBox("Phone number is not complete, please enter valid phone number", "Settings", wxICON_WARNING | wxOK);
					evt.Veto();
				}
			}
				break;
			case 3: {
				auto email = v.GetString().ToStdString();
				const std::regex rex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
				if (!std::regex_match(email, rex)) {
					wxMessageBox("Invalid Email Address", "Settings", wxICON_WARNING | wxOK);
					evt.Veto();
					return;
				}
			}
				break;
			default:
				break;
			}
		}
		catch (std::exception& exp) {
			spdlog::error(exp.what());
			return;
		}

	});


	mSettingProperties[1]->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& evt) {
		if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
			wxMessageBox("User account cannot perform this function", "Product Information", wxICON_INFORMATION | wxOK);
			evt.Skip();
			return;
		}
		
		wxPGProperty* props = evt.GetProperty();
		if (!props || props->IsCategory()) return;
		try {
			size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
			wxVariant v = evt.GetPropertyValue();
			switch (n)
			{
			case 0: {
				int sel = v.GetInteger();
				if (sel == wxNOT_FOUND) break;
				MainPharmacy->pharmacyType.reset();
				MainPharmacy->pharmacyType.set(sel);
			}
				break;
			case 1:
				MainPharmacy->name = std::move(v.GetString().ToStdString());
				break;
			case 2:
				MainPharmacy->contact.phoneNumber = std::move(v.GetString().ToStdString());
				break;
			case 3:
				MainPharmacy->contact.email = std::move(v.GetString().ToStdString());
				break;
			case 4:
				MainPharmacy->contact.website = std::move(v.GetString().ToStdString());
				break;
			case 5:
				MainPharmacy->addy.number = std::move(v.GetString().ToStdString());
				break;
			case 6:
				MainPharmacy->addy.street = std::move(v.GetString().ToStdString());
				break;
			case 7:
				MainPharmacy->addy.city = std::move(v.GetString().ToStdString());
				break;
			case 8:
				MainPharmacy->addy.lga = std::move(v.GetString().ToStdString());
				break;
			case 9:
				MainPharmacy->addy.state = std::move(v.GetString().ToStdString());
				break;
			case 10:
				MainPharmacy->addy.country = std::move(v.GetString().ToStdString());
				break;
			default:
				break;
			}
			MainPharmacy->updateSig(*MainPharmacy);
		}
		catch (std::exception& exp) {
			spdlog::error(exp.what());
			return;
		}
		mMainFrame->UpdateWelcomePage();
	});
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	bSizer5->Add(mSettingProperties[1], wxSizerFlags().Expand().Proportion(1).Border(wxALL, 3));

	bSizer5->Fit(mSettingProperties[1]);
	panel->SetSizer(bSizer5);
	panel->Layout();
}

void pof::Application::ShowAccountSettings(wxPropertySheetDialog& sd)
{
	auto panel = sd.GetBookCtrl()->GetPage(2);
	if (!panel) return;
	mSettingProperties[2] = new wxPropertyGridManager(panel, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION |
		wxPGMAN_DEFAULT_STYLE);
	mSettingProperties[2]->SetExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MODE_BUTTONS);

	auto grid = mSettingProperties[2]->AddPage("Account", wxNullBitmap);
	grid->GetGrid()->SetBackgroundColour(*wxWHITE);
	grid->GetGrid()->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->GetGrid()->SetCaptionTextColour(*wxBLACK);
	grid->GetGrid()->SetMarginColour(wxTheColourDatabase->Find("Aqua"));

	//create tool bar
	auto tool = mSettingProperties[2]->GetToolBar();
	if (tool) {
		tool->SetBackgroundColour(*wxWHITE);
		tool->SetWindowStyleFlag(wxTB_HORZ_TEXT);
		tool->AddStretchableSpace();
		tool->AddSeparator();
		tool->AddTool(ID_RESET_ACCOUNT, "Reset password", wxArtProvider::GetBitmap("application"));
		tool->AddTool(ID_DELETE_ACCOUNT, "Delete Account", wxArtProvider::GetBitmap("action_delete"));
		tool->Bind(wxEVT_TOOL, std::bind_front(&pof::Application::OnResetAccount, this), ID_RESET_ACCOUNT);
		tool->Bind(wxEVT_TOOL, std::bind_front(&pof::Application::OnDeleteAccount, this), ID_DELETE_ACCOUNT);
		
		tool->Realize();
		mSettingProperties[2]->Update();
	}
	//the grid settings
	auto ct0 = grid->Append(new wxPropertyCategory("User account details"));
	auto tt0 = grid->Append(new wxStringProperty("Account Type", "at", MainAccount->GetAccountTypeString()));
	auto pp0 = grid->Append(new wxStringProperty("Name", "0", MainAccount->name));
	pp0->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	auto pp1 = grid->Append(new wxStringProperty("Last Name", "1", MainAccount->lastname));
	pp1->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	auto pp2 = grid->Append(new wxStringProperty("Username", "2", MainAccount->username));
	pp1->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	auto pp3 = grid->Append(new wxStringProperty("Email", "3", MainAccount->email));
	auto pp4 = grid->Append(new wxStringProperty("Phone Number", "4", MainAccount->phonenumber));
	pp4->SetValidator(wxTextValidator{wxFILTER_DIGITS});

	auto pp5 = grid->Append(new wxStringProperty("Reg Number", "5", MainAccount->regnumber));

	tt0->Enable(false);
	pp2->Enable(false);

	grid->SetPropertyHelpString(tt0, "Account type. This cannot be changed");
	grid->SetPropertyHelpString(pp0, "Change account Name");
	grid->SetPropertyHelpString(pp1, "Change account Last Name");
	grid->SetPropertyHelpString(pp1, "Account Username. This cannot be changed");
	grid->SetPropertyHelpString(pp3, "Change account email");
	grid->SetPropertyHelpString(pp4, "Change phone number");
	grid->SetPropertyHelpString(pp5, "Change reg number");

	mSettingProperties[2]->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& evt) {
		wxPGProperty* props = evt.GetProperty();
		if (!props || props->IsCategory()) return;
		try {
		size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
		wxVariant v = evt.GetPropertyValue();
		switch (n)
		{
		case 3:{
			auto email = v.GetString().ToStdString();
			const std::regex rex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
			if (!std::regex_match(email, rex)) {
				wxMessageBox("Invalid Email Address", "Settings", wxICON_WARNING | wxOK);
				evt.Veto();
				return;
			}
		}
			break;
		case 4: {
			auto phonenumber = v.GetString().ToStdString();
			if (phonenumber.size() != 11) {
				//cannot properly validate number just the count
				wxMessageBox("Phone number is not complete, please enter valid phone number", "Settings", wxICON_WARNING | wxOK);
				evt.Veto();
				return;
			}
		}
			break;
		default:
			break;
		}
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}
	});


	mSettingProperties[2]->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& evt) {
	wxPGProperty* props = evt.GetProperty();
	if (!props || props->IsCategory()) return;
	try {
		size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
		wxVariant v = evt.GetPropertyValue();
		switch (n)
		{
		case 0:
			MainAccount->name = v.GetString().ToStdString();
			break;
		case 1:
			MainAccount->lastname = v.GetString().ToStdString();
			break;
		case 3:
			MainAccount->email = v.GetString().ToStdString();
			break;
		case 4:
			MainAccount->phonenumber = v.GetString().ToStdString();
			break;
		case 5:
			MainAccount->regnumber = v.GetString().ToStdString();
			break;
		default:
			break;
		}
		MainAccount->UpdateAccount();
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}

	});
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	bSizer5->Add(mSettingProperties[2], wxSizerFlags().Expand().Proportion(1).Border(wxALL, 3));

	bSizer5->Fit(mSettingProperties[2]);
	panel->SetSizer(bSizer5);
	panel->Layout();
}

void pof::Application::ShowSaleSettings(wxPropertySheetDialog& sd)
{
	auto panel = sd.GetBookCtrl()->GetPage(3);
	if (!panel) return;
	mSettingProperties[3] = new wxPropertyGridManager(panel, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION |
		wxPGMAN_DEFAULT_STYLE);
	mSettingProperties[3]->SetExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MODE_BUTTONS);

	auto grid = mSettingProperties[3]->AddPage("Sale", wxNullBitmap);
	grid->GetGrid()->SetBackgroundColour(*wxWHITE);
	grid->GetGrid()->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->GetGrid()->SetCaptionTextColour(*wxBLACK);
	grid->GetGrid()->SetMarginColour(wxTheColourDatabase->Find("Aqua"));
	//the grid settings
	auto tool = mSettingProperties[3]->GetToolBar();
	if (tool) {
		tool->SetBackgroundColour(*wxWHITE);
	}

	grid->Append(new wxPropertyCategory("Sale Printer"));
	auto pp0 = grid->Append(new wxBoolProperty("Show preview on Sale", "0", bShowPreviewOnSale));
	auto pp1 = grid->Append(new wxBoolProperty("Show print prompt on Sale", "1", bShowPrintPrompt));
	auto pp2 = grid->Append(new wxBoolProperty("Show print page setup", "2", bShowPageSetup));
	auto pp3 = grid->Append(new wxBoolProperty("Use saved costs", "3", bUseSavedCost));

	grid->SetPropertyHelpString(pp0, "Show the receipt as preview before printing.");
	grid->SetPropertyHelpString(pp1, "Show printing prompt before printing.");
	grid->SetPropertyHelpString(pp2, "Show printing page setup before printing.");
	grid->SetPropertyHelpString(pp3, "Use the saved cost for calulating profit/loss. If false, it would use the current cost of the item.");
	mSettingProperties[3]->Bind(wxEVT_PG_CHANGING, [&](wxPropertyGridEvent& evt) {
		if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
			wxMessageBox("User account cannot perform this function", "Settings", wxICON_INFORMATION | wxOK);
			evt.Veto();
			return;
		}
	});
	
	mSettingProperties[3]->Bind(wxEVT_PG_CHANGED, [&](wxPropertyGridEvent& evt) {
	wxPGProperty* props = evt.GetProperty();
	if (!props || props->IsCategory()) return;
	try {
		size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName().ToStdString());
		wxVariant v = evt.GetPropertyValue();
		switch (n)
		{
		case 0:
			bShowPreviewOnSale = v.GetBool();
			break;
		case 1:
			bShowPrintPrompt = v.GetBool();
			break;
		case 2:
			bShowPageSetup = v.GetBool();
		case 3:
			bUseSavedCost = v.GetBool();
			break;
		default:
			break;
		}
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}

	});

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	bSizer5->Add(mSettingProperties[3], wxSizerFlags().Expand().Proportion(1).Border(wxALL, 3));

	bSizer5->Fit(mSettingProperties[3]);
	panel->SetSizer(bSizer5);
	panel->Layout();
}

void pof::Application::ShowEmployeeSettings(wxPropertySheetDialog& sd)
{
	auto panel = sd.GetBookCtrl()->GetPage(4);
	if (!panel) return;
	wxSizer* s = new wxBoxSizer(wxVERTICAL);

	auto users = MainAccount->GetAllAccounts();
	if (!users) return;

	wxDataViewCtrl* view = new wxDataViewCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	auto data = std::make_shared<pof::base::data>(std::move(users.value()));
	pof::DataModel* model = new pof::DataModel(data);
	
	//haram code
	view->AppendTextColumn("Username", 7, wxDATAVIEW_CELL_INERT, sd.FromDIP(150), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	view->AppendTextColumn("First Name", 2, wxDATAVIEW_CELL_INERT, sd.FromDIP(150), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	view->AppendTextColumn("Last Name", 3, wxDATAVIEW_CELL_INERT, sd.FromDIP(150), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	view->AppendTextColumn("Phonenumber", 4, wxDATAVIEW_CELL_INERT, sd.FromDIP(150), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	view->AppendTextColumn("Email", 5, wxDATAVIEW_CELL_INERT, sd.FromDIP(150), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	view->AppendTextColumn("Type", 100, wxDATAVIEW_CELL_INERT, sd.FromDIP(150), wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);

	view->AssociateModel(model);
	model->Reload();
	model->DecRef();


	pof::DataModel::SpeicalColHandler_t handle;
	handle.first = [data](size_t row, size_t col) -> wxVariant {
		if (row >= data->size()) return wxString{};
		auto& priv = boost::variant2::get<std::uint32_t>((*data)[row].first[1]);
		std::bitset<32> p(priv);
		for (int i = 0; i < pof::Account::max_account_type; i++) {
			if (p.test(i)) {
				return std::string(pof::Account::account_type_string[i]);
			}
		}

		return wxString{};
	};
	model->SetSpecialColumnHandler(100, std::move(handle));

	view->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, [v = view,data](wxDataViewEvent& evt) {
		auto item = evt.GetItem();
		if (!item.IsOk()) return;
		auto& row = (*data)[pof::DataModel::GetIdxFromItem(item)];

		wxMenu* menu = new wxMenu;
		menu->Append(1, "Delete account");
		menu->Bind(wxEVT_MENU, [&](wxCommandEvent& evt) {
			if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
				wxMessageBox("User account cannot perform this function", "Settings", wxICON_INFORMATION | wxOK);
				return;
			}
			if (wxMessageBox("Are you sure you want to delete this account?\nDeleting this account removes all data associated with this account.", "Delete account", wxICON_WARNING | wxYES_NO) == wxNO) return;
			wxGetApp().MainAccount->DeleteAccount(row);
			
		}, 1);
		v->PopupMenu(menu);
	});


	s->Add(view, wxSizerFlags().Proportion(1).Border(wxALL, 5).Expand());
	panel->SetSizer(s);
	s->Fit(panel);

}

void pof::Application::OnResetAccount(wxCommandEvent& evt)
{
	wxMessageBox("Current version cannot reset account", "Application", wxICON_WARNING | wxOK);

}
void pof::Application::OnDeleteAccount(wxCommandEvent& evt) {
	if (wxMessageBox("Are you sure you want to delete account", "Application", wxICON_WARNING | wxYES_NO)
		== wxNO) return;


}

void pof::Application::SaveReceiptPageSettings() const
{
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/ReceiptSettings"));
	config->Write(wxT("leftMargin"), leftMargin);
	config->Write(wxT("topMargin"), topMargin);
	config->Write(wxT("bottomMargin"), bottomMargin);
	config->Write(wxT("rightMargin"), leftMargin);
	config->Write(wxT("copies"), copies);
	config->Write(wxT("paperSize"), static_cast<int>(paperSize));
	
}

void pof::Application::LoadReceiptPageSettings()
{
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/ReceiptSettings"));
	config->Read(wxT("leftMargin"), &leftMargin);
	config->Read(wxT("topMargin"), &topMargin);
	config->Read(wxT("bottomMargin"), &bottomMargin);
	config->Read(wxT("rightMargin"), &rightMargin);
	config->Read(wxT("copies"), &copies);
	int ps = 0;
	config->Read(wxT("paperSize"), &ps);
	paperSize = static_cast<wxPaperSize>(ps);
}

void pof::Application::SaveFont()
{
	wxFont f = mFontSettings.GetChosenFont();
	wxConfigBase* config = wxConfigBase::Get();
	
	config->SetPath(wxT("/Font"));
	if (f.IsOk()) {
		config->Write(wxT("PointSize"), f.GetPointSize());
		config->Write(wxT("Family"), static_cast<int>(f.GetFamily()));
		config->Write(wxT("Style"), static_cast<int>(f.GetStyle()));
		config->Write(wxT("Face"), f.GetFaceName());
	}

	f = mReceiptFontSettings.GetChosenFont();
	if (f.IsOk()) {
		config->Write(wxT("ReceiptPointSize"), f.GetPointSize());
		config->Write(wxT("ReceiptFamily"), static_cast<int>(f.GetFamily()));
		config->Write(wxT("ReceiptStyle"), static_cast<int>(f.GetStyle()));
		config->Write(wxT("ReceiptFace"), f.GetFaceName());
	}
}

void pof::Application::LoadFont()
{
	wxFont f;

	wxConfigBase* config = wxConfigBase::Get();
	wxString readData;

	int pointsize = 0;
	int ff = 0;
	int style = 0;

	config->SetPath(wxT("/Font"));
	config->Read(wxT("PointSize"), &pointsize);
	config->Read(wxT("Style"), &style);
	config->Read(wxT("Family"), &ff);
	config->Read(wxT("Face"), &readData);

	if (pointsize != 0) {
		f.SetPointSize(pointsize);
		f.SetFamily(ff);
		f.SetStyle(style);
		f.SetFaceName(readData);
		mFontSettings.SetChosenFont(f);
	}

	//receipt font
	config->Read(wxT("ReceiptPointSize"), &pointsize);
	config->Read(wxT("ReceiptStyle"), &style);
	config->Read(wxT("ReceiptFamily"), &ff);
	config->Read(wxT("ReceiptFace"), &readData);
	if (pointsize != 0) {
		f.SetPointSize(pointsize);
		f.SetFamily(ff);
		f.SetStyle(style);
		f.SetFaceName(readData);
		mReceiptFontSettings.SetChosenFont(f);
	}
}

void pof::Application::DBFuncWeek(pof::base::database::conn_t conn, int arg, pof::base::database::value_arr_t values) {
	std::uint64_t dur = pof::base::database::arg<std::uint64_t>(conn, values);
	auto tt = pof::base::data::datetime_t(pof::base::data::datetime_t::duration(dur));
	auto week = date::floor<date::weeks>(tt);

	pof::base::database::result(conn, static_cast<std::uint64_t>(week.time_since_epoch().count()));
}

void pof::Application::LoadFormulationChoices()
{
	auto p = std::filesystem::current_path() / ".data" / "mics.json";
	if (!fs::exists(p)) {
		FormulationChoices.Add("NOT SPECIFIED");
		FormulationChoices.Add("TABLET");
		FormulationChoices.Add("CAPSULE");
		FormulationChoices.Add("SOLUTION");
		FormulationChoices.Add("SUSPENSION");
		FormulationChoices.Add("SYRUP");
		FormulationChoices.Add("IV");
		FormulationChoices.Add("IM");
		FormulationChoices.Add("EMULSION");
		FormulationChoices.Add("COMSUMABLE");
		FormulationChoices.Add("POWDER"); //needles, cannula and the rest
		FormulationChoices.Add("OINTMNET"); //needles, cannula and the rest
		FormulationChoices.Add("EYE DROP"); //needles, cannula and the rest
		FormulationChoices.Add("SUPPOSITORY"); //needles, cannula and the rest
		FormulationChoices.Add("LOZENGES"); //needles, cannula and the rest
		FormulationChoices.Add("OIL"); //needles, cannula and the rest

		SaveFormulationChoices();
		return;
	}
	std::fstream file(p, std::ios::in);
	if (!file.is_open()) {
		spdlog::error("Failed to open misc file for reading");
	}
	std::stringstream os;
	os << file.rdbuf();
	file.close();
	
	nl::json obj = nl::json::parse(os.str());

	auto i = obj.find("formchoices");
	if (i == obj.end()) {
		FormulationChoices.Add("NOT SPECIFIED");
		FormulationChoices.Add("TABLET");
		FormulationChoices.Add("CAPSULE");
		FormulationChoices.Add("SOLUTION");
		FormulationChoices.Add("SUSPENSION");
		FormulationChoices.Add("SYRUP");
		FormulationChoices.Add("IV");
		FormulationChoices.Add("IM");
		FormulationChoices.Add("EMULSION");
		FormulationChoices.Add("COMSUMABLE");
		FormulationChoices.Add("POWDER"); //needles, cannula and the rest
		FormulationChoices.Add("OINTMNET"); //needles, cannula and the rest
		FormulationChoices.Add("EYE DROP"); //needles, cannula and the rest
		FormulationChoices.Add("SUPPOSITORY"); //needles, cannula and the rest
		FormulationChoices.Add("LOZENGES"); //needles, cannula and the rest
		FormulationChoices.Add("OIL"); //needles, cannula and the rest

		SaveFormulationChoices();
		return;
	}

	for (auto& s : *i){
		FormulationChoices.Add(static_cast<std::string>(s));
	}
}

void pof::Application::AddFormulationChoices(const std::string& choice)
{
	FormulationChoices.insert(FormulationChoices.end() - 2, choice);
	SaveFormulationChoices();
}

void pof::Application::SaveFormulationChoices()
{
	auto p = std::filesystem::current_path() / ".data" / "mics.json";
	bool ex = fs::exists(p);
	std::stringstream str;
	nl::json obj = nl::json::object();
	nl::json formchoices = nl::json::array();
	
	//if it exists read and parse it
	if (ex){
		std::fstream file(p, std::ios::in);
		if (!file.is_open()) {
			spdlog::error("Filed to open miscs file for writing");
			return;
		}
		str << file.rdbuf();
		obj = nl::json::parse(str.str());
	}

	std::fstream file(p, std::ios::out);
	if (!file.is_open()) {
		spdlog::error("Filed to open miscs file for writing");
		return;
	}
	for (auto& form : FormulationChoices) {
		formchoices.push_back(form.ToStdString());
	}
	obj["formchoices"] = formchoices;
	file << obj.dump();
}

void pof::Application::UpdateFormulationChoices(const wxArrayString& choices)
{
	FormulationChoices = choices;
	SaveFormulationChoices();
}

void pof::Application::LoadStrengthChoices()
{
	auto p = std::filesystem::current_path() / ".data" / "mics.json";
	if (!fs::exists(p)){
		StrengthChoices.Add("NOT SPECIFIED");
		StrengthChoices.Add("g");
		StrengthChoices.Add("mg");
		StrengthChoices.Add("mcg");

		StrengthChoices.Add("L");
		StrengthChoices.Add("ml");
		StrengthChoices.Add("ml");

		StrengthChoices.Add("%v/v");
		StrengthChoices.Add("%w/v");

		StrengthChoices.Add("mol");
		StrengthChoices.Add("mmol");

		SaveStrengthChoices();
		return;
	}
	std::fstream file(p, std::ios::in);
	if (!file.is_open()) {
		spdlog::error("Failed to open misc file for reading");
	}
	std::stringstream os;
	os << file.rdbuf();
	file.close();

	nl::json obj = nl::json::parse(os.str());
	auto i = obj.find("strengthchoices");
	if (i == obj.end()){
		StrengthChoices.Add("NOT SPECIFIED");
		StrengthChoices.Add("g");
		StrengthChoices.Add("mg");
		StrengthChoices.Add("mcg");

		StrengthChoices.Add("L");
		StrengthChoices.Add("ml");
		StrengthChoices.Add("ml");

		StrengthChoices.Add("%v/v");
		StrengthChoices.Add("%w/v");

		StrengthChoices.Add("mol");
		StrengthChoices.Add("mmol");

		SaveStrengthChoices();
		return;
	}

	for (auto& c : *i) {
		StrengthChoices.Add(static_cast<std::string>(c));
	}
}

void pof::Application::SaveStrengthChoices()
{
	auto p = std::filesystem::current_path() / ".data" / "mics.json";
	bool ex = fs::exists(p);
	std::stringstream str;
	nl::json obj = nl::json::object();
	nl::json formchoices = nl::json::array();

	//cannot open for in and out
	if (ex) {
		std::fstream file(p, std::ios::in);
		if (!file.is_open()) {
			spdlog::error("Filed to open miscs file for writing");
			return;
		}
		str << file.rdbuf();
		obj = nl::json::parse(str.str());
	}

	std::fstream file(p, std::ios::out);
	if (!file.is_open()) {
		spdlog::error("Filed to open miscs file for writing");
		return;
	}
	for (auto& form : StrengthChoices) {
		formchoices.push_back(form.ToStdString());
	}
	obj["strengthchoices"] = formchoices;
	file << obj.dump();
}

pof::RegexValidator::RegexValidator(std::regex&& reg, const std::string& errorstr)
: pattern(reg), estr(errorstr){

}

wxObject* pof::RegexValidator::Clone() const
{
	auto p = pattern;
	auto s = estr;
	auto sp = new RegexValidator(std::move(p), s);
	return sp;
}

bool pof::RegexValidator::TransferFromWindow()
{
	return true;
}

bool pof::RegexValidator::TransferToWindow()
{
	return true;
}

bool pof::RegexValidator::Validate(wxWindow* parent)
{
	wxTextCtrl* control = dynamic_cast<wxTextCtrl*>(parent);
	if (!control) return false;


	auto&& v = control->GetValue().ToStdString();
	if (!v.empty() && !std::regex_match(v, pattern)) {
		wxMessageBox(estr, "Validator", wxICON_WARNING | wxOK);
		return false;
	}
	return true;
}

wxString pof::RegexValidator::IsValid(const wxString& val) const
{
	if (!std::regex_match(val.ToStdString(), pattern)) {
		return estr;
	}
	return wxEmptyString;
}
