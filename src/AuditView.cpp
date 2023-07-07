#include "AuditView.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::AuditView, wxPanel)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::AuditView::ID_DATA_VIEW, pof::AuditView::OnItemActivated)
EVT_DATAVIEW_CACHE_HINT(pof::AuditView::ID_DATA_VIEW, pof::AuditView::OnCacheHint)
EVT_COMBOBOX(pof::AuditView::ID_FILTER_TYPE, pof::AuditView::OnFilterSelected)
EVT_BUTTON(wxID_APPLY, pof::AuditView::OnApplyFilter)
EVT_TOOL(wxID_FORWARD, pof::AuditView::OnNextPage)
EVT_TOOL(wxID_BACKWARD, pof::AuditView::OnBackPage)
EVT_TOOL(pof::AuditView::ID_COLOUR_TYPE, pof::AuditView::OnColourAuditType)
END_EVENT_TABLE()


pof::AuditView::AuditView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
	: wxPanel(parent, id, position, size, style) {
	mAuiManager.SetManagedWindow(this);
	SetBackgroundColour(*wxWHITE); //move to theme
	CreateToolBar();
	CreateDataView();
	SetupAuiTheme();
	mAuiManager.SetFlags(AUIMGRSTYLE);
	mAuiManager.Update();
}

pof::AuditView::~AuditView()
{
	mAuiManager.UnInit();
}

void pof::AuditView::CreateToolBar()
{
	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mToolBar->SetToolBitmapSize(wxSize(16, 16));

	mBack = mToolBar->AddTool(wxID_BACKWARD, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"));
	mToolBar->AddSpacer(5);
	mNext = mToolBar->AddTool(wxID_FORWARD, wxEmptyString, wxArtProvider::GetBitmap("arrow_next"));

	wxArrayString choices;
	choices.reserve(static_cast<size_t>(pof::AuditManager::auditType::MAX));



	choices.push_back("INFORMATION");
	choices.push_back("SALE");
	choices.push_back("PRODUCT");
	choices.push_back("CATEGORY");
	mToolBar->AddSeparator();
	mFilterType = new wxChoice(mToolBar, ID_FILTER_TYPE, wxDefaultPosition, wxSize(200, -1), choices);
	mFilterType->SetBackgroundColour(*wxWHITE);
	auto text = new wxStaticText(mToolBar, wxID_ANY, "Filter: ");
	text->SetBackgroundColour(*wxWHITE);
	mToolBar->AddControl(text);
	mToolBar->AddSpacer(10);
	mToolBar->AddControl(mFilterType, "Filter");
	mToolBar->AddSpacer(10);
	mToolBar->AddTool(wxID_APPLY, "Apply", wxArtProvider::GetBitmap("action_check"), "Apply selected filter");
	mToolBar->AddSpacer(10);
	mToolBar->AddTool(ID_COLOUR_TYPE, "Type Highlight", wxArtProvider::GetBitmap("pen"), "Highlight different audit types", wxITEM_CHECK);

	mToolBar->Realize();
	mAuiManager.AddPane(mToolBar, wxAuiPaneInfo().Name("ToolBar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::AuditView::CreateDataView()
{
	wxPanel* panel = new wxPanel(this, wxID_ANY,wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* size = new wxBoxSizer(wxVERTICAL);

	mInfoBar = new wxInfoBar(panel, ID_INFOBAR);
	mDataView = new wxDataViewCtrl(panel, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES);
	auto& ad = wxGetApp().mAuditManager.GetAuditData();
	mDataView->AssociateModel(ad.get());
	mDataView->SetDoubleBuffered(true);
	//columns
	mDataView->AppendTextColumn("Type", pof::AuditManager::AUDIT_TYPE, wxDATAVIEW_CELL_INERT, 100);
	mDataView->AppendTextColumn("Date", pof::AuditManager::AUDIT_DATE, wxDATAVIEW_CELL_INERT, 100);
	mDataView->AppendTextColumn("Username", pof::AuditManager::AUDIT_USER_NAME, wxDATAVIEW_CELL_INERT, 100);
	mDataView->AppendTextColumn("Audit Log", pof::AuditManager::AUDIT_MESSAGE, wxDATAVIEW_CELL_INERT);

	size->Add(mInfoBar, wxSizerFlags().Expand().Proportion(0).Border(wxALL, 2));
	size->Add(mDataView, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 2));

	panel->SetSizer(size);
	size->Fit(panel);
	panel->Layout();

	//SetupDataViewStyle();
	mAuiManager.AddPane(panel, wxAuiPaneInfo().Name("DataView").CenterPane().CaptionVisible(false));
}

void pof::AuditView::CreatePopupMenu()
{
}

void pof::AuditView::CreatePopupAdditionalInformation()
{
	mAdditionalInfoPopup = new wxPopupTransientWindow(mDataView);
	wxPanel* panel = new wxPanel(mAdditionalInfoPopup);

	panel->SetSize(400, 400);
}

void pof::AuditView::OnCacheHint(wxDataViewEvent& evt)
{
	spdlog::info("On Cache Hint from {:d} to {:d}", evt.GetCacheFrom(), evt.GetCacheTo());
}

void pof::AuditView::OnItemActivated(wxDataViewEvent& evt)
{
}

void pof::AuditView::OnBackPage(wxCommandEvent& evt)
{
}

void pof::AuditView::OnNextPage(wxCommandEvent& evt)
{
	if (mPageRanges.first.empty()) return;

	auto& range = mPageRanges.first[mPageRanges.second];
	range_t newRange = { range.second, mDataView->GetCountPerPage() + range.second };


}

void pof::AuditView::OnFilterSelected(wxCommandEvent& evt)
{
}

void pof::AuditView::OnApplyFilter(wxCommandEvent& evt)
{
	int sel = mFilterType->GetSelection();
	if (sel == wxNOT_FOUND) return;

	pof::AuditManager::auditType at = static_cast<pof::AuditManager::auditType>(sel);
	mPageRanges.first.clear();
	mPageRanges.second = 0;

	int count = mDataView->GetCountPerPage();
	wxGetApp().mAuditManager.LoadType(at, 0, count);
	mPageRanges.first.push_back({ 0, count }); //this might be different if it is ordered
}

void pof::AuditView::OnColourAuditType(wxCommandEvent& evt)
{
	wxGetApp().mAuditManager.bColourAuditTypes = evt.IsChecked();
}

void pof::AuditView::OnAuiThemeChange()
{
	auto auiArtProvider = mAuiManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::AuditView::ShowAdditionalInfoPopup()
{
}

void pof::AuditView::SetupAuiTheme()
{
	auto auiArtProvider = mAuiManager.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::AuditView::OnAuiThemeChange, this));
}

void pof::AuditView::SetupDataViewStyle()
{
	mRowHeights = 50;
	mHeaderAttr.SetBackgroundColour(*wxWHITE);
	auto Info = wxFontInfo(10).Weight(24).FaceName("Helvetica").Bold();
	mHeaderAttr.SetFont(wxFont(std::move(Info)));

	auto Info2 = wxFontInfo(11).FaceName("Helvetica");
	mDataViewFont = wxFont(std::move(Info2));


	mDataView->SetFont(mDataViewFont);
	mDataView->SetHeaderAttr(mHeaderAttr);
	mDataView->SetRowHeight(mRowHeights);
}
