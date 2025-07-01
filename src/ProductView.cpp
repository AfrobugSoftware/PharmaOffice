#include "ProductView.h"
//#include "Application.h"
#include "PofPch.h"

#include <cmath>

BEGIN_EVENT_TABLE(pof::ProductView, wxPanel)
	EVT_SIZE(pof::ProductView::OnResize)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnProductActivated)
	EVT_DATAVIEW_ITEM_BEGIN_DRAG(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnBeginDrag)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnContextMenu)
	EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnHeaderClicked)
	EVT_DATAVIEW_CACHE_HINT(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnCacheHint)

	//TOOLS
	EVT_TOOL(pof::ProductView::ID_ADD_PRODUCT, pof::ProductView::OnAddProduct)
	EVT_TOOL(pof::ProductView::ID_ADD_CATEGORY, pof::ProductView::OnAddCategory)
	EVT_TOOL(pof::ProductView::ID_SELECT_MULTIPLE, pof::ProductView::OnSelection)
	EVT_TOOL(pof::ProductView::ID_OUT_OF_STOCK, pof::ProductView::OnOutOfStock)
	EVT_TOOL(pof::ProductView::ID_PACKS, pof::ProductView::OnPacks)
	EVT_TOOL(pof::ProductView::ID_ORDER_LIST, pof::ProductView::OnShowOrderList)
	EVT_TOOL(pof::ProductView::ID_SHOW_SUPPLIER, pof::ProductView::OnShowSupplier)
	EVT_TOOL(pof::ProductView::ID_SHOW_PRODUCT, pof::ProductView::OnShowHiddenProduct)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::ProductView::ID_PRODUCT_EXPIRE, pof::ProductView::OnExpiredProducts)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::ProductView::ID_REPORTS, pof::ProductView::OnReportDropdown)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::ProductView::ID_FUNCTIONS, pof::ProductView::OnFunctions)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::ProductView::ID_CHARTS, pof::ProductView::OnChartDropDown)


	//button
	EVT_BUTTON(pof::ProductView::ID_ADD_PRODUCT, pof::ProductView::OnAddProduct)


	//Search
	EVT_SEARCH(pof::ProductView::ID_SEARCH, pof::ProductView::OnSearchProduct)
	EVT_SEARCH_CANCEL(pof::ProductView::ID_SEARCH, pof::ProductView::OnSearchCleared)
	EVT_TEXT(pof::ProductView::ID_SEARCH, pof::ProductView::OnSearchProduct)

	//CONTEXT MENU
	EVT_MENU(pof::ProductView::ID_REMOVE_PRODUCT, pof::ProductView::OnRemoveProduct)
	EVT_MENU(pof::ProductView::ID_ADD_ORDER_LIST, pof::ProductView::OnAddProductToOrderList)
	EVT_MENU(pof::ProductView::ID_ADD_INVENTORY, pof::ProductView::OnAddInventory)

	EVT_MENU(pof::ProductView::ID_REPORTS_CONSUMPTION_PATTERN, pof::ProductView::OnConsumptionPattern)
	EVT_MENU(pof::ProductView::ID_REPORTS_ENDOFDAY, pof::ProductView::OnEndOfDayReport)
	EVT_MENU(pof::ProductView::ID_REPORTS_INVENTORY, pof::ProductView::OnEndOfDayReport)
	EVT_MENU(pof::ProductView::ID_REPORTS_PROFITLOSS, pof::ProductView::OnEndOfDayReport)
	EVT_MENU(pof::ProductView::ID_REPORTS_PRODUCT_SOLD, pof::ProductView::OnEndOfDayReport)
	EVT_MENU(pof::ProductView::ID_REPORTS_ENDOFWEEK, pof::ProductView::OnEndOfDayReport)
	EVT_MENU(pof::ProductView::ID_REPORTS_EXPIRED_PRODUCTS, pof::ProductView::OnEndOfDayReport)

	EVT_MENU(pof::ProductView::ID_REPORTS_EOM, pof::ProductView::OnEndOfMonth)
	EVT_MENU(pof::ProductView::ID_REMOVE_FROM_CATEGORY, pof::ProductView::OnRemoveFromCategory)
	EVT_MENU(pof::ProductView::ID_FUNCTION_BROUGHT_FORWARD, pof::ProductView::OnBFFunction)
	EVT_MENU(pof::ProductView::ID_SHOW_COST_PRICE, pof::ProductView::OnShowCostPrice)
	EVT_MENU(pof::ProductView::ID_PRODUCT_MARKUP, pof::ProductView::OnMarkUp)
	EVT_MENU(pof::ProductView::ID_FUNCTION_STOCK_CHECK, pof::ProductView::OnSCFunction)
	EVT_MENU(pof::ProductView::ID_FUNCTION_MARK_UP_PRODUCTS, pof::ProductView::OnMarkUpProducts)
	EVT_MENU(pof::ProductView::ID_MOVE_PRODUCT_STOCK, pof::ProductView::OnMoveExpiredStock)
	EVT_MENU(pof::ProductView::ID_DOWNLOAD_EXCEL, pof::ProductView::OnDownloadExcel)
	EVT_MENU(pof::ProductView::ID_CREATE_CONTROLLED_BOOK, pof::ProductView::OnCreateControlBook)
	EVT_MENU(pof::ProductView::ID_ADD_VARIANT, pof::ProductView::OnAddVariant)
	EVT_MENU(pof::ProductView::ID_STORE_SUMMARY, pof::ProductView::OnStoreSummary)
	EVT_MENU(pof::ProductView::ID_INCR_PRICE, pof::ProductView::OnIncrPrice)
	EVT_MENU(pof::ProductView::ID_INCR_PRODUCT_PRICE, pof::ProductView::OnIncrPrice)
	EVT_MENU(pof::ProductView::ID_OPEN_PRODUCT_INFO, pof::ProductView::OnOpenProductInfo)
	EVT_MENU(pof::ProductView::ID_DOWNLOAD_ACTUAL_STOCK, pof::ProductView::OnDownloadActualStock)
	EVT_MENU(pof::ProductView::ID_HIDE_PRODUCT, pof::ProductView::OnHideProduct)
	EVT_MENU(pof::ProductView::ID_TRANSFERS, pof::ProductView::OnShowTransfers)

	EVT_MENU(pof::ProductView::ID_CHARTS_WEEKLY_SALES, pof::ProductView::OnWeeklySales)
	EVT_MENU(pof::ProductView::ID_CHARTS_COMPARE_SALES, pof::ProductView::OnCompareSales)

	EVT_MENU(pof::ProductView::ID_SEARCH_BY_NAME, pof::ProductView::OnSearchFlag)
	EVT_MENU(pof::ProductView::ID_SEARCH_BY_GENERIC_NAME, pof::ProductView::OnSearchFlag)
	EVT_MENU(pof::ProductView::ID_SEARCH_BY_CATEGORY, pof::ProductView::OnSearchFlag)
	EVT_MENU(pof::ProductView::ID_UPDATE_QUANTITY, pof::ProductView::OnUpdateQuantity)
	//TIMER
	EVT_TIMER(pof::ProductView::ID_STOCK_CHECK_TIMER, pof::ProductView::OnStockCheckTimer)
	//UI update
	EVT_UPDATE_UI(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnUpdateUI)
END_EVENT_TABLE()


pof::ProductView::ProductView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) 
	: wxPanel( parent, id, pos, size, style ), mStockCheckTimer(this, ID_STOCK_CHECK_TIMER)
{
	//SetDoubleBuffered(true);
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(AUIMGRSTYLE);
	SetSizeHints(wxDefaultSize, wxDefaultSize);
	SetBackgroundColour(*wxWHITE); //move to theme

	SetupAuiTheme();
	CreateToolBar();
	CreateDataView();
	CreateProductInfo();
	CreateAttibutes();
	CreateSpecialCols();
	Style();
	CreateAccTable();
	CreateEmptyPanel();
	CreateNoResultPane();
	CreateSupplierView();
	//load base data
	wxGetApp().mProductManager.LoadProductsFromDatabase();
	wxGetApp().mProductManager.LoadCategories();

	//DoBroughtForward();

	CheckEmpty();
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

void pof::ProductView::SetupDataViewStyle()
{
	//move to theme
	//for now 
	mRowHeights = 20;
	mHeaderAttr.SetBackgroundColour(*wxWHITE);
	auto Info = wxFontInfo(10).Weight(24).FaceName("Helvetica").Bold();
	mHeaderAttr.SetFont(wxFont(std::move(Info)));

	auto Info2 = wxFontInfo(11).FaceName("Helvetica");
	mDataViewFont = wxFont(std::move(Info2));

	
	m_dataViewCtrl1->SetFont(mDataViewFont);
	m_dataViewCtrl1->SetHeaderAttr(mHeaderAttr);
	m_dataViewCtrl1->SetRowHeight(mRowHeights);
}

bool pof::ProductView::ProductAdd(pof::base::data::row_t&& productRow)
{
	wxGetApp().mProductManager.GetProductData()->StoreData(std::forward<pof::base::data::row_t>(productRow));

	return false;
}

void pof::ProductView::ReloadProductView()
{
	mActiveCategory.clear();
	m_searchCtrl1->SetDescriptiveText("Search for products");
	wxGetApp().mProductManager.GetProductData()->Reload();
	UpdateCategoryTool(""s);
}

void pof::ProductView::CreateAccTable()
{
	wxAcceleratorEntry entries[7];
	entries[0].Set(wxACCEL_CTRL, (int)'A', ID_ADD_PRODUCT);
	entries[1].Set(wxACCEL_CTRL, (int)'R', ID_REMOVE_PRODUCT);
	entries[2].Set(wxACCEL_CTRL, (int)'O', ID_ORDER_LIST);
	entries[3].Set(wxACCEL_CTRL, (int)'E', ID_REPORTS_ENDOFDAY);
	entries[4].Set(wxACCEL_CTRL, (int)'M', ID_REPORTS_EOM);
	entries[5].Set(wxACCEL_CTRL, (int)'C', ID_SHOW_COST_PRICE);
	entries[6].Set(wxACCEL_CTRL, (int)'S', ID_ADD_INVENTORY);



	wxAcceleratorTable accel(7, entries);
	this->SetAcceleratorTable(accel);
}

void pof::ProductView::CreateEmptyPanel()
{
	mEmpty = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m5 = new wxPanel(mEmpty, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);


	bSizer8->Add(0, 0, 1, wxEXPAND, 5);

	wxPanel* m7 = new wxPanel(m5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap("supplement-bottle"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No product in pharmacy"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxButton* btn = new wxButton(m7, ID_ADD_PRODUCT);
	btn->SetBitmap(wxArtProvider::GetBitmap("add_task", wxART_OTHER, FromDIP(wxSize(16,16))));
	btn->SetLabel("Add product");
	btn->SetBackgroundColour(*wxWHITE);
	bSizer9->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);


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


	mEmpty->SetSizer(bSizer6);
	mEmpty->Layout();

	m_mgr.AddPane(mEmpty, wxAuiPaneInfo().Name("Empty").CenterPane().CaptionVisible(false).Hide());

}

void pof::ProductView::OnResize(wxSizeEvent& evt)
{
	//ReSizeColumns();
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
	auto Model       = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	auto& row        = Model->GetDatastore()[idx];
	auto& meta       = Model->GetDatastore().get_metadata();
	auto DataObject  = new pof::DataObject("PRODUCTDATA"s, row, meta);
	evt.SetDataObject(DataObject);
}

void pof::ProductView::OnHeaderClicked(wxDataViewEvent& evt)
{
	if (mSelectionCol && mSelectionCol == evt.GetDataViewColumn()) {
		static bool sel = true; //
		m_dataViewCtrl1->Freeze();
		auto& items = wxGetApp().mProductManager.GetProductData()->GetDataViewItems();
		if (sel) {
			if (!mSelections.empty()) {
				mSelections.clear();
				m_dataViewCtrl1->SetSelections({});
				sel = false;
			}
			else {
				std::ranges::copy(items, std::inserter(mSelections, mSelections.end()));
			}
		}
		else {
			for (auto& item : items) {
				mSelections.erase(item);
			}
		}
		sel = !sel;
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();
		evt.Veto();
	}
	else {
		evt.Skip();
	}
}

void pof::ProductView::OnExpiredProducts(wxAuiToolBarEvent& evt)
{
	pof::ExpiredView ev(nullptr, wxID_ANY);
	ev.ShowModal();
}

void pof::ProductView::OnAddProduct(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST) && !wxGetApp().bAllowOtherUsersInventoryPermission) {
		wxMessageBox("User accoount cannot add product to store", "Add Product", wxICON_INFORMATION | wxOK);
		return;
	}
	pof::AddProdutDialog dialog(this);
	dialog.Center();
	if (dialog.ShowModal() == wxID_OK) {
		auto productopt = dialog.GetAddedProduct();
		if (productopt.has_value()) {
			wxGetApp().mProductManager.GetProductData()->StoreData(std::move(productopt.value()));
		}
		auto productinvenopt = dialog.GetAddedInventory();
		if (productinvenopt.has_value()) {
			wxGetApp().mProductManager.GetInventory()->StoreData(std::move(productinvenopt.value()));
		}
		const size_t count = wxGetApp().mProductManager.GetProductData()->GetDatastore().size();
		m_dataViewCtrl1->SetFocus();
		m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);
		m_dataViewCtrl1->Select(pof::DataModel::GetItemFromIdx(count - 1));

		pof::DataModel* datam = wxGetApp().mProductManager.GetProductData().get();
		if (!m_searchCtrl1->IsEmpty()) {
			//in search sat
			bool empty = false;
			std::string search = m_searchCtrl1->GetValue().ToStdString();

			if (!mActiveCategory.empty()) {
				empty = datam->StringSearchAndReloadSet(pof::ProductManager::PRODUCT_NAME, std::move(search));
			}
			else {
				empty = datam->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, std::move(search));
			}

		}
		else if (!mActiveCategory.empty()){
			datam->ReloadSet(); //reloads the active category
		}

		mInfoBar->ShowMessage("Product Added Sucessfully", wxICON_INFORMATION);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, "Created A product");
	}
	CheckEmpty(); 
}

void pof::ProductView::OnAddCategory(wxCommandEvent& evt)
{
	while (1) {
		wxTextEntryDialog dialog(this, "Please enter a name for a category", "ADD CATEGORY");
		dialog.Center();
		if (dialog.ShowModal() == wxID_OK) {
			auto CategoryName = dialog.GetValue().ToStdString();
			if (CategoryName.empty()) return;
			auto& cat = wxGetApp().mProductManager.GetCategories();

			//check if conflighting
			if (std::ranges::any_of(cat, [&](const pof::base::data::row_t& row) -> bool {
				return CategoryName == boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::CATEGORY_NAME]);
			})) {
				wxMessageBox("Category Name Already Exist, Please Try Again", "ADD CATEGORY", wxICON_WARNING | wxOK);
				continue;
			}
			CategoryAddSignal(CategoryName);
			size_t id = 1; //0 is the default category
			if (!cat.empty()) {
				auto& v = cat.back();
				id = boost::variant2::get<std::uint64_t>(v.first[pof::ProductManager::CATEGORY_ID]);
				id++;
			}

			pof::base::data::row_t row;
			row.first.push_back(id);
			row.first.push_back(CategoryName);
			cat.insert(std::move(row));
			wxGetApp().mProductManager.AddCategory(CategoryName);
			break;
		}
		else {
			break;
		}
	}
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, "Created a category");
}

