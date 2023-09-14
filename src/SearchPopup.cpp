#include "SearchPopup.h"
BEGIN_EVENT_TABLE(pof::SearchPopup, wxPopupWindow)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::SearchPopup::ID_DATA_VIEW, pof::SearchPopup::OnDataItemSelected)
END_EVENT_TABLE()


pof::SearchPopup::SearchPopup(wxWindow* parent, std::shared_ptr<pof::base::data> ptrData, const std::vector<std::pair<std::string, size_t>>& colNames, const std::vector<size_t>& colSizes)
: wxPopupTransientWindow(parent){
	mTableModel = new pof::DataModel(ptrData);
	CreateDataView(colNames, colSizes);
}

void pof::SearchPopup::SearchString(size_t col, const std::string& searchString)
{
	mTableModel->StringSearchAndReload(col, searchString);
}

void pof::SearchPopup::CreateDataView(const std::vector<std::pair<std::string, size_t>>& colNames, const std::vector<size_t>& colSizes)
{
	assert(colNames.size() == colSizes.size());
	mTable = new wxDataViewCtrl(this, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES);
	mTable->AssociateModel(mTableModel);
	mTableModel->DecRef();

	//columns
	int i = 0;
	for (auto& pair : colNames) {
		mTable->AppendTextColumn(pair.first, pair.second, wxDATAVIEW_CELL_INERT, colSizes[i]);
		i++;
	}

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(mTable, wxSizerFlags().Expand().Proportion(1));
	

	this->SetSizer(sizer);
	this->Layout();
	sizer->Fit(this);

}

void pof::SearchPopup::CaptureFocus()
{
	mTable->SetFocus();
}

void pof::SearchPopup::OnDataItemSelected(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	sSelectedSignal(mTableModel->GetDatastore()[idx]);

	Dismiss();
}
