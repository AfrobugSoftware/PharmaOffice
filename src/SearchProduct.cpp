#include "SearchProduct.h"
#include "Application.h"


BEGIN_EVENT_TABLE(pof::SearchProduct, wxDialog)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::SearchProduct::ID_SEARCH_VIEW, pof::SearchProduct::OnActivated)
EVT_SEARCH(pof::SearchProduct::ID_SEARCH_CTRL, pof::SearchProduct::OnSearch)
EVT_CLOSE(pof::SearchProduct::OnClose)
EVT_SEARCH_CANCEL(pof::SearchProduct::ID_SEARCH_CTRL, pof::SearchProduct::OnSearchCancelled)
EVT_TEXT(pof::SearchProduct::ID_SEARCH_CTRL, pof::SearchProduct::OnSearch)
EVT_TOOL(pof::SearchProduct::ID_ADD_PRODUCT, pof::SearchProduct::OnAddProduct )
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
}

pof::SearchProduct::~SearchProduct()
{
	mModel->RemoveSpecialColumnHandler(1000);
	m_mgr.UnInit();

}

void pof::SearchProduct::CreateToolBar()
{
	SearchProductBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_OVERFLOW);
	SearchProductBar->SetToolBitmapSize(wxSize(16, 16));
	SearchCtrl = new wxSearchCtrl(SearchProductBar, ID_SEARCH_CTRL, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	SearchCtrl->ShowSearchButton(true);
#endif
	SearchCtrl->ShowCancelButton(true);
	SearchCtrl->SetToolTip(wxT("Search for product to sell"));
	SearchCtrl->SetDescriptiveText("Search products by name");

	SearchProductBar->AddControl(SearchCtrl);
	//m_tool1 = SearchProductBar->AddTool(ID_SEARCH_BUTTON, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
	SearchProductBar->AddStretchSpacer();
	SearchProductBar->AddSeparator();
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
	SearchData->AppendToggleColumn(wxT("Select"), 1000, wxDATAVIEW_CELL_ACTIVATABLE, 50);
	SearchData->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Formulation"), pof::ProductManager::PRODUCT_FORMULATION, wxDATAVIEW_CELL_INERT, 300, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, 300, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Stock"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, 300, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	SearchData->AppendTextColumn(wxT("Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, 120, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	bSizer1->Add(SearchData, 1, wxALL | wxEXPAND, 0);

	SearchData->AssociateModel(mModel);
	mModel->DecRef();

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
	if (!item.IsOk()) return;

	mSelectedProduct = pof::DataModel::GetIdxFromItem(item);
	mSelectedProducts.clear(); 

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
	evt.Skip();
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
	EndModal(wxID_OK);
}

void pof::SearchProduct::DoSearch(const std::string& search_for)
{
	if (search_for.empty()) {
		mModel->Reload();
		return;
	}
	mModel->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, search_for);
}
