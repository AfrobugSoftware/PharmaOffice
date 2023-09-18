#include "ProductView.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::ProductView, wxPanel)
EVT_SIZE(pof::ProductView::OnResize)
EVT_DATAVIEW_ITEM_ACTIVATED(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnProductActivated)
EVT_DATAVIEW_ITEM_BEGIN_DRAG(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnBeginDrag)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnContextMenu)
EVT_DATAVIEW_COLUMN_HEADER_CLICK(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnHeaderClicked)
//TOOLS
EVT_TOOL(pof::ProductView::ID_ADD_PRODUCT, pof::ProductView::OnAddProduct)
EVT_TOOL(pof::ProductView::ID_ADD_CATEGORY, pof::ProductView::OnAddCategory)
EVT_TOOL(pof::ProductView::ID_PRODUCT_EXPIRE, pof::ProductView::OnExpiredProducts)
EVT_TOOL(pof::ProductView::ID_SELECT_MULTIPLE, pof::ProductView::OnSelection)
EVT_TOOL(pof::ProductView::ID_OUT_OF_STOCK, pof::ProductView::OnOutOfStock)
EVT_TOOL(pof::ProductView::ID_PACKS, pof::ProductView::OnPacks)
EVT_TOOL(pof::ProductView::ID_ORDER_LIST, pof::ProductView::OnShowOrderList)
EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::ProductView::ID_REPORTS, pof::ProductView::OnReportDropdown)
EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::ProductView::ID_FUNCTIONS, pof::ProductView::OnFunctions)


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
EVT_MENU(pof::ProductView::ID_REMOVE_FROM_CATEGORY, pof::ProductView::OnRemoveFromCategory)
EVT_MENU(pof::ProductView::ID_FUNCTION_BROUGHT_FORWARD, pof::ProductView::OnBFFunction)
EVT_MENU(pof::ProductView::ID_SHOW_COST_PRICE, pof::ProductView::OnShowCostPrice)
EVT_MENU(pof::ProductView::ID_PRODUCT_MARKUP, pof::ProductView::OnMarkUp)
EVT_MENU(pof::ProductView::ID_FUNCTION_STOCK_CHECK, pof::ProductView::OnSCFunction)
EVT_MENU(pof::ProductView::ID_FUNCTION_MARK_UP_PRODUCTS, pof::ProductView::OnMarkUpProducts)
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

	//load base data
	wxGetApp().mProductManager.LoadProductsFromDatabase();
	wxGetApp().mProductManager.LoadCategories();


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

