#include "AuditView.h"
BEGIN_EVENT_TABLE(pof::AuditView, wxPanel)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::AuditView::ID_DATA_VIEW, pof::AuditView::OnItemActivated)
EVT_DATAVIEW_CACHE_HINT(pof::AuditView::ID_DATA_VIEW, pof::AuditView::OnCacheHint)
EVT_COMBOBOX(pof::AuditView::ID_FILTER_TYPE, pof::AuditView::OnFilterSelected)
EVT_BUTTON(wxID_APPLY, pof::AuditView::OnApplyFilter)
EVT_TOOL(wxID_FORWARD, pof::AuditView::OnNextPage)
EVT_TOOL(wxID_BACKWARD, pof::AuditView::OnBackPage)
EVT_TOOL(pof::AuditView::ID_COLOUR_TYPE, pof::AuditView::OnColourAuditType)
END_EVENT_TABLE()


pof::AuditView::AuditView(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size)
	: wxPanel(parent, id, position, size), mAuiManager{this} {
	
	CreateToolBar();
	CreateDataView();
	mAuiManager.Update();
}

void pof::AuditView::CreateToolBar()
{
	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mToolBar->SetToolBitmapSize(wxSize(16, 16));

	mNext = mToolBar->AddTool(wxID_FORWARD, wxEmptyString, wxArtProvider::GetBitmap("action_forward"));
	mToolBar->AddSpacer(10);
	mBack = mToolBar->AddTool(wxID_BACKWARD, wxEmptyString, wxArtProvider::GetBitmap("action_back"));

	wxArrayString choices;
	choices.reserve(static_cast<size_t>(pof::AuditManager::auditType::MAX));



	choices.push_back("INFORMATION");
	choices.push_back("SALE");
	choices.push_back("PRODUCT");
	choices.push_back("CATEGORY");
	mToolBar->AddSeparator();
	mFilterType = new wxComboBox(mToolBar, ID_FILTER_TYPE, wxEmptyString, wxDefaultPosition, wxSize(100, -1), choices);
	mToolBar->AddControl(mFilterType, "Filter");
	mToolBar->AddSpacer(10);
	mToolBar->AddControl(new wxButton(mToolBar, wxID_APPLY));
	mToolBar->AddSpacer(20);
	mToolBar->AddTool(ID_COLOUR_TYPE, "Type Highlight", wxArtProvider::GetBitmap(""));

	mToolBar->Realize();
	mAuiManager.AddPane(mToolBar, wxAuiPaneInfo().Name("ProductToolBar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::AuditView::CreateDataView()
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* size = new wxBoxSizer(wxVERTICAL);

	mInfoBar = new wxInfoBar(panel, ID_INFOBAR);
	mDataView = new wxDataViewCtrl(panel, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES);
	auto& ad = wxGetApp().mAuditManager.GetAuditData();
	mDataView->AssociateModel(ad.get());

	//columns
	mDataView->AppendTextColumn("Serial No", pof::AuditManager::AUDIT_ID, wxDATAVIEW_CELL_INERT, 50);
	mDataView->AppendTextColumn("Date", pof::AuditManager::AUDIT_DATE, wxDATAVIEW_CELL_INERT, 70);
	mDataView->AppendTextColumn("Account Username", pof::AuditManager::AUDIT_USER_NAME, wxDATAVIEW_CELL_INERT, 100);
	mDataView->AppendTextColumn("Message", pof::AuditManager::AUDIT_MESSAGE, wxDATAVIEW_CELL_INERT);



	panel->SetSizer(size);
	size->Fit(panel);
	panel->Layout();

	mAuiManager.AddPane(panel, wxAuiPaneInfo());
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
}

void pof::AuditView::OnItemActivated(wxDataViewEvent& evt)
{
}

void pof::AuditView::OnBackPage(wxCommandEvent& evt)
{
}

void pof::AuditView::OnNextPage(wxCommandEvent& evt)
{
}

void pof::AuditView::OnFilterSelected(wxCommandEvent& evt)
{
}

void pof::AuditView::OnApplyFilter(wxCommandEvent& evt)
{
}

void pof::AuditView::OnColourAuditType(wxCommandEvent& evt)
{
}

void pof::AuditView::ShowAdditionalInfoPopup()
{
}
