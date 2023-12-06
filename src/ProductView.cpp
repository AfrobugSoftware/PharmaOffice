#include "ProductView.h"
//#include "Application.h"
#include "PofPch.h"

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
	EVT_MENU(pof::ProductView::ID_MOVE_PRODUCT_STOCK, pof::ProductView::OnMoveExpiredStock)
	EVT_MENU(pof::ProductView::ID_DOWNLOAD_EXCEL, pof::ProductView::OnDownloadExcel)
	EVT_MENU(pof::ProductView::ID_CREATE_CONTROLLED_BOOK, pof::ProductView::OnCreateControlBook)
	EVT_MENU(pof::ProductView::ID_ADD_VARIANT, pof::ProductView::OnAddVariant)

	//TIMER
	EVT_TIMER(pof::ProductView::ID_STOCK_CHECK_TIMER, pof::ProductView::OnStockCheckTimer)
	//UI update
	EVT_UPDATE_UI(pof::ProductView::ID_DATA_VIEW, pof::ProductView::OnUpdateUI)
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

pof::ProductView::ProductView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) 
	: wxPanel( parent, id, pos, size, style ), mStockCheckTimer(this, ID_STOCK_CHECK_TIMER)
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

	DoBroughtForward();
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
			RemoveCheckedState(mExpireProductItem);
			return;
		}
		if (items->empty()) {
			wxMessageBox("There are no expired products in the store", "PRODUCTS", wxICON_INFORMATION | wxOK);
			RemoveCheckedState(mExpireProductItem);
			return;
		}
		wxGetApp().mProductManager.GetProductData()->Reload(*items);
	}
	else {
		wxGetApp().mProductManager.GetProductData()->Reload();
	}
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
		mInfoBar->ShowMessage("Product Added Sucessfully", wxICON_INFORMATION);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, "Created A product");
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
}

void pof::ProductView::OnContextMenu(wxDataViewEvent& evt)
{
	if (!m_dataViewCtrl1->GetSelection().IsOk()) return;
	wxMenu* menu = new wxMenu;
	auto orderlist = menu->Append(ID_ADD_ORDER_LIST, "Add order list", nullptr);
	wxMenu* catSub = new wxMenu;
	CreateCategoryMenu(catSub);
	if (!mActiveCategory.empty()) {
		auto cat = menu->Append(wxID_ANY, "Move to category", catSub);
		auto remvcat = menu->Append(ID_REMOVE_FROM_CATEGORY, fmt::format("Remove from \'{}\'", mActiveCategory), nullptr);
		remvcat->SetBitmap(wxArtProvider::GetBitmap("action_remove"));
	}
	else {
		auto cat = menu->Append(wxID_ANY, "Add to category", catSub);
	}
	auto inven = menu->Append(ID_ADD_INVENTORY, "Add stock", nullptr);
	auto var = menu->Append(ID_ADD_VARIANT, "Add variant", nullptr);
	menu->AppendSeparator();
	auto markup = menu->Append(ID_PRODUCT_MARKUP, "Mark-up product", nullptr);

	menu->AppendSeparator();
	if (mSelections.empty()) {
		auto remv = menu->Append(ID_REMOVE_PRODUCT, "Remove product from store", nullptr);
		auto moveEx = menu->Append(ID_MOVE_PRODUCT_STOCK, "Clear stock as expired", nullptr);
	}
	else {
		auto remv = menu->Append(ID_REMOVE_PRODUCT, fmt::format("Remove {:d} products from store", mSelections.size()), nullptr);
		auto moveEx = menu->Append(ID_MOVE_PRODUCT_STOCK, fmt::format("Clear {:d} products stocks as expired", mSelections.size()), nullptr);
	}
	auto crb = menu->Append(ID_CREATE_CONTROLLED_BOOK, "Create Controlled Register", nullptr);
	/*orderlist->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY));
	remv->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE));
	cat->SetBitmap(wxArtProvider::GetBitmap("folder_files"));
	inven->SetBitmap(wxArtProvider::GetBitmap("file"));*/

	PopupMenu(menu);
}

