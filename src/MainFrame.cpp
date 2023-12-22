#include "MainFrame.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::MainFrame, wxFrame)
	EVT_CLOSE(pof::MainFrame::OnClose)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_IMPORT_JSON, pof::MainFrame::OnImportJson)
	EVT_MENU(pof::MainFrame::ID_MENU_HELP_ABOUT, pof::MainFrame::OnAbout)
	EVT_MENU(pof::MainFrame::ID_MENU_HELP_SETTINGS, pof::MainFrame::OnShowSettings)
	EVT_MENU(pof::MainFrame::ID_MENU_VIEW_LOG, pof::MainFrame::OnShowLog)
	EVT_MENU(pof::MainFrame::ID_MENU_EXPORT_JSON, pof::MainFrame::OnExportJson)
	EVT_MENU(pof::MainFrame::ID_MENU_EXPORT_EXCEL, pof::MainFrame::OnExportExcel)
	EVT_MENU(pof::MainFrame::ID_MENU_EXPORT_CSV, pof::MainFrame::OnExportCSV)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_SAVE, pof::MainFrame::OnTestSave)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_LOAD, pof::MainFrame::OnTestLoad)
	EVT_MENU(pof::MainFrame::ID_MENU_VIEW_SHOW_MODULES, pof::MainFrame::OnShowModules)
	EVT_MENU(pof::MainFrame::ID_MENU_ACCOUNT_SIGN_OUT, pof::MainFrame::OnSignOut)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_IMPORT_EXCEL, pof::MainFrame::OnImportExcel)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_MARKUP_SETTINGS, pof::MainFrame::OnMarkupSettings)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_NOTIF_OS, pof::MainFrame::OnNotif)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_NOTIF_EXPIRE, pof::MainFrame::OnNotif)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_SALE_ALERTS_CLASS, pof::MainFrame::OnSaleAlerts)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_SALE_ALERTS_OS, pof::MainFrame::OnSaleAlerts)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_SALE_ALERTS_EXPIRE, pof::MainFrame::OnSaleAlerts)
	EVT_MENU(pof::MainFrame::ID_MENU_PHARMACY_BACKUP, pof::MainFrame::OnBackupData)
	EVT_MENU(pof::MainFrame::ID_MENU_PHARMACY_ROLLBACK, pof::MainFrame::OnRollbackData)
	EVT_UPDATE_UI(pof::MainFrame::ID_MENU_VIEW_SHOW_MODULES,pof::MainFrame::OnMenuUpdateUI)
	EVT_IDLE(pof::MainFrame::OnIdle)
END_EVENT_TABLE()

