#include "SearchPopup.h"
BEGIN_EVENT_TABLE(pof::SearchPopup, wxPopupWindow)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::SearchPopup::ID_DATA_VIEW, pof::SearchPopup::OnDataItemSelected)
EVT_SET_FOCUS(pof::SearchPopup::OnSetFocus)
EVT_KILL_FOCUS(pof::SearchPopup::OnKillFocus)
END_EVENT_TABLE()


pof::SearchPopup::SearchPopup(wxWindow* parent, std::shared_ptr<pof::base::data> ptrData, const std::vector<std::pair<std::string, size_t>>& colNames, const std::vector<size_t>& colSizes)
: wxPopupTransientWindow(parent, wxBORDER_NONE), mPopManager(this){
	mTableModel = new pof::DataModel(ptrData);
	mBook = new wxSimplebook(this, wxID_ANY);
	CreateDataView(colNames, colSizes);
	CreateNoResultPanel();

	mPopManager.AddPane(mBook, wxAuiPaneInfo().Name("Book").Caption("Book").CenterPane().Show());
	mPopManager.Update();
}

bool pof::SearchPopup::SearchString(size_t col, const std::string& searchString)
{
	bool empty = mTableModel->StringSearchAndReload(col, searchString);
	auto& modelItems = mTableModel->GetDataViewItems();
	if (modelItems.empty()) {
		ShowNoResult(searchString);
	}
	else {
		mBook->SetSelection(DATA_VIEW);
	}
	return empty;
}

void pof::SearchPopup::CreateDataView(const std::vector<std::pair<std::string, size_t>>& colNames, const std::vector<size_t>& colSizes)
{
	assert(colNames.size() == colSizes.size());
	wxPanel* p = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mTable = new wxDataViewCtrl(p, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxNO_BORDER);
	mTable->AssociateModel(mTableModel);
	mTableModel->DecRef();

	//columns
	int i = 0;
	for (auto& pair : colNames) {
		mTable->AppendTextColumn(pair.first, pair.second, wxDATAVIEW_CELL_INERT, FromDIP(colSizes[i]));
		i++;
	}

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(mTable, wxSizerFlags().Expand().Proportion(1));
	

	p->SetSizer(sizer);
	p->Layout();
	sizer->Fit(p);

	mBook->AddPage(p, "View", false);
}

void pof::SearchPopup::CreateNoResultPanel()
{
	mNoResult = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mNoResult->SetBackgroundColour(*wxWHITE);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mNoResult, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_WARNING, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mNoResultText = new wxStaticText(m7, wxID_ANY, wxT("No result"), wxDefaultPosition, wxDefaultSize, 0);
	mNoResultText->Wrap(100);
	bSizer9->Add(mNoResultText, 1, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

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


	mNoResult->SetSizer(bSizer6);
	mNoResult->Layout();

	mBook->AddPage(mNoResult, "Empty", true);
}

void pof::SearchPopup::CaptureFocus()
{
	mTable->SetFocus();
}

void pof::SearchPopup::PushSpecialCol(pof::DataModel::SpeicalColHandler_t&& spc, size_t col)
{
	mTableModel->SetSpecialColumnHandler(col, std::forward<pof::DataModel::SpeicalColHandler_t>(spc));
}

void pof::SearchPopup::ChangeFont(const wxFont& font)
{
	mTable->SetFont(font);
	mNoResultText->SetFont(font);
}

void pof::SearchPopup::SetSelected(const wxDataViewItem& item)
{
	if (item.IsOk()) {
		mTable->Select(item);
		mTable->EnsureVisible(item);
	}
}

void pof::SearchPopup::SetActivated(const wxDataViewItem& item)
{
	if (item.IsOk()) {
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		sSelectedSignal(mTableModel->GetDatastore()[idx]);

		Dismiss();
	}
}

wxDataViewItem pof::SearchPopup::GetNext(int dir)
{
	auto& items = mTableModel->GetDataViewItems();
	if (items.empty()) return wxDataViewItem{};
	//wrap
	if (dir >= static_cast<int>(items.size())) return items.front();
	if (dir < 0) return items.Last();

	return items[dir];
}

wxDataViewItem pof::SearchPopup::GetTop() const
{
	auto& ds = mTableModel->GetDataViewItems();
	return ds.empty() ? wxDataViewItem() : ds.front();
}

wxDataViewItem pof::SearchPopup::GetLast() const
{
	auto& ds = mTableModel->GetDataViewItems();
	return ds.empty() ? wxDataViewItem() : ds.Last();
}

void pof::SearchPopup::OnDataItemSelected(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	sSelectedSignal(mTableModel->GetDatastore()[idx]);

	Dismiss();
}

void pof::SearchPopup::OnSetFocus(wxFocusEvent& evt)
{
	spdlog::info("focus on object");
	mTable->SetFocus();
	evt.Skip();
}

void pof::SearchPopup::OnKillFocus(wxFocusEvent& evt)
{
	spdlog::info("focus killed on object");
	evt.Skip();
}

void pof::SearchPopup::ShowNoResult(const std::string& search)
{
	mNoResult->Freeze();
	mNoResultText->SetLabelText(fmt::format("No product \"{}\" in store", search));
	mNoResult->Layout();
	mNoResult->Thaw();

	mBook->SetSelection(NO_RESULT);
}
