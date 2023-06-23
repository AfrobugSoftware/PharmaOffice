#include "ProductView.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::ProductView, wxPanel)
EVT_SIZE(pof::ProductView::OnResize)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnProductActivated)
EVT_DATAVIEW_ITEM_BEGIN_DRAG(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnBeginDrag)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnContextMenu)

//TOOLS
EVT_TOOL(pof::ProductView::ID_ADD_PRODUCT, pof::ProductView::OnAddProduct)
EVT_TOOL(pof::ProductView::ID_ADD_CATEGORY, pof::ProductView::OnAddCategory)
EVT_TOOL(pof::ProductView::ID_PRODUCT_EXPIRE, pof::ProductView::OnExpiredProducts)
EVT_TOOL(pof::ProductView::ID_SELECT_MULTIPLE, pof::ProductView::OnSelection)
EVT_TOOL(pof::ProductView::ID_SHOW_COST_PRICE, pof::ProductView::OnShowCostPrice)

//Search
EVT_SEARCH(pof::ProductView::ID_SEARCH, pof::ProductView::OnSearchProduct)
EVT_SEARCH_CANCEL(pof::ProductView::ID_SEARCH, pof::ProductView::OnSearchCleared)
EVT_TEXT(pof::ProductView::ID_SEARCH, pof::ProductView::OnSearchProduct)

//CONTEXT MENU
EVT_MENU(pof::ProductView::ID_REMOVE_PRODUCT, pof::ProductView::OnRemoveProduct)
EVT_MENU(pof::ProductView::ID_ADD_ORDER_LIST, pof::ProductView::OnAddProductToOrderList)

END_EVENT_TABLE()



// Using the construction of a static object to ensure that the help provider is set
class wxFBContextSensitiveHelpSetter
{
public:
wxFBContextSensitiveHelpSetter()
{
wxHelpProvider::Set( new wxSimpleHelpProvider );
}
};

static wxFBContextSensitiveHelpSetter s_wxFBSetTheHelpProvider;

pof::ProductView::ProductView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(AUIMGRSTYLE);
	SetBackgroundColour(*wxWHITE); //move to theme
	CreateToolBar();
	CreateDataView();
	CreateProductInfo();
	SetupAuiTheme();
	CreateAttibutes();
	CreateSpecialCols();
	Style();
	m_mgr.Update();
}

pof::ProductView::~ProductView()
{
	m_mgr.UnInit();
	
}

void pof::ProductView::ReSizeColumns()
{
	auto width = GetSize().GetWidth();
	//15, 55, 10, 10, 10 
	float fact = 1.0f / 100.0f;
	
	int colWidth = static_cast<int>(width * (15.0f * fact));
	colWidth = colWidth > mSerialNumCol->GetWidth() ? colWidth : mSerialNumCol->GetWidth();
	mSerialNumCol->SetWidth(colWidth);


	colWidth = static_cast<int>(width * (55.0f * fact));
	colWidth = colWidth > mProductNameCol->GetWidth() ? colWidth : mProductNameCol->GetWidth();
	mProductNameCol->SetWidth(colWidth);

	colWidth = static_cast<int>(width * (10.0f * fact));
	colWidth = colWidth > mProductClass ->GetWidth() ? colWidth : mProductClass->GetWidth();
	mProductClass->SetWidth(colWidth);

	colWidth = static_cast<int>(width * (10.0f * fact));
	colWidth = colWidth > mStockLevel->GetWidth() ? colWidth : mStockLevel->GetWidth();
	mProductUnitPriceCol->SetWidth(colWidth);

	colWidth = static_cast<int>(width * (10.0f * fact));
	colWidth = colWidth > mProductUnitPriceCol->GetWidth() ? colWidth : mProductUnitPriceCol->GetWidth();
	mProductUnitPriceCol->SetWidth(colWidth);

	if (mProductCostPriceCol != nullptr) {
		colWidth = static_cast<int>(width * (10.0f * fact));
		colWidth = colWidth > mProductCostPriceCol->GetWidth() ? colWidth : mProductCostPriceCol->GetWidth();
		mProductUnitPriceCol->SetWidth(colWidth);
	}
}