void pof::ProductView::OnRemoveFromCategory(wxCommandEvent& evt)
{
	if (mActiveCategory.empty()) return;
	if (mSelections.empty()) {
		auto item = m_dataViewCtrl1->GetSelection();
		if (!item.IsOk()) return;

		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
		auto& v = datastore[idx].first;

		auto& ud = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]);
		v[pof::ProductManager::PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);

		wxGetApp().mProductManager.UpdatePD(std::make_tuple(ud, std::uint64_t(0)), { "uuid", "category" });
	}
	else {
		for (auto& item : mSelections) {
			if (!item.IsOk()) return;
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
			auto& v = datastore[idx].first;

			auto& ud = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]);
			v[pof::ProductManager::PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);

			wxGetApp().mProductManager.UpdatePD(std::make_tuple(ud, std::uint64_t(0)), { "uuid", "category_id" });
		}
	}
	//reactivate
	OnCategoryActivated(mActiveCategory);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, "Removed a product from category");

}

void pof::ProductView::OnSearchFlag(wxCommandEvent& evt)
{
	auto id = evt.GetId();
	mSearchFlag.reset();


	switch (id)
	{
	case ID_SEARCH_BY_NAME:
		mSearchFlag.set(0);
		m_searchCtrl1->SetDescriptiveText(fmt::format("Search for products by name {}", mActiveCategory));
		break;
	case ID_SEARCH_BY_GENERIC_NAME:
		mSearchFlag.set(1);
		m_searchCtrl1->SetDescriptiveText(fmt::format("Search for products by  genric name {}", mActiveCategory));
		break;
	case ID_SEARCH_BY_CATEGORY:
		mSearchFlag.set(2);
		m_searchCtrl1->SetDescriptiveText(fmt::format("Search for products by category {}", mActiveCategory));
		break;
	default:
		spdlog::error("invalid search");
		break;
	}
}

void pof::ProductView::OnContextMenu(wxDataViewEvent& evt)
{	

	if (!m_dataViewCtrl1->GetSelection().IsOk() && mSelections.empty()) return;
	wxMenu* menu = new wxMenu;
	auto open = menu->Append(ID_OPEN_PRODUCT_INFO, "Open product", nullptr);
	wxMenu* catSub = new wxMenu;
	CreateCategoryMenu(catSub);
	if (!mActiveCategory.empty()) {
		auto cat = menu->Append(wxID_ANY, "Move to category", catSub);
		auto remvcat = menu->Append(ID_REMOVE_FROM_CATEGORY, fmt::format("Remove from \'{}\'", mActiveCategory), nullptr);
		//remvcat->SetBitmap(wxArtProvider::GetBitmap("action_remove"));
	}
	else {
		auto cat = menu->Append(wxID_ANY, "Add to category", catSub);
	}
	auto inven = menu->Append(ID_ADD_INVENTORY, "Add stock", nullptr);
	menu->AppendSeparator();
	auto markup = menu->Append(ID_PRODUCT_MARKUP, "Mark-up product", nullptr);
	auto var = menu->Append(ID_ADD_VARIANT, "Create variant", nullptr);
	auto crb = menu->Append(ID_CREATE_CONTROLLED_BOOK, "Create Controlled Register", nullptr);
	auto upp = menu->Append(ID_UPDATE_QUANTITY, "Change quantity", nullptr);
	menu->AppendSeparator();
	if (mSelections.empty()) {
		auto orderlist = menu->Append(ID_ADD_ORDER_LIST, "Add order list", nullptr);
		auto inc = menu->Append(ID_INCR_PRODUCT_PRICE, "Increase price", nullptr);

		menu->AppendSeparator();
		auto moveEx = menu->Append(ID_MOVE_PRODUCT_STOCK, "Clear stock as expired", nullptr);
		auto hi = menu->Append(ID_HIDE_PRODUCT, "Hide product", nullptr);
		auto remv = menu->Append(ID_REMOVE_PRODUCT, "Remove product from store", nullptr);
	}
	else {
		auto orderlist = menu->Append(ID_ADD_ORDER_LIST, fmt::format("Add {:d} products to order list", mSelections.size()), nullptr);
		auto inc = menu->Append(ID_INCR_PRODUCT_PRICE, fmt::format("Increase {:d} product prices", mSelections.size()), nullptr);

		menu->AppendSeparator();
		auto moveEx = menu->Append(ID_MOVE_PRODUCT_STOCK, fmt::format("Clear {:d} products stocks as expired", mSelections.size()), nullptr);
		auto hide = menu->Append(ID_HIDE_PRODUCT, fmt::format("Hide {:d} products", mSelections.size()), nullptr);
		auto remv = menu->Append(ID_REMOVE_PRODUCT, fmt::format("Remove {:d} products from store", mSelections.size()), nullptr);
	}
	/*orderlist->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY));
	remv->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE));
	cat->SetBitmap(wxArtProvider::GetBitmap("folder_files"));
	inven->SetBitmap(wxArtProvider::GetBitmap("file"));*/

	m_dataViewCtrl1->PopupMenu(menu);
}

void pof::ProductView::OnRemoveProduct(wxCommandEvent& evt)
{
	//check privilage
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Remove product", wxICON_INFORMATION | wxOK);
		return;
	}
	if (wxMessageBox("Deleteing a product deletes all the data associated with the product, Do you wish to continue?", "REMOVE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO) return;
	
	wxCredentialEntryDialog dialog(this, "User credentials are required to remove this item", "Products");
	dialog.Center(wxBOTH);
	dialog.SetBackgroundColour(*wxWHITE);
	while (1) {
		if (dialog.ShowModal() == wxID_CANCEL) return;
		auto cred = dialog.GetCredentials();
		if (!wxGetApp().MainAccount->ValidateCredentials(cred.GetUser().ToStdString(),
			cred.GetPassword().GetAsString().ToStdString())) {
			wxMessageBox("Invalid username or password", "Reports", wxICON_WARNING | wxOK);
			continue;
		}
		break;
	}
	
	if (mSelections.empty()) {
		auto item = m_dataViewCtrl1->GetSelection();
		if (!item.IsOk()) return;
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
		auto iter = wxGetApp().mProductManager.GetProductData()->GetDatastore().begin();
		auto next = std::next(iter, idx);
		std::string name = boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]);

		//check if the product is currently in the sale view
		if (std::ranges::any_of(wxGetApp().mSaleManager.GetSaleData()->GetDatastore(), [&row](auto& v) -> bool {
			return row.first[pof::ProductManager::PRODUCT_UUID] == v.first[pof::SaleManager::PRODUCT_UUID];
			})) {
			wxMessageBox("Cannot remove product that is currently in the sale, please complete or clear product from sale before removing product", "Remove product", wxICON_INFORMATION | wxOK);
			return;
		}


		wxProgressDialog dlg("Removing product", "Deleting from database...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
		bool status = false;
		bool stop = false;
		auto showFailedStatus = [&]() {
			mInfoBar->ShowMessage(fmt::format("Failed to Removed {} from store", name), wxICON_INFORMATION);
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Failed Remove {} product from store", name));
		};

		stop = dlg.Update(10, "Removing product from packs");
		status = wxGetApp().mProductManager.RemoveProductInPacks(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(20, "Removing product from order list");
		status = wxGetApp().mProductManager.RemoveProductInOrderListData(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(30, "Removing product from stock checks");
		status = wxGetApp().mProductManager.RemoveProductInStockCheckData(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(40, "Removing product warnings");
		status = wxGetApp().mProductManager.RemoveProductInWarningsData(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(50, "Removing product inventory data");
		status = wxGetApp().mProductManager.RemoveProductInOrderListData(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(60, "Removing product from expired");
		status = wxGetApp().mProductManager.RemoveProductInExpiredStock(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(65, "Removing product from patients");
		status = wxGetApp().mProductManager.RemoveProductInMedication(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(70, "Removing product sale history");
		status = wxGetApp().mSaleManager.RemoveProductSaleHistory(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(80, "Removing product from saved sale");
		status = wxGetApp().mSaleManager.RemoveProductSaveSale(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(85, "Removing product from posion book");
		status = wxGetApp().mProductManager.RemoveProductInPoisonBook(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		stop = dlg.Update(90, "Removing product from posion book");
		status = wxGetApp().mProductManager.RemoveProductInInvoice(next);
		if (!stop || !status) {
			showFailedStatus(); return;
		}

		dlg.Update(100, "Finishing...");
		m_dataViewCtrl1->Freeze();
		removeSignal(next);
		wxGetApp().mProductManager.GetProductData()->RemoveData(item);
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();
		mInfoBar->ShowMessage(fmt::format("Removed {} from store successfully", name), wxICON_INFORMATION);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Remove {} product from store", name));
	}
	else {
		wxProgressDialog dlg("Removing product", "Deleting from database...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
		bool status = false;
		bool stop = false;
		size_t count = 0;
		size_t failedCount = 0;
		auto up = [&](size_t c) -> size_t {
			return ((float)c / (float)mSelections.size()) * 80.0f;
		};
		auto showFailedStatus = [&](std::string_view name) {
			failedCount++;
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Failed Remove {} product from store", name));
		};
		wxDataViewItemArray items;
		m_dataViewCtrl1->Freeze();
		try {
			for (auto& item : mSelections) {
				if (!item.IsOk()) continue;
				size_t idx = pof::DataModel::GetIdxFromItem(item);
				auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
				auto iter = wxGetApp().mProductManager.GetProductData()->GetDatastore().begin();
				auto next = std::next(iter, idx);
				std::string name = boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]);

				//check if the product is currently in the sale view
				if (std::ranges::any_of(wxGetApp().mSaleManager.GetSaleData()->GetDatastore(), [&row](auto& v) -> bool {
					return row.first[pof::ProductManager::PRODUCT_UUID] == v.first[pof::SaleManager::PRODUCT_UUID];
					})) {
					wxMessageBox("Cannot remove product that is currently in the sale, please complete or clear product from sale before removing product", "Remove product", wxICON_INFORMATION | wxOK);
					continue;
				}
				stop = dlg.Update(up(count), fmt::format("Removing {}", name));
				if (!stop) {
					//try to remove items already deleted
					wxGetApp().mProductManager.GetProductData()->RemoveData(items);
					m_dataViewCtrl1->Thaw();
					m_dataViewCtrl1->Refresh();
					mSelections.clear();
					CheckEmpty();
					return; //topped removed
				}
				status = wxGetApp().mProductManager.RemoveProductInPacks(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInOrderListData(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInStockCheckData(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInWarningsData(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInOrderListData(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInExpiredStock(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInMedication(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mSaleManager.RemoveProductSaleHistory(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mSaleManager.RemoveProductSaveSale(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInPoisonBook(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				status = wxGetApp().mProductManager.RemoveProductInInvoice(next);
				if (!status) {
					showFailedStatus(name); continue;
				}

				removeSignal(next);
				items.push_back(item);
				wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Remove {} product from store", name));
				count++;
			}
			dlg.Update(90, "Finishing...");
			wxGetApp().mProductManager.GetProductData()->RemoveData(items);
			dlg.Update(100);
			m_dataViewCtrl1->Thaw();
			m_dataViewCtrl1->Refresh();
			mInfoBar->ShowMessage(fmt::format("Removed {:d} products from store successfully", count), wxICON_INFORMATION);

			if (failedCount != 0) {
				//display the failed set
				wxMessageBox(fmt::format("Could not remove {:d} from store", failedCount), "Remove Product", wxICON_INFORMATION | wxOK);
			}
			mSelections.clear();
		}
		catch (std::exception& exp){
			spdlog::critical(exp.what());
			wxMessageBox("Critial error in remove multiple products", "Critical error", wxICON_INFORMATION | wxOK);
		}
	}

	//reload the view when we modify the contents
	pof::DataModel* datam = wxGetApp().mProductManager.GetProductData().get();
	if (!m_searchCtrl1->IsEmpty()) {
		//in search sat
		bool empty = false;
		std::string search = m_searchCtrl1->GetValue().ToStdString();

		if (!mActiveCategory.empty()) {
			empty = datam->StringSearchAndReloadSet(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}
		else {
			empty = datam->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}

	}
	else if (!mActiveCategory.empty()) {
		datam->ReloadSet(); //reloads the active category
	}

	CheckEmpty();
}

void pof::ProductView::OnAddProductToOrderList(wxCommandEvent& evt)
{
	wxBusyCursor cur;
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	if (mSelections.empty()) {
		auto item = m_dataViewCtrl1->GetSelection();
		if (!item.IsOk()) return;
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = datastore[idx];

		pof::base::data::duuid_t uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
		wxGetApp().mProductManager.AddToOrderList(uuid, 1);

		std::string& name = boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_NAME]);
		mInfoBar->ShowMessage(fmt::format("Added {} to order list", name), wxICON_INFORMATION);
	}
	else {
		for (auto& item : mSelections) {
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& row = datastore[idx];

			pof::base::data::duuid_t uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);
			wxGetApp().mProductManager.AddToOrderList(uuid, 1);

		}
		mInfoBar->ShowMessage(fmt::format("{:d} Items Added To Order List", mSelections.size()), wxICON_INFORMATION);
		m_dataViewCtrl1->Freeze();
		mSelections.clear();
		m_dataViewCtrl1->Thaw();
	}
}


void pof::ProductView::OnAddItemsToCategory(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	std::uint64_t catIdx = static_cast<std::uint64_t>(evt.GetId());
	auto& catDatastore = wxGetApp().mProductManager.GetCategories();
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto iter = std::ranges::find_if(catDatastore, [&](const pof::base::data::row_t& data) -> bool {
		spdlog::info("ID {:d} Name {}", boost::variant2::get<std::uint64_t>(data.first[pof::ProductManager::CATEGORY_ID]),
		boost::variant2::get<pof::base::data::text_t>(data.first[pof::ProductManager::CATEGORY_NAME]));
		return catIdx == boost::variant2::get<std::uint64_t>(data.first[pof::ProductManager::CATEGORY_ID]);
	});
	if (iter == catDatastore.end()) return;
	auto& name = boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::CATEGORY_NAME]);
	if (mSelections.empty()) {
		auto item = m_dataViewCtrl1->GetSelection();
		if (!item.IsOk()) return;

		size_t idx = pof::DataModel::GetIdxFromItem(item);
		datastore[idx].first[pof::ProductManager::PRODUCT_CATEGORY] = catIdx;

		pof::ProductInfo::PropertyUpdate updatePropery;
		updatePropery.mUpdatedElememts.set(static_cast<size_t>(pof::ProductManager::PRODUCT_CATEGORY));
		updatePropery.mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
		updatePropery.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_CATEGORY] = catIdx;
		updatePropery.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] =
			datastore[idx].first[pof::ProductManager::PRODUCT_UUID];

		mProductinfo->SignalUpdate(updatePropery);

		auto& itemName = boost::variant2::get<pof::base::data::text_t>(datastore[idx].first[pof::ProductManager::PRODUCT_NAME]);
		mInfoBar->ShowMessage(fmt::format("Added {} to \'{}\'", itemName, name), wxICON_INFORMATION);
	}
	else {
		pof::ProductInfo::PropertyUpdate updatePropery;
		updatePropery.mUpdatedElememts.set(static_cast<size_t>(pof::ProductManager::PRODUCT_CATEGORY));
		updatePropery.mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
		
		for (auto& item : mSelections) {
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			datastore[idx].first[pof::ProductManager::PRODUCT_CATEGORY] = catIdx;

			updatePropery.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_CATEGORY] = catIdx;
			updatePropery.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] =
				datastore[idx].first[pof::ProductManager::PRODUCT_UUID];

			mProductinfo->SignalUpdate(updatePropery);
		}
		mInfoBar->ShowMessage(fmt::format("Added {:d} items to \'{}\'", mSelections.size(), name), wxICON_INFORMATION);
		m_dataViewCtrl1->Freeze();
		mSelections.clear();
		m_dataViewCtrl1->Thaw();
	}

}

void pof::ProductView::OnSearchProduct(wxCommandEvent& evt)
{
	RemoveCheckedState(mOutOfStockItem);
	RemoveCheckedState(mExpireProductItem);
	auto& suppPane = m_mgr.GetPane("Supplier");

	if (wxGetApp().mProductManager.GetProductData()->GetDatastore().empty()) return;
	if (suppPane.IsOk() && suppPane.IsShown()) return;


	pof::DataModel* datam = wxGetApp().mProductManager.GetProductData().get();
	assert(datam != nullptr);
	m_dataViewCtrl1->Freeze();
	std::string search = evt.GetString().ToStdString();
	bool empty = false;
	if (search.empty()) {
		//go back to what was there before the search?
		if (!mActiveCategory.empty()) {
			datam->ReloadSet();
		}
		else {
			datam->Reload();
		}
	}
	else {
		int col = pof::ProductManager::PRODUCT_NAME;
		if (mSearchFlag.test(1)) col = pof::ProductManager::PRODUCT_GENERIC_NAME;

		if (!mActiveCategory.empty()) {
			empty = datam->StringSearchAndReloadSet(col, std::move(search));
		}
		else {
			empty = datam->StringSearchAndReloadSet(col, std::move(search));
		}

	}

	if (empty) {
		ShowNoResult(search);
	}
	else {
		auto& d = m_mgr.GetPane("DataView");
		auto& p = m_mgr.GetPane("NoResult");
		if (p.IsShown()) p.Hide();
		if (!d.IsShown()) {
			d.Show();
		}
		m_mgr.Update();
	}

	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Update();
}

void pof::ProductView::OnSearchCleared(wxCommandEvent& evt)
{
	pof::DataModel* datam = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(datam != nullptr);
	
	m_dataViewCtrl1->Freeze();

	if (!mActiveCategory.empty()) {
		datam->ReloadSet();
	}
	else {
		datam->Reload();
	}
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Update();
}

void pof::ProductView::OnSelection(wxCommandEvent& evt)
{
	if (wxGetApp().mProductManager.GetProductData()->GetDataViewItems().empty()) {
		RemoveCheckedState(mSelectItem);
		return;
	}

	if (evt.IsChecked()) {
		ShowSelectionColumn();
	}
	else {
		HideSelectionColumn();
	}
}

void pof::ProductView::OnShowCostPrice(wxCommandEvent& evt)
{
	static bool clicked = true;
	if (clicked) {
		ShowCostPriceColumn();
	}
	else {
		HideCostPriceColumn();
	}
	clicked = !clicked;
}

//move logic to read directly from the database 
void pof::ProductView::OnOutOfStock(wxCommandEvent& evt)
{
	auto& pd = wxGetApp().mProductManager.GetProductData();
	static bool checked = true;
	if (checked){
		//disable expire product Item
		//clear all the states
		RemoveCheckedState(mExpireProductItem);
		mActiveCategory.clear();
		m_searchCtrl1->Clear();
		m_searchCtrl1->SetDescriptiveText("Search for product");

		auto& datastore = pd->GetDatastore();
		std::vector<wxDataViewItem> items;
		items.reserve(300); //hurestic,
		for (size_t i = 0; i < datastore.size(); i++){
			std::uint64_t stock = 
				boost::variant2::get<std::uint64_t>(datastore[i].first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			std::uint64_t compare = 0;
			if (wxGetApp().bUseMinStock){
				compare = 
					boost::variant2::get<std::uint64_t>(datastore[i].first[pof::ProductManager::PRODUCT_MIN_STOCK_COUNT]);
			}
			if (stock <= compare) {
				items.emplace_back(wxDataViewItem{ reinterpret_cast<void*>(i + 1) });
			}
		}
		items.shrink_to_fit();
		if (items.empty()) {
			wxMessageBox("No out of stock product in store", "PRODUCTS", wxICON_INFORMATION | wxOK);
			RemoveCheckedState(mOutOfStockItem);
			return;
		}
		pd->Reload(std::move(items));
	}
	else {
		pd->Reload();
	}
	checked = !checked;
}

void pof::ProductView::OnAddInventory(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& pd = wxGetApp().mProductManager.GetProductData();

	//check if product is a servie 
	if (boost::variant2::get<std::string>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_CLASS]) 
	  == pof::ProductManager::CLASS_TYPE[3]) {
		wxMessageBox("Cannot add stock to a service", "Add stock", wxICON_INFORMATION | wxOK);
		return;
	}
	//check if product has expired inventory
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST) && !wxGetApp().bAllowOtherUsersInventoryPermission) {
		wxMessageBox("User accoount cannot add inventory to stock", "Add Inventory", wxICON_INFORMATION | wxOK);
		return;
	}

	//check if product has stock
	if (boost::variant2::get<std::uint64_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT])
		!= static_cast<std::uint64_t>(0) &&
		 wxMessageBox("Product has stock avaliable, do you wish to add more stock?", "Add stock", wxICON_INFORMATION | wxYES_NO) == wxNO) {
		return; //has stock and does not what to add more
	}

	auto items = wxGetApp().mProductManager.DoExpiredProducts();
	if (!items.has_value()) return;
	if (std::ranges::any_of(items.value(), [&](const wxDataViewItem& i) -> bool {
		auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[pof::DataModel::GetIdxFromItem(i)];
	return boost::variant2::get<boost::uuids::uuid>(row.first[pof::ProductManager::PRODUCT_UUID])
		== boost::variant2::get<boost::uuids::uuid>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID]);
		})) {
		wxMessageBox("Product has expired inventory, clear inventory before adding new", "Add Inventory", wxICON_WARNING | wxOK);
		return;
	}

	

	pof::InventoryDialog dialog(nullptr, boost::variant2::get<pof::base::data::duuid_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID]));
	if (dialog.ShowModal() == wxID_OK) {
		auto& Inven = dialog.GetData();
		Inven.first[pof::ProductManager::INVENTORY_PRODUCT_UUID] = pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID];

		pof::ProductInfo::PropertyUpdate mPropertyUpdate;
		mPropertyUpdate.mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
			mPropertyUpdate.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] = pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID];

		
		mPropertyUpdate.mUpdatedElememts.set(pof::ProductManager::PRODUCT_STOCK_COUNT);
		mPropertyUpdate.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_STOCK_COUNT] = boost::variant2::get<std::uint64_t>(Inven.first[pof::ProductManager::INVENTORY_STOCK_COUNT]);
		
		mPropertyUpdate.mUpdatedElememts.set(pof::ProductManager::PRODUCT_COST_PRICE);
		mPropertyUpdate.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_COST_PRICE] = boost::variant2::get<pof::base::currency>(Inven.first[pof::ProductManager::INVENTORY_COST]);


		std::string cls = boost::variant2::get<pof::base::data::text_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_CLASS]);
		if (cls == "CONTROLLED"){
			//product is a controlled medication
			pof::base::data::row_t row;
			//create the first entry into the book table	
			auto& v = row.first;
			auto& p = pd->GetDatastore()[idx].first;

			v.push_back(p[pof::ProductManager::PRODUCT_UUID]);
			v.push_back("ENTRY"s);
			v.push_back(wxGetApp().MainPharmacy->GetAddressAsString());
			v.push_back(fmt::format("{} {}", wxGetApp().MainAccount->lastname, wxGetApp().MainAccount->name));
			v.push_back(static_cast<std::uint64_t>(1));
			v.push_back(static_cast<std::uint64_t>(0));
			//create with the current stock
			std::uint64_t stock = boost::variant2::get<std::uint64_t>(p[pof::ProductManager::PRODUCT_STOCK_COUNT])
				+ boost::variant2::get<std::uint64_t>(Inven.first[pof::ProductManager::INVENTORY_STOCK_COUNT]);
			v.push_back(stock);
			v.push_back(stock);
			v.push_back(pof::base::data::clock_t::now());

			if (!wxGetApp().mPoisonBookManager.IsBookCreated(dialog.mProductUuid)) {
				//create book ?
				wxMessageBox("No book created for controlled medication, a new book would be created to track this inventory", "Inventory", wxICON_INFORMATION | wxOK);
				wxGetApp().mPoisonBookManager.CreateNewBook(std::move(row));
			}
			else {
				//add as normal
				wxGetApp().mPoisonBookManager.GetBook()->StoreData(std::move(row));
			}
		}

		mProductinfo->SignalUpdate(mPropertyUpdate);
		wxGetApp().mProductManager.GetInventory()->StoreData(std::move(Inven));

		auto name = boost::variant2::get<pof::base::data::text_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_NAME]);
		mInfoBar->ShowMessage(fmt::format("{} stock was updated successfully!", name), wxICON_INFORMATION);
	}
}

