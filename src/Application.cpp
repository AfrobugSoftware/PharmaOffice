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
	if (!wxApp::OnInit()) return false;
	mChecker = std::make_unique<wxSingleInstanceChecker>();
	if (mChecker->IsAnotherRunning()) {
		//..find out how to pop up the current running application
		mChecker.reset(nullptr);
		return false;
	}
	//SetUseBestVisual(true, true);


	//load the settings
	LoadSettings();

	gVersion = "1.0.1 pre beta";
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
	
	if (MainPharmacy->name.empty()){
		//lunch register pharmacy
		bool registerd = RegisterPharmacyLocal();
		if (!registerd) {
			//wxMessageBox("Failed to register pharamcy, cannot start PharmaOffice", "PharamOffice", wxICON_ERROR | wxOK);
			OnExit();
			return false;
		}
	}
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
		//check if in active session, if we are asked to keep the signin
		bool ssin = false;
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
	else {
		if (!SignIn()) {
			//failed signed in
			OnExit();
			return false;
		}
		//test	
	}

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
	bool status = CreateMainFrame();
	
	SaveSettings();
	return status;
}

int pof::Application::OnExit()
{
	wxApp::OnExit();

	mNetManager.stop();
	if (bUsingLocalDatabase) {
		mLocalDatabase->flush_db();
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

		pof::base::func_aggregate CostMulti;
		CostMulti.name = "CostMulti";
		CostMulti.arg_count = 2;
		CostMulti.func = pof::base::cost_multi;

		mLocalDatabase->register_func(costAgg);
		mLocalDatabase->register_func(monthFunc);
		mLocalDatabase->register_func(daysFunc);
		mLocalDatabase->register_func(scaleCost);
		mLocalDatabase->register_func(CostMulti);
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
			wxID_ANY, wxDefaultPosition, wxSize(1422, 762));
	}
	else {
		mMainFrame = new pof::MainFrame(nullptr,
			wxID_ANY, wxPoint(x,y), wxSize(w, h));
	}



	mMainFrame->mAccount = MainAccount;
	mMainFrame->Center(wxBOTH);
	if (bMaximizeOnLoad) mMainFrame->Maximize();
	return mMainFrame->Show();
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
	MainAccount->mLocalDatabase = mLocalDatabase;

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
	wxPoint pos = mMainFrame->GetPosition();
	wxSize size = mMainFrame->GetSize();

	config->Write(wxT("PosX"), pos.x);
	config->Write(wxT("PosY"), pos.y);
	config->Write(wxT("SizeW"), size.x);
	config->Write(wxT("SizeH"), size.y);

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
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, fmt::format("{} signed in at {:%d/%M/%Y %H:%M:%S}", MainAccount->username, MainAccount->signintime));
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
	mSaleManager.CreateSaveSaleTable();

	MainAccount->CreateAccountInfoTable();
	MainAccount->CreateSessionTable();
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
	sd.Create(mMainFrame, wxID_ANY, "Settings", wxDefaultPosition, wxSize(980, 659));
	sd.Center(wxBOTH);
	sd.CreateButtons(wxOK | wxCANCEL);
	sd.SetBackgroundColour(wxColour(255, 255, 255));
	sd.GetBookCtrl()->SetInternalBorder(10);
	sd.SetSizeHints(wxSize(980, 659), wxSize(980, 659));

	//application 
	wxPanel* gpanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* ppanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* apanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	wxPanel* spanel = new wxPanel(sd.GetBookCtrl(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	//General
	sd.GetBookCtrl()->AddPage(gpanel, "General");
	sd.GetBookCtrl()->AddPage(ppanel, "Pharmacy");
	sd.GetBookCtrl()->AddPage(apanel, "Account");
	sd.GetBookCtrl()->AddPage(spanel, "Sale");
	
	
	ShowGeneralSettings(sd);
	ShowPharmacySettings(sd);
	ShowAccountSettings(sd);
	ShowSaleSettings(sd);

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
		tool->AddTool(wxID_ANY, "Reset password", wxArtProvider::GetBitmap("application"));
		tool->AddTool(wxID_ANY, "Delete Account", wxArtProvider::GetBitmap("action_delete"));
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
	auto pp0 = grid->Append(new wxBoolProperty("Show Preview on Sale", "0", bShowPreviewOnSale));
	auto pp1 = grid->Append(new wxBoolProperty("Show Print prompt on Sale", "1", bShowPrintPrompt));

	grid->SetPropertyHelpString(pp0, "Show the receipt as preview before printing");
	grid->SetPropertyHelpString(pp1, "Show printing prompt before printing");
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