void pof::ProductView::ReloadProductView()
{
	mActiveCategory.clear();
	m_searchCtrl1->SetDescriptiveText("Search for products");
	wxGetApp().mProductManager.GetProductData()->Reload();
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

void pof::ProductView::OnHeaderClicked(wxDataViewEvent& evt)
{
	if (mSelectionCol && mSelectionCol == evt.GetDataViewColumn()) {
		static bool sel = true; //
		m_dataViewCtrl1->Freeze();
		auto& items = wxGetApp().mProductManager.GetProductData()->GetDataViewItems();
		if (sel) {
			if (!mSelections.empty()) {
				mSelections.clear();
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

void pof::ProductView::OnExpiredProducts(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		
		RemoveCheckedState(mOutOfStockItem);
		mActiveCategory.clear();
		m_searchCtrl1->SetDescriptiveText("Search for product");


		auto items = wxGetApp().mProductManager.DoExpiredProducts();
		if (!items.has_value()) {
			return;
		}
		if (items->empty()) {
			wxMessageBox("There are no expired products in the store", "PRODUCTS", wxICON_INFORMATION | wxOK);
		}
		wxGetApp().mProductManager.GetProductData()->Reload(*items);
	}
	else {
		wxGetApp().mProductManager.GetProductData()->Reload();
	}
}

void pof::ProductView::OnAddProduct(wxCommandEvent& evt)
{
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
		mInfoBar->ShowMessage("Product Added Sucessfully", wxICON_INFORMATION);
	}
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

}

void pof::ProductView::OnSearchFlag(wxCommandEvent& evt)
{
}

void pof::ProductView::OnContextMenu(wxDataViewEvent& evt)
{
	wxMenu* menu = new wxMenu;
	auto orderlist = menu->Append(ID_ADD_ORDER_LIST, "Add Order List", nullptr);
	auto remv = menu->Append(ID_REMOVE_PRODUCT, "Remove Product", nullptr);
	wxMenu* catSub = new wxMenu;
	CreateCategoryMenu(catSub);
	auto cat = menu->Append(wxID_ANY, "Add to Category", catSub);
	if (!mActiveCategory.empty()) {
		auto remvcat = menu->Append(ID_REMOVE_FROM_CATEGORY, fmt::format("Remove from \'{}\'", mActiveCategory), nullptr);
		remvcat->SetBitmap(wxArtProvider::GetBitmap("action_remove"));
	}
	auto inven = menu->Append(ID_ADD_INVENTORY, "Add Inventory", nullptr);
	menu->AppendSeparator();
	auto markup = menu->Append(ID_PRODUCT_MARKUP, "Mark Up product", nullptr);

	/*orderlist->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY));
	remv->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE));
	cat->SetBitmap(wxArtProvider::GetBitmap("folder_files"));
	inven->SetBitmap(wxArtProvider::GetBitmap("file"));*/

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
	}
}

void pof::ProductView::OnSearchProduct(wxCommandEvent& evt)
{
	RemoveCheckedState(mOutOfStockItem);
	RemoveCheckedState(mExpireProductItem);

	pof::DataModel* datam = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	assert(datam != nullptr);
	m_dataViewCtrl1->Freeze();
	std::string search = evt.GetString().ToStdString();
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
		if (!mActiveCategory.empty()) {
			datam->StringSearchAndReloadSet(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}
		else {
			datam->StringSearchAndReload(pof::ProductManager::PRODUCT_NAME, std::move(search));
		}
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
	if (evt.IsChecked()){
		//disable expire product Item
		//clear all the states
		RemoveCheckedState(mExpireProductItem);
		mActiveCategory.clear();
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
			return;
		}
		pd->Reload(std::move(items));
	}
	else {
		pd->Reload();
	}
}

void pof::ProductView::OnAddInventory(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& pd = wxGetApp().mProductManager.GetProductData();

	pof::InventoryDialog dialog(nullptr);
	if (dialog.ShowModal() == wxID_OK) {
		auto& Inven = dialog.GetData();
		Inven.first[pof::ProductManager::INVENTORY_MANUFACTURER_NAME] = "D-GLOPA PHARMACEUTICALS";
		Inven.first[pof::ProductManager::INVENTORY_PRODUCT_UUID] = pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID];

		pof::ProductInfo::PropertyUpdate mPropertyUpdate;
		mPropertyUpdate.mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
			mPropertyUpdate.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] = pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID];

		
		mPropertyUpdate.mUpdatedElememts.set(pof::ProductManager::PRODUCT_STOCK_COUNT);
		mPropertyUpdate.mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_STOCK_COUNT] =
			boost::variant2::get<std::uint64_t>(Inven.first[pof::ProductManager::INVENTORY_STOCK_COUNT])
			+ boost::variant2::get<std::uint64_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_STOCK_COUNT]);

		mProductinfo->SignalUpdate(mPropertyUpdate);
		wxGetApp().mProductManager.GetInventory()->StoreData(std::move(Inven));

		auto name = boost::variant2::get<pof::base::data::text_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_NAME]);
		mInfoBar->ShowMessage(fmt::format("{} Inventory was updated successfully!", name), wxICON_INFORMATION);
	}
}

void pof::ProductView::OnReportDropdown(wxAuiToolBarEvent& evt)
{
	if (evt.IsDropDownClicked()) {
		wxMenu* menu = new wxMenu;
		menu->Append(ID_REPORTS_ENDOFDAY, "End Of Day", nullptr);
		menu->Append(ID_REPORTS_CONSUMPTION_PATTERN, "Consumption pattern", nullptr);


		m_auiToolBar1->PopupMenu(menu);
	}
}


void pof::ProductView::OnConsumptionPattern(wxCommandEvent& evt)
{
	pof::ReportsDialog dialog(this, wxID_ANY, wxEmptyString);
	if(dialog.LoadReport(pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN)) dialog.ShowModal();
}

