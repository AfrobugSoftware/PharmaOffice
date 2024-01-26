#include "SearchProduct.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::SearchProduct, wxDialog)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::SearchProduct::ID_SEARCH_VIEW, pof::SearchProduct::OnActivated)
EVT_DATAVIEW_SELECTION_CHANGED(pof::SearchProduct::ID_SEARCH_VIEW, pof::SearchProduct::OnItemSelected)
EVT_SEARCH(pof::SearchProduct::ID_SEARCH_CTRL, pof::SearchProduct::OnSearch)
EVT_CLOSE(pof::SearchProduct::OnClose)
EVT_SEARCH_CANCEL(pof::SearchProduct::ID_SEARCH_CTRL, pof::SearchProduct::OnSearchCancelled)
EVT_TEXT(pof::SearchProduct::ID_SEARCH_CTRL, pof::SearchProduct::OnSearch)
EVT_TOOL(pof::SearchProduct::ID_ADD_PRODUCT, pof::SearchProduct::OnAddProduct )
EVT_TOOL(pof::SearchProduct::ID_FILTER, pof::SearchProduct::OnFilter)
EVT_TOOL(pof::SearchProduct::ID_RESET, pof::SearchProduct::OnReset)
EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::SearchProduct::ID_SEARCH_VIEW, pof::SearchProduct::OnHeaderClicked)
EVT_CHOICE(pof::SearchProduct::ID_CATEGORY_FILTER, pof::SearchProduct::OnFilerChoice)
EVT_CHOICE(pof::SearchProduct::ID_FORMULATION_FILTER, pof::SearchProduct::OnFilerChoice)
END_EVENT_TABLE()


pof::SearchProduct::SearchProduct(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : 
	wxDialog(parent, id, title, pos, size, style), mSelectedProduct(-1)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
	CreateViewManagerDefaultArt();
	CreateToolBar();
	CreateSearchView();

	//load the whole product relation
	SetUpView();

	m_mgr.Update();
	this->Centre(wxBOTH);


	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::SearchProduct::~SearchProduct()
{
	mModel->RemoveSpecialColumnHandler(1000);
	m_mgr.UnInit();

}

void pof::SearchProduct::CreateToolBar()
{
	SearchProductBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW);
	SearchProductBar->SetToolBitmapSize(wxSize(16, 16));
	SearchCtrl = new wxSearchCtrl(SearchProductBar, ID_SEARCH_CTRL, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	SearchCtrl->ShowSearchButton(true);
#endif
	SearchCtrl->ShowCancelButton(true);
	SearchCtrl->SetToolTip(wxT("Search for product to sell"));
	SearchCtrl->SetDescriptiveText("Search products by name");

	SearchProductBar->AddControl(SearchCtrl);
	SearchProductBar->AddStretchSpacer();
	SearchProductBar->AddSeparator();

	auto& category = wxGetApp().mProductManager.GetCategories();
	wxArrayString choices;

	for (auto& value : category) {
		choices.push_back(boost::variant2::get<pof::base::data::text_t>(value.first[pof::ProductManager::CATEGORY_NAME]));
	}

	mCategoryFilter = new wxChoice(SearchProductBar, ID_CATEGORY_FILTER, wxDefaultPosition, wxSize(200, -1), choices);
	mCategoryFilter->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mCategoryFilter);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = mCategoryFilter->GetStringSelection();
	if (!sel.IsEmpty()) {
		dc.DrawLabel(sel, rect, wxALIGN_CENTER);
	}
	});
	SearchProductBar->AddControl(mCategoryFilter, "Select Category");
	SearchProductBar->AddSpacer(10);

	mFormulationFilter = new wxChoice(SearchProductBar, ID_FORMULATION_FILTER, wxDefaultPosition, wxSize(200, -1), wxGetApp().FormulationChoices);
	mFormulationFilter->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mFormulationFilter);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = mFormulationFilter->GetStringSelection();
	if (!sel.IsEmpty()) {
		dc.DrawLabel(sel, rect, wxALIGN_CENTER);
		}
	});
	SearchProductBar->AddControl(mFormulationFilter, "Select Formulaton");

	SearchProductBar->AddSpacer(5);
	SearchProductBar->AddTool(ID_FILTER, wxT("Filter"), wxArtProvider::GetBitmap("pen"));
	SearchProductBar->AddSpacer(2);
	SearchProductBar->AddTool(ID_RESET, wxT("Reset"), wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR, wxSize(16,16)));
	SearchProductBar->AddSpacer(2);
	SearchProductBar->AddTool(ID_ADD_PRODUCT, wxT("Add Product"), wxArtProvider::GetBitmap("action_add"));
	SearchProductBar->Realize();
	m_mgr.AddPane(SearchProductBar, wxAuiPaneInfo().Name(wxT("SearchToolBar")).Top().MinSize(-1, 30).DockFixed().CloseButton(false).PaneBorder(false).Dock().Resizable().BottomDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).Layer(10).ToolbarPane());

}

