#include "MainFrame.h"

BEGIN_EVENT_TABLE(pof::MainFrame, wxFrame)
	
END_EVENT_TABLE()

pof::MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
: wxFrame(parent, id, "PHARMAOFFICE", position, size), mAuiManager(this) {

	CreateMenuBar();
	CreateToolBar();


	CreateLogView();
	CreateStatusBar();
	SetIcon(wxArtProvider::GetIcon("PHARMAOFFICE"));
	mAuiManager.Update();
}

pof::MainFrame::~MainFrame()
{
	
}

void pof::MainFrame::CreateToolBar()
{
	wxAuiToolBar* bar = new wxAuiToolBar(this, ID_TOOL_BAR, wxDefaultPosition,
		wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TBTOOL_TEXT_LEFT);

	mAuiManager.AddPane(bar, wxAuiPaneInfo().Name("MainToolBar").ToolbarPane().Resizable().Floatable(false).
	LeftDockable(false).RightDockable(false).BottomDockable(false).Top().Show());
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

void pof::MainFrame::OnAbout(wxCommandEvent& evt)
{

}