void pof::ProductView::OnEndOfDayReport(wxCommandEvent& evt)
{
	pof::ReportsDialog dialog(this, wxID_ANY, wxEmptyString);
	if (dialog.LoadReport(pof::ReportsDialog::ReportType::EOD)) dialog.ShowModal();
}

void pof::ProductView::OnPacks(wxCommandEvent& evt)
{
	pof::PackView pks(nullptr);
	if (pks.ShowModal() == wxID_OK) {
		
	}
}

void pof::ProductView::OnFunctions(wxAuiToolBarEvent& evt)
{
	if (evt.IsDropDownClicked())
	{
		wxMenu* menu = new wxMenu;
		auto bf = menu->Append(ID_FUNCTION_BROUGHT_FORWARD, "Brougt Forward", nullptr);
		auto scf = menu->Append(ID_FUNCTION_STOCK_CHECK, "Stock Check", nullptr);
		auto mark = menu->Append(ID_FUNCTION_MARK_UP_PRODUCTS, "Markup Products", nullptr);
		auto scp = menu->AppendCheckItem(ID_SHOW_COST_PRICE, "Show cost price",wxT("Show the cost price for each product"));

	
		m_auiToolBar1->PopupMenu(menu);
	}
}

void pof::ProductView::OnBFFunction(wxCommandEvent& evt)
{
	if (!wxGetApp().mProductManager.IsStockCheckComplete()) {
		wxMessageBox("Stock check is not complete for this month, complete stock check before brought forward", "BROUGHT FORWARD", wxICON_INFORMATION | wxOK);
		return;
	}
	wxBusyCursor cursor;
	wxGetApp().mProductManager.InventoryBroughtForward();
}

void pof::ProductView::OnSCFunction(wxCommandEvent& evt)
{
	//we need a stock check dialog
	pof::StockCheck dialog(this);
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

	size_t idx = pof::DataModel::GetIdxFromItem(item);

	//the actual markup should come from the settings of the pharmaoffice
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto& row = datastore[idx];
	auto& v = row.first;
	auto& uid = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]);
	wxGetApp().mProductManager.MarkUpProducts(uid, 0.3); //30% mark up for texts
	m_dataViewCtrl1->Freeze();
	wxGetApp().mProductManager.RefreshRowFromDatabase(uid, row);
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Update();


	//FIX THE OVERWRITE
	row.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
	wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);

}

void pof::ProductView::OnMarkUpProducts(wxCommandEvent& evt)
{
	wxGetApp().mProductManager.MarkUpProducts(0.3);
	
	//refresh the display
	m_dataViewCtrl1->Freeze();
	wxGetApp().mProductManager.GetProductData()->Clear();
	wxGetApp().mProductManager.LoadProductsFromDatabase();
	wxGetApp().mProductManager.GetProductData()->Reload();
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Refresh();
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
			Iter->first[i] = mUpdatedElem.mUpdatedElementsValues.first[i];
			Iter->second.second.set(i); //set the column as modified
		}
	}



	int idx = std::distance(DataStore.begin(), Iter);
	wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
	DataStore.set_state(idx,pof::base::data::state::MODIFIED);
	
	std::string& name = boost::variant2::get<pof::base::data::text_t>(Iter->first[pof::ProductManager::PRODUCT_NAME]);
	mInfoBar->ShowMessage(fmt::format("{} is updated sucessfully", name), wxICON_INFORMATION);

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
		wxGetApp().mProductManager.GetProductData()->Reload(std::move(items));
		mActiveCategory = name;
		m_searchCtrl1->SetDescriptiveText(fmt::format("Search for products in {}", name));
	}
	else {
		if (!mActiveCategory.empty()) {
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

}

void pof::ProductView::CreateDataView()
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	mInfoBar = new wxInfoBar(panel, wxID_ANY);
	

	m_dataViewCtrl1 = new wxDataViewCtrl(panel, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES);
	auto& pm = wxGetApp().mProductManager;
	m_dataViewCtrl1->AssociateModel(pm.GetProductData().get());
	pm.GetProductData()->DecRef();
	
	mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Serial #"), pof::ProductManager::PRODUCT_SERIAL_NUM, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductClass = m_dataViewCtrl1->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductUnitPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("Stock Count"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockLevel = m_dataViewCtrl1->AppendTextColumn(wxT("Unit Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);

	sizer->Add(mInfoBar, wxSizerFlags().Expand().Border(wxALL, 2));
	sizer->Add(m_dataViewCtrl1, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 2));

	panel->SetSizer(sizer);
	panel->Layout();
	sizer->Fit(panel);
	//SetupDataViewStyle();
	m_mgr.AddPane(panel, wxAuiPaneInfo().Name("DataView").CenterPane().CaptionVisible(false));
	
}