void pof::SearchProduct::CreateSearchView()
{
	m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_mgr.AddPane(m_panel1, wxAuiPaneInfo().Center().CaptionVisible(false).CloseButton(false).PaneBorder(false).Movable(false).Dock().Resizable().FloatingSize(wxDefaultSize).BottomDockable(false).TopDockable(false).LeftDockable(false).RightDockable(false).Floatable(false).CentrePane());

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	mModel = new DataModel(wxGetApp().mProductManager.GetProductData()->ShareDatastore());

	SearchData = new wxDataViewCtrl(m_panel1, ID_SEARCH_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES | wxDV_ROW_LINES);
	selectedCol = SearchData->AppendToggleColumn(wxT("Select"), 1000, wxDATAVIEW_CELL_ACTIVATABLE, 50);
	SearchData->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 300, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Formulation"), pof::ProductManager::PRODUCT_FORMULATION, wxDATAVIEW_CELL_INERT, 100, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, 100, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Stock"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, 100, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	bSizer1->Add(SearchData, 1, wxALL | wxEXPAND, 0);

	SearchData->AssociateModel(mModel);
	mModel->DecRef();
	mModel->Reload();

	pof::DataModel::SpeicalColHandler_t selectCol;
	selectCol.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mSelectedProducts.find(row);
		return (found != mSelectedProducts.end());
	};

	selectCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {
		if (value.GetBool()) {
			auto [iter, inserted] = mSelectedProducts.insert(row);
			return inserted;
		}
		else {
			mSelectedProducts.erase(row);
			return true;
		}
	};

	mModel->SetSpecialColumnHandler(1000, std::move(selectCol));
	m_panel1->SetSizer(bSizer1);
	m_panel1->Layout();
	bSizer1->Fit(m_panel1);
}

void pof::SearchProduct::CreateViewManagerDefaultArt()
{
	wxColour colour = wxTheColourDatabase->Find("Aqua");
	wxAuiDockArt* art = m_mgr.GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 24);
	art->SetMetric(wxAUI_DOCKART_GRIPPER_SIZE, 5);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 5);
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, *wxWHITE);
	art->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, colour);
	art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, *wxWHITE);
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	art->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_VENETIAN_BLINDS_HINT);
}

void pof::SearchProduct::SetUpView()
{
	
}

void pof::SearchProduct::OnItemSelected(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	mSelectedProduct = pof::DataModel::GetIdxFromItem(item);
}

void pof::SearchProduct::OnActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk() || !mSelectedProducts.empty()) return;

	mSelectedProduct = pof::DataModel::GetIdxFromItem(item);
 

	EndModal(wxID_OK);
}

void pof::SearchProduct::OnSearch(wxCommandEvent& evt) {
	std::string search = evt.GetString().ToStdString();
	DoSearch(search);
}

void pof::SearchProduct::OnSearchCancelled(wxCommandEvent& evt)
{
	mModel->Reload();
}

void pof::SearchProduct::OnClose(wxCloseEvent& evt)
{
	SearchCtrl->Destroy();
	SetReturnCode(wxID_CANCEL);
	evt.Skip();
}

const pof::base::data::row_t& pof::SearchProduct::GetSelectedProduct() const
{
	//mSelectedProduct should always point to a valid product
	return mModel->GetDatastore()[mSelectedProduct];
}

const std::vector<std::reference_wrapper<pof::base::data::row_t>> 
	pof::SearchProduct::GetSelectedProducts() const
{
	std::vector<std::reference_wrapper<pof::base::data::row_t>> ret;
	if (mSelectedProducts.empty()) return ret;
	auto& datastore = mModel->GetDatastore();

	ret.reserve(mSelectedProducts.size());
	for (auto& i : mSelectedProducts) {
		ret.emplace_back(std::ref(datastore[i]));
	}
	return ret;
}