void pof::ProductView::OnRemoveProduct(wxCommandEvent& evt)
{
	//check privilage
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Remove product", wxICON_INFORMATION | wxOK);
		return;
	}
	if (wxMessageBox("Deleteing a product deletes all the data associated with the product, Do you wish to continue?", "REMOVE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO) return;
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

		stop = dlg.Update(65, "Removing product from expired");
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

		dlg.Update(100, "Finishing...");
		m_dataViewCtrl1->Freeze();
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

	//check if product has expired inventory
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST) && !wxGetApp().bAllowOtherUsersInventoryPermission) {
		wxMessageBox("User accoount cannot add inventory to stock", "Add Inventory", wxICON_INFORMATION | wxOK);
		return;
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


	pof::InventoryDialog dialog(nullptr);
	dialog.mProductUuid = boost::variant2::get<pof::base::data::duuid_t>(pd->GetDatastore()[idx].first[pof::ProductManager::PRODUCT_UUID]);
	if (dialog.ShowModal() == wxID_OK) {
		auto& Inven = dialog.GetData();
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
	/*if(!wxGetApp().mProductManager.IsStockCheckComplete())
	{
		wxMessageBox("Please complete stock check before generating consumption pattern for this month", "Reports", wxICON_INFORMATION | wxOK);
	}*/
	pof::ReportsDialog dialog(this, wxID_ANY, wxEmptyString);
	if(dialog.LoadReport(pof::ReportsDialog::ReportType::COMSUMPTION_PATTARN, pof::base::data::clock_t::now())) dialog.ShowModal();
}

void pof::ProductView::OnEndOfDayReport(wxCommandEvent& evt)
{
	pof::ReportsDialog dialog(this, wxID_ANY, wxEmptyString);
	if (dialog.LoadReport(pof::ReportsDialog::ReportType::EOD, pof::base::data::clock_t::now())) dialog.ShowModal();
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

		if (!mActiveCategory.empty()) {
			wxMenu* catMenu = new wxMenu;
			auto ctp = menu->Append(wxID_ANY, "Category functions", catMenu);
		}
		auto dexl = menu->Append(ID_DOWNLOAD_EXCEL, "Download as excel", nullptr);
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
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::INFORMATION, "Brought Forward");
}

void pof::ProductView::OnSCFunction(wxCommandEvent& evt)
{
	//we need a stock check dialog
	pof::StockCheck dialog(this);
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

	size_t idx = pof::DataModel::GetIdxFromItem(item);

	//the actual markup should come from the settings of the pharmaoffice
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto& row = datastore[idx];
	auto& v = row.first;
	auto& uid = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]);
	wxGetApp().mProductManager.MarkUpProducts(uid); //30% mark up for texts
	m_dataViewCtrl1->Freeze();
	wxGetApp().mProductManager.RefreshRowFromDatabase(uid, row);
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Update();


	//FIX THE OVERWRITE
	row.second.second.set(pof::ProductManager::PRODUCT_UNIT_PRICE);
	wxGetApp().mProductManager.GetProductData()->Signal(pof::DataModel::Signals::UPDATE, idx);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Marked up {}",
		boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])));

}