void pof::ProductView::SaveColumnWidths()
{
}

void pof::ProductView::SetupAuiTheme()
{
	auto auiArtProvider = m_mgr.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
	pof::AuiTheme::sSignal.connect(std::bind_front(&pof::ProductView::OnAuiThemeChange, this));
}

void pof::ProductView::OnResize(wxSizeEvent& evt)
{
	ReSizeColumns();
	evt.Skip();
}

void pof::ProductView::OnAuiThemeChange()
{
	auto auiArtProvider = m_mgr.GetArtProvider();
	pof::AuiTheme::Update(auiArtProvider);
}

void pof::ProductView::OnProductActivated(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	
	auto Model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(Model != nullptr && "Model is not valid");
	auto& datastore = Model->GetDatastore();
	const size_t idx = pof::DataModel::GetIdxFromItem(item);
	const pof::base::data::row_t& row = datastore[idx];
	mProductinfo->Load(row);
	SwapCenterPane(true);
	
}

void pof::ProductView::OnBeginDrag(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;
	const size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto Model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	auto& row = Model->GetDatastore()[idx];
	auto& meta = Model->GetDatastore().get_metadata();
	auto DataObject = new pof::DataObject("PRODUCTDATA"s, row, meta);
	evt.SetDataObject(DataObject);
}

void pof::ProductView::OnExpiredProducts(wxCommandEvent& evt)
{
	if (wxGetApp().bUsingLocalDatabase) {
		//
	}
}

void pof::ProductView::OnAddProduct(wxCommandEvent& evt)
{
	pof::AddProdutDialog dialog(this);
	dialog.Center();
	if (dialog.ShowModal() == wxID_OK) {
		auto productopt = dialog.GetAddedProduct();
		if (productopt.has_value()) {
			wxGetApp().mProductManager.GetProductData()->EmplaceData(std::move(productopt.value()));
		}
		auto productinvenopt = dialog.GetAddedInventory();
		if (productinvenopt.has_value()) {
			wxGetApp().mProductManager.GetInventory()->EmplaceData(std::move(productinvenopt.value()));
		}
		wxMessageBox("Product Added", "ADD PRODUCT");
	}
}

void pof::ProductView::OnAddCategory(wxCommandEvent& evt)
{
	wxTextEntryDialog dialog(this, "Please enter a name for a category", "ADD CATEGORY");
	dialog.Center();
	if (dialog.ShowModal() == wxID_OK) {
		auto CategoryName = dialog.GetValue().ToStdString();
		if (CategoryName.empty()) return;
		//what else are we gonna do with category
		if (wxGetApp().bUsingLocalDatabase) {
			//write category to database 
		}
		CategoryAddSignal(CategoryName);
		//add to the database
	}

}

void pof::ProductView::OnSearchFlag(wxCommandEvent& evt)
{
}

void pof::ProductView::OnContextMenu(wxDataViewEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto orderlist = menu->Append(ID_ADD_ORDER_LIST, "Add Order List", nullptr);
	auto remv = menu->Append(ID_REMOVE_PRODUCT, "Remove Product", nullptr);
	orderlist->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY));
	remv->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE));

	PopupMenu(menu);
}

void pof::ProductView::OnRemoveProduct(wxCommandEvent& evt)
{
	//check privilage
	
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	if (wxMessageBox("Deleteing a product deletes all the data associated with the product, Do you wish to continue?", "REMOVE PRODUCT", wxICON_WARNING | wxYES_NO) == wxYES) {
		

	}
}

void pof::ProductView::OnAddProductToOrderList(wxCommandEvent& evt)
{
}

void pof::ProductView::OnSearchProduct(wxCommandEvent& evt)
{
	pof::DataModel* datam = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(datam != nullptr);
	m_dataViewCtrl1->Freeze();
	std::string search = evt.GetString().ToStdString();
	if (search.empty()) {
		//go back to what was there before the search?
		datam->Reload();
	}
	else {
		datam->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, std::move(search));
	}
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Update();
}

