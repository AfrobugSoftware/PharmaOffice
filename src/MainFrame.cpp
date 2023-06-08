#include "MainFrame.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::MainFrame, wxFrame)
	EVT_CLOSE(pof::MainFrame::OnClose)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_IMPORT_JSON, pof::MainFrame::OnImportJson)
	EVT_MENU(pof::MainFrame::ID_MENU_HELP_ABOUT, pof::MainFrame::OnAbout)
	EVT_MENU(pof::MainFrame::ID_MENU_VIEW_LOG, pof::MainFrame::OnShowLog)
	EVT_MENU(pof::MainFrame::ID_MENU_EXPORT_JSON, pof::MainFrame::OnExportJson)
	EVT_MENU(pof::MainFrame::ID_MENU_EXPORT_EXCEL, pof::MainFrame::OnExportExcel)
	EVT_MENU(pof::MainFrame::ID_MENU_EXPORT_CSV, pof::MainFrame::OnExportCSV)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_SAVE, pof::MainFrame::OnTestSave)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_LOAD, pof::MainFrame::OnTestLoad)
END_EVENT_TABLE()

pof::MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent, id, "PHARMAOFFICE", position, size), mAuiManager(this, AUIMGRSTYLE) {
	SetBackgroundColour(*wxWHITE); //wrap in theme
	SetupAuiTheme();
	CreateMenuBar();
	CreateWorkSpace();
	CreateLogView();
	CreateViews();
	CreateModules();
	CreateStatusBar();
	//SetIcon(wxArtProvider::GetIcon("PHARMAOFFICE"));
	mAuiManager.Update();
}

pof::MainFrame::~MainFrame()
{
	
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


	//product menu
	Menus[2]->Append(ID_MENU_PRODUCT_IMPORT_JSON, "Import Json\tCtrl-J", nullptr);
	wxMenu* exportSubMenu = new wxMenu;
	exportSubMenu->Append(ID_MENU_EXPORT_JSON, "JSON", nullptr);
	exportSubMenu->Append(ID_MENU_EXPORT_EXCEL, "EXCEL", nullptr);
	exportSubMenu->Append(ID_MENU_EXPORT_CSV, "CSV", nullptr);
	Menus[2]->Append(ID_MENU_PRODUCT_EXPORT, "Export\tCtrl-E", exportSubMenu);

	//TEST
	Menus[2]->Append(ID_MENU_PRODUCT_SAVE, "Save\tCtrl-S", nullptr);
	Menus[2]->Append(ID_MENU_PRODUCT_LOAD, "Load\tCtrl-L", nullptr);



	//view
	Menus[5]->Append(ID_MENU_VIEW_LOG, "Log\tCtrl-L", nullptr);

	//about
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

	mAuiManager.AddPane(mLogView.get(), wxAuiPaneInfo().Name("LogView").Caption("LOGS").Bottom().Show().MinSize(-1, 200));
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
	mAuiManager.AddPane(mModules, wxAuiPaneInfo().Name("Modules")
		.CaptionVisible(false).Left().BottomDockable(false).TopDockable(false).Show());

	//set the module to view pipeline
	mModules->mModuleViews.insert({mModules->mProducts, mProductView});
	mModules->mModuleViews.insert({ mModules->mSales, mSaleView });
	mModules->mModuleViews.insert({ mModules->mPrescriptions, mPrescriptionView });
		
}

void pof::MainFrame::CreateWorkSpace()
{
	mWorkspace = new pof::Workspace(this, ID_WORKSPACE, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);


	mAuiManager.AddPane(mWorkspace, wxAuiPaneInfo().Name("Workspace").CaptionVisible(false).CenterPane().Show());
}

void pof::MainFrame::CreateImageList()
{
	mImageList = std::make_unique<wxImageList>(16, 16);

	mWorkspace->SetImageList(mImageList.get());
	mModules->SetImageList(mImageList.get());
}