void pof::ProductView::OnMarkUpProducts(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Mark up", wxICON_INFORMATION | wxOK);
		return;
	}
	if (wxMessageBox("Mark up changes the price for all products in the store, are you sure you want to continue?" , "Mark up",
		wxICON_WARNING | wxYES_NO) == wxNO) return;

	wxGetApp().mProductManager.MarkUpProducts();
	
	//refresh the display
	m_dataViewCtrl1->Freeze();
	wxGetApp().mProductManager.GetProductData()->Clear();
	wxGetApp().mProductManager.LoadProductsFromDatabase();
	wxGetApp().mProductManager.GetProductData()->Reload();
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Refresh();
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
		if (stockCount == 0) return; //cannot move empty stock

		wxGetApp().mProductManager.MoveStockToExpire(pid, stockCount);
		//zero out the stock that was moved
		std::uint64_t temp = stockCount;
		m_dataViewCtrl1->Freeze();
		stockCount = 0;

		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();
		wxGetApp().mProductManager.UpdatePD(std::make_tuple(pid, stockCount), { "uuid", "stock_count" });
		mInfoBar->ShowMessage(fmt::format("{} was moved to expired successfully", name), wxICON_INFORMATION);
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
	if (wxGetApp().bCheckExpiredOnUpdate){
		auto now = std::chrono::system_clock::now();
		if (now >= mExpireProductWatchDog + mWatchDogDuration){
			auto items = wxGetApp().mProductManager.DoExpiredProducts();
			if (!items.has_value()) { }
			else if (!items->empty()){
				mInfoBar->ShowMessage(fmt::format("{:d} products in store has expired", items->size()), wxICON_WARNING);
				mInfoBar->Refresh();
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
				mInfoBar->ShowMessage(fmt::format("{:d} products in store are out of stock", items->size()), wxICON_WARNING);
				mInfoBar->Refresh();
			}
		}
		mOutOfStockProductWatchDog = now;
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
	excel::XLDocument doc;
	doc.create(fullPath.string());
	if (!doc.isOpen()) {
		spdlog::error("Canont open xlsx file");
		return;
	}

	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	auto wks = doc.workbook().worksheet("Sheet1");
	wks.setName("Products");
	const size_t colSize = m_dataViewCtrl1->GetColumnCount();
	const size_t rowSize = datastore.size();
	const size_t firstRow = 1;
	const size_t firstCol = 1;

	auto range = wks.range(excel::XLCellReference(firstRow, firstCol), excel::XLCellReference(rowSize, colSize));
	auto iter = range.begin();
	//write header
	auto writeHeader = [&](const std::string& name) {
		iter->value().set(name);
		iter++;
	};
	wxBusyCursor cursor;
	writeHeader("Serial Number");
	writeHeader("Product Name");
	writeHeader("Package Size");
	writeHeader("Stock Count");
	writeHeader("Unit Price");
	if(mProductCostPriceCol != nullptr)
			writeHeader("Cost Price");

	auto& metadata = datastore.get_metadata();
	constexpr const std::array<int, 6> colIdx = {
		pof::ProductManager::PRODUCT_SERIAL_NUM,
		pof::ProductManager::PRODUCT_NAME,
		pof::ProductManager::PRODUCT_PACKAGE_SIZE,
		pof::ProductManager::PRODUCT_STOCK_COUNT,
		pof::ProductManager::PRODUCT_UNIT_PRICE,
		pof::ProductManager::PRODUCT_COST_PRICE
	};
	size_t col = 0;
	size_t i = 0;
	size_t row = 0;
	for (; iter != range.end(); iter++){
		pof::base::data::kind knd = metadata[colIdx[col]];
		auto& v = iter->value();
		switch (knd)
		{
		case pof::base::data::kind::int32:
			v.set(boost::variant2::get<std::int32_t>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::int64:
			v.set(boost::variant2::get<std::int64_t>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::uint32:
			v.set(boost::variant2::get<std::uint32_t>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::uint64:
			v.set(boost::variant2::get<std::uint64_t>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::float32:
			v.set(boost::variant2::get<float>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::float64:
			v.set(boost::variant2::get<double>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::datetime:
		{
			const std::string tf = fmt::format("{:%d/%m/%y}", boost::variant2::get<pof::base::data::datetime_t>(datastore[row].first[colIdx[col]]));
			v.set(tf);
		}
			break;
		case pof::base::data::kind::text:
			v.set(boost::variant2::get<pof::base::data::text_t>(datastore[row].first[colIdx[col]]));
			break;
		case pof::base::data::kind::blob:
			break;
		case pof::base::data::kind::uuid:
		{
			std::stringstream os;
			os << boost::variant2::get<pof::base::data::duuid_t>(datastore[row].first[colIdx[col]]);
			v.set(os.str());
		}
			break;
		case pof::base::data::kind::currency:
		{
			const std::string tf = fmt::format("{:cu}", boost::variant2::get<pof::base::data::currency_t>(datastore[row].first[colIdx[col]]));
			v.set(tf);
		}
			break;
		case pof::base::data::kind::null:
			break;
		default:
			break;
		}
		col = ++i % colSize;
		row =   i / colSize;
	}


	doc.save();
	doc.close();
	wxMessageBox(fmt::format("Downloaded excel to \n{}", fullPath.string()), "Excel export", wxICON_INFORMATION | wxOK);
}

void pof::ProductView::OnCacheHint(wxDataViewEvent& evt){
	//look into these cache stuff
	////
	wxBusyCursor crs;
	size_t i = evt.GetCacheFrom();
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	if (evt.GetCacheTo() >= datastore.size()) return;
	spdlog::info("Loading cache from {:d} to {:d}", evt.GetCacheFrom(), evt.GetCacheTo());
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
}

void pof::ProductView::OnAddVariant(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;
	size_t idx = pof::DataModel::GetIdxFromItem(item);

	wxDialog dialog;
	

	if (dialog.ShowModal() == wxID_CANCEL) return;
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
	
	std::string& name = boost::variant2::get<pof::base::data::text_t>(Iter->first[pof::ProductManager::PRODUCT_NAME]);
	mInfoBar->ShowMessage(fmt::format("{} is updated sucessfully", name), wxICON_INFORMATION);
	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Updated {} product information", name));
}

//remove this one, use only activated 
void pof::ProductView::OnCategorySelected(const std::string& name)
{
	wxMessageBox(fmt::format("{} is selected", name), "Category");
}

void pof::ProductView::ShowCostPriceColumn()
{
	mProductUnitPriceCol->SetWidth(100);
	mProductCostPriceCol = m_dataViewCtrl1->AppendTextColumn("Cost Price", pof::ProductManager::PRODUCT_COST_PRICE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
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
		m_dataViewCtrl1->Freeze();
		wxGetApp().mProductManager.GetProductData()->Reload(std::move(items));
		mActiveCategory = name;
		m_dataViewCtrl1->Thaw();
		m_dataViewCtrl1->Refresh();
		if (mInfoBar->IsShown()) mInfoBar->Dismiss();
		m_searchCtrl1->SetDescriptiveText(fmt::format("Search for products in {}", name));
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
	mInfoBar->SetAutoLayout(true);

	m_dataViewCtrl1 = new wxDataViewCtrl(panel, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_ROW_LINES | wxDV_HORIZ_RULES);
	auto& pm = wxGetApp().mProductManager;
	m_dataViewCtrl1->AssociateModel(pm.GetProductData().get());
	pm.GetProductData()->DecRef();
	
	//mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Serial #"), pof::ProductManager::PRODUCT_SERIAL_NUM, wxDATAVIEW_CELL_INERT, 50, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("Name"), pof::ProductManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 450, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mSerialNumCol = m_dataViewCtrl1->AppendTextColumn(wxT("Class"), pof::ProductManager::PRODUCT_CLASS, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductFormulation = m_dataViewCtrl1->AppendTextColumn(wxT("Formulation"), pof::ProductManager::PRODUCT_FORMULATION, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductClass = m_dataViewCtrl1->AppendTextColumn(wxT("Package Size"), pof::ProductManager::PRODUCT_PACKAGE_SIZE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockLevel = m_dataViewCtrl1->AppendTextColumn(wxT("Stock Count"), pof::ProductManager::PRODUCT_STOCK_COUNT, wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mProductUnitPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("Unit Price"), pof::ProductManager::PRODUCT_UNIT_PRICE, wxDATAVIEW_CELL_INERT, 70, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);

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

	m_auiToolBar1->AddTool(ID_SELECT_MULTIPLE, wxT("Select"), wxArtProvider::GetBitmap("action_check"), "Select multiple products", wxITEM_CHECK);
	m_auiToolBar1->AddSpacer(2);
	mReportItem = m_auiToolBar1->AddTool(ID_REPORTS, wxT("Reports"), wxArtProvider::GetBitmap("file"), wxT("Store reports"));
	mReportItem->SetHasDropDown(true);
	auto mFuncDropItem = m_auiToolBar1->AddTool(ID_FUNCTIONS, wxT("Functions"), wxArtProvider::GetBitmap("file"), wxT("Run a function on all products in the store"));
	mFuncDropItem->SetHasDropDown(true);
	m_auiToolBar1->AddSeparator();

	m_searchCtrl1 = new wxSearchCtrl(m_auiToolBar1, ID_SEARCH, wxEmptyString, wxDefaultPosition, wxSize(350, -1), wxWANTS_CHARS);
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
	m_auiToolBar1->AddSpacer(5);
	m_auiToolBar1->AddControl(m_searchCtrl1);

	m_auiToolBar1->AddStretchSpacer();
	m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddSpacer(2);
	m_auiToolBar1->AddTool(ID_ADD_PRODUCT, wxEmptyString, wxArtProvider::GetBitmap("action_add"), "Add a new Product");
	m_auiToolBar1->AddSpacer(2);
	m_auiToolBar1->AddTool(ID_ADD_CATEGORY, wxEmptyString, wxArtProvider::GetBitmap("application"), wxT("Creates a new Category for medical products"));
	m_auiToolBar1->AddSpacer(2);

	mOutOfStockItem = m_auiToolBar1->AddTool(ID_OUT_OF_STOCK, wxEmptyString, wxArtProvider::GetBitmap("folder_open"), wxT("Shows the list of products that are out of stock"), wxITEM_CHECK);
	m_auiToolBar1->AddSpacer(2);
	mExpireProductItem = m_auiToolBar1->AddTool(ID_PRODUCT_EXPIRE, wxEmptyString, wxArtProvider::GetBitmap("time"), wxT("List of Products that are expired, or expired alerted"), wxITEM_CHECK);
	m_auiToolBar1->AddSpacer(2);
	auto mOrderListItem = m_auiToolBar1->AddTool(ID_ORDER_LIST, wxEmptyString, wxArtProvider::GetBitmap("file"), wxT("Products that are to be ordered"), wxITEM_NORMAL);
	m_auiToolBar1->AddSpacer(2);
	m_auiToolBar1->AddTool(ID_PACKS, wxEmptyString, wxArtProvider::GetBitmap(wxART_FOLDER, wxART_TOOLBAR, wxSize(16,16)), wxT("Pharamcy Packs"));
	m_auiToolBar1->Realize();

	m_mgr.AddPane(m_auiToolBar1, wxAuiPaneInfo().Name("ProductToolBar").ToolbarPane().Top().MinSize(-1, 30).ToolbarPane().Top().DockFixed().Row(1).LeftDockable(false).RightDockable(false).Floatable(false).BottomDockable(false));
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
	wxColour col = wxTheColourDatabase->Find("Navajo_white");
	//m_dataViewCtrl1->SetAlternateRowColour(wxColour(col.Red(),col.Green(), col.Blue(), 50));
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