void pof::ProductView::OnSearchCleared(wxCommandEvent& evt)
{
	pof::DataModel* datam = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(datam != nullptr);
	m_dataViewCtrl1->Freeze();
	datam->Reload();
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Update();
}

void pof::ProductView::OnSelection(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowSelectionColumn();
	}
	else {
		HideSelectionColumn();
	}
}

void pof::ProductView::OnShowCostPrice(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		ShowCostPriceColumn();
	}
	else {
		HideCostPriceColumn();
	}
}

void pof::ProductView::OnProductInfoUpdated(const pof::ProductInfo::PropertyUpdate& mUpdatedElem)
{
	auto& DatModelptr = wxGetApp().mProductManager.GetProductData();
	auto& DataStore = DatModelptr->GetDatastore();
	auto Iter = std::find_if(DataStore.begin(), DataStore.end(), [&](const pof::base::data::row_t& elem) -> bool {
		return  (boost::variant2::get<std::uint64_t>(elem.first[pof::ProductManager::PRODUCT_SERIAL_NUM]) ==
			boost::variant2::get<uint64_t>(mUpdatedElem.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_SERIAL_NUM]));
	});
	if (Iter == DataStore.end()) return; //nothing to update

	for (size_t i = 0; i < pof::ProductManager::PRODUCT_MAX; i++) {
		if (mUpdatedElem.mUpdatedElememts.test(i)) {
			Iter->first[i] = mUpdatedElem.mUpdatedElementsValues.first[i];
			Iter->second.second.set(i); //set the column as modified
		}
	}
	int idx = std::distance(DataStore.begin(), Iter);
	DataStore.set_state(idx,pof::base::data::state::MODIFIED);
	wxDataViewItem i{ reinterpret_cast<void*>(++idx) };
	DatModelptr->AddAttr(i, mUpdatedAttr); //set timer to remove attribute
}

//remove this one, use only activated 
void pof::ProductView::OnCategorySelected(const std::string& name)
{
	wxMessageBox(fmt::format("{} is selected", name), "Category");
}