void pof::ProductView::OnReportDropdown(wxAuiToolBarEvent& evt)
{
	wxMenu* menu = new wxMenu;
	menu->Append(ID_REPORTS_ENDOFDAY, "End of day", nullptr);
	menu->Append(ID_REPORTS_ENDOFWEEK, "End of week", nullptr);
	menu->Append(ID_REPORTS_EOM, "End of month", nullptr);
	menu->Append(ID_REPORTS_PROFITLOSS, "Profit/Loss", nullptr);
	menu->Append(ID_REPORTS_INVENTORY, "Stock purchase report for month", nullptr);
	menu->Append(ID_REPORTS_PRODUCT_SOLD, "Product sold report for month", nullptr);
	menu->Append(ID_REPORTS_EXPIRED_PRODUCTS, "Expired products for month", nullptr);

	wxPoint pos = mReportItem->GetSizerItem()->GetPosition();
	wxSize sz = mReportItem->GetSizerItem()->GetSize();

	m_auiToolBar1->PopupMenu(menu, wxPoint{pos.x, pos.y + sz.y + 2});
}


void pof::ProductView::OnConsumptionPattern(wxCommandEvent& evt)
{
	/*if(!wxGetApp().mProductManager.IsStockCheckComplete())
	{
		wxMessageBox("Please complete stock check before generating consumption pattern for this month", "Reports", wxICON_INFORMATION | wxOK);
	}*/
	pof::ReportsDialog dialog(nullptr, wxID_ANY, wxEmptyString);
	if(dialog.LoadReport(pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN, pof::base::data::clock_t::now())) dialog.ShowModal();
}

void pof::ProductView::OnEndOfDayReport(wxCommandEvent& evt)
{
	wxWindowID id = evt.GetId();
	pof::ReportsDialog::ReportType rep;
	switch (id)
	{
	case ID_REPORTS_ENDOFDAY:
		rep = pof::ReportsDialog::ReportType::EOD;
		break;
	case ID_REPORTS_INVENTORY:
		rep = pof::ReportsDialog::ReportType::IM;
		break;
	case ID_REPORTS_PRODUCT_SOLD:
		rep = pof::ReportsDialog::ReportType::PSM;
		break;
	case ID_REPORTS_PROFITLOSS:
		rep = pof::ReportsDialog::ReportType::PL;
		break;
	case ID_REPORTS_ENDOFWEEK:
		rep = pof::ReportsDialog::ReportType::EOW;
		break;
	case ID_REPORTS_EXPIRED_PRODUCTS:
		rep = pof::ReportsDialog::ReportType::EXP;
		break;
	default:
		return;
	}

	pof::ReportsDialog dialog(nullptr, wxID_ANY, wxEmptyString);
	if (dialog.LoadReport(rep, pof::base::data::clock_t::now())) dialog.ShowModal();
}

void pof::ProductView::OnPacks(wxCommandEvent& evt)
{
	pof::PackView pks(nullptr);
	if (pks.ShowModal() == wxID_OK) {
		
	}
}

void pof::ProductView::OnFunctions(wxAuiToolBarEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto bf = menu->Append(ID_FUNCTION_BROUGHT_FORWARD, "Brougt Forward", nullptr);
	auto scf = menu->Append(ID_FUNCTION_STOCK_CHECK, "Stock Check", nullptr);
	auto mark = menu->Append(ID_FUNCTION_MARK_UP_PRODUCTS, "Markup Products", nullptr);
	auto scp = menu->AppendCheckItem(ID_SHOW_COST_PRICE, "Show cost price",wxT("Show the cost price for each product"));

	if (!mActiveCategory.empty()) {
		wxMenu* catMenu = new wxMenu;
		auto ctp = menu->Append(wxID_ANY, "Category functions", catMenu);
	}
	auto ss = menu->Append(ID_STORE_SUMMARY, "Pharmacy store summary", nullptr);
	auto inc = menu->Append(ID_INCR_PRICE, "Increase store prices", nullptr);
	if (mSelections.empty()) {
		auto dexl = menu->Append(ID_DOWNLOAD_EXCEL, "Export products as excel", nullptr);
	}
	else {
		auto dexl = menu->Append(ID_DOWNLOAD_EXCEL, fmt::format("Export {:d} products as excel", mSelections.size()), nullptr);
	}
	auto ddd = menu->Append(ID_DOWNLOAD_ACTUAL_STOCK, "Download actual stock", nullptr);

	wxPoint pos = mFuncDropItem->GetSizerItem()->GetPosition();
	wxSize sz = mFuncDropItem->GetSizerItem()->GetSize();

	m_auiToolBar1->PopupMenu(menu, wxPoint{ pos.x, pos.y + sz.y + 2 });
}

void pof::ProductView::OnBFFunction(wxCommandEvent& evt)
{
	if (!wxGetApp().mProductManager.IsStockCheckComplete()) {
		wxMessageBox("Stock check is not complete for this month, complete stock check before brought forward", "BROUGHT FORWARD", wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cursor;
	wxGetApp().mProductManager.InventoryBroughtForward();
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Brought Forward");
}

void pof::ProductView::OnSCFunction(wxCommandEvent& evt)
{
	//we need a stock check dialog
	pof::StockCheck dialog(nullptr, wxID_ANY);
	//dialog.Center(wxBOTH);
	dialog.ShowModal();
}

void pof::ProductView::OnShowOrderList(wxCommandEvent& evt)
{
	wxGetApp().mProductManager.LoadOrderList();
	pof::OrderListView orderView(this);
	wxGetApp().mProductManager.GetOrderList()->Reload();
	orderView.ShowModal();

}

void pof::ProductView::OnMarkUp(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	if (wxMessageBox(fmt::format("Are you sure you want to mark-up to {:.2f}% of the cost price?", wxGetApp().mProductManager.gMarkup), "Products",
		wxICON_INFORMATION | wxYES_NO) == wxNO) return;
	if (wxGetApp().mProductManager.gMarkup <= 0.01f) {
		wxMessageBox("No mark up set, please set mark up percent", "Mark up", wxICON_WARNING | wxOK);
		return;
	}

	size_t idx = pof::DataModel::GetIdxFromItem(item);

	//the actual markup should come from the settings of the pharmaoffice
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto& prod = datastore[idx];
	
	pof::base::currency& price =
		boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
	pof::base::currency& cost_price =
		boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_COST_PRICE]);
	pof::base::data::duuid_t& uuid =
		boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);

	//round up ??
	const float rnd = (100.0f + wxGetApp().mProductManager.gMarkup) / 100.0f;
	price = cost_price * static_cast<double>(rnd);

	price.nearest_hundred(); //round to nearest hundred

	prod.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
	wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
	prod.second.second.reset();

	wxGetApp().mProductManager.GetProductData()->ItemChanged(item);

}

void pof::ProductView::OnMarkUpProducts(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Mark up", wxICON_INFORMATION | wxOK);
		return;
	}
	if (wxMessageBox("Mark up changes the price for all products in the store, are you sure you want to continue?" , "Mark up",
		wxICON_WARNING | wxYES_NO) == wxNO) return;
	if (wxGetApp().mProductManager.gMarkup <= 0.01f) {
		wxMessageBox("No mark up set, please set mark up percent", "Mark up", wxICON_WARNING | wxOK);
		return;
	}

	wxProgressDialog dlg("Marking up products", "please wait...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
	size_t i = 0;
	float pg = 0.0f;
	const size_t count = wxGetApp().mProductManager.GetProductData()->GetDatastore().size();

	for (auto& prod : wxGetApp().mProductManager.GetProductData()->GetDatastore()){
		pof::base::currency& price =
			boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
		pof::base::currency& cost_price = 
			boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_COST_PRICE]);
		pof::base::data::duuid_t& uuid =
			boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);

		//round up ??
		const float rnd = (100.0f + wxGetApp().mProductManager.gMarkup) / 100.0f;
		price = cost_price * static_cast<double>(rnd);

		price.nearest_hundred(); //round to nearest hundred

		prod.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
		wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, i);
		prod.second.second.reset();

		wxGetApp().mProductManager.GetProductData()->ItemChanged(pof::DataModel::GetItemFromIdx(i));
		pg = static_cast<float>(((float)i / (float)count) * 100.f);
		i++;
		dlg.Update(pg);
	}
	wxMessageBox("Successfully marked up products");
}