void pof::SearchProduct::OnAddProduct(wxCommandEvent& evt)
{
	if (mSelectedProduct == wxNOT_FOUND && mSelectedProducts.empty()) {
		wxMessageBox("No product selected to add, please select one", "SEARCH PRODUCT", wxICON_WARNING | wxOK);
		return;
	}
	if(IsModal()) 
		EndModal(wxID_OK);
}
void pof::SearchProduct::OnHeaderClicked(wxDataViewEvent& evt)
{
	if (selectedCol == evt.GetDataViewColumn()){
		static bool sel = true;
		SearchData->Freeze();
		auto& items = mModel->GetDataViewItems();
		if (sel) {
			if (!mSelectedProducts.empty()) {
				mSelectedProducts.clear();
				sel = false;
			}
			else {
				std::ranges::transform(items, std::inserter(mSelectedProducts, mSelectedProducts.end()), [&](const wxDataViewItem& item) {return pof::DataModel::GetIdxFromItem(item); });
			}
		}
		else {
			for (auto& item : items) {
				mSelectedProducts.erase(pof::DataModel::GetIdxFromItem(item));
			}
		}
		sel = !sel;
		SearchData->Thaw();
		SearchData->Refresh();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

void pof::SearchProduct::OnFilter(wxCommandEvent& evt)
{
	if (mCategoryName.empty() && mFormulation.empty()) return; //nothing to filer
	
	std::vector<wxDataViewItem> mCategoryItems;
	auto& datastore = mModel->GetDatastore();
	mCategoryItems.reserve(datastore.size());

	if (!mCategoryName.empty())
	{
	//filter the store with the category them the formulation.
		auto& categories = wxGetApp().mProductManager.GetCategories();
		//auto categories = mCategoryFilter->GetStrings();
		auto iter = std::ranges::find_if(categories, [&](const pof::base::data::row_t& row) -> bool {
			return (boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::CATEGORY_NAME]) == mCategoryName);
			});
		if (iter == categories.end()) return;

		size_t i = 0;
		for (const auto& row : datastore) {
			if (boost::variant2::get<std::uint64_t>(row.first[pof::ProductManager::PRODUCT_CATEGORY])
				== boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::CATEGORY_ID])) {
				if (mFormulation ==
					boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_FORMULATION])) {
					mCategoryItems.emplace_back(pof::DataModel::GetItemFromIdx(i));
				}
			}
			i++;
		}
	} else {
		//filter the whole store with the formulation only
		size_t i = 0;
		for (const auto& row : datastore) {
			if (mFormulation ==
				boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_FORMULATION])) {
				mCategoryItems.emplace_back(pof::DataModel::GetItemFromIdx(i));
			}
			i++;
		}
	}
	mCategoryItems.shrink_to_fit();
	SearchData->Freeze();
	mModel->Reload(std::move(mCategoryItems));
	SearchData->Thaw();
	SearchData->Refresh();
}

void pof::SearchProduct::OnFilerChoice(wxCommandEvent& evt)
{
	int id = evt.GetId();
	switch (id)
	{
	case ID_CATEGORY_FILTER:
		mCategoryName = mCategoryFilter->GetStringSelection().ToStdString();
		break;
	case ID_FORMULATION_FILTER:
		mFormulation = mFormulationFilter->GetStringSelection().ToStdString();
		break;
	default:
		break;
	}
}

void pof::SearchProduct::OnReset(wxCommandEvent& evt)
{
	mCategoryName.clear();
	mFormulation.clear();
	
	mCategoryFilter->Freeze();
	mFormulationFilter->Freeze();
	mCategoryFilter->SetSelection(wxNOT_FOUND);
	mFormulationFilter->SetSelection(wxNOT_FOUND);

	mFormulationFilter->Thaw();
	mCategoryFilter->Thaw();
	mFormulationFilter->Refresh();
	mFormulationFilter->Refresh();

	SearchData->Freeze();
	mModel->Reload();
	SearchData->Thaw();
	SearchData->Refresh();

}

void pof::SearchProduct::DoSearch(const std::string& search_for)
{
	if (search_for.empty()) {
		mModel->Reload();
		return;
	}
	mModel->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, search_for);
}
