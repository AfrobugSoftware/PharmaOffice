//#include "Application.h"
#include "PofPch.h"
#include <net.h>
#include <errc.h>
#include <string>
#include <spdlog/spdlog.h>
#include <thread>

#include <fstream>

//#include <random>
//
//#include <data.h>
//#include <data_tuple.h>
//#include <packages.h>



//#include "DataModel.h"
//
//#include "datatree.h"

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

	//load the settings
	LoadSettings();

	gVersion = "1.0.0 pre beta";
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

	//TestAccountAndPharmacy();
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

	SaveSettings();
	mProductManager.UpdateTimeCheck(today);
	return CreateMainFrame();
}

int pof::Application::OnExit()
{
	wxApp::OnExit();

	mNetManager.stop();
	if (bUsingLocalDatabase) {
		mLocalDatabase->flush_db();
		sqlite3_shutdown();
	}
	SaveSettings();
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
	config->SetPath(wxT("/mainframe"));
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
	config->Write(wxT("Version"), wxString(gVersion));


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
	wxString version;
	config->Read(wxT("Version"), &version);
	gVersion = version;
	
	config->SetPath(wxT("/mainframe"));
	int x, y, w, h;
	x = y = w = h = -1;
	config->Read(wxT("PosX"), &x);
	config->Read(wxT("PosY"), &y);
	config->Read(wxT("SizeW"), &w);
	config->Read(wxT("SizeH"), &h);



	//config->SetPath(wxT("/"));
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

	//the grid settings
	auto pp0 = grid->Append(new wxBoolProperty("Test Settings", "Test Settings", true));
	grid->SetPropertyHelpString(pp0, "This is a help test string");
	pp0->SetBackgroundColour(*wxWHITE);


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

	auto grid = mSettingProperties[1]->AddPage("General", wxNullBitmap);
	//the grid settings
	auto ct0 = grid->Append(new wxPropertyCategory("Pharmacy details"));
	wxString mPharamcyTypeValueChoices[] = { wxT("COMMUNITY"), wxT("HOSPITAL"), wxT("INDUSTRY"), wxT("WHOLESALE"), wxT("EDUCATIONAL") };
	wxPGChoices ptypes;
	ptypes.Add(5, mPharamcyTypeValueChoices);
	auto pp0 = grid->Append(new wxEnumProperty("Type", "ptypes" , ptypes));
	auto pp1 = grid->Append(new wxStringProperty("Name", "pname", MainPharmacy->name));
	auto pType = MainPharmacy->GetPharmacyType();
	size_t idx = pType.to_ullong() % (1ull << 5);
	if (idx >= 5) idx = 0;
	pp0->SetValue(wxVariant(mPharamcyTypeValueChoices[idx]));


	auto pp2 = grid->Append(new wxStringProperty("Phone Number", "pnum", MainPharmacy->contact.phoneNumber));
	auto pp3 = grid->Append(new wxStringProperty("Email", "pemail", MainPharmacy->contact.email));
	auto pp4 = grid->Append(new wxStringProperty("Website", "pwebsite", MainPharmacy->contact.website));

	auto ct1 = grid->Append(new wxPropertyCategory("Pharmacy Address"));

	auto pp5 = grid->Append(new wxStringProperty("No", "no", MainPharmacy->addy.number));
	auto pp6 = grid->Append(new wxStringProperty("Street", "street", MainPharmacy->addy.street));
	auto pp7 = grid->Append(new wxStringProperty("City", "city", MainPharmacy->addy.city));
	auto pp8 = grid->Append(new wxStringProperty("L.G.A", "lga", MainPharmacy->addy.lga));
	auto pp9 = grid->Append(new wxStringProperty("State", "state", MainPharmacy->addy.state));
	auto pp10 = grid->Append(new wxStringProperty("Country", "country", MainPharmacy->addy.country));


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


	
	grid->SetPropertyBackgroundColour("ptypes", *wxWHITE);
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

	auto grid = mSettingProperties[2]->AddPage("General", wxNullBitmap);

	//the grid settings
	grid->Append(new wxBoolProperty("Test Settings", "Test Settings", true));

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

	//the grid settings
	grid->Append(new wxBoolProperty("Test Settings", "Test Settings", true));

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	bSizer5->Add(mSettingProperties[3], wxSizerFlags().Expand().Proportion(1).Border(wxALL, 3));

	bSizer5->Fit(mSettingProperties[3]);
	panel->SetSizer(bSizer5);
	panel->Layout();
}