void pof::ProductView::OnMoveExpiredStock(wxCommandEvent& evt)
{
	if (mSelections.empty()) {
		auto item = m_dataViewCtrl1->GetSelection();
		if (!item.IsOk()) return;
		wxBusyCursor cursor;
		auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		pof::base::data::text_t& name = boost::variant2::get<pof::base::data::text_t>(datastore[idx].first[pof::ProductManager::PRODUCT_NAME]);

		auto items = wxGetApp().mProductManager.DoExpiredProducts();
		if (!items.has_value()) return;
		if (!std::ranges::any_of(items.value(), [&](const wxDataViewItem& i) -> bool {return item == i; })) {
			wxMessageBox(fmt::format("{} has not expired", name), "EXPIRED", wxICON_WARNING | wxOK);
			return;
		}

		pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(datastore[idx].first[pof::ProductManager::PRODUCT_UUID]);
		std::uint64_t& stockCount = boost::variant2::get<std::uint64_t>(datastore[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
		//cannot move empty stock
		if (stockCount == 0) {
			wxMessageBox(fmt::format("{} has no stock to move", name), "Product", wxICON_INFORMATION | wxOK);
			return;
		} 
		auto reqS = wxGetTextFromUser("Please enter the amount of stock you want to move?", "Move stock").ToStdString();
		std::uint64_t remvStock = 0;
		std::uint64_t req = 0;
		try {
			req = boost::lexical_cast<std::uint64_t>(reqS);
			if (req > stockCount) {
				wxMessageBox("Cannot remove more than the stock count", "Move stock", wxICON_ERROR | wxOK);
				return;
			}
			remvStock = stockCount - req;
		}
		catch (const std::exception& exp) {
			wxMessageBox("Invalud stock amount", "Move stock", wxICON_ERROR | wxOK);
			return;
		}

		wxGetApp().mProductManager.MoveStockToExpire(pid, req);
		//zero out the stock that was moved
		m_dataViewCtrl1->Freeze();
		wxGetApp().mProductManager.UpdatePD(std::make_tuple(pid, remvStock), { "uuid", "stock_count" });
		datastore[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT] = remvStock;
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();
		mInfoBar->ShowMessage(fmt::format("{} of quantity {:d} was moved to expired successfully", name, req), wxICON_INFORMATION);
	}
	else {
		wxIcon cop;
		cop.CopyFromBitmap(wxArtProvider::GetBitmap("supplement-bottle"));
		wxBusyInfo info
		(
			wxBusyInfoFlags()
			.Parent(this)
			.Icon(cop)
			.Title("Clearing as expired")
			.Text("Please wait...")
			.Foreground(*wxBLACK)
			.Background(*wxWHITE)
			.Transparency(4 * wxALPHA_OPAQUE / 5)
		);
		
		m_dataViewCtrl1->Freeze();
		auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
		auto items = wxGetApp().mProductManager.DoExpiredProducts();
		size_t count = 0;
		for(auto& item : mSelections){
			if (!items.has_value()) return;
			if (!std::ranges::any_of(items.value(), [&](const wxDataViewItem& i) -> bool {return item == i; })) {
				//wxMessageBox("Product is not expired", "EXPIRED", wxICON_WARNING | wxOK);
				continue;
			}

			size_t idx = pof::DataModel::GetIdxFromItem(item);
			pof::base::data::duuid_t& pid = boost::variant2::get<pof::base::data::duuid_t>(datastore[idx].first[pof::ProductManager::PRODUCT_UUID]);
			std::uint64_t& stockCount = boost::variant2::get<std::uint64_t>(datastore[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			if (stockCount == 0) continue; //cannot move empty stock

			wxGetApp().mProductManager.MoveStockToExpire(pid, stockCount);
			std::uint64_t temp = stockCount;
			stockCount = 0;
			wxGetApp().mProductManager.UpdatePD(std::make_tuple(pid, stockCount), { "uuid", "stock_count" });
			count++;
		}
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();
		mInfoBar->ShowMessage(fmt::format("{:d} products was moved to expired successfully", count), wxICON_INFORMATION);
	}
}

void pof::ProductView::OnUpdateUI(wxUpdateUIEvent& evt)
{
	//check for expired
	static size_t count = 0;
	if (!mSelections.empty() && count != mSelections.size()) {
		wxDataViewItemArray arr;
		arr.reserve(mSelections.size());
		for (auto& sel : mSelections) {
			arr.push_back(sel);
		}
		m_dataViewCtrl1->SetSelections(arr);
	}
	count = mSelections.size();

	std::vector<std::string> f;
	if (wxGetApp().bCheckExpiredOnUpdate){
		auto now = std::chrono::system_clock::now();
		if (now >= mExpireProductWatchDog + mWatchDogDuration){
			auto items = wxGetApp().mProductManager.DoExpiredProducts();
			if (!items.has_value()) { }
			else if (!items->empty()){
				f.push_back(fmt::format("{:d} products in store has expired", items->size()));
			}
		}
		mExpireProductWatchDog = now;
	}
	if (wxGetApp().bCheckOutOfStockOnUpdate){
		auto now = std::chrono::system_clock::now();
		if (now >= mOutOfStockProductWatchDog + mWatchDogDuration){
			auto items = wxGetApp().mProductManager.DoOutOfStock();
			if (!items.has_value()) {}
			else if (!items->empty()) {
				f.push_back(fmt::format("{:d} products in store are out of stock", items->size()));
			}
		}
		mOutOfStockProductWatchDog = now;
	}
	if (!f.empty()) {
		mInfoBar->ShowMessage("");
		mInfoBar->ShowMessage(fmt::format("{}", fmt::join(f, " and ")), wxICON_INFORMATION);
	}
	if (wxGetApp().bNotifyStockCheckInComplete) {
		CheckIfStockCheckIsComplete();
	}
}

void pof::ProductView::OnDownloadExcel(wxCommandEvent& evt)
{
	wxFileDialog dialog(this, "Save Excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}
	try {

	excel::XLDocument doc;
	doc.create(fullPath.string());
	if (!doc.isOpen()) {
		spdlog::error("Canont open xlsx file");
		return;
	}

	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName("Products");
	const size_t firstRow = 1;
	const size_t firstCol = 1;


	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	const bool isSelected = !mSelections.empty();

	size_t rowSize = isSelected ? mSelections.size() : datastore.size();
	size_t colSize = m_dataViewCtrl1->GetColumnCount();
	
	if (mSelectionCol != nullptr){
		colSize--;
	}

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize + 1, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};
	wxBusyCursor cursor;
	writeHeader("Product Name");
	writeHeader("Strength");
	writeHeader("Class");
	writeHeader("Formulation");
	writeHeader("Package size");
	writeHeader("Stock count");
	writeHeader("Unit price");
	if (mProductCostPriceCol != nullptr) {
			writeHeader("Cost Price");
	}

	auto& metadata = datastore.get_metadata();
	constexpr const std::array<int, 8> colIdx = {
		pof::ProductManager::PRODUCT_NAME,
		pof::ProductManager::PRODUCT_STRENGTH,
		pof::ProductManager::PRODUCT_CLASS,
		pof::ProductManager::PRODUCT_FORMULATION,
		pof::ProductManager::PRODUCT_PACKAGE_SIZE,
		pof::ProductManager::PRODUCT_STOCK_COUNT,
		pof::ProductManager::PRODUCT_UNIT_PRICE,
		pof::ProductManager::PRODUCT_COST_PRICE
	};

	size_t col = 0;
	size_t i = 0;
	size_t row = 0;
		for (size_t i = 0; i < rowSize; i++)
		{
			row = isSelected ? pof::DataModel::GetIdxFromItem(*std::next(mSelections.begin(), i)) : 0;
			//iter++; //move to the next column

			for (size_t j = 0; j < colSize; j++)
			{
				pof::base::data::kind knd = metadata[colIdx[j]];
				auto& v = iter->value();
				col = colIdx[j];

				switch (knd)
				{
				case pof::base::data::kind::int32:
					v.set(boost::variant2::get<std::int32_t>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::int64:
					v.set(boost::variant2::get<std::int64_t>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::uint32:
					v.set(boost::variant2::get<std::uint32_t>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::uint64:
					v.set(boost::variant2::get<std::uint64_t>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::float32:
					v.set(boost::variant2::get<float>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::float64:
					v.set(boost::variant2::get<double>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::datetime:
				{
					const std::string tf = fmt::format("{:%d/%m/%y}", boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[col]));
					v.set(tf);
				}
				break;
				case pof::base::data::kind::text:
					v.set(boost::variant2::get<pof::base::data::text_t>(datastore[row].first[col]));
					break;
				case pof::base::data::kind::blob:
					break;
				case pof::base::data::kind::uuid:
				{
					std::stringstream os;
					os << boost::variant2::get<pof::base::data::duuid_t>(datastore[row].first[col]);
					v.set(os.str());
				}
				break;
				case pof::base::data::kind::currency:
				{
					const std::string tf = fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(datastore[row].first[col]));
					v.set(tf);
				}
				break;
				case pof::base::data::kind::null:
					break;
				default:
					break;
				}
				iter++; //move to the next column
			}
		}

	doc.save();
	doc.close();

	} catch (std::exception& exp) {
		spdlog::error(exp.what());
		wxMessageBox(exp.what(), "Excel export", wxICON_ERROR | wxOK);
		return;
	}

	wxMessageBox(fmt::format("Downloaded excel to \n{}", fullPath.string()), "Excel export", wxICON_INFORMATION | wxOK);
}

void pof::ProductView::OnCacheHint(wxDataViewEvent& evt){
	//look into these cache stuff
	////
	wxBusyCursor crs;
	size_t i = evt.GetCacheFrom();
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	if (evt.GetCacheTo() >= datastore.size()) return;
//	spdlog::info("Loading cache from {:d} to {:d}", evt.GetCacheFrom(), evt.GetCacheTo());
	//do this in the ProductManager
	/*for (; i < evt.GetCacheTo(); i++) {
		auto& row = datastore[i];
		wxGetApp().mProductManager.RefreshRowFromDatabase(
			boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]), row);
	}*/

}

void pof::ProductView::OnStockCheckTimer(wxTimerEvent& evt)
{
	mInfoBar->ShowMessage("Stock check is incomplete, please finish stock check before end of month");
}

void pof::ProductView::OnCreateControlBook(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Product", wxICON_INFORMATION | wxOK);
		return;
	}
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;
	auto& prow = wxGetApp().mProductManager.GetProductData()->GetDatastore()[pof::DataModel::GetIdxFromItem(item)];
	auto& puid = boost::variant2::get<pof::base::data::duuid_t>(prow.first[pof::ProductManager::PRODUCT_UUID]);
	auto& name = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_NAME]);
	auto& cls = boost::variant2::get<pof::base::data::text_t>(prow.first[pof::ProductManager::PRODUCT_CLASS]);
	
	if (cls != "CONTROLLED"){
		wxMessageBox(fmt::format("{} is not a controlled medicaiton", name), "Products", wxICON_INFORMATION | wxOK);
		return;
	}

	if (wxGetApp().mPoisonBookManager.IsBookCreated(puid)) {
		wxMessageBox(fmt::format("Controlled book register already created for {}", name), "Products", wxICON_INFORMATION | wxOK);
		return;
	}
	pof::base::data::row_t row;
	//create the first entry into the book table	
	auto& v = row.first;
	auto& p = prow.first;

	v.push_back(p[pof::ProductManager::PRODUCT_UUID]);
	v.push_back("ENTRY"s);
	v.push_back(wxGetApp().MainPharmacy->GetAddressAsString());
	v.push_back(fmt::format("{} {}", wxGetApp().MainAccount->lastname, wxGetApp().MainAccount->name));
	v.push_back(static_cast<std::uint64_t>(1));
	v.push_back(static_cast<std::uint64_t>(0));
	//create with the current stock
	std::uint64_t stock = boost::variant2::get<std::uint64_t>(p[pof::ProductManager::PRODUCT_STOCK_COUNT]);
	v.push_back(stock);
	v.push_back(stock);
	v.push_back(pof::base::data::clock_t::now());

	if (!wxGetApp().mPoisonBookManager.CreateNewBook(std::move(row))){
		wxMessageBox(fmt::format("Failed to create book for {}", name), "Products", wxICON_INFORMATION | wxOK);
	}
	else {
		mInfoBar->ShowMessage(fmt::format("Created controlled book for {}", name));
	}
	addPoisonBook(prow);
}

void pof::ProductView::OnAddVariant(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Add variant", wxICON_INFORMATION | wxOK);
		return;
	}

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& prod = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
	auto& name = boost::variant2::get<pof::base::data::text_t>(prod.first[pof::ProductManager::PRODUCT_NAME]);
	double temp = 0.0f;
	auto& price = boost::variant2::get<pof::base::data::currency_t>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
	wxFloatingPointValidator<double> val(2, &temp, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 999999999999);

	wxDialog dialog(this, wxID_ANY, "Add variation", wxDefaultPosition, wxSize(591, 413), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	//dialog.SetSizeHints(wxDefaultSize, wxDefaultSize);
	dialog.SetBackgroundColour(*wxWHITE);
	wxDialog* d = std::addressof(dialog);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m_panel1 = new wxPanel(d, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel1->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	auto TitleText = new wxStaticText(m_panel1, wxID_ANY, wxT("Add Product Variation"), wxDefaultPosition, wxDefaultSize, 0);
	TitleText->Wrap(-1);
	TitleText->SetFont(wxFontInfo().Bold().AntiAliased());
	bSizer2->Add(TitleText, 1, wxALL, 5);

	auto m_staticText18 = new wxStaticText(m_panel1, wxID_ANY, fmt::format("Create different variation for {}", name), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText18->Wrap(-1);
	bSizer2->Add(m_staticText18, 0, wxALL, 5);

	auto warn = new wxCheckBox(m_panel1, wxID_ANY, fmt::format("Add warnings"));
	bSizer2->Add(warn, 0, wxEXPAND | wxALL, 5);

	m_panel1->SetSizer(bSizer2);
	m_panel1->Layout();
	bSizer2->Fit(m_panel1);

	bSizer1->Add(m_panel1, 0, wxEXPAND | wxALL, 5);

	auto m_panel2 = new wxPanel(d, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	auto m_panel4 = new wxPanel(m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	
	wxStaticBoxSizer* sbSizer7 = new wxStaticBoxSizer(new wxStaticBox(m_panel4, wxID_ANY, wxT("Product Details")), wxVERTICAL);

	auto m_scrolledWindow2 = new wxPanel(sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer2->AddGrowableCol(1);
	fgSizer2->SetFlexibleDirection(wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	
	auto productname = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Product name"), wxDefaultPosition, wxDefaultSize, 0);
	productname->Wrap(-1);
	fgSizer2->Add(productname, 0, wxALL, 5);

	auto productnamevalue = new wxTextCtrl(m_scrolledWindow2, wxID_ANY, name, wxDefaultPosition, wxDefaultSize, 0);
	productnamevalue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	productnamevalue->SetMaxLength(250);
	fgSizer2->Add(productnamevalue, 1, wxALL | wxEXPAND, 5);

	//name entries
	
	auto formulationLabel = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Formulation"), wxDefaultPosition, wxDefaultSize, 0);
	formulationLabel->Wrap(-1);
	fgSizer2->Add(formulationLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	auto formulation = new wxChoice(m_scrolledWindow2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxGetApp().FormulationChoices);
	formulation->SetSelection(0);
	fgSizer2->Add(formulation, 0, wxALL | wxEXPAND, 5);

	auto strength = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Strength"), wxDefaultPosition, wxDefaultSize, 0);
	strength->Wrap(-1);
	fgSizer2->Add(strength, 0, wxALL, 5);

	auto strengthvalue = new wxTextCtrl(m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	strengthvalue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	strengthvalue->SetMaxLength(250);
	fgSizer2->Add(strengthvalue, 1, wxALL | wxEXPAND, 5);

	auto strengthtypelabel = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Strength Type"), wxDefaultPosition, wxDefaultSize, 0);
	strengthtypelabel->Wrap(-1);
	fgSizer2->Add(strengthtypelabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	auto strengthtype = new wxChoice(m_scrolledWindow2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxGetApp().StrengthChoices);
	strengthtype->SetSelection(0);
	fgSizer2->Add(strengthtype, 1, wxALL | wxEXPAND, 5);

	auto packsizelabel = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Package Size"), wxDefaultPosition, wxDefaultSize, 0);
	packsizelabel->Wrap(-1);
	fgSizer2->Add(packsizelabel, 0, wxALL, 5);

	auto packsize = new wxSpinCtrl(m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxALIGN_LEFT, 0, 999999999);
	fgSizer2->Add(packsize, 1, wxALL | wxEXPAND, 5);

	auto salepricelabel = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Sale Price"), wxDefaultPosition, wxDefaultSize, 0);
	salepricelabel->Wrap(-1);
	fgSizer2->Add(salepricelabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	auto saleprice = new wxTextCtrl(m_scrolledWindow2, wxID_ANY, fmt::format("{:cu}", price), wxDefaultPosition, wxDefaultSize, 0);
	saleprice->SetValidator(val);
	fgSizer2->Add(saleprice, 0, wxALL | wxEXPAND, 5);

	m_scrolledWindow2->SetSizer(fgSizer2);
	m_scrolledWindow2->Layout();
	fgSizer2->Fit(m_scrolledWindow2);
	sbSizer7->Add(m_scrolledWindow2, 0, wxEXPAND | wxALL, 5);

	m_panel4->SetSizer(sbSizer7);
	m_panel4->Layout();
	sbSizer7->Fit(m_panel4);
	bSizer3->Add(m_panel4, 1, wxEXPAND | wxALL, 5);

	m_panel2->SetSizer(bSizer3);
	m_panel2->Layout();
	bSizer3->Fit(m_panel2);

	bSizer1->Add(m_panel2, 1, wxEXPAND | wxALL, 5);


	auto m_panel7 = new wxPanel(d, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxVERTICAL);

	auto m_sdbSizer2 = new wxStdDialogButtonSizer();
	auto m_sdbSizer2OK = new wxButton(m_panel7, wxID_OK);
	m_sdbSizer2->AddButton(m_sdbSizer2OK);
	auto m_sdbSizer2Cancel = new wxButton(m_panel7, wxID_CANCEL);
	m_sdbSizer2->AddButton(m_sdbSizer2Cancel);
	m_sdbSizer2->Realize();

	bSizer4->Add(m_sdbSizer2, 0, wxEXPAND, 5);

	m_panel7->SetSizer(bSizer4);
	m_panel7->Layout();
	bSizer4->Fit(m_panel7);
	bSizer1->Add(m_panel7, 0, wxEXPAND | wxALL, 5);

	d->SetSizer(bSizer1);
	d->Layout();
	d->Center(wxBOTH);
	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	d->SetIcon(appIcon);

	if (dialog.ShowModal() == wxID_CANCEL) return;

	//transfer to from window
	pof::base::data::row_t newprod;
	auto& v = newprod.first;
	auto& p = prod.first;

	v.resize(pof::ProductManager::PRODUCT_MAX);
	v[pof::ProductManager::PRODUCT_UUID] = boost::uuids::random_generator_mt19937{}();
	v[pof::ProductManager::PRODUCT_SERIAL_NUM] = pof::GenRandomId();
	v[pof::ProductManager::PRODUCT_NAME] = productnamevalue->GetValue().ToStdString();
	v[pof::ProductManager::PRODUCT_GENERIC_NAME] = p[pof::ProductManager::PRODUCT_GENERIC_NAME];
	v[pof::ProductManager::PRODUCT_FORMULATION] = wxGetApp().FormulationChoices[formulation->GetSelection()].ToStdString();
	v[pof::ProductManager::PRODUCT_MIN_STOCK_COUNT] = static_cast<std::uint64_t>(0);
	v[pof::ProductManager::PRODUCT_CLASS] = p[pof::ProductManager::PRODUCT_CLASS];
	v[pof::ProductManager::PRODUCT_BARCODE] = ""s;
	v[pof::ProductManager::PRODUCT_COST_PRICE] = p[pof::ProductManager::PRODUCT_COST_PRICE];
	v[pof::ProductManager::PRODUCT_UNIT_PRICE] = pof::base::data::currency_t(saleprice->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_USAGE_INFO] = p[pof::ProductManager::PRODUCT_USAGE_INFO];
	v[pof::ProductManager::PRODUCT_HEALTH_CONDITIONS] = p[pof::ProductManager::PRODUCT_HEALTH_CONDITIONS];
	v[pof::ProductManager::PRODUCT_DESCRIP] = p[pof::ProductManager::PRODUCT_DESCRIP];
	v[pof::ProductManager::PRODUCT_STRENGTH] = strengthvalue->GetValue().ToStdString();
	v[pof::ProductManager::PRODUCT_STRENGTH_TYPE] = std::move(strengthtype->GetStringSelection().ToStdString());
	v[pof::ProductManager::PRODUCT_PACKAGE_SIZE] = static_cast<std::uint64_t>(packsize->GetValue());
	v[pof::ProductManager::PRODUCT_SIDEEFFECTS] = p[pof::ProductManager::PRODUCT_SIDEEFFECTS];
	v[pof::ProductManager::PRODUCT_STOCK_COUNT] = static_cast<std::uint64_t>(0);
	v[pof::ProductManager::PRODUCT_EXPIRE_PERIOD] = static_cast<std::uint64_t>(0);
	v[pof::ProductManager::PRODUCT_TO_EXPIRE_DATE] = ""s;
	v[pof::ProductManager::PRODUCT_CATEGORY] = p[pof::ProductManager::PRODUCT_CATEGORY];

	if (warn->GetValue()){
		wxGetApp().mProductManager.DuplicateWarning(
			boost::variant2::get<pof::base::data::duuid_t>(p[pof::ProductManager::PRODUCT_UUID]),
			boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
	}


	wxGetApp().mProductManager.GetProductData()->StoreData(std::move(newprod));
	const size_t count = wxGetApp().mProductManager.GetProductData()->GetDatastore().size();
	m_dataViewCtrl1->SetFocus();
	m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);
	m_dataViewCtrl1->Select(pof::DataModel::GetItemFromIdx(count - 1));

	mInfoBar->ShowMessage(fmt::format("Sucessfully created a variation for {}", name), wxICON_INFORMATION);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Created a variation for {}", name));
}

void pof::ProductView::OnEndOfMonth(wxCommandEvent& evt)
{
	pof::ReportsDialog dialog(nullptr, wxID_ANY, wxEmptyString);
	if (dialog.LoadReport(pof::ReportsDialog::ReportType::EOM, pof::base::data::clock_t::now())) dialog.ShowModal();
}

void pof::ProductView::OnStoreSummary(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	if (wxGetApp().mProductManager.GetProductData()->GetDatastore().empty()) {
		wxMessageBox("Store is empty, please add products to see summary", "Products", wxICON_INFORMATION | wxOK);
		return;
	}

	wxDialog dialog(this, wxID_ANY, "Store summary", wxDefaultPosition, FromDIP(wxSize(991, 513)), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	//dialog.SetSizeHints(wxDefaultSize, wxDefaultSize);
	dialog.SetBackgroundColour(*wxWHITE);
	wxDialog* d = std::addressof(dialog);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	wxPanel* m_panel1 = new wxPanel(d, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel1->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	auto font = wxGetApp().mFontSettings.GetChosenFont();
	const int fontSize = std::max(std::min(12, font.GetPointSize()), 9);;
	const int valueFontSize = std::max(std::min(13, font.GetPointSize()), 10);
	wxFont valueFont(wxFontInfo(valueFontSize).AntiAliased().Family(font.GetFamily()).FaceName(font.GetFaceName()).Style(font.GetStyle()).Bold());
	wxFont descripFont = wxFont(wxFontInfo(fontSize).Family(font.GetFamily()).AntiAliased().FaceName(font.GetFaceName()).Style(font.GetStyle()));

	wxPanel* cp = new wxPanel(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bS2;
	bS2 = new wxBoxSizer(wxHORIZONTAL);

	auto TitleText = new wxStaticText(cp, wxID_ANY, wxT("Store summary"), wxDefaultPosition, wxDefaultSize, 0);
	TitleText->Wrap(-1);
	TitleText->SetFont(valueFont);
	bS2->AddStretchSpacer(1);
	bS2->Add(TitleText, 0, wxEXPAND | wxALL, 5);
	bS2->AddStretchSpacer(1);

	cp->SetSizer(bS2);
	bS2->SetSizeHints(cp);

	bSizer2->Add(cp, 0, wxEXPAND | wxALL, 5);

	//first panel
	wxPanel* cp2 = new wxPanel(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bS3;
	bS3 = new wxBoxSizer(wxHORIZONTAL);

	bS3->AddStretchSpacer();
	//total product
	wxPanel* tpp = new wxPanel(cp2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
	wxBoxSizer* tpps;
	tpps = new wxBoxSizer(wxVERTICAL);

	auto totalProductslabel = new wxStaticText(tpp, wxID_ANY, wxT("Total Products"), wxDefaultPosition, wxDefaultSize, 0);
	totalProductslabel->Wrap(-1);
	totalProductslabel->SetFont(descripFont);

	auto totalProducts = new wxStaticText(tpp, wxID_ANY, fmt::format("{:d}",
		wxGetApp().mProductManager.GetProductData()->GetDatastore().size()), wxDefaultPosition, wxDefaultSize, 0);
	totalProducts->Wrap(-1);
	totalProducts->SetFont(valueFont);

	tpps->AddSpacer(FromDIP(5));
	tpps->Add(totalProducts, 1, wxALL | wxEXPAND, FromDIP(5));
	tpps->Add(totalProductslabel, 0, wxALL | wxEXPAND, FromDIP(5));
	tpps->AddSpacer(FromDIP(2));

	tpp->SetSizer(tpps);
	tpps->SetSizeHints(tpp);

	bS3->Add(tpp, 0, wxALL, FromDIP(5));


	//expired products
	auto exp = wxGetApp().mProductManager.DoExpiredProducts();


	wxPanel* expp = new wxPanel(cp2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
	wxBoxSizer* expps;
	expps = new wxBoxSizer(wxVERTICAL);

	auto expProductslabel = new wxStaticText(expp, wxID_ANY, wxT("Total Expired Products"), wxDefaultPosition, wxDefaultSize, 0);
	expProductslabel->Wrap(-1);
	expProductslabel->SetFont(descripFont);

	auto expProducts = new wxStaticText(expp, wxID_ANY, fmt::format("{:d}", exp.value_or(std::vector<wxDataViewItem>{}).size()), wxDefaultPosition, wxDefaultSize, 0);
	expProducts->Wrap(-1);
	expProducts->SetFont(valueFont);

	expps->AddSpacer(FromDIP(5));
	expps->Add(expProducts, 1, wxALL | wxEXPAND, FromDIP(5));
	expps->Add(expProductslabel, 0, wxALL | wxEXPAND, FromDIP(5));
	expps->AddSpacer(FromDIP(2));

	expp->SetSizer(expps);
	expps->SetSizeHints(expp);

	bS3->Add(expp, 0, wxALL, FromDIP(5));


	//out of stock
	auto os = wxGetApp().mProductManager.DoOutOfStock();

	wxPanel* osp = new wxPanel(cp2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
	wxBoxSizer* osps;
	osps = new wxBoxSizer(wxVERTICAL);

	auto osProductslabel = new wxStaticText(osp, wxID_ANY, wxT("Total Out of stock Products"), wxDefaultPosition, wxDefaultSize, 0);
	osProductslabel->Wrap(-1);
	osProductslabel->SetFont(descripFont);

	auto osProducts = new wxStaticText(osp, wxID_ANY, fmt::format("{:d}", os.value_or(std::vector<wxDataViewItem>{}).size()), wxDefaultPosition, wxDefaultSize, 0);
	osProducts->Wrap(-1);
	osProducts->SetFont(valueFont);

	osps->AddSpacer(FromDIP(5));
	osps->Add(osProducts, 1, wxALL | wxEXPAND, FromDIP(5));
	osps->Add(osProductslabel, 0, wxALL | wxEXPAND, FromDIP(5));
	osps->AddSpacer(FromDIP(2));

	osp->SetSizer(osps);
	osps->SetSizeHints(osp);

	bS3->Add(osp, 0, wxALL, FromDIP(5));

	const auto yearStrings = wxGetApp().mSaleManager.GetStoreYears();
	if (yearStrings.empty()) {
		wxMessageBox("No sales in store cannot do summary", "Store summary", wxICON_INFORMATION | wxOK);
		return;
	}
	wxArrayString arr;
	for (const auto& ys : yearStrings) {
		arr.push_back(fmt::format("{:%Y}", ys));
	}
	wxSingleChoiceDialog box(NULL, "Please select a year to summarize.", "Store summary",arr);
	const auto ret = box.ShowModal();
	if (ret != wxID_OK) {
		return;
	}
	int found = box.GetSelection();
	if (found == wxNOT_FOUND) return;
	//total rev
	//auto dt = pof::base::data::clock_t::now();
	auto& dt = yearStrings[found];
	auto rev = wxGetApp().mSaleManager.GetYearTotalRevenue(dt);
	if (rev) {

		wxPanel* revp = new wxPanel(cp2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
		wxBoxSizer* revps;
		revps = new wxBoxSizer(wxVERTICAL);

		auto revProductslabel = new wxStaticText(revp, wxID_ANY, fmt::format("Total revenue for {:%Y}", dt), wxDefaultPosition, wxDefaultSize, 0);
		revProductslabel->Wrap(-1);
		revProductslabel->SetFont(descripFont);

		auto revProducts = new wxStaticText(revp, wxID_ANY, fmt::format("{:cu}", rev.value()), wxDefaultPosition, wxDefaultSize, 0);
		revProducts->Wrap(-1);
		revProducts->SetFont(valueFont);

		revps->AddSpacer(FromDIP(5));
		revps->Add(revProducts, 1, wxALL | wxEXPAND, FromDIP(5));
		revps->Add(revProductslabel, 0, wxALL | wxEXPAND, FromDIP(5));
		revps->AddSpacer(FromDIP(2));

		revp->SetSizer(revps);
		revps->SetSizeHints(revp);

		bS3->Add(revp, 0, wxALL, FromDIP(5));
	}

	auto tsc = wxGetApp().mProductManager.GetTotalStockCost();
	if (tsc) {
		wxPanel* tscp = new wxPanel(cp2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
		wxBoxSizer* tscps;
		tscps = new wxBoxSizer(wxVERTICAL);

		auto tscProductslabel = new wxStaticText(tscp, wxID_ANY, wxT("Total Stock cost   "), wxDefaultPosition, wxDefaultSize, 0);
		tscProductslabel->Wrap(-1);
		tscProductslabel->SetFont(descripFont);

		auto tscProducts = new wxStaticText(tscp, wxID_ANY, fmt::format("{:cu}", tsc.value_or(pof::base::currency{})), wxDefaultPosition, wxDefaultSize, 0);
		tscProducts->Wrap(-1);
		tscProducts->SetFont(valueFont);

		tscps->AddSpacer(FromDIP(5));
		tscps->Add(tscProducts, 1, wxALL | wxEXPAND, FromDIP(5));
		tscps->Add(tscProductslabel, 0, wxALL | wxEXPAND, FromDIP(5));
		tscps->AddSpacer(FromDIP(2));

		tscp->SetSizer(tscps);
		tscps->SetSizeHints(tscp);

		bS3->Add(tscp, 0, wxALL, FromDIP(5));
	}

	bS3->AddStretchSpacer();

	cp2->SetSizer(bS3);
	bS3->SetSizeHints(cp2);

	bSizer2->Add(cp2, 0, wxEXPAND | wxALL, 5);

	wxPanel* cp3 = new wxPanel(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bS4 = new wxBoxSizer(wxHORIZONTAL);

	std::vector<wxString> cats = { "Profit", "Cost" };
	CategorySimpleDataset* dataset = new CategorySimpleDataset(cats.data(), cats.size());
	auto totalPL = wxGetApp().mSaleManager.GetTotalPL(dt);
	auto pair = totalPL.value_or(std::make_pair(pof::base::currency{}, pof::base::currency{}));
	std::vector<double> amounts = { static_cast<double>(pair.first), static_cast<double>(pair.second) };
	dataset->AddSerie(wxT("Amount"), amounts.data(), amounts.size());
	wxColour carr[2] = { *wxGREEN, *wxRED };
	ColorScheme scheme(carr, 2);
	dataset->SetRenderer(new CategoryRenderer(scheme));

	PiePlot* plot = new PiePlot();
	plot->SetDataset(dataset);
	plot->SetUsedSerie(0);
	plot->SetColorScheme(&scheme);
	//plot->Set3DView(true);
	plot->SetBackground(new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));

	Legend* legend = new Legend(wxCENTER, wxRIGHT, new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));
	plot->SetLegend(legend);

	const auto header = new Header(fmt::format("P/L for {:%Y}", dt));
	Chart* mCurrentChart = new Chart(plot, header);

	wxChartPanel* mChartPanel = new wxChartPanel(cp3, wxID_ANY, NULL, wxDefaultPosition, FromDIP(wxSize(400, 400))); // pie chart for reveneue break down
	mChartPanel->SetChart(mCurrentChart);
#ifdef wxUSE_GRAPHICS_CONTEXT
	mChartPanel->SetAntialias(true);
	mChartPanel->SetWindowStyle(wxNO_BORDER);
#endif
	ZoomMode* mode = new ChartZoom();
	mode->SetAllowHorizontalZoom(true);
	mode->SetAllowVertialZoom(true);
	mChartPanel->SetMode(mode);
	
	bS4->AddStretchSpacer();
	bS4->Add(mChartPanel, 0, wxALL | wxEXPAND, FromDIP(5));


	cats = { "JAN", "FEB", "MAR", "APL", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	amounts.clear();
	amounts.resize(cats.size());
	std::ranges::fill(amounts, 0.0);

	CategorySimpleDataset* mthdataset = new CategorySimpleDataset(cats.data(), cats.size());
	auto monthsales = wxGetApp().mSaleManager.GetMonthlySales(dt);
	
	for (const auto& i : monthsales.value_or(pof::base::data{})) {
		spdlog::info("{:%d/%m/%Y}", boost::variant2::get<pof::base::data::datetime_t>(i.first[0]) + date::days(1));
		auto month = date::floor<date::months>(boost::variant2::get<pof::base::data::datetime_t>(i.first[0]) + date::days(1));
		int monthCount = month.time_since_epoch().count() % 12;
		amounts[monthCount] = static_cast<double>(boost::variant2::get<pof::base::currency>(i.first[1]));
	}
	mthdataset->AddSerie(wxT("Amount"), amounts.data(), amounts.size());
	BarType* barType = new NormalBarType(FromDIP(20));
	mthdataset->SetRenderer(new BarRenderer(barType));
	BarPlot* mthplot = new BarPlot();

	NumberAxis* leftAxis = new NumberAxis(AXIS_LEFT);
	leftAxis->SetMargins(FromDIP(5), FromDIP(0));
	leftAxis->SetLabelTextColour(*wxBLACK);
	leftAxis->SetLabelPen(wxPen(*wxBLACK));
	leftAxis->SetMajorGridlinePen(wxPen(*wxBLACK));
	mthplot->AddAxis(leftAxis);

	CategoryAxis* bottomAxis = new CategoryAxis(AXIS_BOTTOM);
	bottomAxis->SetMargins(FromDIP(5), FromDIP(5));
	bottomAxis->SetLabelTextColour(*wxBLACK);
	bottomAxis->SetLabelPen(wxPen(*wxBLACK));
	mthplot->AddAxis(bottomAxis);

	mthplot->AddDataset(mthdataset);

	mthplot->SetBackground(new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));

	mthplot->LinkDataHorizontalAxis(0, 0);

	mthplot->LinkDataVerticalAxis(0, 0);
	Legend* mthlegend = new Legend(wxCENTER, wxRIGHT, new FillAreaDraw(*wxTRANSPARENT_PEN, *wxTRANSPARENT_BRUSH));
	mthplot->SetLegend(mthlegend);

	Chart* mthCurrentChart = new Chart(mthplot, new Header(fmt::format("Monthly sales for {:%Y}", dt)));

	wxChartPanel* mChartPanel2 = new wxChartPanel(cp3, wxID_ANY, NULL, wxDefaultPosition, FromDIP(wxSize(400, 400))); // pie chart for reveneue break down
	mChartPanel2->SetChart(mthCurrentChart);
#ifdef wxUSE_GRAPHICS_CONTEXT
	mChartPanel2->SetAntialias(true);
	mChartPanel2->SetWindowStyle(wxNO_BORDER);
#endif
	ZoomMode* mode2 = new ChartZoom();
	mode2->SetAllowHorizontalZoom(true);
	mode2->SetAllowVertialZoom(true);
	mChartPanel2->SetMode(mode2);

	bS4->Add(mChartPanel2, 0, wxALL | wxEXPAND, FromDIP(5));
	bS4->AddStretchSpacer();

	cp3->SetSizer(bS4);
	bS4->SetSizeHints(cp3);
	bSizer2->Add(cp3, 0, wxEXPAND | wxALL, FromDIP(5));


	m_panel1->SetSizer(bSizer2);
	m_panel1->Layout();
	bSizer2->Fit(m_panel1);

	bSizer1->Add(m_panel1, 1, wxEXPAND | wxALL, FromDIP(5));

	d->SetSizer(bSizer1);
	d->Layout();
	d->Center(wxBOTH);
	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	d->SetIcon(appIcon);

	d->ShowModal();
}

void pof::ProductView::OnIncrPrice(wxCommandEvent& evt)
{
	int id = evt.GetId();
	auto str = wxGetTextFromUser("Please enter the pecentage you want to increae the price by (%)\nFor example 2% would increase N100 to N102.", "Increase price");
	if (str.empty()) return;
	float percent = 0.0f;
	try {
		percent = boost::lexical_cast<float>(str);
		if (percent > 100.0f || percent < 0.0f){
			wxMessageBox("Price should be between 0 and 100", "Increase price", wxICON_WARNING | wxOK);
			return;
		}
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
		wxMessageBox("Invalid input", "Increase price", wxICON_ERROR | wxOK);
		return;
	}
	switch (id)
	{
	case ID_INCR_PRICE:
	{
		wxProgressDialog dlg("Increasing prices", "please wait...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
		int i = 0;
		int pg = 0;

		auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
		const size_t count = datastore.size();
		for (auto& prod : datastore){
			pof::base::currency& price =
				boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
			pof::base::data::duuid_t& uuid =
				boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);
				
			//round up ??
			const float rnd = (100.0f + percent) / 100.0f;
			price = price * static_cast<double>(rnd);

			price.nearest_hundred(); //round to nearest hundred


			prod.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
			wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, i);
			prod.second.second.reset();

			wxGetApp().mProductManager.GetProductData()->ItemChanged(pof::DataModel::GetItemFromIdx(i));
			pg = static_cast<float>(((float)i / (float)count) * 100.f);
			i++;
			dlg.Update(pg);
		}
		wxMessageBox("Successfully increased store prices", "Increase price", wxICON_INFORMATION | wxOK);
		m_dataViewCtrl1->SetFocus();
	}
		break;
	case ID_INCR_PRODUCT_PRICE:
	{
		wxBusyCursor cursor;
		if (!mSelections.empty())
		{
			wxProgressDialog dlg("Increasing prices", "please wait...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
			int i = 0;
			int pg = 0;
			const size_t count = mSelections.size();

			for (const auto& item : mSelections)
			{
				const size_t idx = pof::DataModel::GetIdxFromItem(item);
				auto& prod = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];

				pof::base::currency& price =
					boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
				pof::base::data::duuid_t& uuid =
					boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);

				//round up ??
					//round up ??
				const float rnd = (100.0f + percent) / 100.0f;
				price = price * static_cast<double>(rnd);

				price.nearest_hundred(); //round to nearest hundred


				prod.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
				wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
				prod.second.second.reset();

				wxGetApp().mProductManager.GetProductData()->ItemChanged(item);

				pg = static_cast<float>(((float)i / (float)count) * 100.f);
				i++;
				dlg.Update(pg);

			}
			wxMessageBox(fmt::format("Successfully increased {:d} store prices", mSelections.size()), "Increase price", wxICON_INFORMATION | wxOK);

		}
		else {
 			auto item = m_dataViewCtrl1->GetSelection();
			if (!item.IsOk()) return;
			
			const size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& prod = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];

			pof::base::currency& price =
				boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
			pof::base::data::duuid_t& uuid =
				boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);

			//round up ??
				//round up ??
			const float rnd = (100.0f + percent) / 100.0f;
			price = price * static_cast<double>(rnd);

			price.nearest_hundred(); //round to nearest hundred
			
			prod.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
			wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
			prod.second.second.reset();

			wxGetApp().mProductManager.GetProductData()->ItemChanged(item);

		}
	}
		m_dataViewCtrl1->SetFocus();
		break;
	}
}

void pof::ProductView::OnOpenProductInfo(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	const size_t idx = pof::DataModel::GetIdxFromItem(item);
	const pof::base::data::row_t& row = datastore[idx];
	mProductinfo->Load(row);
	SwapCenterPane(true);
}

void pof::ProductView::OnShowSupplier(wxCommandEvent& evt)
{
	auto& pane = m_mgr.GetPane("Supplier");
	auto& dataview = m_mgr.GetPane("DataView");
	auto& noresult = m_mgr.GetPane("NoResult");
	auto& empty = m_mgr.GetPane("Empty");

	static int c = -1;
	if (evt.IsChecked())
	{
		pane.Show();
		m_searchCtrl1->Clear();

		if (dataview.IsShown()) {
			dataview.Hide();
			c = 0;
		}
		else if (noresult.IsShown()) { noresult.Hide(); c = 1; }
		else if (empty.IsShown()) { empty.Hide(); c = 2; }
	}
	else {
		pane.Hide();
		switch (c)
		{
		case 0:
			dataview.Show();
			break;
		case 1:
			noresult.Show();
			break;
		case 2:
			empty.Show();
			CheckEmpty(); //possible to add product while in supplier view, check empty to see if we are still empty
			break;
		default:
			break;
		}
	}
	m_mgr.Update();
	mSupplierView->CheckEmpty(pof::SupplierView::SUPPLIER_VIEW);
}

void pof::ProductView::OnProfitLoss(wxCommandEvent& evt)
{
}

void pof::ProductView::OnDownloadActualStock(wxCommandEvent& evt)
{
	std::optional<pof::base::data> datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	if (datastore->empty()) {
		wxMessageBox("No transaction to save", "Reports", wxICON_INFORMATION | wxOK);
		return;
	}

	wxFileDialog dialog(this, "Save actual stock as excel file", wxEmptyString, wxEmptyString, "Excel files (*.xlsx)|*.xlsx",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_CANCEL) return;
	auto filename = dialog.GetPath().ToStdString();
	auto fullPath = fs::path(filename);

	if (fullPath.extension() != ".xlsx") {
		wxMessageBox("File extension is not compactable with .xlsx or .xls files", "Export Excel",
			wxICON_INFORMATION | wxOK);
		return;
	}

	wxBusyCursor cursor;
	excel::XLDocument doc;
	doc.create(fullPath.string());
	if (!doc.isOpen()) {
		spdlog::error("Canont open xlsx file");
		return;
	}

	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName("Actual stock");

	const size_t colSize = 4; //name, stock, unit price, actual price
	const size_t rowSize = datastore.value().size() + 3; //plus title and total row
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};

	writeHeader("Product");
	writeHeader("Stock count");
	writeHeader("Unit price");
	writeHeader("Actual price");
	
	pof::base::currency totalAmount;
	wxProgressDialog dlg("Downloading actual stock", "please wait...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL | wxPD_AUTO_HIDE);
	int i = 0;
	int pg = 0;
	const int count = datastore->size();
	for (auto& prod : datastore.value()) {
		const auto& name = boost::variant2::get<pof::base::data::text_t>(prod.first[pof::ProductManager::PRODUCT_NAME]);
		const auto& stock = boost::variant2::get<std::uint64_t>(prod.first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
		const auto& unit_price = boost::variant2::get<pof::base::currency>(prod.first[pof::ProductManager::PRODUCT_UNIT_PRICE]);

		iter->value().set(name);
		iter++;

		iter->value().set(stock);
		iter++;

		iter->value().set(fmt::format("{:cu}", unit_price));
		iter++;

		const pof::base::currency actual_price = unit_price * static_cast<double>(stock);

		iter->value().set(fmt::format("{:cu}", actual_price));
		iter++;

		totalAmount += actual_price;

		pg = static_cast<float>(((float)i / (float)count) * 100.f);
		i++;
		if (!dlg.Update(pg)) {
			doc.close();
			std::filesystem::remove(fullPath); //remove the file
			return;
		}
	}	
	//total amount
	iter++;
	iter++;

	iter->value().set("Total stock amount");
	iter++;
	iter->value().set(fmt::format("{:cu}", totalAmount));

	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Saved data to {}", fullPath.string()), "Download Actual stock", wxICON_INFORMATION | wxOK);
	
}

void pof::ProductView::OnWeeklySales(wxCommandEvent& evt)
{
	pof::ChartDialog chart(this);
	chart.LoadChart(pof::ChartDialog::WEEKLY_SALES);
	chart.ShowModal();
}

void pof::ProductView::OnChartDropDown(wxAuiToolBarEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto d = menu->Append(ID_CHARTS_WEEKLY_SALES, "Weekly sales", nullptr);
	if (!mSelections.empty()) {
		auto x = menu->Append(ID_CHARTS_COMPARE_SALES, fmt::format("Compare {:d} products", mSelections.size()), nullptr);
	}

	wxPoint pos = mChartsDropItem->GetSizerItem()->GetPosition();
	wxSize sz = mChartsDropItem->GetSizerItem()->GetSize();

	m_auiToolBar1->PopupMenu(menu, wxPoint{ pos.x, pos.y + sz.y + 2 });
}

void pof::ProductView::OnCompareSales(wxCommandEvent& evt)
{
	//if (mSelections.empty()) return;
	if (mSelections.size() > 5) {
		wxMessageBox("Trying to compare more than 5 products, not possible", "Compare sales", wxICON_ERROR | wxOK);
		return;
	}
	std::vector<pof::base::data::duuid_t> duuids;
	duuids.reserve(mSelections.size());
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	for (auto& s : mSelections){
		size_t i = pof::DataModel::GetIdxFromItem(s);
		const auto& puid = boost::variant2::get<pof::base::data::duuid_t>(datastore[i].first[pof::ProductManager::PRODUCT_UUID]);
		duuids.emplace_back(puid);
	}

	pof::ChartDialog chart(this);
	chart.mCompareSalesArg = std::move(duuids);
	chart.LoadChart(pof::ChartDialog::COMPARE_SALES);
	chart.ShowModal();
}

void pof::ProductView::OnHideProduct(wxCommandEvent& evt)
{
	wxBusyCursor cur;
	m_dataViewCtrl1->Freeze();
	if (mSelections.empty()) {
		auto item = m_dataViewCtrl1->GetSelection();
		if (!item.IsOk()) return;
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);

		wxGetApp().mProductManager.HideProduct(uuid);
	}
	else {
		for (auto& item : mSelections) {
			if (!item.IsOk()) return;
			size_t idx = pof::DataModel::GetIdxFromItem(item);
			auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
			auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);

			wxGetApp().mProductManager.HideProduct(uuid);
		}
		mSelections.clear();
	}
	wxGetApp().mProductManager.LoadProductsFromDatabase(); // reload ??
	//reload the view when we modify the contents
	pof::DataModel* datam = wxGetApp().mProductManager.GetProductData().get();
	//check if we are in different states
	std::bitset<32> osstate(mOutOfStockItem->GetState());
	std::bitset<32> expstate(mExpireProductItem->GetState());
	if (osstate.test(5)) {

	}
	else if (expstate.test(5)) {
		//expired state

	}

	if (!m_searchCtrl1->IsEmpty()) {
		//in search sat
		bool empty = false;
		std::string search = m_searchCtrl1->GetValue().ToStdString();

		if (!mActiveCategory.empty()) {
			empty = datam->StringSearchAndReloadSet(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}
		else {
			empty = datam->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}

		if (empty) {
			ShowNoResult(search);
		}
		else {
			auto& d = m_mgr.GetPane("DataView");
			auto& p = m_mgr.GetPane("NoResult");
			if (p.IsShown()) p.Hide();
			if (!d.IsShown()) {
				d.Show();
			}
			m_mgr.Update();
		}
	}
	else if (!mActiveCategory.empty()) {
		datam->ReloadSet(); //reloads the active category
	}
	m_dataViewCtrl1->Thaw();
}

void pof::ProductView::OnShowHiddenProduct(wxCommandEvent& evt)
{
	auto count = wxGetApp().mProductManager.GetHiddenCount();
	if (!count.has_value()) {
		wxMessageBox("Error in reading hidden product, call D-GLOPA ADMIN", "Hidden products", wxICON_ERROR | wxOK);
		return;
	}
	if (count.value() == 0) {
		wxMessageBox("No hidden products", "Hidden products", wxICON_INFORMATION | wxOK);
		return;
	}
	auto hidden = wxGetApp().mProductManager.GetHiddenProducts();
	if (!hidden.has_value()) {
		wxMessageBox("Error in reading hidden product data, call D-GLOPA ADMIN", "Hidden products", wxICON_ERROR | wxOK);
		return;
	}

	pof::SearchProduct productselect(this, wxID_ANY, std::make_shared<pof::base::data>(hidden.value()), "Hidden products- Please select products to show in store");
	if (productselect.ShowModal() == wxID_CANCEL) return;

	m_dataViewCtrl1->Freeze();
	wxBusyCursor cur;
	if (productselect.HasMultipleSelections()) {
		auto products = productselect.GetSelectedProducts();
		for (auto& wrap : products) {
			auto& row = wrap.get();
			auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);

			wxGetApp().mProductManager.ShowProduct(uuid);
		}
	}
	else {
		auto& prod = productselect.GetSelectedProduct();
		auto& uuid = boost::variant2::get<pof::base::data::duuid_t>(prod.first[pof::ProductManager::PRODUCT_UUID]);

		wxGetApp().mProductManager.ShowProduct(uuid);
	}

	wxGetApp().mProductManager.LoadProductsFromDatabase();

	//reload the view when we modify the contents
	pof::DataModel* datam = wxGetApp().mProductManager.GetProductData().get();

	if (!m_searchCtrl1->IsEmpty()) {
		//in search sat
		bool empty = false;
		std::string search = m_searchCtrl1->GetValue().ToStdString();

		if (!mActiveCategory.empty()) {
			empty = datam->StringSearchAndReloadSet(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}
		else {
			empty = datam->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}

		if (empty) {
			ShowNoResult(search);
		}
		else {
			auto& d = m_mgr.GetPane("DataView");
			auto& p = m_mgr.GetPane("NoResult");
			if (p.IsShown()) p.Hide();
			if (!d.IsShown()) {
				d.Show();
			}
			m_mgr.Update();
		}
	}
	else if (!mActiveCategory.empty()) {
		datam->ReloadSet(); //reloads the active category
	}

	m_dataViewCtrl1->Thaw();
	mInfoBar->ShowMessage("Successfully shown product", wxICON_INFORMATION);
}

void pof::ProductView::OnExpiredMonth(wxCommandEvent& evt)
{
	//auto id = evt.GetId();
	//mInfoBar->Dismiss();
	//
	////reset the state
	//m_auiToolBar2->ToggleTool(evt.GetId(), true);
	//mActiveCategory.clear();
	//m_searchCtrl1->Clear();
	//m_searchCtrl1->SetDescriptiveText("Search for product");

	//wxBusyCursor cur;
	//auto items = wxGetApp().mProductManager.DoExpiredProducts(std::chrono::months(id));
	//if (!items.has_value()) {
	//	wxMessageBox("Failed to read database, contact D-GLOPA staff", "Products", wxICON_ERROR | wxOK);
	//	return;
	//}

	//if (items->empty()) {
	//	if(id == 1)mInfoBar->ShowMessage(fmt::format("There are no products expiring in {:d} month in the store", id), wxICON_INFORMATION);
	//	else mInfoBar->ShowMessage(fmt::format("There are no products expiring in {:d} months in the store", id), wxICON_INFORMATION);
	//	
	//	RemoveCheckedState(mExpireProductItem);
	//	return;
	//}
	//wxGetApp().mProductManager.GetProductData()->Reload(*items);
}

void pof::ProductView::OnSeachFlag(wxCommandEvent& evt)
{
}

void pof::ProductView::OnUpdateQuantity(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Backup", wxICON_INFORMATION | wxOK);
		return;
	}
	wxCredentialEntryDialog dialog(this, "User credentials are to update product quantity", "Change Quantity");
	dialog.Center(wxBOTH);
	dialog.SetBackgroundColour(*wxWHITE);
	while (1) {
		if (dialog.ShowModal() == wxID_CANCEL) return;
		auto cred = dialog.GetCredentials();
		if (!wxGetApp().MainAccount->ValidateCredentials(cred.GetUser().ToStdString(),
			cred.GetPassword().GetAsString().ToStdString())) {
			wxMessageBox("Invalid user name or password", "Update quantity", wxICON_WARNING | wxOK);
			continue;
		}
		break;
	}
	auto quan = wxGetTextFromUser("Please enter a new quantity");
	if (quan.empty()) return;
	try {
		size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
		const std::uint64_t q = boost::lexical_cast<std::uint64_t>(quan);
		const auto& pd = *wxGetApp().mProductManager.GetProductData();
		const auto& uid = boost::variant2::get<boost::uuids::uuid>(row.first[pof::ProductManager::PRODUCT_UUID]);
		const auto& name = boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]);

		wxGetApp().mProductManager.UpdatePD(std::make_tuple(uid, q), { "uuid","stock_count" });

		//refresh item
		m_dataViewCtrl1->Freeze();
		row.first[pof::ProductManager::PRODUCT_STOCK_COUNT] = q;
		m_dataViewCtrl1->Thaw();
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Updated {} product quantity to {:d}", name, q));


	}
	catch (const std::exception& exp) {
		wxMessageBox("Invalid quantity", "Quantity", wxICON_ERROR | wxOK);
	}
}

void pof::ProductView::OnShowTransfers(wxCommandEvent& evt)
{
	pof::TransferView tv(nullptr);
	tv.ShowModal();
}

void pof::ProductView::OnDataViewFontChange(const wxFont& font)
{
	m_dataViewCtrl1->Freeze();
	m_dataViewCtrl1->SetFont(font);
	m_dataViewCtrl1->Thaw();

	mNoResultText->SetFont(font);
	mInfoBar->SetFont(font);
	mProductinfo->OnChangeFont(font);
}

void pof::ProductView::OnProductInfoUpdated(const pof::ProductInfo::PropertyUpdate& mUpdatedElem)
{
	auto& DatModelptr = wxGetApp().mProductManager.GetProductData();
	auto& DataStore = DatModelptr->GetDatastore();
	auto Iter = std::find_if(DataStore.begin(), DataStore.end(), [&](const pof::base::data::row_t& elem) -> bool {
		return  (boost::variant2::get<pof::base::data::duuid_t>(elem.first[pof::ProductManager::PRODUCT_UUID]) ==
			boost::variant2::get<pof::base::data::duuid_t>(mUpdatedElem.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID]));
	});
	if (Iter == DataStore.end()) return; //nothing to update

	for (size_t i = 0; i < pof::ProductManager::PRODUCT_MAX; i++) {
		if (mUpdatedElem.mUpdatedElememts.test(i)) {
			if (i == pof::ProductManager::PRODUCT_STOCK_COUNT){
				Iter->first[i] = boost::variant2::get<std::uint64_t>(Iter->first[i]) + 
						boost::variant2::get<std::uint64_t>(mUpdatedElem.mUpdatedElementsValues.first[i]);
			}
			else {
				Iter->first[i] = mUpdatedElem.mUpdatedElementsValues.first[i];
			}
			Iter->second.second.set(i); //set the column as modified
		}
	}



	int idx = std::distance(DataStore.begin(), Iter);
	wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
	DataStore.set_state(idx,pof::base::data::state::MODIFIED);
	Iter->second.second.reset();

	std::string& name = boost::variant2::get<pof::base::data::text_t>(Iter->first[pof::ProductManager::PRODUCT_NAME]);
	mInfoBar->ShowMessage(fmt::format("{} is updated sucessfully", name), wxICON_INFORMATION);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Updated {} product information", name));
}

void pof::ProductView::OnProductStockRemoved(const pof::base::data::duuid_t& uuid,std::uint64_t newstock)
{
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();

	auto iter = std::find_if(datastore.begin(), datastore.end(), [&](const pof::base::data::row_t& elem) -> bool {
			return  boost::variant2::get<pof::base::data::duuid_t>(elem.first[pof::ProductManager::PRODUCT_UUID]) == uuid;
	});

	if (iter != datastore.end()) {
		iter->first[pof::ProductManager::PRODUCT_STOCK_COUNT] = newstock;
		iter->second.second.set(pof::ProductManager::PRODUCT_STOCK_COUNT);
		int idx = std::distance(datastore.begin(), iter);
		wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
		iter->second.second.reset();
	}
}

//remove this one, use only activated 
void pof::ProductView::OnCategorySelected(const std::string& name)
{
	wxMessageBox(fmt::format("{} is selected", name), "Category");
}

void pof::ProductView::ShowCostPriceColumn()
{
	mProductUnitPriceCol->SetWidth(100);
	mProductCostPriceCol = m_dataViewCtrl1->AppendTextColumn("Cost Price", pof::ProductManager::PRODUCT_COST_PRICE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
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
	mSelectionCol = m_dataViewCtrl1->PrependToggleColumn(wxT("Select"), SELECTION_MODEL_COL,
		 wxDATAVIEW_CELL_ACTIVATABLE, FromDIP(50));
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
	//check if ProductInfo is shown
	auto& prodinfo = m_mgr.GetPane("ProductInfo");
	if (prodinfo.IsOk() && prodinfo.IsShown()) return; //do not activate if on prodinfo

	auto& cats = wxGetApp().mProductManager.GetCategories();
	auto iter = std::ranges::find_if(cats, [&](auto& row) -> bool {return (boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::CATEGORY_NAME]) == name);});
	if (iter == cats.end()) {
		spdlog::error("No such category as {}", name);
		return;
	}
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto& found = *iter;
	const std::uint64_t CAT_ID = boost::variant2::get<std::uint64_t>(found.first[pof::ProductManager::CATEGORY_ID]);
	std::vector<wxDataViewItem> items;
	items.reserve(300); //hueristic lol
	for (size_t i = 0; i < datastore.size(); i++) {
		auto& datum = datastore[i].first[pof::ProductManager::PRODUCT_CATEGORY];
		if (datum.index() == static_cast<size_t>(datastore.get_metadata()[pof::ProductManager::PRODUCT_CATEGORY])) {
			const std::uint64_t productCatId = boost::variant2::get<std::uint64_t>(datastore[i].first[pof::ProductManager::PRODUCT_CATEGORY]);
			if (productCatId == CAT_ID) {
				items.emplace_back(wxDataViewItem{ reinterpret_cast<void*>(i + 1) });
			}
		}
	}

	items.shrink_to_fit();
	if (!items.empty()) {
		//check for expired
		m_dataViewCtrl1->Freeze();
		wxGetApp().mProductManager.GetProductData()->Reload(items);
		mActiveCategory = name;
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();

		std::vector<std::string> f;
		if (wxGetApp().bCheckExpiredOnUpdate) {
			auto expitems = wxGetApp().mProductManager.DoExpiredProducts();
			if(expitems.has_value() && !expitems->empty()) {
				int quan = 0;;
				for (auto& i : items) {
					const bool found = std::ranges::binary_search(expitems.value(), i);
					if (found) quan++;
				}
				if(quan != 0)
					if(quan == 1)f.push_back(fmt::format("{:d} product has expired", expitems->size()));
					else f.push_back(fmt::format("{:d} products has expired", expitems->size()));
			}
		}

		if (wxGetApp().bCheckOutOfStockOnUpdate) {
			auto ositems = wxGetApp().mProductManager.DoOutOfStock();
			if (ositems.has_value() && !ositems->empty()) {
				int quan = 0;
				for (auto& i : items){
					const bool found = std::ranges::binary_search(ositems.value(), i);
					if (found) {
						quan++;
						if (wxGetApp().bHighlightOutOfStockInCategory) {
							//use audit attribute
							auto& sa = wxGetApp().mAuditManager.auditAttr[static_cast<int>(pof::AuditManager::auditType::SALE)];
							wxGetApp().mProductManager.GetProductData()->AddAttr(i, sa);
						}
					}
				}
				if(quan != 0) 
					if(quan == 1)f.push_back(fmt::format("{:d} product is out of stock", quan));
					else f.push_back(fmt::format("{:d} products are out of stock", quan));
			}
		}
		if (!f.empty()) {
			mInfoBar->ShowMessage("");
			mInfoBar->ShowMessage(fmt::format("{} in {}", fmt::join(f, " and "), name), wxICON_INFORMATION);
		}
		else {
			if (mInfoBar->IsShown()) mInfoBar->Dismiss();
		}

		m_searchCtrl1->Clear();
		m_searchCtrl1->SetDescriptiveText(fmt::format("Search for products in {}", name));
		UpdateCategoryTool(name);
	}
	else {
		if (!mActiveCategory.empty() && name.empty()) {
			//reactivated from remove product from category
			mActiveCategory.clear();
			wxGetApp().mProductManager.GetProductData()->Reload();
			return;
		}
		mInfoBar->ShowMessage(fmt::format("{} is empty", name), wxICON_INFORMATION);
	}
}

void pof::ProductView::OnCategoryRemoved(const std::string& name)
{
	wxBusyCursor cursor;
	if (mActiveCategory == name) {
		wxGetApp().mProductManager.GetProductData()->Reload();
		mActiveCategory.clear();
		UpdateCategoryTool(""s);
	}

	wxGetApp().mProductManager.RemoveCategory(name);
	mInfoBar->ShowMessage(fmt::format("Removed \'{}\' successfully", name));
}

void pof::ProductView::OnCategoryEdited(const std::string& oldName, const std::string& newName)
{
	auto& catDatastore = wxGetApp().mProductManager.GetCategories();
	auto iter = std::ranges::find_if(catDatastore, [&](pof::base::data::row_t& row) -> bool {
		auto& v = row.first;
		auto& catName = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::CATEGORY_NAME]);
		return (oldName == catName);
	});
	if (iter == catDatastore.end()) return;
	auto& v = iter->first;
	spdlog::info("Changed name to in product {}", newName);
	v[pof::ProductManager::CATEGORY_NAME] = newName;
	iter->second.second.set(pof::ProductManager::CATEGORY_NAME); //set update to update database
	wxGetApp().mProductManager.UpdateCategory(iter);
	if (mActiveCategory == oldName)
	{
		mActiveCategory = newName;
		UpdateCategoryTool(newName);
	}
}

void pof::ProductView::UpdateCategoryTool(const std::string& name)
{
	mCatTextCtrl->SetLabelText(name);
	mCatNameItem->SetMinSize(FromDIP(mCatTextCtrl->GetSize()));
	m_auiToolBar2->Realize();
}

void pof::ProductView::CreateDataView()
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	mInfoBar = new wxInfoBar(panel, wxID_ANY);
	mInfoBar->SetAutoLayout(true);

	m_dataViewCtrl1 = new wxDataViewCtrl(panel, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	auto& pm = wxGetApp().mProductManager;
	m_dataViewCtrl1->AssociateModel(pm.GetProductData().get());
	pm.GetProductData()->DecRef();

	//wxFont font(wxFontInfo(12).AntiAliased());
	//m_dataViewCtrl1->SetFont(font);

	//mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Serial #"), pof::ProductManager::PRODUCT_SERIAL_NUM, wxDATAVIEW_CELL_INERT, 50, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, FromDIP(450), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	m_dataViewCtrl1->AppendTextColumn(wxT("Strength"), 11111, wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Class"), pof::ProductManager::PRODUCT_CLASS, wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductFormulation = m_dataViewCtrl1->AppendTextColumn(wxT("Formulation"), pof::ProductManager::PRODUCT_FORMULATION, wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductClass = m_dataViewCtrl1->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockLevel = m_dataViewCtrl1->AppendTextColumn(wxT("Stock Count"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, FromDIP(100), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductUnitPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("Unit Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, FromDIP(70), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE |  wxDATAVIEW_COL_REORDERABLE);

	sizer->Add(mInfoBar, wxSizerFlags().Expand().Border(wxALL, FromDIP(2)));
	sizer->Add(m_dataViewCtrl1, wxSizerFlags().Expand().Proportion(1).Border(wxALL, FromDIP(2)));

	panel->SetSizer(sizer);
	panel->Layout();
	sizer->Fit(panel);
	//SetupDataViewStyle();
	m_mgr.AddPane(panel, wxAuiPaneInfo().Name("DataView").CenterPane().CaptionVisible(false));
	
}

void pof::ProductView::CreateToolBar()
{
	m_auiToolBar1 = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	//m_auiToolBar1->SetToolBitmapSize(wxSize(FromDIP(16),FromDIP(16)));

	mChartsDropItem = m_auiToolBar1->AddTool(ID_CHARTS, "Charts", wxArtProvider::GetBitmap("insert_chart", wxART_OTHER, FromDIP(wxSize(16,16))), "Charts for different data");
	mChartsDropItem->SetHasDropDown(true);
	m_auiToolBar1->AddSpacer(FromDIP(2));

	mReportItem = m_auiToolBar1->AddTool(ID_REPORTS, wxT("Reports"), wxArtProvider::GetBitmap("monitoring", wxART_OTHER, FromDIP(wxSize(16,16))), wxT("Store reports"));
	mReportItem->SetHasDropDown(true);
	mFuncDropItem = m_auiToolBar1->AddTool(ID_FUNCTIONS, wxT("Functions"), wxArtProvider::GetBitmap("settings", wxART_OTHER, FromDIP(wxSize(16,16))), wxT("Run a function on all products in the store"));
	mFuncDropItem->SetHasDropDown(true);
	m_auiToolBar1->AddSpacer(FromDIP(10));
	//m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddStretchSpacer();

	m_searchCtrl1 = new wxSearchCtrl(m_auiToolBar1, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(FromDIP(450), FromDIP(- 1)), wxWANTS_CHARS);
#ifndef __WXMAC__
	m_searchCtrl1->ShowSearchButton(true);
#endif
	m_searchCtrl1->ShowCancelButton(true);
	m_searchCtrl1->SetDescriptiveText("Search for products");
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_GENERIC_NAME, "Search by generic name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->Check(ID_SEARCH_BY_NAME, true);
	mSearchFlag.set(0, true);

	m_searchCtrl1->SetMenu(menu);
	m_auiToolBar1->AddSpacer(FromDIP(5));
	m_auiToolBar1->AddControl(m_searchCtrl1);

	m_auiToolBar1->AddStretchSpacer();
	//m_auiToolBar1->AddSpacer(FromDIP(10));
	auto ss = m_auiToolBar1->AddTool(ID_SHOW_SUPPLIER, "Invoices", wxArtProvider::GetBitmap(wxT("inventory"), wxART_OTHER, FromDIP(wxSize(16, 16))), "Show suppliers/invoices", wxITEM_CHECK);
	//m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddSpacer(FromDIP(2));
	m_auiToolBar1->AddTool(ID_ADD_PRODUCT, wxT("Add product"), wxArtProvider::GetBitmap("add", wxART_OTHER, FromDIP(wxSize(16,16))), "Add a new Product");
	m_auiToolBar1->AddSpacer(FromDIP(2));
	m_auiToolBar1->AddTool(ID_ADD_CATEGORY, wxT("Add category"), wxArtProvider::GetBitmap("shopping_bag", wxART_OTHER, FromDIP(wxSize(16,16))), wxT("Creates a new Category for medical products"));
	m_auiToolBar1->AddSpacer(FromDIP(2));
	m_auiToolBar1->AddTool(ID_PACKS, wxT("Add pack"), wxArtProvider::GetBitmap("add", wxART_OTHER, FromDIP(wxSize(16, 16))), wxT("Pharamcy Packs"));
	m_auiToolBar1->Realize();

	m_mgr.AddPane(m_auiToolBar1, wxAuiPaneInfo().Name("ProductToolBar").ToolbarPane().Top().MinSize(FromDIP(wxSize(-1,30))).Resizable().ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));

	//tool bar 2
	m_auiToolBar2 = new wxAuiToolBar(this, ID_TOOLBAR2, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	//m_auiToolBar2->SetToolBitmapSize(wxSize(FromDIP(16), FromDIP(16)));
	mSelectItem = m_auiToolBar2->AddTool(ID_SELECT_MULTIPLE, wxT("Select"), wxArtProvider::GetBitmap("select_check", wxART_OTHER, FromDIP(wxSize(16,16))), "Select multiple products", wxITEM_CHECK);
	m_auiToolBar2->AddSpacer(FromDIP(2));
	
	//m_auiToolBar2->AddStretchSpacer();
	m_auiToolBar2->AddSeparator();
	mOutOfStockItem = m_auiToolBar2->AddTool(ID_OUT_OF_STOCK, wxT("Out of stock"), wxArtProvider::GetBitmap("shopping_cart_off", wxART_OTHER, FromDIP(wxSize(16,16))), wxT("Shows the list of products that are out of stock"), wxITEM_CHECK);
	m_auiToolBar2->AddSpacer(FromDIP(2));
	mExpireProductItem = m_auiToolBar2->AddTool(ID_PRODUCT_EXPIRE, wxT("Expired"), wxArtProvider::GetBitmap("acute", wxART_OTHER, FromDIP(wxSize(16,16))), wxT("List of Products that are expired, or expired alerted"));

	m_auiToolBar2->AddSpacer(FromDIP(2));
	auto mOrderListItem = m_auiToolBar2->AddTool(ID_ORDER_LIST, wxT("Order list"), wxArtProvider::GetBitmap("edit_note", wxART_OTHER, FromDIP(wxSize(16,16))), wxT("Products that are to be ordered"), wxITEM_NORMAL);
	m_auiToolBar2->AddSpacer(FromDIP(2));
	m_auiToolBar2->AddTool(ID_TRANSFERS, wxT("Transfer list"), wxArtProvider::GetBitmap("edit_note", wxART_OTHER, FromDIP(wxSize(16, 16))), wxT("Products that are transfered from this pharmacy"), wxITEM_NORMAL);
	//m_auiToolBar2->AddSpacer(FromDIP(2));
	//m_auiToolBar2->AddControl(new wxTextCtrl(m_auiToolBar2, wxID_ANY));

	m_auiToolBar2->AddSpacer(FromDIP(2));
	m_auiToolBar2->AddTool(ID_SHOW_PRODUCT, wxT("Hidden products"), wxArtProvider::GetBitmap("block", wxART_OTHER, FromDIP(wxSize(16, 16))), wxT("Show hidden products"));
	
	m_auiToolBar2->AddStretchSpacer();
	mCatTextCtrl = new wxStaticText(m_auiToolBar2, wxID_ANY, wxEmptyString);
	mCatTextCtrl->SetBackgroundColour(*wxWHITE);
	mCatNameItem = m_auiToolBar2->AddControl(mCatTextCtrl, wxEmptyString);

	m_auiToolBar2->Realize();
	m_mgr.AddPane(m_auiToolBar2, wxAuiPaneInfo().Name("ProductToolBar2").ToolbarPane().Top().MinSize(FromDIP(wxSize(-1, 30))).Resizable().DockFixed().Row(2).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
}

void pof::ProductView::CreateProductInfo()
{
	mProductinfo = new pof::ProductInfo(this, ID_PRODUCTINFO);
	mProductinfo->AttachBackSlot([&]() { SwapCenterPane(false); });
	mProductinfo->AttachPropertyUpdateSlot(std::bind_front(&pof::ProductView::OnProductInfoUpdated, this));
	mProductinfo->mStockRemvSig.connect(std::bind_front(&pof::ProductView::OnProductStockRemoved, this));

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

	pof::DataModel::SpeicalColHandler_t spl;
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	spl.first = [&](size_t row, size_t col) -> wxVariant {
		auto& st = boost::variant2::get<pof::base::data::text_t>((datastore)[row].first[pof::ProductManager::PRODUCT_STRENGTH]);
		auto& stt = boost::variant2::get<pof::base::data::text_t>((datastore)[row].first[pof::ProductManager::PRODUCT_STRENGTH_TYPE]);
		if (stt == "NOT SPECIFIED") return ""s;

		auto string = fmt::format("{}{}", st, stt);
		return string;
	};

	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(model != nullptr);
	model->SetSpecialColumnHandler(SELECTION_MODEL_COL, std::move(SelectionCol));
	model->SetSpecialColumnHandler(11111, std::move(spl));
}

void pof::ProductView::CreateNoResultPane()
{
	mNoResult = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
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
	mNoResultText->SetFont(wxFontInfo(12));
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

	m_mgr.AddPane(mNoResult, wxAuiPaneInfo().Name("NoResult").CenterPane().CaptionVisible(false).Hide());
}

void pof::ProductView::CreateSupplierView()
{
	mSupplierView = new pof::SupplierView(this);
	mSupplierView->LoadSuppliers();
	m_mgr.AddPane(mSupplierView, wxAuiPaneInfo().Name("Supplier").CenterPane().Hide());
	wxGetApp().mfontSignal.connect(std::bind_front(&pof::SupplierView::OnChangeFont, mSupplierView));
}

void pof::ProductView::Style()
{
	wxColour col = wxTheColourDatabase->Find("Navajo_white");
	//m_dataViewCtrl1->SetAlternateRowColour(wxColour(col.Red(),col.Green(), col.Blue(), 50));
}

void pof::ProductView::SwapCenterPane(bool IsInventoryView)
{
	auto& Pane = m_mgr.GetPane("ProductInfo");
	auto& ProductPane = m_mgr.GetPane("DataView");
	auto& ProductToolBarPane = m_mgr.GetPane("ProductToolBar");
	auto& ProductToolBarPane2 = m_mgr.GetPane("ProductToolBar2");
	if (!Pane.IsOk() || !ProductPane.IsOk() || !ProductToolBarPane.IsOk() || !ProductToolBarPane2.IsOk()) return;
	Pane.Show(IsInventoryView);
	ProductPane.Show(!IsInventoryView);
	ProductToolBarPane.Show(!IsInventoryView);
	ProductToolBarPane2.Show(!IsInventoryView);
	m_mgr.Update();

}

void pof::ProductView::CreateCategoryMenu(wxMenu* menu)
{
	if (!menu) return;
	auto& categories = wxGetApp().mProductManager.GetCategories();
	for (auto& cat : categories) {
		auto& name = boost::variant2::get<pof::base::data::text_t>(cat.first[pof::ProductManager::CATEGORY_NAME]);
		if (name == mActiveCategory) continue;
		size_t range = boost::variant2::get<std::uint64_t>(cat.first[pof::ProductManager::CATEGORY_ID]);
		menu->Append(range, name, nullptr);
		menu->Bind(wxEVT_MENU, std::bind_front(&pof::ProductView::OnAddItemsToCategory, this),range);
	}
}

void pof::ProductView::DoBroughtForward()
{
	if (!wxGetApp().bAutomaticBroughtForward) return;
	auto today = pof::base::data::clock_t::now();
	if (wxGetApp().mProductManager.CheckAction(pof::ProductManager::BROUGHT_FORWARD, today)) return;

	//we have not done brought forward this month
	wxBusyCursor cursor;
	wxGetApp().mProductManager.InventoryBroughtForward();
	wxGetApp().mProductManager.AddAction(pof::ProductManager::BROUGHT_FORWARD);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Automatic Brought Forward");
}

void pof::ProductView::RemoveCheckedState(wxAuiToolBarItem* item)
{
	if (!item) return;
	m_auiToolBar1->Freeze();
	std::bitset<32> bitset(item->GetState());
	if (bitset.test(5)) bitset.flip(5);
	item->SetState(bitset.to_ulong());
	m_auiToolBar1->Thaw();
	m_auiToolBar1->Refresh();
	
}

void pof::ProductView::CheckIfStockCheckIsComplete()
{
	auto today = pof::base::data::clock_t::now();
	auto month = date::floor<date::months>(today);
	auto nextMonth = month + date::months(1);
	constexpr std::chrono::milliseconds ms =
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(60));
	//should be 5 days before the next month
	if ((month + date::days(5)) >= nextMonth) {
		if (!wxGetApp().mProductManager.IsStockCheckComplete()) {
			if (!mStockCheckTimer.IsRunning()) {
				mStockCheckTimer.Start(ms.count());
			}
		}else{
			mStockCheckTimer.Stop();
		}
	}
}

void pof::ProductView::CheckEmpty()
{
	auto& e = m_mgr.GetPane("Empty");
	auto& d = m_mgr.GetPane("DataView");
	auto& pane = m_mgr.GetPane("Supplier");
	if (pane.IsShown()) return;

	if (!d.IsOk() || !e.IsOk()) return;

	if (wxGetApp().mProductManager.GetProductData()->GetDatastore().empty()){
		e.Show();
		d.Hide();
	}
	else {
		e.Hide();
		d.Show();
	}
	m_mgr.Update();
}

void pof::ProductView::ShowNoResult(const std::string& search)
{
	mNoResult->Freeze();
	mNoResultText->SetLabelText(fmt::format("No product \"{}\" in store", search));
	mNoResult->Layout();
	mNoResult->Thaw();

	auto& pane = m_mgr.GetPane("NoResult");
	auto& d = m_mgr.GetPane("DataView");
	if (!pane.IsOk() || !d.IsOk()) return;
	if (d.IsShown()) d.Hide();
	pane.Show();
	m_mgr.Update();
}