void pof::MainFrame::CreateViews()
{
	auto workspaceBook = std::addressof(mWorkspace->GetWorkspacebook());

	mProductView = new pof::ProductView(workspaceBook, ID_PRODUCT_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mSaleView = new pof::SaleView(workspaceBook, ID_SALE_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mPrescriptionView = new pof::PrescriptionView(workspaceBook, ID_PRESCRIPTION_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	
	mProductView->Hide();
	mSaleView->Hide();
	mPrescriptionView->Hide();
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
	info.SetName(wxT("D-Glopa PharmaOffice"));
	info.SetVersion(wxT("0.0.1 pre beta")); //version string need to come from settings
	info.SetDescription(wxT("Pharmacy mamagement system aid in the managment of pharmaceutical products, sale, transactions, prescription, expiry and so much more"));
	info.SetCopyright(wxT("(C) 2023 D-Glopa Technologies"));
	info.AddDeveloper("Ferife Zino :)");
	wxAboutBox(info);
}

void pof::MainFrame::OnClose(wxCloseEvent& evt)
{
	spdlog::drop_all();
	mLogView.reset();

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
			wxMessageBox("File is not a JSON file, with proper extension", "JSON IMPORT");
			return;
		}
		std::fstream fs(fp, std::ios::in);
		if (!fs.is_open()) {
			wxMessageBox("Cannot Open JSON FILE, file corrrupted", "JSON IMPORT", wxICON_ERROR | wxOK);
			return;
		}
		try {
			pof::base::data datastore;
			datastore.set_metadata(wxGetApp().mProductManager.GetProductData()->GetDatastore().get_metadata());
			nlohmann::json js;
			fs >> js;
			if (js.empty()) {
				wxMessageBox("JSON READ ERROR, FILE CORRUPTED", "JSON IMPORT", wxICON_ERROR | wxOK);
				return;
			}

			wxProgressDialog progress("Importing json", "Reading file...", js.size(), this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL);
			size_t value = 0;
			for (auto iter = js.begin(); iter != js.end(); iter++) {
				nlohmann::json& tempjs = *iter;
				pof::base::data::row_t row; 
				row.first.resize(pof::ProductManager::PRODUCT_MAX);
				/*row.first[pof::ProductManager::PRODUCT_UUID] = boost::uuids::uuid();
				row.first[pof::ProductManager::PRODUCT_GENERIC_NAME] = std::string();
				row.first[pof::ProductManager::PRODUCT_CLASS] = std::string();
				row.first[pof::ProductManager::PRODUCT_BARCODE] = std::string();*/


				row.first[pof::ProductManager::PRODUCT_SERIAL_NUM] = pof::base::data::data_t(static_cast<std::uint64_t>(tempjs["Serial number"]));
				row.first[pof::ProductManager::PRODUCT_NAME] = pof::base::data::data_t(static_cast<std::string>(tempjs["Name"])); //name
				row.first[pof::ProductManager::PRODUCT_PACKAGE_SIZE] = pof::base::data::data_t(static_cast<std::uint64_t>(tempjs["Package size"]));
				row.first[pof::ProductManager::PRODUCT_STOCK_COUNT] = pof::base::data::data_t(static_cast<std::uint64_t>(tempjs["Stock count"]));
				row.first[pof::ProductManager::PRODUCT_UNIT_PRICE] = pof::base::data::data_t(pof::base::data::currency_t(static_cast<std::string>(tempjs["Unit price"])));
				row.first[pof::ProductManager::PRODUCT_CATEGORY] = pof::base::data::data_t(static_cast<std::uint64_t>(tempjs["Category id"]));

				datastore.insert(std::move(row));
				if (!progress.Update(++value, fmt::format("Reading {}", iter.key()))) {
					wxMessageBox("IMPORT CANCELED", "JSON IMPORT");
					return;
				}
				spdlog::info("{}", iter.key());
				std::this_thread::sleep_for(3ms);
			}
			//write the data to the product manager 
			wxMessageBox(fmt::format("Complete! Loaded {:d} products.", datastore.size()), "JSON IMPORT", wxICON_INFORMATION | wxOK);
			wxGetApp().mProductManager.EmplaceProductData(std::move(datastore));
		}
		catch (const std::exception& exp) {
			wxMessageBox(exp.what(), "JSON IMPORT", wxICON_ERROR | wxOK);
			return;
		}
	}

}

void pof::MainFrame::OnModuleSlot(pof::Modules::const_iterator win, Modules::Evt notif)
{
	switch (notif) {
	case Modules::Evt::ACTIVATED:
		mWorkspace->AddSpace(win->second, mModules->GetText(win), mModules->GetImage(win)); //where are the windows kept and created
		break;
	case Modules::Evt::SEL_CHANGED:
		break;
	case Modules::Evt::COLLAPASED:
		break;
	default:
		break;
	}
}

void pof::MainFrame::OnAuiThemeChangeSlot()
{
	auto auiArtProvider = mAuiManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

