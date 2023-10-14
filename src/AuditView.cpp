#include "AuditView.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::AuditView, wxPanel)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::AuditView::ID_DATA_VIEW, pof::AuditView::OnItemActivated)
EVT_DATAVIEW_CACHE_HINT(pof::AuditView::ID_DATA_VIEW, pof::AuditView::OnCacheHint)
EVT_COMBOBOX(pof::AuditView::ID_FILTER_TYPE, pof::AuditView::OnFilterSelected)
EVT_TOOL(wxID_APPLY, pof::AuditView::OnApplyFilter)
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
	choices.push_back("ALL");

	mToolBar->AddSeparator();
	mToolBar->AddStretchSpacer();
	mFilterType = new wxChoice(mToolBar, ID_FILTER_TYPE, wxDefaultPosition, wxSize(200, -1), choices);
	mFilterType->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mFilterType);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = mFilterType->GetStringSelection();
	if (!sel.IsEmpty()) {
		dc.DrawLabel(sel, rect, wxALIGN_CENTER);
	}
	});
	auto text = new wxStaticText(mToolBar, wxID_ANY, "Filter: ");
	text->SetBackgroundColour(*wxWHITE);
	mToolBar->AddControl(text);
	mToolBar->AddSpacer(5);
	mToolBar->AddControl(mFilterType, "Filter");
	mToolBar->AddSpacer(5);
	mToolBar->AddTool(ID_COLOUR_TYPE, "Type Highlight", wxArtProvider::GetBitmap("pen"), "Highlight different audit types", wxITEM_CHECK);
	mToolBar->AddSpacer(5);
	mToolBar->AddTool(wxID_APPLY, "Apply", wxArtProvider::GetBitmap("action_check"), "Apply selected filter");
	mToolBar->AddSpacer(5);
	mToolBar->AddTool(ID_DOWNLOAD_EXCEL, "Download as EXCEL", wxArtProvider::GetBitmap("download"), "Download Audit as EXCEL worksheet");

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
	if (mPageRanges.empty() || mPageRanges.size() == 1) return;
	mPageRanges.pop(); //remove the added page range
	auto& top = mPageRanges.top();

	wxGetApp().mAuditManager.GetAuditData()->Clear();
	if (mCurType == pof::AuditManager::auditType::ALL) {
		wxGetApp().mAuditManager.LoadCache(top.first, top.second);
	}
	else {
		wxGetApp().mAuditManager.LoadType(mCurType, top.first, top.second);
	}
}

void pof::AuditView::OnNextPage(wxCommandEvent& evt)
{
	if (mPageRanges.empty()) return;
	auto& curTop = mPageRanges.top();
	range_t newRange = std::make_pair(curTop.second, curTop.second - mDataView->GetCountPerPage());
	
	if (static_cast<int>(newRange.second) < 0){
		newRange.second = 0;
	}

	if (newRange.first == newRange.second) return;

	wxGetApp().mAuditManager.GetAuditData()->Clear();
	if (mCurType == pof::AuditManager::auditType::ALL)
	{
		wxGetApp().mAuditManager.LoadCache(newRange.first, newRange.second);
	}
	else {
		wxGetApp().mAuditManager.LoadType(mCurType, newRange.first, newRange.second);
	}

	mPageRanges.push(std::move(newRange));
}

void pof::AuditView::OnFilterSelected(wxCommandEvent& evt)
{

}

void pof::AuditView::OnApplyFilter(wxCommandEvent& evt)
{
	int sel = mFilterType->GetSelection();
	if (sel == wxNOT_FOUND) return;
	

	mCurType = static_cast<pof::AuditManager::auditType>(sel);
	bFilterType = true;
	int count = mDataView->GetCountPerPage();
	mPageRanges = {};
	wxGetApp().mAuditManager.GetAuditData()->Clear();
	if (mCurType == pof::AuditManager::auditType::ALL){
		auto dataSize = wxGetApp().mAuditManager.GetDataSize();
		assert(dataSize);
		//calculate the audit from the back
		auto s = std::make_pair(dataSize.value(), dataSize.value() - count);
		mPageRanges.push(s);
		wxGetApp().mAuditManager.LoadCache(s.first,s.second);
	}
	else {
		auto dataSize = wxGetApp().mAuditManager.GetDataSize(mCurType);
		assert(dataSize);
		//calculate the audit from the back
		auto s = std::make_pair(dataSize.value(), dataSize.value() - count);
		mPageRanges.push(s);
		wxGetApp().mAuditManager.LoadType(mCurType, s.first, s.second);
	}
}

void pof::AuditView::OnColourAuditType(wxCommandEvent& evt)
{
	wxGetApp().mAuditManager.bColourAuditTypes = evt.IsChecked();
}

void pof::AuditView::OnDownloadExcel(wxCommandEvent& evt)
{
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
	mHeaderAttr.SetBackgroundColour(*wxLIGHT_GREY);
	auto Info = wxFontInfo(10).Weight(24).FaceName("Helvetica").Bold();
	mHeaderAttr.SetFont(wxFont(std::move(Info)));

	auto Info2 = wxFontInfo(11).FaceName("Helvetica");
	mDataViewFont = wxFont(std::move(Info2));


	mDataView->SetFont(mDataViewFont);
	mDataView->SetHeaderAttr(mHeaderAttr);
	mDataView->SetRowHeight(mRowHeights);
}