pof::MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent, id, "PharmaOffice - Personal", position, size), mAuiManager(this, AUIMGRSTYLE) {
	SetBackgroundColour(*wxWHITE); //wrap in theme
	SetupAuiTheme();
	CreateMenuBar();

	mPager = new wxSimplebook(this, ID_PAGER);
	CreateWelcomePage();
	CreateWorkSpace();

	mPager->AddPage(mWelcomePage, "Welcome", true);
	mPager->AddPage(mWorkspace, "Workspace", false);

	mAuiManager.AddPane(mPager, wxAuiPaneInfo().Name("Pager").CaptionVisible(false).CenterPane().Show());


	CreateLogView();
	CreateViews();
	CreateModules();
	CreateImageList();
	//CreateStatusBar();
	//SetIcon(wxArtProvider::GetIcon("PHARMAOFFICE"));
	mAuiManager.Update();
	SetExtraStyle(wxWS_EX_PROCESS_IDLE);
	mExpireWatchTime = std::chrono::system_clock::now();

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::MainFrame::~MainFrame()
{
	
}


void pof::MainFrame::ReloadFrame()
{
	mModules->ReloadAccountDetails();
}

std::string pof::MainFrame::Perspective()
{
	// TODO: insert return statement here
	return mAuiManager.SavePerspective().ToStdString();
}

void pof::MainFrame::Perspective(const std::string& pers)
{
	if (pers.empty()) return;
	mAuiManager.LoadPerspective(pers);
}

void pof::MainFrame::UpdateWelcomePage()
{
	pharmName->Freeze();
	pharmName->SetLabelText(fmt::format("Welcome to {}", wxGetApp().MainPharmacy->name));
	pharmName->Thaw();

	mWelcomePage->Layout();
	mWelcomePage->Refresh();
}

void pof::MainFrame::PinPatient(const pof::base::data::duuid_t& puid, const std::string& name)
{
	mModules->Freeze();
	mModules->AppendPatientChild(puid, name);
	mModules->Thaw();
}

void pof::MainFrame::CreateMenuBar()
{
	constexpr const size_t MenuCount = 8;
	auto Menus = std::array<wxMenu*, MenuCount>{
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu
	};

	auto MenuTitle = std::array<wxString, MenuCount>{
		"Accounts",
		"Pharmacy",
		"Products",
		"Formulary",
		"Prescriptions",
		"View",
		"Extentions",
		"Help"
	};
	//account menu
	Menus[0]->Append(ID_MENU_ACCOUNT_SIGN_OUT, "Sign Out", nullptr);

	//pharmacy menu
	Menus[1]->Append(ID_MENU_PHARMACY_BACKUP, "Backup Store Data", nullptr, "Back up the current data in the database");
	Menus[1]->Append(ID_MENU_PHARMACY_ROLLBACK, "Roll back Store Data", nullptr, "Roll back database to a backed up database");


	//product menu
	wxMenu* exportSubMenu = new wxMenu;
	wxMenu* importSubMenu = new wxMenu;

	importSubMenu->Append(ID_MENU_PRODUCT_IMPORT_JSON, "Import Json", nullptr);
	importSubMenu->Append(ID_MENU_PRODUCT_IMPORT_EXCEL, "Import Excel", nullptr);
	
	exportSubMenu->Append(ID_MENU_EXPORT_JSON, "JSON", nullptr);
	exportSubMenu->Append(ID_MENU_EXPORT_EXCEL, "EXCEL", nullptr);
	exportSubMenu->Append(ID_MENU_EXPORT_CSV, "CSV", nullptr);
	
	auto al1  = Menus[2]->AppendCheckItem(ID_MENU_PRODUCT_SALE_ALERTS_OS, "Alert Out of Stock", "Alert when products out of stock are sold");
	auto al2  = Menus[2]->AppendCheckItem(ID_MENU_PRODUCT_SALE_ALERTS_CLASS, "Alert product class", "Alert on product class");
	auto al3  = Menus[2]->AppendCheckItem(ID_MENU_PRODUCT_SALE_ALERTS_EXPIRE, "Alert expire", "Alert on product expired");
	auto it  = Menus[2]->AppendCheckItem(ID_MENU_PRODUCT_NOTIF_OS, "Notify Out of Stock", "Notify when products go out of stock");
	auto it2 = Menus[2]->AppendCheckItem(ID_MENU_PRODUCT_NOTIF_EXPIRE, "Notify Expire Products", "Notify when products are expired");
	
	al1->Check(wxGetApp().bCheckOutOfStock);
	al2->Check(wxGetApp().bCheckPOM);
	al3->Check(wxGetApp().bCheckExpired);
	it->Check(wxGetApp().bCheckOutOfStockOnUpdate);
	it2->Check(wxGetApp().bCheckExpiredOnUpdate);

	Menus[2]->AppendSeparator();
	Menus[2]->Append(ID_MENU_PRODUCT_MARKUP_SETTINGS, "Mark-up", nullptr);
	Menus[2]->Append(wxID_ANY, "Import Products", importSubMenu);
	Menus[2]->Append(ID_MENU_PRODUCT_EXPORT, "Export Products", exportSubMenu);


	//TEST
#ifdef _DEBUG
	Menus[2]->AppendSeparator();
	Menus[2]->Append(ID_MENU_PRODUCT_SAVE, "Save\tCtrl-S", nullptr);
	Menus[2]->Append(ID_MENU_PRODUCT_LOAD, "Load\tCtrl-L", nullptr);
#endif // DEBUG

	//view
#ifdef _DEBUG
	Menus[5]->Append(ID_MENU_VIEW_LOG, "Log\tCtrl-L", nullptr);
#endif	
	auto item =Menus[5]->AppendCheckItem(ID_MENU_VIEW_SHOW_MODULES, "Modules\tCtrl-M");
	item->Check();


	//about
	Menus[7]->Append(ID_MENU_HELP_SETTINGS, "Settings", nullptr);
	Menus[7]->Append(ID_MENU_HELP_ABOUT, "About", nullptr);

	wxMenuBar* bar = new wxMenuBar(MenuCount, Menus.data(), MenuTitle.data());
	SetMenuBar(bar);
}

void pof::MainFrame::CreateStatusBar()
{
	wxStatusBar* bar = new wxStatusBar(this, ID_STATUS_BAR);


	SetStatusBar(bar);
}

void pof::MainFrame::CreateTheAcceleratorTable()
{

}

void pof::MainFrame::CreateLogView()
{
#ifdef _DEBUG
	mLogView = std::make_shared<wxTextCtrl>(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH2);
	auto sink = std::make_shared<spdlog::sinks::LogSink_mt>(mLogView);

	mAuiManager.AddPane(mLogView.get(), wxAuiPaneInfo().Name("LogView").Caption("LOGS").Bottom().MinSize(-1, 200).Hide());
#else
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt");
#endif // DEBUG
	auto Logger = std::make_shared<spdlog::logger>("Log", sink);
	spdlog::register_logger(Logger);
	spdlog::set_default_logger(Logger);

}

void pof::MainFrame::CreateModules()
{
	mModules = new pof::Modules(this, ID_MODULE);
	mModules->SetSlot(std::bind_front(&pof::MainFrame::OnModuleSlot, this));
	mModules->SetSlot(std::bind_front(&pof::MainFrame::OnModuleSlotReload, this));
	mModules->SetChildTreeSlot(std::bind_front(&pof::ProductView::OnCategoryActivated, mProductView));
	mModules->SetChildTreeRemoveSlot(std::bind_front(&pof::ProductView::OnCategoryRemoved, mProductView));
	mModules->SetChildTreeEditSlot(std::bind_front(&pof::ProductView::OnCategoryEdited, mProductView));
	mModules->SetPatientChildSlot(std::bind_front(&pof::PatientView::OnPatientPinSelected, mPatientView));


	mAuiManager.AddPane(mModules, wxAuiPaneInfo().Name("Modules")
		.CaptionVisible(false).Left().BottomDockable(false).Floatable(false).TopDockable(false).Show());

	//set the module to view pipeline
	mModules->mModuleViews.insert({mModules->mProducts, mProductView});
	mModules->mModuleViews.insert({ mModules->mSales, mSaleView });
	mModules->mModuleViews.insert({ mModules->mPrescriptions, mPrescriptionView });
	mModules->mModuleViews.insert({ mModules->mAuditTrails, mAuditView });
	mModules->mModuleViews.insert({ mModules->mPaitents,  mPatientView });
	mModules->mModuleViews.insert({ mModules->mPoisionBook, mPoisonBookView });

	//load cat to modules
	auto& cat = wxGetApp().mProductManager.GetCategories();
	std::ranges::for_each(cat, [&](const pof::base::data::row_t& row) {
		auto& name = boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::CATEGORY_NAME]);
		mProductView->CategoryAddSignal(name);
	});

	//load pinned patients
	LoadPinnedPatients();
}