void pof::ProductView::CreateToolBar()
{
	m_auiToolBar1 = new wxAuiToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_NO_AUTORESIZE | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	m_auiToolBar1->SetToolBitmapSize(wxSize(16,16));

	mReportItem = m_auiToolBar1->AddTool(ID_REPORTS, wxT("Reports"), wxArtProvider::GetBitmap("file"), wxT("Store reports"));
	mReportItem->SetHasDropDown(true);
	auto mFuncDropItem = m_auiToolBar1->AddTool(ID_FUNCTIONS, wxT("Functions"), wxArtProvider::GetBitmap("file"), wxT("Run a function on all products in the store"));
	mFuncDropItem->SetHasDropDown(true);

	m_auiToolBar1->AddStretchSpacer();
	m_auiToolBar1->AddSeparator();

	m_searchCtrl1 = new wxSearchCtrl(m_auiToolBar1, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(400, -1), wxWANTS_CHARS);
#ifndef __WXMAC__
	m_searchCtrl1->ShowSearchButton(true);
#endif
	m_searchCtrl1->ShowCancelButton(true);
	m_searchCtrl1->SetDescriptiveText("Search for products");
	wxMenu* menu = new wxMenu;
	menu->AppendRadioItem(ID_SEARCH_BY_NAME, "Search by name");
	menu->AppendRadioItem(ID_SEARCH_BY_CATEGORY, "Search by category");
	menu->Check(ID_SEARCH_BY_NAME, true);

	m_searchCtrl1->SetMenu(menu);
	m_auiToolBar1->AddControl(m_searchCtrl1);

	m_auiToolBar1->AddStretchSpacer();
	m_auiToolBar1->AddTool(ID_SELECT_MULTIPLE, wxT("Select"), wxArtProvider::GetBitmap("action_check"), "Select multiple products", wxITEM_CHECK);
	m_auiToolBar1->AddTool(ID_ADD_PRODUCT, wxT("Add Product"), wxArtProvider::GetBitmap("action_add"), "Add a new Product");
	m_auiToolBar1->AddTool(ID_ADD_CATEGORY, wxT("Add Category"), wxArtProvider::GetBitmap("application"), wxT("Creates a new Category for medical products"));

	mOutOfStockItem = m_auiToolBar1->AddTool(ID_OUT_OF_STOCK, wxT("Out Of Stock"), wxArtProvider::GetBitmap("folder_open"), wxT("Shows the list of products that are out of stock"), wxITEM_CHECK);
	mExpireProductItem = m_auiToolBar1->AddTool(ID_PRODUCT_EXPIRE, wxT("Expired Products"), wxArtProvider::GetBitmap("time"), wxT("List of Products that are expired, or expired alerted"), wxITEM_CHECK);
	auto mOrderListItem = m_auiToolBar1->AddTool(ID_ORDER_LIST, wxT("Order List"), wxArtProvider::GetBitmap("time"), wxT("Products that are to be ordered"), wxITEM_NORMAL);
	m_auiToolBar1->AddTool(ID_PACKS, wxT("Pharm Packs"), wxArtProvider::GetBitmap(wxART_FOLDER));
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
	//m_dataViewCtrl1->SetAlternateRowColour(wxTheColourDatabase->Find("Navajo_white"));
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

void pof::ProductView::CreateCategoryMenu(wxMenu* menu)
{
	if (!menu) return;
	auto& categories = wxGetApp().mProductManager.GetCategories();
	for (auto& cat : categories) {
		auto& name = boost::variant2::get<pof::base::data::text_t>(cat.first[pof::ProductManager::CATEGORY_NAME]);
		size_t range = boost::variant2::get<std::uint64_t>(cat.first[pof::ProductManager::CATEGORY_ID]);
		menu->Append(range, name, nullptr);
		menu->Bind(wxEVT_MENU, std::bind_front(&pof::ProductView::OnAddItemsToCategory, this),range);
	}
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