void pof::ProductView::ShowCostPriceColumn()
{
	mProductCostPriceCol = m_dataViewCtrl1->AppendTextColumn("COST PRICE", pof::ProductManager::PRODUCT_COST_PRICE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
}

void pof::ProductView::HideCostPriceColumn()
{
	if (mProductCostPriceCol) {
		m_dataViewCtrl1->Freeze();
		m_dataViewCtrl1->DeleteColumn(mProductCostPriceCol);
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Update();
		mProductCostPriceCol = nullptr;
	}
}

void pof::ProductView::ShowSelectionColumn(){
	mSelectionCol = m_dataViewCtrl1->PrependToggleColumn(wxT("SELECT"), SELECTION_MODEL_COL,
		 wxDATAVIEW_CELL_ACTIVATABLE, 50);
}

void pof::ProductView::HideSelectionColumn()
{
	if (mSelectionCol != nullptr) {
		mSelections.clear();

		m_dataViewCtrl1->Freeze();
		m_dataViewCtrl1->DeleteColumn(mSelectionCol);
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Update();
		mSelectionCol = nullptr;
	}
}


void pof::ProductView::OnCategoryActivated(const std::string& name)
{
	wxMessageBox(fmt::format("{} is selected", name), "Category");
}

void pof::ProductView::CreateDataView()
{

	m_dataViewCtrl1 = new wxDataViewCtrl(this, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES);
	auto& pm = wxGetApp().mProductManager;
	m_dataViewCtrl1->AssociateModel(pm.GetProductData().get());
	pm.GetProductData()->DecRef();
	
	mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Serial #"), pof::ProductManager::PRODUCT_SERIAL_NUM, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductClass = m_dataViewCtrl1->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductUnitPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("Stock Count"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockLevel = m_dataViewCtrl1->AppendTextColumn(wxT("Unit Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);

	m_mgr.AddPane(m_dataViewCtrl1, wxAuiPaneInfo().Name("DataView").CenterPane().CaptionVisible(false));
	
}

void pof::ProductView::CreateToolBar()
{
	m_auiToolBar1 = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	m_auiToolBar1->SetToolBitmapSize(wxSize(16,16));

	m_searchCtrl1 = new wxSearchCtrl(m_auiToolBar1, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(500, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	m_searchCtrl1->ShowSearchButton(true);
#endif
	m_searchCtrl1->ShowCancelButton(true);
	m_searchCtrl1->SetDescriptiveText("Search products by name");
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->Check(ID_SEARCH_BY_NAME, true);

	m_searchCtrl1->SetMenu(menu);
	m_auiToolBar1->AddControl(m_searchCtrl1);

	m_auiToolBar1->AddStretchSpacer();
	m_auiToolBar1->AddSeparator();

	m_auiToolBar1->AddTool(ID_SHOW_COST_PRICE, wxT("Cost Price"), wxArtProvider::GetBitmap(wxART_MINUS, wxART_TOOLBAR), "Select multiple products", wxITEM_CHECK);
	m_auiToolBar1->AddTool(ID_SELECT_MULTIPLE, wxT("Select"), wxArtProvider::GetBitmap(wxART_MINUS, wxART_TOOLBAR), "Select multiple products", wxITEM_CHECK);
	m_auiToolBar1->AddTool(ID_ADD_PRODUCT, wxT("Add Product"), wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR), "Add a new Product");
	m_auiToolBar1->AddTool(ID_ADD_CATEGORY, wxT("Add Category"), wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR), wxT("Creates a new Category for medical products"));
	m_auiToolBar1->AddTool(ID_PRODUCT_EXPIRE, wxT("Expired Products"), wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR), wxT("List of Products that are expired, or expired alerted"));


	m_auiToolBar1->Realize();

	m_mgr.AddPane(m_auiToolBar1, wxAuiPaneInfo().Name("ProductToolBar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Resizable().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::ProductView::CreateProductInfo()
{
	mProductinfo = new pof::ProductInfo(this, ID_PRODUCTINFO);
	mProductinfo->AttachBackSlot([&]() { SwapCenterPane(false); });
	mProductinfo->AttachPropertyUpdateSlot(std::bind_front(&pof::ProductView::OnProductInfoUpdated, this));

	m_mgr.AddPane(mProductinfo, wxAuiPaneInfo().Name("ProductInfo").CenterPane().Hide());

}

void pof::ProductView::CreateAttibutes()
{
	mUpdatedAttr = std::make_shared<wxDataViewItemAttr>();
	mUpdatedAttr->SetBackgroundColour(wxTheColourDatabase->Find("Tomato"));
}

void pof::ProductView::CreateSpecialCols()
{
	pof::DataModel::SpeicalColHandler_t SelectionCol;
	SelectionCol.first = [&](size_t row, size_t col) -> wxVariant {
		auto found = mSelections.find(wxDataViewItem{ reinterpret_cast<void*>(row + 1) });
		return wxVariant((found != mSelections.end()));
	};

	SelectionCol.second = [&](size_t row, size_t col, const wxVariant& data) -> bool {
		if (data.GetBool()) {
			auto [iter, inserted] = mSelections.insert(wxDataViewItem{ reinterpret_cast<void*>(row + 1) });
			return inserted;
		}
		else {
			mSelections.erase(wxDataViewItem{ reinterpret_cast<void*>(row + 1) });
			return true;
		}
	};

	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(model != nullptr);
	model->SetSpecialColumnHandler(SELECTION_MODEL_COL, std::move(SelectionCol));
}

void pof::ProductView::Style()
{
	//m_dataViewCtrl1->SetAlternateRowColour(wxTheColourDatabase->Find("Aqua"));
}

void pof::ProductView::SwapCenterPane(bool IsInventoryView)
{
	auto& Pane = m_mgr.GetPane("ProductInfo");
	auto& ProductPane = m_mgr.GetPane("DataView");
	auto& ProductToolBarPane = m_mgr.GetPane("ProductToolBar");
	if (!Pane.IsOk() || !ProductPane.IsOk() || !ProductToolBarPane.IsOk()) return;
	Pane.Show(IsInventoryView);
	ProductPane.Show(!IsInventoryView);
	ProductToolBarPane.Show(!IsInventoryView);
	m_mgr.Update();

}