void pof::MainFrame::CreateWorkSpace()
{
	mWorkspace = new pof::Workspace(mPager, ID_WORKSPACE, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	mWorkspace->AddNotifSlot(std::bind_front(&pof::MainFrame::OnWorkspaceNotif, this));
}

void pof::MainFrame::CreateImageList()
{
	const wxSize xy = wxArtProvider::GetSizeHint(wxART_LIST);
	mImageList = std::make_unique<wxImageList>(xy.x, xy.y);
	mImageList->Add(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_LIST));
	mImageList->Add(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_LIST));
	mImageList->Add(wxArtProvider::GetBitmap("folder_files"));
	mImageList->Add(wxArtProvider::GetBitmap("user"));
	mImageList->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_LIST));
		
	mWorkspace->SetImageList(mImageList.get());
	mModules->SetImageList(mImageList.get());
}

void pof::MainFrame::CreateViews()
{
	auto workspaceBook = std::addressof(mWorkspace->GetWorkspacebook());

	mProductView = new pof::ProductView(workspaceBook, ID_PRODUCT_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mSaleView = new pof::SaleView(workspaceBook, ID_SALE_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mPrescriptionView = new pof::PrescriptionView(workspaceBook, ID_PRESCRIPTION_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mAuditView = new pof::AuditView(workspaceBook, ID_AUDIT_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mPatientView = new pof::PatientView(workspaceBook, ID_PATIENT_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mPoisonBookView = new pof::PoisonBookView(workspaceBook, ID_POISONBOOK_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	//set up slots
	mProductView->CategoryAddSignal.connect(std::bind_front(&pof::MainFrame::OnCategoryAdded, this));
	mProductView->removeSignal.connect(std::bind_front(&pof::PoisonBookView::OnProductRemoved, mPoisonBookView));
	mPatientView->fSaleSignal.connect(std::bind_front(&pof::SaleView::OnAddMedicationsToSale,mSaleView));

	mSaleView->mSaleCompleted.connect(std::bind_front(&pof::PatientView::OnPatientSaleCompleted, mPatientView));

	mProductView->Hide();
	mSaleView->Hide();
	mPrescriptionView->Hide();
	mAuditView->Hide();
	mPatientView->Hide();
	mPoisonBookView->Hide();
}

void pof::MainFrame::CreateWelcomePage()
{
	mWelcomePage = new wxPanel(mPager, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mWelcomePage->SetBackgroundColour(*wxWHITE);
	mWelcomePage->SetDoubleBuffered(true);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mWelcomePage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);


	auto today = std::chrono::system_clock::now();
	std::string todayTime = fmt::format("{:%H:%M}", today);
	auto day = date::floor<date::days>(today);
	auto month = date::floor<date::months>(today);

	int monthCount = month.time_since_epoch().count() % 12;
	auto u = static_cast<unsigned>(day.time_since_epoch().count());
	int dayCount = static_cast<unsigned char>(day.time_since_epoch().count() >= -4 ? (u + 4) % 7 : u % 7);

	std::stringstream os;
	os << fmt::format("{:%d} {}, {} {:%Y}", today, dayNames[dayCount], monthNames[monthCount], today);



	time1 = new wxStaticText(m7, wxID_ANY, todayTime, wxDefaultPosition, wxDefaultSize, 0);
	time1->SetFont(wxFontInfo(64).AntiAliased().Family(wxFONTFAMILY_SWISS));
	time1->Wrap(-1);
	time1->SetDoubleBuffered(true);
	bSizer9->Add(time1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


	date1 = new wxStaticText(m7, wxID_ANY, os.str(), wxDefaultPosition, wxDefaultSize, 0);
	date1->SetFont(wxFontInfo(12).AntiAliased().Family(wxFONTFAMILY_SWISS));
	date1->Wrap(-1);
	date1->SetDoubleBuffered(true);
	bSizer9->Add(date1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	pharmName = new wxStaticText(m7, wxID_ANY, fmt::format("Welcome to {}", wxGetApp().MainPharmacy->name), wxDefaultPosition, wxDefaultSize, 0);
	pharmName->Wrap(-1);
	bSizer9->Add(pharmName, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


	bSizer9->AddSpacer(20);

	mSelectList = new wxListCtrl(m7, wxID_ANY, wxDefaultPosition, wxSize(380, 300), wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxFULL_REPAINT_ON_RESIZE | wxLC_EDIT_LABELS | wxNO_BORDER);
	CreateSelectList();
	bSizer9->Add(mSelectList, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);


	m7->SetSizer(bSizer9);
	m7->Layout();
	bSizer9->Fit(m7);
	bSizer8->Add(m7, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);


	m5->SetSizer(bSizer8);
	m5->Layout();
	bSizer8->Fit(m5);
	bSizer6->Add(m5, 1, wxEXPAND | wxALL, 5);


	mWelcomePage->SetSizer(bSizer6);
	mWelcomePage->Layout();


}

void pof::MainFrame::CreateSelectList()
{
	wxImageList* imagelist = new wxImageList(48, 48);
	imagelist->Add(wxArtProvider::GetBitmap("supplement-bottle"));
	imagelist->Add(wxArtProvider::GetBitmap("checkout"));
	imagelist->Add(wxArtProvider::GetBitmap("doctor"));
	imagelist->Add(wxArtProvider::GetBitmap("prescription"));

	mSelectList->AssignImageList(imagelist, wxIMAGE_LIST_NORMAL);

	wxListItem item;
	item.SetId(0);
	item.SetText("Products");
	item.SetImage(0);
	item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);

	mSelectList->InsertItem(item);

	item.SetId(1);
	item.SetText("Sales");
	item.SetImage(1);
	item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);

	mSelectList->InsertItem(item);

	
	item.SetId(2);
	item.SetText("Patients");
	item.SetImage(2);
	item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);

	mSelectList->InsertItem(item);

	item.SetId(3);
	item.SetText("Prescriptions");
	item.SetImage(3);
	item.SetMask(wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT);

	mSelectList->InsertItem(item);
	mSelectList->Bind(wxEVT_LIST_ITEM_ACTIVATED, std::bind_front(&pof::MainFrame::OnWelcomePageSelect, this));
}

void pof::MainFrame::LoadPinnedPatients()
{
	auto pp = wxGetApp().mPatientManager.GetPinnedList();
	if (!pp.has_value() || pp->empty()) return;

	for (const auto& tup : pp.value()) {
		PinPatient(std::get<0>(tup), std::get<1>(tup));
	}
}

void pof::MainFrame::SetupAuiTheme()
{
	auto auiArtProvider = mAuiManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::Register(std::bind_front(&pof::MainFrame::OnAuiThemeChangeSlot, this));
}

void pof::MainFrame::OnAbout(wxCommandEvent& evt)
{
	wxAboutDialogInfo info;
	info.SetName(wxT("PharmaOffice"));
	info.SetVersion(wxGetApp().gVersion); //version string need to come from settings
	info.SetDescription(wxT("Pharmacy mamagement system aid in the managment of pharmaceutical products, sale, transactions, prescription, expiry and so much more"));
	info.SetCopyright(wxT("(C) 2023 D-Glopa Technologies"));
	info.AddDeveloper("Ferife Zino :)");
	wxAboutBox(info);
}

void pof::MainFrame::OnClose(wxCloseEvent& evt)
{
	spdlog::drop_all();
	mLogView.reset();
	wxGetApp().SaveSettings();

	evt.Skip();
}

void pof::MainFrame::OnShowLog(wxCommandEvent& WXUNUSED(evt))
{
	auto& pane = mAuiManager.GetPane("LogView");
	if (pane.IsOk() && !pane.IsShown()) {
		pane.Show();
		mAuiManager.Update();
	}
	
}

void pof::MainFrame::OnExportJson(wxCommandEvent& evt)
{
}

void pof::MainFrame::OnExportExcel(wxCommandEvent& evt)
{

}

void pof::MainFrame::OnExportCSV(wxCommandEvent& evt)
{
}

void pof::MainFrame::OnUpdateUI(wxUpdateUIEvent& evt)
{
	wxGetApp().bMaximizeOnLoad = IsMaximized();
}

void pof::MainFrame::OnShowModules(wxCommandEvent& evt)
{
	if (!mModules) return;
	auto& pane = mAuiManager.GetPane("Modules");
	if (!pane.IsOk()) return;

	if (evt.IsChecked()) {
		pane.Show();
	}
	else {
		pane.Hide();
	}
	mAuiManager.Update();
}

void pof::MainFrame::OnSignOut(wxCommandEvent& evt)
{
	mAccount->DoSignOut();
	wxGetApp().SignOut();
}

void pof::MainFrame::OnTestSave(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	std::fstream fs("packed.dat", std::ios::out | std::ios::binary);
	if (!fs.is_open()) return;

	auto& modelPrt = wxGetApp().mProductManager.GetProductData();
	auto packed = modelPrt->Pack();

	fs.write(reinterpret_cast<const char*>(packed.data()), packed.size());
}

void pof::MainFrame::OnTestLoad(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	std::fstream fs("packed.dat", std::ios::in | std::ios::binary);
	if (!fs.is_open()) return;

	fs.seekg(0, std::ios::end);
	size_t size = fs.tellg();
	fs.seekg(0, std::ios::beg);

	pof::base::pack_t pack;
	pack.resize(size);

	fs.read(pack.data(), size);

	auto& modelPtr = wxGetApp().mProductManager.GetProductData();
	try {
		modelPtr->Unpack(std::move(pack));
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
	}
}

void pof::MainFrame::OnImportExcel(wxCommandEvent& evt)
{
	wxFileDialog dialog(this, "Open Excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_FILE_MUST_EXIST | wxFD_OPEN );
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Import Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}
	excel::XLDocument doc;

	//do mapping
	std::unordered_map<size_t,std::string> mHeaders;
	{
		

	}
}

void pof::MainFrame::OnMarkupSettings(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Mark up", wxICON_INFORMATION | wxOK);
		return;
	}

	wxTextEntryDialog dialog(this, "Please enter the precentage(%) Mark-Up\nRange ( 0 - 100 )", "Product Markup");
	dialog.SetTextValidator(wxTextValidator{wxFILTER_DIGITS});
	if (dialog.ShowModal() == wxID_OK) {
		float input =  static_cast<float>(std::atoi(dialog.GetValue().ToStdString().c_str()));
		input = std::max(0.0f, std::min(input, 100.0f));
		input /= 100.0f;
		wxGetApp().mProductManager.gMarkup = input;
	}
}

void pof::MainFrame::OnNotif(wxCommandEvent& evt)
{
	wxWindowID id = evt.GetId();
	bool checked = evt.IsChecked();
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/application"));
	switch (id)
	{
	case ID_MENU_PRODUCT_NOTIF_OS:
		wxGetApp().bCheckOutOfStockOnUpdate = checked;
		config->Write(wxT("CheckOutOfStockOnUpdate"), checked);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Changed state of notify out of stock");
		break;
	case ID_MENU_PRODUCT_NOTIF_EXPIRE:
		wxGetApp().bCheckExpiredOnUpdate = checked;
		config->Write(wxT("CheckExpiredOnUpdate"), checked);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Changed state of notify on expire");
		break;
	default:
		break;
	}
	config->SetPath(wxT("/"));

}

void pof::MainFrame::OnSaleAlerts(wxCommandEvent& evt)
{
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/application"));
	if (!config) return;
	bool checked = evt.IsChecked();
	switch (evt.GetId())
	{
	case ID_MENU_PRODUCT_SALE_ALERTS_CLASS:
		wxGetApp().bCheckPOM = checked;
		config->Write(wxT("CheckPOM"), checked);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Changed state of check product class on sale");
		break;
	case ID_MENU_PRODUCT_SALE_ALERTS_OS:
		wxGetApp().bCheckOutOfStock = checked;
		config->Write(wxT("CheckOutOfStock"), checked);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Changed state of check product out of stock on sale");
		break;
	case ID_MENU_PRODUCT_SALE_ALERTS_EXPIRE:
		wxGetApp().bCheckExpired = checked;
		config->Write(wxT("CheckExpired"), checked);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Changed state of check product expired on sale");
		break;
	default:
		break;
	}
	config->SetPath(wxT("/"));
}

void pof::MainFrame::OnShowSettings(wxCommandEvent& evt)
{
	wxGetApp().ShowSettings();
}

void pof::MainFrame::OnIdle(wxIdleEvent& evt)
{
	wxGetApp().bMaximizeOnLoad = IsMaximized();
	if (wxGetApp().bCheckExpirePeiodOnIdle){
		//check how often, 30 mins
		auto now = std::chrono::system_clock::now();
		if (now >= mExpireWatchTime + std::chrono::minutes(30)){
			mExpireWatchTime = now;
		}
	}
	int sel = mPager->GetSelection();
	if (sel == PAGE_WELCOME)
	{
		//update the time and date
		auto today = std::chrono::system_clock::now();
		std::string todayTime = fmt::format("{:%H:%M}", today);
		auto day = date::floor<date::days>(today);
		auto month = date::floor<date::months>(today);

		int monthCount = month.time_since_epoch().count() % 12;
		auto u = static_cast<unsigned>(day.time_since_epoch().count());
		int dayCount = static_cast<unsigned char>(day.time_since_epoch().count() >= -4 ? (u + 4) % 7 : u % 7);

		std::stringstream os;
		os << fmt::format("{:%d} {}, {} {:%Y}", today, dayNames[dayCount], monthNames[monthCount], today);

		time1->Freeze();
		time1->SetLabelText(todayTime);
		time1->Thaw();
		time1->Refresh();

		date1->Freeze();
		date1->SetLabelText(os.str());
		date1->Thaw();
		date1->Refresh();
	}
}

void pof::MainFrame::OnWelcomePageSelect(wxListEvent& evt)
{
	int sel = evt.GetItem().GetId();
	//how to trigger the module to change page
	switch (sel)
	{
	case 0:
		mModules->activateModule(mModules->mProducts);
		break;
	case 1:
		mModules->activateModule(mModules->mSales);
		break;
	case 2:
		mModules->activateModule(mModules->mPaitents);
		break;
	case 3:
		mModules->activateModule(mModules->mPrescriptions);
		break;
	default:
		break;
	}
}

void pof::MainFrame::OnMenuUpdateUI(wxUpdateUIEvent& evt)
{
	wxWindowID id = evt.GetId();
	switch (id)
	{
	case ID_MENU_VIEW_SHOW_MODULES:
	{
		auto& pane = mAuiManager.GetPane("Modules");
		if (!pane.IsOk()) return;
		evt.Check(pane.IsShown());
	}
		break;
	}
}

void pof::MainFrame::OnImportJson(wxCommandEvent& evt)
{
	wxFileDialog dialog(this);
	dialog.Center();
	if (dialog.ShowModal() == wxID_OK)
	{
		std::string filename = dialog.GetPath().ToStdString();
		if (filename.empty()) return;
		std::filesystem::path fp(filename);
		if (fp.extension().string() != ".json") {
			wxMessageBox("File is not a JSON file, with proper extension", "Json Import");
			return;
		}
		std::fstream fs(fp, std::ios::in);
		if (!fs.is_open()) {
			wxMessageBox("Cannot Open JSON FILE, file corrrupted", "Json Import", wxICON_ERROR | wxOK);
			return;
		}
		try {
			pof::base::data datastore;
			datastore.set_metadata(wxGetApp().mProductManager.GetProductData()->GetDatastore().get_metadata());
			nlohmann::json js;
			fs >> js;
			if (js.empty()) {
				wxMessageBox("JSON READ ERROR, FILE CORRUPTED", "Json Import", wxICON_ERROR | wxOK);
				return;
			}

			wxProgressDialog progress("Importing json", "Reading file...", js.size(), this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
			size_t value = 0;
			auto& pds = wxGetApp().mProductManager.GetProductData()->GetDatastore();
			for (auto iter = js.begin(); iter != js.end(); iter++) {
				nlohmann::json& tempjs = *iter;
				pof::base::data::row_t row; 
				//check if name already exsit in store
				if (std::ranges::any_of(pds, [&](const auto& r) -> bool {
					return boost::variant2::get<pof::base::data::text_t>(r.first[pof::ProductManager::PRODUCT_NAME])
					== static_cast<std::string>(tempjs["Name"]);
				})) {
					wxMessageBox(fmt::format("{} already exist in store, skipping", static_cast<std::string>(tempjs["Name"])), "Json Import", wxICON_INFORMATION | wxOK);
					continue;
				}
				
				row.first.resize(pof::ProductManager::PRODUCT_MAX);
				row.first[pof::ProductManager::PRODUCT_UUID] = wxGetApp().mProductManager.UuidGen();
				row.first[pof::ProductManager::PRODUCT_SERIAL_NUM] = pof::GenRandomId();
				row.first[pof::ProductManager::PRODUCT_NAME] = pof::base::data::data_t(static_cast<std::string>(tempjs["Name"])); //name
				row.first[pof::ProductManager::PRODUCT_PACKAGE_SIZE] = pof::base::data::data_t(static_cast<std::uint64_t>(tempjs["Package size"]));
				row.first[pof::ProductManager::PRODUCT_STOCK_COUNT] = static_cast<std::uint64_t>(0);
				row.first[pof::ProductManager::PRODUCT_UNIT_PRICE] = pof::base::data::data_t(pof::base::data::currency_t(static_cast<std::string>(tempjs["Unit price"])));
				row.first[pof::ProductManager::PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);

				datastore.insert(std::move(row));
				if (!progress.Update(++value, fmt::format("Reading {}", iter.key()))) {
					wxMessageBox("Import Cancelled", "Json Import");
					return;
				}
				spdlog::info("{}", iter.key());
				//std::this_thread::sleep_for(3ms);
			}
			//write the data to the product manager 
			wxMessageBox(fmt::format("Complete! Loaded {:d} products.", datastore.size()), "JSON IMPORT", wxICON_INFORMATION | wxOK);
			auto& meta = wxGetApp().mProductManager.GetProductData()->GetDatastore().get_metadata();
			datastore.set_metadata(meta);
			wxGetApp().mProductManager.StoreProductData(std::move(datastore));
		}
		catch (const std::exception& exp) {
			wxMessageBox(exp.what(), "JSON IMPORT", wxICON_ERROR | wxOK);
			return;
		}
	}

}

void pof::MainFrame::OnBackupData(wxCommandEvent& evt)
{
	//next step is to consider silent backups
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Backup", wxICON_INFORMATION | wxOK);
		return;
	}

	//generate backupname
	auto today = pof::base::data::clock_t::now();
	pof::base::data::datetime_t::duration::rep count = today.time_since_epoch().count();
	auto file = fmt::format("PharmaOfficebackup{:d}", count);
	auto backupPath = fs::current_path() / ".backup";
	if (!fs::is_directory(backupPath)) {
		fs::create_directory(backupPath);
	}

	wxFileDialog dialog(this, "Backup database", backupPath.string(), file, "db files (*.db)|*.db",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);
	if (fullPath.extension() != ".db") {
		wxMessageBox("File extension is not compactable with .db files", "Backup database",
			wxICON_INFORMATION | wxOK);
		return;
	}
	wxProgressDialog dlg("Backup database", "backing up, please wait...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
	auto progress = [&](int pg) -> bool {
		return dlg.Update(pg);
	};
	bool status = wxGetApp().mLocalDatabase->backup(fullPath, progress);
	if (!status) {
		dlg.Update(100);
		spdlog::error(wxGetApp().mLocalDatabase->err_msg());
		wxMessageBox("Backup failed, the backup file may be incomplete or corrupt, please try again", "Backup data", wxICON_ERROR | wxOK);

	}
	else {
		wxMessageBox(fmt::format("Successfully backed up data to {}", fullPath.string()), "Backup data", wxICON_INFORMATION | wxOK);	
	}
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Backed up database");
}

void pof::MainFrame::OnRollbackData(wxCommandEvent& evt)
{
	//next step is to consider silent backups
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Backup", wxICON_INFORMATION | wxOK);
		return;
	}
	if (wxMessageBox("Rolling back to a backup database would erase all the data in the current database, this is not reversible, are you sure you want to continue?", "Roll back", wxICON_WARNING | wxYES_NO) == wxNO) return;
	wxCredentialEntryDialog dialog( this, "User credentials are required to begin roll back", "Roll back database");
	dialog.Center(wxBOTH);
	dialog.SetBackgroundColour(*wxWHITE);
	while (1) {
		if (dialog.ShowModal() == wxID_CANCEL) return;
		auto cred = dialog.GetCredentials();
		if (!wxGetApp().MainAccount->ValidateCredentials(cred.GetUser().ToStdString(),
			cred.GetPassword().GetAsString().ToStdString())) {
			wxMessageBox("Invalid user name or password", "Roll back", wxICON_WARNING | wxOK);
			continue;
		}
		break;
	}

	auto backupPath = fs::current_path() / ".backup";
	if (!fs::is_directory(backupPath)) {
		fs::create_directory(backupPath);
	}

	wxFileDialog fileDialog(this, "Backup database", backupPath.string(), wxEmptyString, "db files (*.db)|*.db",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (fileDialog.ShowModal() == wxID_CANCEL) return;
	auto filename = fileDialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	wxProgressDialog dlg("Backup database", "backing up, please wait...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
	auto progress = [&](int pg) -> bool {
		return dlg.Update(pg);
	};
	bool status = wxGetApp().mLocalDatabase->rollback_data(fullPath, progress);
	if (!status) {
		dlg.Update(100);
		spdlog::error(wxGetApp().mLocalDatabase->err_msg());
		wxMessageBox("Roll back failed, the backup file may be incomplete or corrupt, please try again", "Roll back data", wxICON_ERROR | wxOK);

	}
	else {
		wxMessageBox(fmt::format("Successfully roll back database to {}", fullPath.string()), "Roll back data", wxICON_INFORMATION | wxOK);
	}
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Rolled back database");
}

void pof::MainFrame::OnModuleSlot(pof::Modules::const_iterator win, Modules::Evt notif)
{
	int sel = wxNOT_FOUND;
	switch (notif) {
	case Modules::Evt::CHILD_ACTIVATED:
	case Modules::Evt::ACTIVATED:
		mWorkspace->AddSpace(win->second, mModules->GetText(win), mModules->GetImage(win)); //where are the windows kept and created
		sel = mPager->GetSelection();
		if (sel != wxNOT_FOUND && sel == PAGE_WELCOME)
		{
			//HAVE YOU MOVED ON ZINO ??
			mPager->SetSelection(PAGE_WORKSPACE);
		}
		break;
	case Modules::Evt::SEL_CHANGED:
		break;
	case Modules::Evt::COLLAPASED:
		break;
	default:
		break;
	}
}

void pof::MainFrame::OnModuleSlotReload(pof::Modules::const_iterator win, Modules::Evt notif)
{
	if (notif == Modules::Evt::ACTIVATED) {
		if (win->first == mModules->mProducts && mProductView->IsActiveCategory()) {
			mProductView->ReloadProductView();
		}
	}
}

void pof::MainFrame::OnAuiThemeChangeSlot()
{
	auto auiArtProvider = mAuiManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::MainFrame::OnCategoryAdded(const std::string& name)
{
	if (name.empty()) return;
	mModules->AppendChildTreeId(mModules->mProducts, name, 2);
	mModules->mModuleTree->Expand(mModules->mProducts);
}

void pof::MainFrame::OnProductModuleSlotReload(pof::Modules::const_iterator win, Modules::Evt notif)
{
}

void pof::MainFrame::OnWorkspaceNotif(pof::Workspace::Notif notif, size_t page)
{
	switch (notif) {
	case pof::Workspace::Notif::CLOSED:
		if (mWorkspace->GetPageCount() == 0){
			mPager->SetSelection(PAGE_WELCOME);
		}
		break;
	}
}

