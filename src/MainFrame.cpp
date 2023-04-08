#include "MainFrame.h"

BEGIN_EVENT_TABLE(pof::MainFrame, wxFrame)
	EVT_CLOSE(pof::MainFrame::OnClose)
END_EVENT_TABLE()

pof::MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent, id, "PHARMAOFFICE", position, size), mAuiManager(this) {

	CreateMenuBar();
	CreateModules();
	CreateWorkSpace();
	CreateLogView();
	CreateStatusBar();
	//SetIcon(wxArtProvider::GetIcon("PHARMAOFFICE"));
	mAuiManager.Update();
}

pof::MainFrame::~MainFrame()
{
	
}


void pof::MainFrame::CreateMenuBar()
{
	constexpr const size_t MenuCount = 6;
	auto Menus = std::array<wxMenu*, MenuCount>{
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu,
		new wxMenu
	};

	auto MenuTitle = std::array<wxString, MenuCount>{
		"Pharmacy",
		"Products",
		"Formulary",
		"Prescriptions",
		"Extentions",
		"Help"
	};



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
	mModules->mModuleViews.insert({mModules->mProducts, });
		
}

void pof::MainFrame::CreateWorkSpace()
{
	mWorkspace = new pof::Workspace(this, ID_WORKSPACE);


	mAuiManager.AddPane(mWorkspace, wxAuiPaneInfo().Name("Workspace").CaptionVisible(false).CenterPane().Show());
}

void pof::MainFrame::CreateImageList()
{
	mImageList = std::make_unique<wxImageList>(16, 16);

	mWorkspace->SetImageList(mImageList.get());
	mModules->SetImageList(mImageList.get());
}

void pof::MainFrame::OnAbout(wxCommandEvent& evt)
{

}

void pof::MainFrame::OnClose(wxCloseEvent& evt)
{
	spdlog::drop_all();
	mLogView.reset();

	evt.Skip();
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
a

