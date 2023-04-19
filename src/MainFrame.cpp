#include "MainFrame.h"

BEGIN_EVENT_TABLE(pof::MainFrame, wxFrame)
	EVT_CLOSE(pof::MainFrame::OnClose)
	EVT_MENU(pof::MainFrame::ID_MENU_PRODUCT_IMPORT_JSON, pof::MainFrame::OnImportJson)
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
	Menus[2]->Append(ID_MENU_PRODUCT_IMPORT_JSON, "Import Json", nullptr);

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

	mAuiManager.AddPane(mLogView.get(), wxAuiPaneInfo().Name("LogView").Caption("LOGS").Bottom().Show());
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
	mProductView = new pof::ProductView(this, ID_PRODUCT_VIEW, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mProductView->Hide();
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

void pof::MainFrame::OnImportJson(wxCommandEvent& evt)
{
	wxFileDialog dialog(this);
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
			pof::base::adapt<size_t, std::string, size_t, size_t, size_t, std::string>(datastore);
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

				row.first.emplace_back(static_cast<size_t>(tempjs["Category id"]));
				row.first.emplace_back(static_cast<std::string>(tempjs["Name"])); //name
				row.first.emplace_back(static_cast<size_t>(tempjs["Package size"]));
				row.first.emplace_back(static_cast<size_t>(tempjs["Serial number"]));
				row.first.emplace_back(static_cast<size_t>(tempjs["Stock count"]));
				row.first.emplace_back(static_cast<std::string>(tempjs["Unit price"]));

				datastore.insert(std::move(row));
				if (!progress.Update(++value, fmt::format("Reading {}", iter.key()))) {
					wxMessageBox("IMPORT CANCELED", "JSON IMPORT");
					return;
				}
				std::this_thread::sleep_for(3ms);
			}
			//write the data to the product manager 
			wxMessageBox(fmt::format("Complete! Loaded {:d} products.", datastore.size()), "JSON IMPORT", wxICON_INFORMATION | wxOK);
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

