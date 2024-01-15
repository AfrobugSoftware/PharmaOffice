#include "SaleView.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::SaleView, wxPanel)
	EVT_BUTTON(pof::SaleView::ID_CHECKOUT, pof::SaleView::OnCheckout)
	EVT_BUTTON(pof::SaleView::ID_CLEAR, pof::SaleView::OnClear)
	EVT_BUTTON(pof::SaleView::ID_SAVE, pof::SaleView::OnSave)

	EVT_TOOL(pof::SaleView::ID_REMOVE_PRODUCT, pof::SaleView::OnRemoveProduct)
	EVT_TOOL(pof::SaleView::ID_HIDE_PRODUCT_VIEW_PROPERTY, pof::SaleView::OnHideProductViewProperty)
	EVT_TOOL(pof::SaleView::ID_PRINT_LABELS, pof::SaleView::OnPrintAsLabels)
	EVT_TOOL(pof::SaleView::ID_PACKS, pof::SaleView::OnShowPacks)
	EVT_TOOL(pof::SaleView::ID_FORM_M, pof::SaleView::OnFormM)
	EVT_TOOL(pof::SaleView::ID_OPEN_SAVE_SALE, pof::SaleView::OnOpenSaveSale)
	EVT_TOOL(pof::SaleView::ID_DISCOUNT, pof::SaleView::OnDiscount)

	EVT_MENU(pof::SaleView::ID_REPRINT_LAST_SALE, pof::SaleView::OnReprintLastSale)
	EVT_MENU(pof::SaleView::ID_RETURN_LAST_SALE, pof::SaleView::OnReturnLastSale)
	EVT_MENU(pof::SaleView::ID_REMOVE_DISCOUNT, pof::SaleView::OnRemoveDiscount)

	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::SaleView::ID_REPRINT, pof::SaleView::OnReprintSale)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(pof::SaleView::ID_RETURN_SALE, pof::SaleView::OnReturnSale)

	EVT_DATAVIEW_ITEM_BEGIN_DRAG(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnBeginDrag)
	EVT_DATAVIEW_ITEM_DROP_POSSIBLE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDropPossible)
	EVT_DATAVIEW_ITEM_DROP(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDrop)
	EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnEditingStarted)
	EVT_DATAVIEW_ITEM_EDITING_DONE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnEditingDone)
	EVT_DATAVIEW_ITEM_START_EDITING(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnValueChanged)
	EVT_DATAVIEW_SELECTION_CHANGED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnSelected)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnSelected)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnContextMenu)

	EVT_TEXT(pof::SaleView::ID_PRODUCT_SEARCH_NAME, pof::SaleView::OnProductNameSearch)
	EVT_SEARCH(pof::SaleView::ID_PRODUCT_SCAN, pof::SaleView::OnScanBarCode)
	
	EVT_PG_CHANGED(pof::SaleView::ID_PRODUCT_VIEW_PROPERTY, pof::SaleView::OnProductPropertyChanged)
	//EVT_UPDATE_UI(pof::SaleView::ID_ACTIVE_UI_TEXT, pof::SaleView::OnSaleUuidTextUI)
END_EVENT_TABLE()

static wxArrayString SplitIntoArrayString(const std::string& string)
{
	wxArrayString Split;
	auto pos = string.find_first_of(",");
	size_t pos2 = 0;
	if (pos == std::string::npos) {
		if (!string.empty()) Split.push_back(string);
		return Split;
	}

	Split.push_back(std::move(string.substr(0, pos)));
	while ((pos2 = string.find_first_of(",", pos + 1)) != std::string::npos) {
		Split.push_back(std::move(string.substr(pos + 1, pos2 - (pos + 1))));
		pos = pos2;
	}
	Split.push_back(std::move(string.substr(pos + 1, string.size() - (pos + 1))));
	return Split;
}


pof::SaleView::SaleView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	this->SetDoubleBuffered(true);
	mCurSaleuuid = boost::uuids::nil_uuid();
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	mTopTools = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTopTools->SetMinSize(wxSize(-1, 30));

	mBottomTools = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mBottomTools->SetMinSize(wxSize(-1, 30));
	mBottomTools->SetDoubleBuffered(true);

	mProductNameText = new wxStaticText(mTopTools, wxID_ANY, wxT("Product Name: "), wxDefaultPosition, wxDefaultSize, 0);
	mProductNameText->SetBackgroundColour(*wxWHITE);
	mProductNameText->Wrap(-1);
	mTopTools->AddControl(mProductNameText);

	mTopTools->AddSpacer(10);

	mProductNameValue = new wxSearchCtrl(mTopTools, ID_PRODUCT_SEARCH_NAME, wxEmptyString, wxDefaultPosition, wxSize(250, -1));
	//mProductNameValue->SetValidator(wxTextValidator{wxFILTER_SPACE});
	mProductNameValue->SetHint("Search product by name");
	mProductNameValue->ShowCancelButton(true);

	mTopTools->AddControl(mProductNameValue);

	mTopTools->AddSpacer(10);

	mScanProduct = new wxStaticText(mTopTools, wxID_ANY, wxT("Scan Product: "), wxDefaultPosition, wxDefaultSize, 0);
	mScanProduct->SetBackgroundColour(*wxWHITE);
	mScanProduct->Wrap(-1);
	mTopTools->AddControl(mScanProduct);

	mTopTools->AddSpacer(10);

	mScanProductValue = new wxSearchCtrl(mTopTools, ID_PRODUCT_SCAN, wxEmptyString, wxDefaultPosition, wxSize(250, -1), 0);
	mScanProductValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	mScanProductValue->SetHint("Scan products");
	mScanProductValue->ShowCancelButton(true);

	mTopTools->AddControl(mScanProductValue);

	mTopTools->AddStretchSpacer();
	
	mTopTools->AddTool(ID_PACKS, wxT("Rx Packs"), wxArtProvider::GetBitmap(wxART_FOLDER, wxART_TOOLBAR, wxSize(16,16)), "Product packs");
	mTopTools->AddTool(ID_PRINT_LABELS, wxT("Print As Labels"), wxArtProvider::GetBitmap("download"), "Print labels of product sold");
	mTopTools->AddTool(ID_REMOVE_PRODUCT, wxT("Remove Product"), wxArtProvider::GetBitmap("action_remove"), "Remove product from sale list");
	mTopTools->AddTool(ID_HIDE_PRODUCT_VIEW_PROPERTY, wxT("View Product"), wxArtProvider::GetBitmap("pen"), "View details about the product");
	
	mTopTools->Realize();

	//mBottomTools->AddTool(ID_FORM_M, wxT("Generate FORM K"), wxArtProvider::GetBitmap("application"), "Generate form K");
	//mBottomTools->AddSpacer(5);
	mBottomTools->AddTool(ID_OPEN_SAVE_SALE, wxT("Saved Sales"), wxArtProvider::GetBitmap("sci"));
	mBottomTools->AddSpacer(5);
	mReprintItem = mBottomTools->AddTool(ID_REPRINT, "Reprint", wxArtProvider::GetBitmap(wxART_PRINT, wxART_TOOLBAR, wxSize(16, 16)), "Reprint a sale");
	mReprintItem->SetHasDropDown(true);
	mBottomTools->AddSpacer(5);
	mReturnItem = mBottomTools->AddTool(ID_RETURN_SALE, "Return", wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR, wxSize(16,16)), "Return an item");
	mBottomTools->AddSpacer(5);
	mBottomTools->AddTool(ID_DISCOUNT, "Add discount", wxArtProvider::GetBitmap("action_add"), "Add discount to an item");
	mReturnItem->SetHasDropDown(true);
	mBottomTools->AddSpacer(5);
	//look for how to make this more dynamic
	auto pt = new wxStaticText(mBottomTools, wxID_ANY, "Payment option:");
	pt->SetBackgroundColour(*wxWHITE);
	mBottomTools->AddControl(pt);
	mBottomTools->AddSpacer(5);

	paymentTypes.push_back("Cash");
	paymentTypes.push_back("Transfer");
	paymentTypes.push_back("POS");
	paymentTypes.push_back("No payment option");

	mPaymentTypes = new wxChoice(mBottomTools, ID_PAYMENT_TYPE, wxDefaultPosition, wxSize(150, 20), paymentTypes);
	mPaymentTypes->SetSelection(paymentTypes.size() - 1);
	mPaymentTypes->Bind(wxEVT_PAINT, [=](wxPaintEvent& evt) {
		wxPaintDC dc(mPaymentTypes);
	wxRect rect(0, 0, dc.GetSize().GetWidth(), dc.GetSize().GetHeight());

	
	dc.SetBrush(*wxWHITE);
	dc.SetPen(*wxGREY_PEN);
	dc.DrawRoundedRectangle(rect, 2.0f);
	dc.DrawBitmap(wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(10, 10)), wxPoint(rect.GetWidth() - 15, (rect.GetHeight() / 2) - 5));
	auto sel = mPaymentTypes->GetStringSelection();
		if (!sel.IsEmpty()) {
			dc.DrawLabel(sel, rect, wxALIGN_CENTER);
		}
	});
	mBottomTools->AddControl(mPaymentTypes, "Payment type");
	mBottomTools->AddStretchSpacer();

	mBottomTools->AddSpacer(5);
	mActiveSaleId = new wxStaticText(mBottomTools, ID_ACTIVE_UI_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mActiveSaleId->SetBackgroundColour(*wxWHITE);
	mActiveSaleId->SetDoubleBuffered(true);
	mActiveSaleTextItem = mBottomTools->AddControl(mActiveSaleId);
	SetActiveSaleIdText(mCurSaleuuid);

	mBottomTools->Realize();
	
	mBook = new wxSimplebook(this, wxID_ANY);

	mDataPane = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	mBook->AddPage(mDataPane, "DataPane", false);
	CreateEmptyPanel();
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);


	wxPanel* panel = new wxPanel(mDataPane, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	mInfoBar = new wxInfoBar(panel, wxID_ANY);
	
	m_dataViewCtrl1 = new wxDataViewCtrl(panel, ID_SALE_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	m_dataViewCtrl1->AssociateModel(wxGetApp().mSaleManager.GetSaleData().get());


	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("Product"), pof::SaleManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 250, wxALIGN_CENTER);
	mQuantityColumn = m_dataViewCtrl1->AppendTextColumn(wxT("Quantity"), pof::SaleManager::PRODUCT_QUANTITY, wxDATAVIEW_CELL_EDITABLE, 100, wxALIGN_CENTER);
	mPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("Price"), pof::SaleManager::PRODUCT_PRICE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mDiscountCol = m_dataViewCtrl1->AppendTextColumn(wxT("Discount"), 1000, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mExtPriceColumn = m_dataViewCtrl1->AppendTextColumn(wxT("Extact Price"), pof::SaleManager::PRODUCT_EXT_PRICE);
	
	mPropertyManager = new wxPropertyGrid(mDataPane, ID_PRODUCT_VIEW_PROPERTY, wxDefaultPosition, wxSize(400, -1),
		wxPG_SPLITTER_AUTO_CENTER | wxPG_STATIC_SPLITTER);
	mPropertyManager->Hide();
	CreateProductDetails();

	sizer->Add(mInfoBar, wxSizerFlags().Expand().Border(wxALL, 0));
	sizer->Add(m_dataViewCtrl1, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 0));

	panel->SetSizer(sizer);
	panel->Layout();
	sizer->Fit(panel);

	bSizer6->Add(panel, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 0));
	bSizer6->Add(mPropertyManager, wxSizerFlags().Expand().Border(wxALL, 0));

	mDataPane->SetSizer(bSizer6);
	mDataPane->Layout();
	bSizer6->Fit(mDataPane);
	
	mSaleOutputPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	mSaleOutputPane->SetDoubleBuffered(true);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	mSaleDisplayPane = new wxPanel(mSaleOutputPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);


	bSizer3->Add(0, 0, 1, wxEXPAND, 5);

	mTextOutPut = new wxPanel(mSaleDisplayPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer(4, 2, 5, 5);

	const int fontSize = 9;
	const int valueFontSize = 10;
	wxFont valueFont(wxFontInfo(valueFontSize).AntiAliased());
	mQuantity = new wxStaticText(mTextOutPut, wxID_ANY, wxT("Quantity"), wxDefaultPosition, wxDefaultSize, 0);
	mQuantity->Wrap(-1);
	mQuantity->SetFont(wxFont(wxFontInfo(fontSize).Bold().AntiAliased()));
	gSizer1->Add(mQuantity, 0, wxALIGN_RIGHT | wxALL, 5);

	mQuantityValue = new wxStaticText(mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mQuantityValue->Wrap(-1);
	mQuantityValue->SetFont(valueFont);
	gSizer1->Add(mQuantityValue, 0, wxALIGN_RIGHT | wxALL, 5);

	mExtQuantity = new wxStaticText(mTextOutPut, wxID_ANY, wxT("Ext. Quantity"), wxDefaultPosition, wxDefaultSize, 0);
	mExtQuantity->Wrap(-1);
	mExtQuantity->SetFont(wxFont(wxFontInfo(fontSize).Bold().AntiAliased()));
	gSizer1->Add(mExtQuantity, 0, wxALIGN_RIGHT | wxALL, 5);

	mExtQuantityItem = new wxStaticText(mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mExtQuantityItem->Wrap(-1);
	mExtQuantityItem->SetFont(valueFont);
	gSizer1->Add(mExtQuantityItem, 0, wxALIGN_RIGHT | wxALL, 5);

	mDiscountAmount = new wxStaticText(mTextOutPut, wxID_ANY, wxT("Discount"), wxDefaultPosition, wxDefaultSize, 0);
	mDiscountAmount->Wrap(-1);
	mDiscountAmount->SetFont(wxFont(wxFontInfo(fontSize).Bold().AntiAliased()));
	gSizer1->Add(mDiscountAmount, 0, wxALIGN_RIGHT | wxALL, 5);

	mDiscountValue = new wxStaticText(mTextOutPut, wxID_ANY, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	mDiscountValue->Wrap(-1);
	mDiscountValue->SetFont(valueFont);
	gSizer1->Add(mDiscountValue, 0, wxALIGN_RIGHT | wxALL, 5);

	mTotalQuantity = new wxStaticText(mTextOutPut, wxID_ANY, wxT("Total Quantity"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalQuantity->Wrap(-1);
	mTotalQuantity->SetFont(wxFont(wxFontInfo(fontSize).Bold().AntiAliased()));
	gSizer1->Add(mTotalQuantity, 0, wxALIGN_RIGHT | wxALL, 5);

	mTotalQuantityValue = new wxStaticText(mTextOutPut, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	mTotalQuantityValue->Wrap(-1);
	mTotalQuantityValue->SetFont(valueFont);
	gSizer1->Add(mTotalQuantityValue, 0, wxALIGN_RIGHT | wxALL, 5);


	mTextOutPut->SetSizer(gSizer1);
	mTextOutPut->Layout();
	gSizer1->Fit(mTextOutPut);
	bSizer3->Add(mTextOutPut, 0, wxEXPAND | wxALL, 5);


	mSaleDisplayPane->SetSizer(bSizer3);
	mSaleDisplayPane->Layout();
	bSizer3->Fit(mSaleDisplayPane);
	bSizer2->Add(mSaleDisplayPane, 1, wxEXPAND | wxALL, 5);

	mSalePaymentButtonsPane = new wxPanel(mSaleOutputPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);


	mClear = new wxButton(mSalePaymentButtonsPane, ID_CLEAR, wxT("CLEAR"), wxDefaultPosition, wxSize(120, 50), wxDOUBLE_BORDER);
	mClear->SetBackgroundColour(*wxWHITE);
	bSizer4->Add(mClear, 0, wxALL, 5);

	mSave = new wxButton(mSalePaymentButtonsPane, ID_SAVE, wxT("SAVE"), wxDefaultPosition, wxSize(120, 50), wxDOUBLE_BORDER);
	mSave->SetBackgroundColour(*wxWHITE);
	bSizer4->Add(mSave, 0, wxALL, 5);

	mCheckout = new wxButton(mSalePaymentButtonsPane, ID_CHECKOUT, wxT("CHECK OUT"), wxDefaultPosition, wxSize(120, 50), wxDOUBLE_BORDER);
	mCheckout->SetBackgroundColour(*wxWHITE);
	bSizer4->Add(mCheckout, 0, wxALL, 5);

	bSizer4->Add(0, 0, 1, wxEXPAND, 5);

	mTotalAmountLabel = new wxStaticText(mSalePaymentButtonsPane, wxID_ANY, wxT("TOTAL AMOUNT: "), wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountLabel->SetFont(wxFont(wxFontInfo(15).Bold().AntiAliased()));
	bSizer4->Add(mTotalAmountLabel, 0, wxALIGN_BOTTOM | wxALL, 5);

	mTotalAmount = new wxStaticText(mSalePaymentButtonsPane, wxID_ANY, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmount->SetFont(wxFont(wxFontInfo(15).Bold().AntiAliased()));
	bSizer4->Add(mTotalAmount, 0, wxALIGN_BOTTOM | wxALL, 5);
	
	mSalePaymentButtonsPane->SetSizer( bSizer4 );
	mSalePaymentButtonsPane->Layout();
	bSizer4->Fit( mSalePaymentButtonsPane );
	bSizer2->Add( mSalePaymentButtonsPane, 0, wxEXPAND | wxALL, 5 );
	
	
	mSaleOutputPane->SetSizer( bSizer2 );
	mSaleOutputPane->Layout();
	bSizer2->Fit( mSaleOutputPane );

	bSizer1->Add(mTopTools, 0, wxEXPAND | wxALL, 0);
	bSizer1->Add(mBottomTools, 0, wxEXPAND |wxALL, 0);
	//bSizer1->Add(mDataPane, 1, wxEXPAND | wxALL, 0);
	bSizer1->Add(mBook, 1, wxEXPAND | wxALL, 0);

	bSizer1->Add( mSaleOutputPane, 0, wxEXPAND | wxALL, 0 );
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit(this);

	wxGetApp().mProductManager.GetProductData()->ConnectSlot(std::bind_front(&pof::SaleView::OnProductUpdate, this), pof::DataModel::Signals::UPDATE);

	SetupDropTarget();
	CreateSpecialColumnHandlers();
	CreateSearchPopup();
	ProductNameKeyEvent(); //experiment
	CreateAccelTable();
	this->Bind(wxEVT_MENU, [&](wxCommandEvent& evt) {
		mScanProductValue->SetFocus();
	}, ID_FOCUS_SCAN);
	wxGetApp().mPrintManager->printSig.connect(std::bind_front(&pof::SaleView::OnSaleComplete, this));
	/*this->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& evt) {
		UpdateSaleDisplay();
	});*/
	CheckEmpty();

	//load receipt style
	/*auto ap = wxGetApp().GetAssertsPath() / "boostrap.css";
	std::fstream file(ap, std::ios::in);
	if(file.is_open()) receiptcss << file.rdbuf();*/
}

pof::SaleView::~SaleView()
{
}

void pof::SaleView::SetupDropTarget()
{
	//allow drops from the dataobject
	auto DataObject = new pof::DataObject("PRODUCTDATA", mDropRow, wxGetApp().mProductManager.GetProductData()->GetDatastore().get_metadata());
	auto DropTarget = new pof::DropTarget(DataObject, std::bind_front(&pof::SaleView::DropData, this));
	SetDropTarget(DropTarget);
}

void pof::SaleView::CreateSpecialColumnHandlers()
{

	pof::DataModel::SpeicalColHandler_t extPriceCol;
	pof::DataModel::SpeicalColHandler_t quantityCol;
	pof::DataModel::SpeicalColHandler_t discountCol;

	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	pof::base::data& dataStore = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();

	extPriceCol.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		auto& pid = boost::variant2::get<boost::uuids::uuid>(v[pof::SaleManager::PRODUCT_UUID]);
		auto& price = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_PRICE]);
		auto& extPrice = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_EXT_PRICE]);
		auto& quantiy = boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]);
		extPrice = price * static_cast<double>(quantiy);
		
		//account for dicount
		auto diter = mDiscounts.find(pid);
		if (diter != mDiscounts.end()) {
			extPrice -= diter->second;
		}

		return wxVariant(fmt::format("{:cu}", extPrice));
	};

	quantityCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		try {
			auto quan = static_cast<std::uint64_t>(atoi(value.GetString().ToStdString().c_str()));
			auto& pid = boost::variant2::get<boost::uuids::uuid>(v[pof::SaleManager::PRODUCT_UUID]);

			//check if the new quantity is more than the available stock?
			auto productIter = std::ranges::find_if(wxGetApp().mProductManager.GetProductData()->GetDatastore(),
				[&](const pof::base::data::row_t& prod) -> bool {
					return v[pof::SaleManager::PRODUCT_UUID] == prod.first[pof::ProductManager::PRODUCT_UUID];
			});
			//do not use minimum stock count here, as sale, should sell it is more than the current stock
			std::uint64_t curStock = boost::variant2::get<std::uint64_t>(productIter->first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			if (quan > curStock) {
				wxMessageBox("Cannot set a quanity that is more than the current stock", "SALE", wxICON_WARNING | wxOK);
				return false;
			}
			stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock - quan)));

			auto& price = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_PRICE]);
			pof::base::currency extPrice = price * static_cast<double>(quan);

			//account for dicount
			auto diter = mDiscounts.find(pid);
			if (diter != mDiscounts.end()) {
				extPrice -= diter->second;
			}

			v[pof::SaleManager::PRODUCT_QUANTITY] = quan;
			v[pof::SaleManager::PRODUCT_EXT_PRICE] = extPrice;
			UpdateSaleDisplay();

			//handle productLabelInfo quantity
			auto& info = mProductLabels[row];
			info.mQuantity = boost::variant2::get<std::uint64_t>(productIter->first[pof::ProductManager::PRODUCT_PACKAGE_SIZE]) * quan;
			return true;
		}
		catch (const std::exception& exp) {
			spdlog::error(exp.what());
			return false;
		}
	};

	discountCol.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		auto& pid = boost::variant2::get<boost::uuids::uuid>(v[pof::SaleManager::PRODUCT_UUID]);
		auto iter = mDiscounts.find(pid);
		pof::base::currency ret;
		if (iter != mDiscounts.end()) {
			ret = iter->second;
		}
		return fmt::format("{:cu}", ret);
	};

	model->SetSpecialColumnHandler(pof::SaleManager::PRODUCT_QUANTITY, std::move(quantityCol));
	model->SetSpecialColumnHandler(pof::SaleManager::PRODUCT_EXT_PRICE, std::move(extPriceCol));
	model->SetSpecialColumnHandler(1000, std::move(discountCol));
}

void pof::SaleView::CreateSearchPopup()
{
	auto sharedData = wxGetApp().mProductManager.GetProductData()->ShareDatastore();
	mSearchPopup = new pof::SearchPopup(mProductNameValue, sharedData, { {"Name", pof::ProductManager::PRODUCT_NAME}, 
			{"Formulation", pof::ProductManager::PRODUCT_FORMULATION}, 
			{"Strength", pof::ProductManager::PRODUCT_STRENGTH}, 
			{"Package size", pof::ProductManager::PRODUCT_PACKAGE_SIZE}, 
			{"Cost", pof::ProductManager::PRODUCT_UNIT_PRICE} }, 
			{275, 100, 100, 100, 100});
	pof::DataModel::SpeicalColHandler_t spl;
	spl.first = [sd = sharedData](size_t row, size_t col) -> wxVariant {
		auto& st = boost::variant2::get<pof::base::data::text_t>((*sd)[row].first[pof::ProductManager::PRODUCT_STRENGTH]);
		auto& stt = boost::variant2::get<pof::base::data::text_t>((*sd)[row].first[pof::ProductManager::PRODUCT_STRENGTH_TYPE]);
		if (stt == "NOT SPECIFIED") return ""s;

		auto string = fmt::format("{}{}", st, stt);
		return string;
	};

	mSearchPopup->PushSpecialCol(std::move(spl), pof::ProductManager::PRODUCT_STRENGTH);
	mSearchPopup->sSelectedSignal.connect(std::bind_front(&pof::SaleView::OnSearchPopup, this));
}

void pof::SaleView::CreateProductDetails()
{
	//create the property grid
	productName = new wxStringProperty("PRODUCT NAME", "0");
	strength = new wxStringProperty("UNIT STRENGTH", "1");
	strength_type = new wxStringProperty("STRENGTH TYPE", "2");
	genArray = new wxStringProperty("PRODUCT GENERIC NAME", "3");
	dirArray = new wxEditEnumProperty("DIRECTION FOR USE", "4");
	stock = new wxIntProperty("CURRENT STOCK", "5");
	warning = new wxEditEnumProperty("WARNING", "6");
	packageSize = new wxIntProperty("PACKAGE SIZE", "7");

	stock->Enable(false);
	//warning->Enable(false);
		
	mPropertyManager->Append(productName);
	mPropertyManager->Append(strength);
	mPropertyManager->Append(strength_type);
	mPropertyManager->Append(genArray);
	mPropertyManager->Append(dirArray);
	mPropertyManager->Append(stock);
	mPropertyManager->Append(warning);
	mPropertyManager->Append(packageSize);

	//auto grid = mPropertyManager->GetGrid();
	mPropertyManager->SetBackgroundColour(*wxWHITE);
	mPropertyManager->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	mPropertyManager->SetCaptionTextColour(*wxBLACK);
}

void pof::SaleView::CreateAccelTable()
{
	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_CTRL, (int)'P', ID_CHECKOUT);
	entries[1].Set(wxACCEL_CTRL, (int)'F', ID_FOCUS_SCAN);


	wxAcceleratorTable accel(2, entries);
	this->SetAcceleratorTable(accel);
}

void pof::SaleView::UpdateSaleDisplay()
{
	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	const pof::base::data& dataStore = model->GetDatastore();
	assert(model != nullptr && "failed model dynamic cast, pof::DataModel not a subclass of model");
	size_t quantity = dataStore.size(), exactQuantity = 0;

	pof::base::currency totalAmount;
	size_t extQuantity = 0;
	try {
		totalAmount = std::accumulate(dataStore.begin(),
			dataStore.end(), totalAmount, [&](pof::base::currency v, const pof::base::data::row_t& i) {
				return v + boost::variant2::get<pof::base::currency>(i.first[pof::SaleManager::PRODUCT_EXT_PRICE]);	
			});

		extQuantity = std::accumulate(dataStore.begin(), dataStore.end(), extQuantity, [&](size_t v,
			const pof::base::data::row_t& i) {
				return v + boost::variant2::get<std::uint64_t>(i.first[pof::SaleManager::PRODUCT_QUANTITY]);
		});
	}
	catch (const std::exception& exp) {
		wxMessageBox(exp.what(), "FATAL ERROR", wxICON_ERROR | wxOK);
		spdlog::error(exp.what());
		return;
	}
	Freeze();
	mQuantityValue->SetLabel(fmt::format("{:d}", quantity));
	mExtQuantityItem->SetLabel(fmt::format("{:d}", extQuantity));
	mDiscountValue->SetLabel(fmt::format("{:cu}", mTotalDiscount));
	mTotalQuantityValue->SetLabel(fmt::format("{:d}", extQuantity));
	mTotalAmount->SetLabel(fmt::format("{:cu}", totalAmount));
	Thaw();
	Update();
	mTextOutPut->Layout();
	mSalePaymentButtonsPane->Layout();

}

void pof::SaleView::ResetSaleDisplay()
{
	Freeze();
	mQuantityValue->SetLabel(fmt::format("{:d}", 0));
	mExtQuantityItem->SetLabel(fmt::format("{:d}", 0));
	mDiscountValue->SetLabel(fmt::format("{:cu}", pof::base::data::currency_t{}));
	mTotalQuantityValue->SetLabel(fmt::format("{:d}", 0));
	mTotalAmount->SetLabel(fmt::format("{:cu}", pof::base::data::currency_t{}));
	Thaw();
	Update();
	mTextOutPut->Layout();
	mSalePaymentButtonsPane->Layout();
}

void pof::SaleView::Checkout()
{
}

void pof::SaleView::OnClear(wxCommandEvent& evt)
{
	auto& datastore = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();
	if (datastore.empty())
	{
		mInfoBar->ShowMessage("Sale is empty");
		return;
	}
	if (wxMessageBox("Are you sure you want to clear current sale?", "SALE", wxICON_WARNING | wxYES_NO) == wxNO) return;

	wxGetApp().mSaleManager.GetSaleData()->Clear();
	if (mPropertyManager->IsShown()) {
		mPropertyManager->Hide();
		mDataPane->Layout();
		mDataPane->Refresh();
	}
	mProductLabels.clear();
	ResetSaleDisplay();
	mCurSaleuuid = boost::uuids::nil_uuid();
	SetActiveSaleIdText(mCurSaleuuid);
	mPaymentTypes->SetSelection(paymentTypes.size() - 1);
	if (mInfoBar->IsShown()){
		mInfoBar->Dismiss();
	}

	std::get<2>(mPosionBookDetails) = false;
	mSaleType = NONE;
	mLocked = false;
	mDiscounts.clear();
	mTotalDiscount = {};
	CheckEmpty();
}

void pof::SaleView::CreateEmptyPanel()
{
	mEmpty = new wxPanel(mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
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

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap("checkout"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, fmt::format("Please add an item to begin sale", wxGetApp().gVersion), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	/*wxButton* btn = new wxButton(m7, ID_TOOL_ADD_INVENTORY);
	btn->SetBitmap(wxArtProvider::GetBitmap("action_add"));
	btn->SetLabel("Add stock");
	btn->SetBackgroundColour(*wxWHITE);
	bSizer9->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);*/


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

	mBook->AddPage(mEmpty, "Empty", false);
}

void pof::SaleView::OnCheckout(wxCommandEvent& evt)
{
	//sale dialog
	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	pof::base::data& dataStore = model->GetDatastore();
	assert(model != nullptr && "failed model dynamic cast, pof::DataModel not a subclass of model");
	if (dataStore.empty()) {
		mInfoBar->ShowMessage("Sale is empty");
		return;
	}

	if (mPaymentTypes->GetSelection() == (paymentTypes.size() - 1)){
		wxMessageBox("Please select a payment option for this sale", "Sale", wxICON_INFORMATION | wxOK);
		return;
	}
	wxGetApp().mSaleManager.mCurPaymentType = paymentTypes[mPaymentTypes->GetSelection()].ToStdString();

	pof::base::currency totalAmount;
	try {
		totalAmount = std::accumulate(dataStore.begin(),
			dataStore.end(), totalAmount, [&](pof::base::currency v, const pof::base::data::row_t& i) {
				return v + boost::variant2::get<pof::base::currency>(i.first[pof::SaleManager::PRODUCT_EXT_PRICE]);

		});
	}
	catch (const std::exception& exp) {
		wxMessageBox(exp.what(), "Fatal error", wxICON_ERROR | wxOK);
		spdlog::error(exp.what());
		return;
	}
	//update the stocks of the sold items, set date
	auto now = pof::base::data::clock_t::now();
	std::vector<std::tuple<pof::base::data::duuid_t, std::uint64_t>> quans;
	quans.reserve(dataStore.size());

	auto& prodDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	for (pof::base::data::row_t& r : dataStore) {
		r.first[pof::SaleManager::SALE_DATE] = now;
		auto& puid = boost::variant2::get<pof::base::data::duuid_t>(r.first[pof::SaleManager::PRODUCT_UUID]);
		auto pIter = std::ranges::find_if(prodDatastore, [&](const pof::base::data::row_t& row)->bool {
			return (puid == boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]));
		});
		auto& v = pIter->first;
		//stock count is guranteed from the sale checks not to be less than the amount sold
		v[pof::ProductManager::PRODUCT_STOCK_COUNT] =
			boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT])
			- boost::variant2::get<std::uint64_t>(r.first[pof::SaleManager::PRODUCT_QUANTITY]);
		quans.emplace_back(std::make_tuple(puid, boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT])));

		if (!mPosionBookEntries.empty()) {
			auto poiter = mPosionBookEntries.find(puid);
			if (poiter == mPosionBookEntries.end()) continue;
			poiter->second.first[pof::PoisonBookManager::RUNBALANCE] = v[pof::ProductManager::PRODUCT_STOCK_COUNT];
		}
	};
	wxIcon cop;
	cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
	wxBusyInfo info
	(
		wxBusyInfoFlags()
		.Parent(this)
		.Icon(cop)
		.Title("Selling")
		.Text("Please wait...")
		.Foreground(*wxBLACK)
		.Background(*wxWHITE)
		.Transparency(4 * wxALPHA_OPAQUE / 5)
	);

	wxGetApp().mProductManager.UpdateProductQuan(std::move(quans));
	wxGetApp().mSaleManager.StoreSale();

	//Print receipt
	SaveLabelInfo(mCurSaleuuid);

	wxGetApp().mPrintManager->gPrintState = pof::PrintManager::RECEIPT;
	wxGetApp().mPrintManager->PrintSaleReceipt(nullptr);
	//wxGetApp().mPrintManager->PrintSaleReceiptHtml(CreateHtmlReciept(), CreateHtmlReciept());
	mLocked = false;

	CheckEmpty();
}

void pof::SaleView::OnSaleComplete(bool status, size_t printState)
{
	//to put back when testing is over
	std::ostringstream os;
	os << mCurSaleuuid;
	switch (printState)
	{
	case pof::PrintManager::RECEIPT:
		if (!status) {
			spdlog::error("Printing failed");
			return;
		}

		//signal the module that created the sale
		mSaleCompleted(mCurSaleuuid, mSaleType);
		mSaleType = NONE;
		StorePoisonBookEnteries(); //store entries if any
		wxGetApp().mSaleManager.GetSaleData()->Clear();
		if (mPropertyManager->IsShown()) {
			mPropertyManager->Hide();
			mDataPane->Layout();
			mDataPane->Refresh();
		}
		mProductLabels.clear(); //remove all the label data
		ResetSaleDisplay();
		wxGetApp().mSaleManager.RemoveSaveSale(mCurSaleuuid);
		
		SaveDiscounts(mCurSaleuuid);
		mTotalDiscount = {};
		mDiscounts.clear();

		mCurSaleuuid = boost::uuids::nil_uuid();
		CheckEmpty();

		mPaymentTypes->SetSelection(paymentTypes.size() - 1);
		std::get<2>(mPosionBookDetails) = false;
		SetActiveSaleIdText(mCurSaleuuid);
		mInfoBar->ShowMessage("Sale complete");
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, fmt::format("Sale {} completed", os.str()));
		break;
	case pof::PrintManager::REPRINT_RECEIPT:
		if (mCurSaleuuid != boost::uuids::nil_uuid()) {
			status = wxGetApp().mSaleManager.RestoreSaveSale(mCurSaleuuid);
			wxGetApp().mSaleManager.RemoveLabels(mCurSaleuuid);
		}
		else {
			wxGetApp().mSaleManager.GetSaleData()->Clear();
		}
		if (!status) {
			mInfoBar->ShowMessage(fmt::format("Reprint failed"));
		}else	mInfoBar->ShowMessage(fmt::format("Reprinted successfully"));
		break;
	default:
		break;
	}
}

void pof::SaleView::OnSave(wxCommandEvent& evt)
{
	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}

	std::ostringstream str;
	str << mCurSaleuuid;
	if (wxGetApp().mSaleManager.GetSaleData()->GetDatastore().empty()) {
		mInfoBar->ShowMessage("Sale is empty", wxICON_INFORMATION);
		return;
	}
	if (wxGetApp().mSaleManager.CheckIfSaved(mCurSaleuuid)){
		mInfoBar->ShowMessage(fmt::format("Sale {} already saved", str.str()), wxICON_INFORMATION);
		return;
	}
	mSaleType = NONE;
	wxGetApp().mSaleManager.SaveSale(mCurSaleuuid);
	SaveLabelInfo(mCurSaleuuid);
	if (!mDiscounts.empty()) {
		SaveDiscounts(mCurSaleuuid);
	}

	//clear the current sale
	wxGetApp().mSaleManager.GetSaleData()->Clear();
	if (mPropertyManager->IsShown()) {
		mPropertyManager->Hide();
		mDataPane->Layout();
		mDataPane->Refresh();
	}
	mProductLabels.clear();
	ResetSaleDisplay();
	mCurSaleuuid = boost::uuids::nil_uuid();
	SetActiveSaleIdText(mCurSaleuuid);
	mPaymentTypes->SetSelection(paymentTypes.size() - 1);
	if (mInfoBar->IsShown()) {
		mInfoBar->Dismiss();
	}

	std::get<2>(mPosionBookDetails) = false;
	mSaleType = NONE;
	mLocked = false;
	mDiscounts.clear();
	mTotalDiscount = {};

	CheckEmpty();

	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, fmt::format("Saved Sale: {}", str.str()));
	wxMessageBox(fmt::format("{} saved successfully", str.str()), "Sale", wxICON_INFORMATION | wxOK);
}

void pof::SaleView::OnDropPossible(wxDataViewEvent& evt)
{
	spdlog::info("Drop possible");
}

void pof::SaleView::OnDrop(wxDataViewEvent& evt)
{
	spdlog::info("Dropped");
}

void pof::SaleView::OnBeginDrag(wxDataViewEvent& evt)
{
}

void pof::SaleView::OnProductNameSearch(wxCommandEvent& evt)
{
	auto searchString = evt.GetString().ToStdString();
	if (searchString.empty()) {
		mSearchPopup->Dismiss();
		return;
	}
	
	wxPoint pos = mProductNameValue->ClientToScreen(wxPoint(0, 0));
	wxSize sz = mProductNameValue->GetClientSize();

	mSearchPopup->SetPosition(wxPoint{ pos.x, pos.y + sz.y  + 5});
	mSearchPopup->SetSize(wxSize(sz.x + 500, 400));

	//mSearchPopup->Show();
	mSearchPopup->CaptureFocus();
	mSearchPopup->Popup();
	mSearchPopup->SearchString(pof::ProductManager::PRODUCT_NAME, searchString);
	mSearchPopup->SetFocus();
}

void pof::SaleView::CheckEmpty()
{
	if (wxGetApp().mSaleManager.GetSaleData()->GetDatastore().empty())
	{
		mBook->SetSelection(SALE_EMPTY);
	}
	else {
		mBook->SetSelection(SALE_VIEW);
	}
}

void pof::SaleView::OnRemoveProduct(wxCommandEvent& evt)
{
	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()){
		wxMessageBox("No item is selected", "Sales", wxICON_INFORMATION | wxOK);
		return;
	}
	auto& datastore = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();
	size_t idx = pof::DataModel::GetIdxFromItem(item);
	if (idx == datastore.size() - 1){
		if (mPropertyManager->IsShown()) {
			mPropertyManager->Hide();
			mDataPane->Layout();
			mDataPane->Refresh();
		}
	}
	else {
		if (mPropertyManager->IsShown()) {
			//load next
			idx++;
			auto& saleData = wxGetApp().mSaleManager.GetSaleData()->GetDatastore()[idx];
			auto& saleProductUuid = boost::variant2::get<pof::base::data::duuid_t>(saleData.first[pof::SaleManager::PRODUCT_UUID]);
			auto iter = std::ranges::find_if(wxGetApp().mProductManager.GetProductData()->GetDatastore(),
				[&](const pof::base::data::row_t& row) -> bool {
					return (boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]) == saleProductUuid);
				});
			if (iter == std::end(wxGetApp().mProductManager.GetProductData()->GetDatastore())) return;
			//load the stock count here, just because
			std::uint64_t curStock = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			curStock -= boost::variant2::get<std::uint64_t>(saleData.first[pof::SaleManager::PRODUCT_QUANTITY]);


			stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock)));
			LoadProductDetails(*iter);
		}
	}

	//remove discount associated with the product
	auto& pid = boost::variant2::get<boost::uuids::uuid>(datastore[pof::DataModel::GetIdxFromItem(item)].first[pof::SaleManager::PRODUCT_UUID]);
	auto i = mDiscounts.find(pid);
	if (i != mDiscounts.end()){
		mTotalDiscount -= i->second;
		mDiscounts.erase(i);
	}

	m_dataViewCtrl1->Freeze();
	if (mInfoBar->IsShown()) mInfoBar->Dismiss();
	mProductLabels.erase(std::next(mProductLabels.begin(), idx));
	wxGetApp().mSaleManager.GetSaleData()->RemoveData(item);
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Refresh();
	UpdateSaleDisplay();

	//reset the sale if we removed all of the items 
	if (wxGetApp().mSaleManager.GetSaleData()->GetDatastore().empty()) {
		mCurSaleuuid = boost::uuids::nil_uuid();
		SetActiveSaleIdText(mCurSaleuuid);
	}
	CheckEmpty();
}

void pof::SaleView::OnSelected(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);


	if (!mPropertyManager->IsShown()) {
		((wxWindowBase*)mPropertyManager)->Show();
	//	mDataPane->Layout();
		auto parent = mPropertyManager->GetParent();
		parent->Layout();
		parent->Refresh();
	}
	auto& saleData = wxGetApp().mSaleManager.GetSaleData()->GetDatastore()[idx];
	auto& saleProductUuid = boost::variant2::get<pof::base::data::duuid_t>(saleData.first[pof::SaleManager::PRODUCT_UUID]);
	auto iter = std::ranges::find_if(wxGetApp().mProductManager.GetProductData()->GetDatastore(),
		[&](const pof::base::data::row_t& row) -> bool {
			return (boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]) == saleProductUuid);
	});

	if (iter == std::end(wxGetApp().mProductManager.GetProductData()->GetDatastore())) return;
	//load the stock count here, just because
	std::uint64_t curStock = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
	curStock -= boost::variant2::get<std::uint64_t>(saleData.first[pof::SaleManager::PRODUCT_QUANTITY]);


	stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock)));
	LoadProductDetails(*iter);
	evt.Skip();
}

void pof::SaleView::OnHideProductViewProperty(wxCommandEvent& evt)
{
	if (!mPropertyManager->IsShown()) return;
	mPropertyManager->Hide();
	mDataPane->Layout();
	mDataPane->Refresh();
}

void pof::SaleView::OnPrintAsLabels(wxCommandEvent& evt)
{
	//print products as labels
	if (mProductLabels.empty()) {
		mInfoBar->ShowMessage("Sale is empty");
		return;
	}
	
	wxIcon cop;
	cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
	wxBusyInfo info
	(
		wxBusyInfoFlags()
		.Parent(this)
		.Icon(cop)
		.Title("Printing")
		.Text("Please wait...")
		.Foreground(*wxBLACK)
		.Background(*wxWHITE)
		.Transparency(4 * wxALPHA_OPAQUE / 5)
	);


	wxGetApp().mPrintManager->gPrintState = pof::PrintManager::LABELS;
	wxGetApp().mPrintManager->PrintLabels(mProductLabels, m_dataViewCtrl1);
}

void pof::SaleView::OnShowPacks(wxCommandEvent& evt)
{
	if (mLocked){
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}
	pof::PackView dialog(this, true);
	if (dialog.ShowModal() == wxID_OK) {
		wxBusyCursor cursor;
		auto prods = dialog.GetPackProducts();
		if (prods.empty()) return;

		for (auto& prod : prods) {

			//find row
			auto iter = std::ranges::find_if(wxGetApp().mProductManager.GetProductData()->GetDatastore(),
				[&](const pof::base::data::row_t& row) -> bool {
					return boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]) == std::get<1>(prod);
				});

			//check if already in sale
			std::optional<pof::base::data::iterator> iterOpt;
			if ((iterOpt = CheckAlreadyAdded(std::get<2>(prod)))) {
				//think and implement
				auto& iterS = iterOpt.value();
				auto& v = iter->first;
				std::uint64_t curStock = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]);
				auto& quan = boost::variant2::get<std::uint64_t>(iterS->first[pof::SaleManager::PRODUCT_QUANTITY]);
				auto& extCur = boost::variant2::get<pof::base::currency>(iterS->first[pof::SaleManager::PRODUCT_EXT_PRICE]);
				auto& cur = boost::variant2::get<pof::base::currency>(iterS->first[pof::SaleManager::PRODUCT_PRICE]);
				quan--; //this is from the function CheckAlreadyAdded, it increments it internally, remove this here
				quan += std::get<3>(prod);
				if (quan > curStock) {
					wxMessageBox(fmt::format("Cannot add \'{}\', Adding Quantity \'{:d}\' that is more than the current stock",
						boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]), std::get<3>(prod)), "SALE", wxICON_WARNING | wxOK);
					quan -= std::get<3>(prod);
					continue; //move to the next product in the pack
				}

				if (mPropertyManager->IsShown()) {
					//update product details
					stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock - quan)));
					LoadProductDetails(*iter);
				}
				extCur = cur * static_cast<double>(quan);
				continue; //move to the next pack
			}
			if (CheckExpired(*iter)) {
				if (wxMessageBox(fmt::format("{} is expired in the inventory, please check expiry date, do you wish to continue?",
					boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO)
				{
					continue;
				}
			}
			bool status = CheckInStock(*iter);
			if (!status) {
				wxMessageBox(fmt::format("{} is out of stock",
					boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
				continue;
			}

			std::uint64_t presentStock = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			if (presentStock < std::get<3>(prod)){
				if (wxMessageBox(fmt::format("Adding more \'{}\' then there is available stock, present stock is {:d}, Do you want to reduce to present stock?",
					boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME]), presentStock), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxYES) {
					std::get<3>(prod) = presentStock;
				}else continue;
			}

			status = CheckProductClass(*iter);
			if (status) {
				if (wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale, do you wish to continue",
					boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO) 
					continue;
			}
			CheckControlled(*iter);

			if (mCurSaleuuid == boost::uuids::nil_uuid()) {
				mCurSaleuuid = uuidGen();
				SetActiveSaleIdText(mCurSaleuuid);
			}

			pof::base::data::row_t rowSale;
			auto& vS = rowSale.first;
			vS.resize(pof::SaleManager::MAX);
			vS[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			vS[pof::SaleManager::PRODUCT_UUID] = std::get<1>(prod);
			vS[pof::SaleManager::PRODUCT_NAME] = std::move(std::get<2>(prod));
			vS[pof::SaleManager::PRODUCT_QUANTITY] = std::get<3>(prod);
			vS[pof::SaleManager::PRODUCT_PRICE] = std::get<5>(prod);
			vS[pof::SaleManager::PRODUCT_EXT_PRICE] = std::get<5>(prod) * static_cast<double>(std::get<3>(prod));


			CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(iter->first[pof::ProductManager::PRODUCT_UUID]));
			mProductLabels.emplace_back(LabelInfo{});
			LoadLabelDetails(mProductLabels.back(), *iter);
			mProductLabels.back().mQuantity *= std::get<3>(prod);
			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));
			size_t count = wxGetApp().mSaleManager.GetSaleData()->GetDatastore().size();
			m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);

		}
		CheckEmpty();
		UpdateSaleDisplay();
	}
}

void pof::SaleView::OnFormM(wxCommandEvent& evt)
{
	wxMessageBox("Form K is not avaliable in this verision", "Sale", wxICON_INFORMATION | wxOK);
}

void pof::SaleView::OnOpenSaveSale(wxCommandEvent& evt)
{
	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}
	auto saveSales = wxGetApp().mSaleManager.GetSavedSales();
	if (!saveSales.has_value()) {
		wxMessageBox("Cannot open save sales, critical error, please call admin", "Sales", wxICON_ERROR | wxOK);
		return;
	}
	pof::SaveSaleDialog dialog(this, wxID_ANY);
	bool status = dialog.LoadSaveSales(saveSales.value());
	if(!status){
		wxMessageBox("No sale is saved", "Sale", wxICON_INFORMATION | wxOK);
		return;
	}
	if (dialog.ShowModal() == wxID_OK){

		if (mCurSaleuuid != boost::uuids::nil_uuid()) {
			wxMessageBox("Sale is not empty, clear, save or checkout current sale before loading a saved sale", "Sale", wxICON_INFORMATION | wxOK);
			return;
		}

		status = wxGetApp().mSaleManager.RestoreSaveSale(dialog.GetSaveSaleId());
		if (status) {
			std::ostringstream str;
			mCurSaleuuid = dialog.GetSaveSaleId();
			str << mCurSaleuuid;
			ReloadLabelInfo(mCurSaleuuid);
			RestoreDiscounts(mCurSaleuuid);

			//recalculate the discounts
			mTotalDiscount = {};
			for (auto& d : mDiscounts) {
				mTotalDiscount += d.second;
			}

			wxGetApp().mSaleManager.RemoveInfo(mCurSaleuuid);
			wxGetApp().mSaleManager.RemoveLabels(mCurSaleuuid);
			//remove from save
			wxGetApp().mSaleManager.RemoveSaveSale(mCurSaleuuid);
			UpdateSaleDisplay();
			SetActiveSaleIdText(mCurSaleuuid);
			wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, fmt::format("Restored Sale: {}", str.str()));
			mInfoBar->ShowMessage(fmt::format("Restored {} sale sucessfully", str.str()), wxICON_INFORMATION | wxOK);
		}
		else {
			wxMessageBox("Failed to restore sale", "Sale", wxICON_INFORMATION | wxOK);
		}
	}
	CheckEmpty();
}

void pof::SaleView::OnValueChanged(wxDataViewEvent& evt)
{

}

void pof::SaleView::OnEditingStarted(wxDataViewEvent& evt)
{
	//move this to setup, no need to call every time
	auto item = evt.GetItem();
	auto Col = evt.GetDataViewColumn();
	auto Ctrl = Col->GetRenderer()->GetEditorCtrl();
	wxIntegerValidator<size_t> val{ NULL };
	val.SetMin(0);
	val.SetMax(10000);
	Ctrl->SetValidator(val);

}

void pof::SaleView::OnEditingDone(wxDataViewEvent& evt)
{
}

void pof::SaleView::OnSaleUuidTextUI(wxUpdateUIEvent& evt)
{
	auto& datastore = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();
	if (datastore.empty()){
		mCurSaleuuid = boost::uuids::nil_uuid();
		SetActiveSaleIdText(mCurSaleuuid);
	}
}

void pof::SaleView::OnReturnSale(wxAuiToolBarEvent& evt)
{
	if (!evt.IsDropDownClicked()) {
		if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
			wxMessageBox("User account cannot perform this function", "Return", wxICON_INFORMATION | wxOK);
			return;
		}

		if (mLocked) {
			mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
			return;
		}

		if (wxGetApp().mSaleManager.GetSaleData()->GetDatastore().empty()) {
			mInfoBar->ShowMessage("No products selected to return");
			return;
		}
		try {
			wxTextEntryDialog dialog(this, "Please enter the receipt ID for this return", "Return");
			if (dialog.ShowModal() != wxID_OK) return;
			std::string uids = dialog.GetValue().ToStdString();
			if (uids.empty()) return;
			boost::uuids::uuid rid = boost::lexical_cast<boost::uuids::uuid>(uids);
			wxIcon cop;
			cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
			wxBusyInfo info
			(
				wxBusyInfoFlags()
				.Parent(this)
				.Icon(cop)
				.Title("Returning")
				.Text(fmt::format("Please wait...."))
				.Foreground(*wxBLACK)
				.Background(*wxWHITE)
				.Transparency(4 * wxALPHA_OPAQUE / 5)
			);
			bool status = false;
			for (auto& retSale : wxGetApp().mSaleManager.GetSaleData()->GetDatastore()) {
				auto& pid = boost::variant2::get<pof::base::data::duuid_t>(retSale.first[pof::SaleManager::PRODUCT_UUID]);
				if (wxGetApp().mSaleManager.CheckReturned(rid, pid)) {
					wxMessageBox("Product has already been returned", "Sales", wxICON_WARNING | wxOK);
					continue;
				}

				auto retQuan = wxGetApp().mSaleManager.GetReturnedProductQuan(pid, rid);
				if (!retQuan.has_value()) goto err;
				status = wxGetApp().mProductManager.ReturnToInventory(pid, retQuan.value());
				if (!status) goto err;
				status = wxGetApp().mSaleManager.ReturnFromSales(rid, pid);
				if (!status) goto err;

				//get product
				auto& prodDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
				auto prodIter = std::ranges::find_if(prodDatastore,
					[&](auto& r) -> bool {
						return boost::variant2::get<pof::base::data::duuid_t>(r.first[pof::ProductManager::PRODUCT_UUID])
						== pid;
					});
				if (std::end(prodDatastore) == prodIter) goto err;
				wxGetApp().mProductManager.RefreshRowFromDatabase(pid, *prodIter);


				// do not delete from sale, the problems is too much, never delete sale
				//status = wxGetApp().mSaleManager.RemoveProductFromSale(pid, rid);
				//if (!status) goto err;

				auto name = boost::variant2::get<pof::base::data::text_t>(prodIter->first[pof::ProductManager::PRODUCT_NAME]);
				wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, fmt::format("Returned {} from {}", name, uids));
			}
			wxGetApp().mSaleManager.GetSaleData()->Clear();
			if (mPropertyManager->IsShown()) {
				mPropertyManager->Hide();
				mDataPane->Layout();
				mDataPane->Refresh();
			}
			ResetSaleDisplay();
			mCurSaleuuid = boost::uuids::nil_uuid();
			SetActiveSaleIdText(mCurSaleuuid);
			if (mInfoBar->IsShown()) {
				mInfoBar->Dismiss();
			}
			CheckEmpty();
			mInfoBar->ShowMessage("Return successfull", wxICON_INFORMATION);
			return;
		}
		catch (std::exception& exp) {
			spdlog::error(exp.what());
			wxMessageBox("Incorrect recepit ID", "Return", wxICON_INFORMATION | wxOK);
			return;
		}
	}
	else {
		wxMenu* menu = new wxMenu;
		menu->Append(ID_RETURN_LAST_SALE, "Return last sale", nullptr);

		wxPoint pos = mReturnItem->GetSizerItem()->GetPosition();
		wxSize sz = mReturnItem->GetSizerItem()->GetSize();

		mBottomTools->PopupMenu(menu, wxPoint{ pos.x, pos.y + sz.y + 2 });
		return;
	}
err:
	mInfoBar->ShowMessage("Return failed", wxICON_ERROR);
	return;
}

void pof::SaleView::OnReprintLastSale(wxCommandEvent& evt)
{
	bool status = false;
	wxIcon cop;
	cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
	wxBusyInfo info
	(
		wxBusyInfoFlags()
		.Parent(this)
		.Icon(cop)
		.Title("Reprinting last sale")
		.Text(fmt::format("Please wait...."))
		.Foreground(*wxBLACK)
		.Background(*wxWHITE)
		.Transparency(4 * wxALPHA_OPAQUE / 5)
	);

	//we have an active sale 
	if (mCurSaleuuid != boost::uuids::nil_uuid()) {
		status = wxGetApp().mSaleManager.SaveSale(mCurSaleuuid);
		SaveLabelInfo(mCurSaleuuid);
		if (!status) {
			spdlog::error("Failed to save sale for reprint");
			mInfoBar->ShowMessage("Failed to reprint last sale");
			return;
		}
	}
	//restore the sale we want to reprint
	status = wxGetApp().mSaleManager.RestoreLastSale();

	if (!status) {
		spdlog::error("Failed to save sale for reprint");
		mInfoBar->ShowMessage("Failed to reprint last sale");
		return;
	}
	wxGetApp().mPrintManager->gPrintState = pof::PrintManager::REPRINT_RECEIPT;
	wxGetApp().mPrintManager->PrintSaleReceipt(nullptr);
}

void pof::SaleView::OnReprintSale(wxAuiToolBarEvent& evt)
{
	if (!evt.IsDropDownClicked()) {
		while (1) {
			wxTextEntryDialog dialog(this, wxT("Please enter receipt ID"), "Reprint");
			if (dialog.ShowModal() == wxID_OK) {


				std::string rid = dialog.GetValue().ToStdString();
				if (rid.empty()) {
					mInfoBar->ShowMessage("Empty receipt id");
					return;
				}
				try {
					boost::uuids::uuid sid = boost::lexical_cast<boost::uuids::uuid>(rid);
					if (!wxGetApp().mSaleManager.CheckIfSaleExists(sid)) {
						mInfoBar->ShowMessage(fmt::format("No sale with reciept {} occured in the store", boost::uuids::to_string(sid)));
						return;
					}
					bool status = false;
					wxIcon cop;
					cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
					wxBusyInfo info
					(
						wxBusyInfoFlags()
						.Parent(this)
						.Icon(cop)
						.Title("Reprinting")
						.Text(fmt::format("{}", boost::uuids::to_string(sid)))
						.Foreground(*wxBLACK)
						.Background(*wxWHITE)
						.Transparency(4 * wxALPHA_OPAQUE / 5)
					);

					//we have an active sale 
					if (mCurSaleuuid != boost::uuids::nil_uuid()) {
						status = wxGetApp().mSaleManager.SaveSale(mCurSaleuuid);
						if (!status) {
							spdlog::error("Failed to save sale for reprint");
							mInfoBar->ShowMessage("Failed to reprint reciept");
							return;
						}
					}
					//restore the sale we want to reprint
					status = wxGetApp().mSaleManager.RestoreSale(sid);
					if (!status) {
						spdlog::error("Failed to save sale for reprint");
						mInfoBar->ShowMessage(fmt::format("Failed to reprint sale {}", boost::uuids::to_string(sid)));
						return;
					}
					ReloadLabelInfo(sid);
					wxGetApp().mPrintManager->gPrintState = pof::PrintManager::REPRINT_RECEIPT;
					wxGetApp().mPrintManager->PrintSaleReceipt(nullptr);
					return;
				}
				catch (boost::bad_lexical_cast& err) {
					spdlog::error(err.what());
					if (wxMessageBox("Incorrect receipt Id, try again ?", "Reprint", wxICON_INFORMATION | wxYES_NO) == wxYES) continue;
					return;
				}
			}
			else {
				break;
			}
		}
	}
	else {
		wxMenu* menu = new wxMenu;
		auto rr = menu->Append(ID_REPRINT_LAST_SALE, "Reprint Last Sale", nullptr);
		wxPoint pos = mReprintItem->GetSizerItem()->GetPosition();
		wxSize sz = mReprintItem->GetSizerItem()->GetSize();

		mBottomTools->PopupMenu(menu, wxPoint{ pos.x, pos.y + sz.y + 2 });
	}
}

void pof::SaleView::DropData(const pof::DataObject& dat)
{
	auto& meta = dat.GetMeta();
	auto& row = dat.GetSetData();

	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}

	if (row.has_value()) {
		auto& val = row.value();
		bool status = CheckInStock(val);
		if (!status) {
			wxMessageBox(fmt::format("{} is out of stock",
				boost::variant2::get<pof::base::data::text_t>(val.first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
		if (CheckExpired(val)) {
			if (wxMessageBox(fmt::format("{} is expired in the inventory, please check expiry date, do you wish to continue?",
				boost::variant2::get<pof::base::data::text_t>(val.first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO)
			{
				return;
			}
		}
		status = CheckProductClass(val);
		if (status) {
			if(wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale, do you wish to continue",
				boost::variant2::get<pof::base::data::text_t>(val.first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO) return;
		}

		CheckControlled(val);

		std::optional<pof::base::data::iterator> iterOpt;
		auto& v = val.first;
		if ((iterOpt = CheckAlreadyAdded(boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]))))
		{
			auto& iterS = iterOpt.value();
			std::uint64_t curStock = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			auto& quan = boost::variant2::get<std::uint64_t>(iterS->first[pof::SaleManager::PRODUCT_QUANTITY]);
			if (quan > curStock) {
				wxMessageBox(fmt::format("Cannot add \'{}\', out of stock",
					boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE", wxICON_WARNING | wxOK);
				quan--;
				return;
			}
			if (mPropertyManager->IsShown()) {
				//update product details
				stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock - quan)));
				LoadProductDetails(val);
			}
		}
		else {
			if (mCurSaleuuid == boost::uuids::nil_uuid()) {
				mCurSaleuuid = uuidGen();
				SetActiveSaleIdText(mCurSaleuuid);
			}

			auto row = pof::base::data::row_t();
			row.first.resize(pof::SaleManager::MAX);
			auto& v = row.first;

			v[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			v[pof::SaleManager::PRODUCT_UUID] = val.first[pof::ProductManager::PRODUCT_UUID];
			v[pof::SaleManager::PRODUCT_NAME] = val.first[pof::ProductManager::PRODUCT_NAME];
			v[pof::SaleManager::PRODUCT_PRICE] = val.first[pof::ProductManager::PRODUCT_UNIT_PRICE];
			v[pof::SaleManager::PRODUCT_QUANTITY] = boost::variant2::get<std::uint64_t>(val.first[pof::ProductManager::PRODUCT_STOCK_COUNT]) > 0 ? static_cast<std::uint64_t>(1) : static_cast<std::uint64_t>(0);
			v[pof::SaleManager::PRODUCT_EXT_PRICE] = boost::variant2::get<pof::base::currency>(val.first[pof::ProductManager::PRODUCT_UNIT_PRICE]) *
				static_cast<double>(boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]));

			mProductLabels.emplace_back(LabelInfo{});
			LoadLabelDetails(mProductLabels.back(), val);
			mProductLabels.back().mQuantity *= boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]);

			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(row));

			size_t count = wxGetApp().mSaleManager.GetSaleData()->GetDatastore().size();
			m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);
		}
		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
		UpdateSaleDisplay();
		CheckEmpty();
	}
	else {
		spdlog::error("Drop data invalid or does not exist");
	}
}

void pof::SaleView::OnSearchPopup(const pof::base::data::row_t& row)
{
	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}

	try {
		auto& v = row.first;
		bool status = CheckInStock(row);
		if (!status) {
			wxMessageBox(fmt::format("{} is out of stock",
				boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
		if (CheckExpired(row)){
			if(wxMessageBox(fmt::format("{} is expired in the inventory, please check expiry date, do you wish to continue?",
					boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO)
			{
				return;
			}
		}
		status = CheckProductClass(row);
		if (status) {
			if (wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale, do you wish to continue?",
				boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO) {
				return;
			}
		}

		CheckControlled(row);

		std::optional<pof::base::data::iterator> iterOpt;
		if (( iterOpt = CheckAlreadyAdded(boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]))))
		{
			auto& iterS = iterOpt.value();
			std::uint64_t curStock = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			auto& quan = boost::variant2::get<std::uint64_t>(iterS->first[pof::SaleManager::PRODUCT_QUANTITY]);
			if (quan > curStock) {
				wxMessageBox(fmt::format("Cannot add \'{}\', out of stock",
						boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE", wxICON_WARNING | wxOK);
				quan--;
				return;
			}
			if (mPropertyManager->IsShown()) {
				//update product details
				stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock - quan)));
				LoadProductDetails(row);
			}
		}
		else {

			if (mCurSaleuuid == boost::uuids::nil_uuid()) {
				mCurSaleuuid = uuidGen();
				SetActiveSaleIdText(mCurSaleuuid);
			}

			pof::base::data::row_t rowSale;
			auto& vS = rowSale.first;
			vS.resize(pof::SaleManager::MAX);

			vS[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			vS[pof::SaleManager::PRODUCT_UUID] = v[pof::ProductManager::PRODUCT_UUID];
			vS[pof::SaleManager::PRODUCT_NAME] = v[pof::ProductManager::PRODUCT_NAME];
			vS[pof::SaleManager::PRODUCT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];
			vS[pof::SaleManager::PRODUCT_QUANTITY] = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]) > 0 ? static_cast<std::uint64_t>(1) : static_cast<std::uint64_t>(0);
			vS[pof::SaleManager::PRODUCT_EXT_PRICE] = boost::variant2::get<pof::base::currency>(v[pof::ProductManager::PRODUCT_UNIT_PRICE]) * 
					static_cast<double>(boost::variant2::get<std::uint64_t>(vS[pof::SaleManager::PRODUCT_QUANTITY]));
			
			mProductLabels.emplace_back(LabelInfo{});
			LoadLabelDetails(mProductLabels.back(), row);
			mProductLabels.back().mQuantity *= boost::variant2::get<std::uint64_t>(vS[pof::SaleManager::PRODUCT_QUANTITY]);

			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));

			size_t count = wxGetApp().mSaleManager.GetSaleData()->GetDatastore().size();
			m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);
		}
		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
		UpdateSaleDisplay();
		CheckEmpty();
		mProductNameValue->Clear(); 
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}
}

void pof::SaleView::OnScanBarCode(wxCommandEvent& evt)
{
	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}

	auto value = evt.GetString().ToStdString();
	if (value.empty()) return;
	constexpr auto inrange = [](int c) ->bool {return (c >= -1 && c <= 255); };
	bool isNum = std::ranges::all_of(value, [&](char c) -> bool { return (inrange(c) && std::isdigit(c)); });
	if (!isNum) {
		wxMessageBox("Unrecongised character in barcode", "SALE", wxICON_WARNING | wxOK);
		return;
	}
	
	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	try {
		auto iter = std::ranges::find_if(datastore, [&](const pof::base::data::row_t& row) -> bool {
			auto& barcode = boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_BARCODE]);
			return (value == barcode);
		});
		if (iter == datastore.end()) {

			wxMessageBox("Product with barcode does not exists", "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
		if (CheckExpired(*iter)) {
			if (wxMessageBox(fmt::format("{} is expired in the inventory, please check expiry date, do you wish to continue?",
				boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO)
			{
				return;
			}
		}
		bool status = CheckProductClass(*iter);
		if (status) {
			if(wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale",
				boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO)) return;
		}

		CheckControlled(*iter);
		auto& v = iter->first;
		std::optional<pof::base::data::iterator> iterOpt;
		if ((iterOpt = CheckAlreadyAdded(boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]))))
		{
			auto& iterS = iterOpt.value();
			std::uint64_t curStock = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]);
			auto& quan = boost::variant2::get<std::uint64_t>(iterS->first[pof::SaleManager::PRODUCT_QUANTITY]);
			if (quan > curStock) {
				wxMessageBox(fmt::format("Cannot add \'{}\', out of stock",
					boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE", wxICON_WARNING | wxOK);
				quan--;
				return;
			}
			if (mPropertyManager->IsShown()) {
				//update product details
				stock->SetValue(wxVariant(static_cast<std::int32_t>(curStock - quan)));
				LoadProductDetails(*iter);
			}
		}
		else {
			if (mCurSaleuuid == boost::uuids::nil_uuid()) {
				mCurSaleuuid = uuidGen();
				SetActiveSaleIdText(mCurSaleuuid);
			}

			auto& v = iter->first;
			pof::base::data::row_t rowSale;

			auto& vS = rowSale.first;
			vS.resize(pof::SaleManager::MAX);

			vS[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			vS[pof::SaleManager::PRODUCT_UUID] = v[pof::ProductManager::PRODUCT_UUID];
			vS[pof::SaleManager::PRODUCT_NAME] = v[pof::ProductManager::PRODUCT_NAME];
			vS[pof::SaleManager::PRODUCT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];
			vS[pof::SaleManager::PRODUCT_QUANTITY] = static_cast<std::uint64_t>(1);
			vS[pof::SaleManager::PRODUCT_EXT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];

			mProductLabels.emplace_back(LabelInfo{});
			LoadLabelDetails(mProductLabels.back(), *iter);
			mProductLabels.back().mQuantity *= boost::variant2::get<std::uint64_t>(vS[pof::SaleManager::PRODUCT_QUANTITY]);
			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));

			size_t count = wxGetApp().mSaleManager.GetSaleData()->GetDatastore().size();
			m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);
		}
		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
		UpdateSaleDisplay();
		CheckEmpty();
		mScanProductValue->Clear();
		mScanProductValue->SetFocus();
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}


}

void pof::SaleView::OnProductPropertyChanged(wxPropertyGridEvent& evt)
{
	wxPGProperty* props = evt.GetProperty();
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk() || !props || props->IsCategory()) return;
	try {
		const size_t n = boost::lexical_cast<size_t>(evt.GetPropertyName());
		const wxVariant& v = evt.GetPropertyValue();
		const size_t idx = pof::DataModel::GetIdxFromItem(item);
		auto& labelInfo = mProductLabels[idx];
		switch (n)
		{
		case 0:
			labelInfo.mProductName = std::move(v.GetString().ToStdString());
			break;
		case 1:
			labelInfo.mStrength = std::move(v.GetString().ToStdString());
			break;
		case 2:
			labelInfo.mStrengthType = std::move(v.GetString().ToStdString());
			break;
		case 4:
		{
			auto& arr = props->GetChoices();
			labelInfo.mDirForUse = std::move(arr.GetLabel(v.GetInteger()).ToStdString());
		}
			break;
		case 5:
		{
			auto& arr = props->GetChoices();
			labelInfo.mWarning = std::move(arr.GetLabel(v.GetInteger()).ToStdString());
		}
			break;
		case 6:
			break;
		default:
			break;
		}

	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
	}

}

void pof::SaleView::OnReturnLastSale(wxCommandEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Return", wxICON_INFORMATION | wxOK);
		return;
	}

	if (mLocked) {
		mInfoBar->ShowMessage("Sale is locked, check out or clear sale to unlock");
		return;
	}

	if (wxMessageBox("Are you sure you want to return last sale?", "Sale", wxICON_INFORMATION | wxYES_NO) == wxNO) return;

	wxIcon cop;
	cop.CopyFromBitmap(wxArtProvider::GetBitmap("checkout"));
	wxBusyInfo info
	(
		wxBusyInfoFlags()
		.Parent(this)
		.Icon(cop)
		.Title("Returning last sale")
		.Text(fmt::format("Please wait...."))
		.Foreground(*wxBLACK)
		.Background(*wxWHITE)
		.Transparency(4 * wxALPHA_OPAQUE / 5)
	);

	auto lastSale = wxGetApp().mSaleManager.GetLastSale();
	if (!lastSale.has_value()) {
		wxMessageBox("Cannot get last sale", "Sale", wxICON_ERROR | wxOK);
		return;
	}
	if (lastSale->empty()) {
		mInfoBar->ShowMessage("No sale to return", wxICON_INFORMATION);
		return;
	}

	//show what you want to return



	bool status = false;
	for (auto& retSale : lastSale.value()) {
		auto& pid = boost::variant2::get<pof::base::data::duuid_t>(retSale.first[pof::SaleManager::PRODUCT_UUID]);
		auto& rid = boost::variant2::get<pof::base::data::duuid_t>(retSale.first[pof::SaleManager::SALE_UUID]);

		auto retQuan = wxGetApp().mSaleManager.GetReturnedProductQuan(pid, rid);
		if (!retQuan.has_value()) goto err;
		status = wxGetApp().mProductManager.ReturnToInventory(pid, retQuan.value());
		if (!status) goto err;
		status = wxGetApp().mSaleManager.ReturnFromSales(rid, pid);
		if (!status) goto err;

		//get product
		auto& prodDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
		auto prodIter = std::ranges::find_if(prodDatastore,
			[&](auto& r) -> bool {
				return boost::variant2::get<pof::base::data::duuid_t>(r.first[pof::ProductManager::PRODUCT_UUID])
				== pid;
			});
		if (std::end(prodDatastore) == prodIter) goto err;
		wxGetApp().mProductManager.RefreshRowFromDatabase(pid, *prodIter);


		auto name = boost::variant2::get<pof::base::data::text_t>(prodIter->first[pof::ProductManager::PRODUCT_NAME]);
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, fmt::format("Returned {} from {}", name, boost::lexical_cast<std::string>(rid)));
	}
	mInfoBar->ShowMessage("Return successfull", wxICON_INFORMATION);
	return;


err:
	mInfoBar->ShowMessage("Return failed", wxICON_ERROR);
	return;

}

bool pof::SaleView::OnAddMedicationsToSale(const pof::base::data& data)
{
	if (data.empty()) return false;
	for (auto& med : data) {
		auto prodIter = std::ranges::find_if(wxGetApp().mProductManager.GetProductData()->GetDatastore(),
			[&](const pof::base::data::row_t& row) -> bool {
				return boost::variant2::get<pof::base::data::duuid_t>(med.first[pof::PatientManager::MED_PRODUCT_UUID]) ==
				boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]);		
		});
		if (prodIter == wxGetApp().mProductManager.GetProductData()->GetDatastore().end()) continue;
		if (CheckExpired(*prodIter)) {
			if (wxMessageBox(fmt::format("{} is expired in the inventory, please check expiry date, do you wish to continue?",
				boost::variant2::get<pof::base::data::text_t>(prodIter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxYES_NO) == wxNO)
			{
				continue;
			}
		}
		bool status = CheckInStock(*prodIter);
		if (!status) {
			wxMessageBox(fmt::format("{} is out of stock",
				boost::variant2::get<pof::base::data::text_t>(prodIter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			continue;
		}
		//check that the stock is enough for what we are adding
		if (boost::variant2::get<std::uint64_t>(med.first[pof::PatientManager::MED_STOCK]) >
				boost::variant2::get<std::uint64_t>(prodIter->first[pof::ProductManager::PRODUCT_STOCK_COUNT])) {
			wxMessageBox(fmt::format("{} required is more than the current stock stock",
				boost::variant2::get<pof::base::data::text_t>(prodIter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			continue;
		}
		
		CheckControlled(*prodIter);

		if (mCurSaleuuid == boost::uuids::nil_uuid()) {
			mCurSaleuuid = uuidGen();
			SetActiveSaleIdText(mCurSaleuuid);
		}

		pof::base::data::row_t rowSale;
		auto& vS = rowSale.first;
		vS.resize(pof::SaleManager::MAX);
		vS[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
		vS[pof::SaleManager::PRODUCT_UUID] = boost::variant2::get<pof::base::data::duuid_t>(med.first[pof::PatientManager::MED_PRODUCT_UUID]);
		vS[pof::SaleManager::PRODUCT_NAME] = boost::variant2::get<pof::base::data::text_t>(med.first[pof::PatientManager::MED_NAME]);
		vS[pof::SaleManager::PRODUCT_QUANTITY] = boost::variant2::get<std::uint64_t>(med.first[pof::PatientManager::MED_STOCK]);
		vS[pof::SaleManager::PRODUCT_PRICE] = boost::variant2::get<pof::base::currency>(prodIter->first[pof::ProductManager::PRODUCT_UNIT_PRICE]);
		vS[pof::SaleManager::PRODUCT_EXT_PRICE] = boost::variant2::get<pof::base::currency>(prodIter->first[pof::ProductManager::PRODUCT_UNIT_PRICE]) 
					* static_cast<double>(boost::variant2::get<std::uint64_t>(med.first[pof::PatientManager::MED_STOCK]));

		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(prodIter->first[pof::ProductManager::PRODUCT_UUID]));
		

		mProductLabels.emplace_back(LabelInfo{});
		LoadLabelDetails(mProductLabels.back(), *prodIter);
		wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));

		size_t count = wxGetApp().mSaleManager.GetSaleData()->GetDatastore().size();
		m_dataViewCtrl1->EnsureVisible(pof::DataModel::GetItemFromIdx(count - 1), mProductNameCol);

	}
	bool ret = !wxGetApp().mSaleManager.GetSaleData()->GetDatastore().empty(); 
	mLocked = ret; // lock sale
	mSaleType = PATIENT;
	CheckEmpty();
	UpdateSaleDisplay();
	return ret;
}

bool pof::SaleView::CheckInStock(const pof::base::data::row_t& product)
{
	auto& v = product.first;
	if (!wxGetApp().bCheckOutOfStock) return true;
	try {
		std::uint64_t stock = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_STOCK_COUNT]);
		std::uint64_t compare = 0;
		if (wxGetApp().bUseMinStock) {
			compare = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_MIN_STOCK_COUNT]);
		}
		return (stock > compare);
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return false;
	}
}

bool pof::SaleView::CheckProductWarning(const pof::base::data::duuid_t& pid)
{
	auto warns = wxGetApp().mProductManager.GetWarning(pid);
	if (!warns) {
		return false;
	}
	wxArrayString values;
	std::vector<std::string> mCritical;
	values.reserve(warns->size());
	mCritical.reserve(warns->size());
	for (auto& tup : *warns){
		values.push_back(std::get<1>(tup));
		if (wxGetApp().bAlertCriticalWarnings) {
			if (std::get<0>(tup) == pof::ProductManager::CRITICAL) {
				mCritical.emplace_back(std::get<1>(tup));
			}
		}
	}
	values.Shrink();
	mCritical.shrink_to_fit();
	wxPGChoices choices;
	choices.Set(values);
	warning->SetChoices(std::move(choices));
	if (wxGetApp().bAlertCriticalWarnings && !mCritical.empty()) {
		auto colorbg = mInfoBar->GetBackgroundColour();
		mDataPane->Layout();
		mInfoBar->ShowMessage(fmt::format("{}", fmt::join(mCritical, ",")), wxICON_WARNING);
		mInfoBar->Layout();
		mDataPane->Refresh();
	}
	else {
		mInfoBar->Dismiss();
	}

	return true;
}

void pof::SaleView::OnContextMenu(wxDataViewEvent& evt)
{
	auto item = evt.GetItem();
	if (!item.IsOk()) return;

	wxMenu* menu = new wxMenu;
	menu->Append(ID_REMOVE_DISCOUNT, "Remove discount", nullptr);

	m_dataViewCtrl1->PopupMenu(menu);

}

bool pof::SaleView::CheckProductClass(const pof::base::data::row_t& product)
{
	auto& v = product.first;
	if (!wxGetApp().bCheckPOM) return false;
	try {
		auto& cass = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_CLASS]);
		return (cass == "POM");
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return false;
	}
	return true;
}

bool pof::SaleView::CheckExpired(const pof::base::data::row_t& product)
{
	auto& v = product.first;
	if (!wxGetApp().bCheckExpired) return false;
	try {
		auto items = wxGetApp().mProductManager.DoExpiredProducts();
		if (!items.has_value()) return false;
		return (std::ranges::any_of(items.value(), [&](const wxDataViewItem& i) ->bool {
			const size_t idx = pof::DataModel::GetIdxFromItem(i);
			auto& r = wxGetApp().mProductManager.GetProductData()->GetDatastore()[idx];
			return (boost::variant2::get<boost::uuids::uuid>(product.first[pof::ProductManager::PRODUCT_UUID])
					== boost::variant2::get<boost::uuids::uuid>(r.first[pof::ProductManager::PRODUCT_UUID]));
		}));
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return false;
	}
	return true;
}

void pof::SaleView::OnDiscount(wxCommandEvent& evt)
{
	if (wxGetApp().mSaleManager.GetSaleData()->GetDatastore().empty()){
		mInfoBar->ShowMessage("Sale is empty");
		return;
	}

	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk())
	{
		wxMessageBox("No item selected", "Sales", wxICON_INFORMATION | wxOK);
		return;
	}
	
	
	auto str = wxGetTextFromUser("Please enter discount amount e.g 50").ToStdString();
	if (str.empty()) return;

	if (!std::ranges::all_of(str, [&](char c) ->bool { return std::isdigit(c); })) {
		wxMessageBox("Invalid discount amount", "Sales", wxICON_ERROR | wxOK);
		return;
	}

	pof::base::currency discount(str);
	auto& s = wxGetApp().mSaleManager.GetSaleData()->GetDatastore()[pof::DataModel::GetIdxFromItem(item)];
	auto& pid = boost::variant2::get<boost::uuids::uuid>(s.first[pof::SaleManager::PRODUCT_UUID]);
	auto& extprice = boost::variant2::get<pof::base::currency>(s.first[pof::SaleManager::PRODUCT_EXT_PRICE]);

	if (discount >= extprice) {
		wxMessageBox("Cannot have discount greater than or same as the extact price of the item", "Sale", wxICON_WARNING | wxOK);
		return;
	}

	auto&& [i, t] = mDiscounts.insert({ pid, discount });
	if (!t) {
		//already inside, updating
		extprice += i->second;
		mTotalDiscount -= i->second;

		i->second = discount;
	}
	mTotalDiscount += discount;
	extprice -= discount;

	wxGetApp().mSaleManager.GetSaleData()->ItemChanged(item);
	UpdateSaleDisplay();
}


bool pof::SaleView::CheckControlled(const pof::base::data::row_t& product)
{
	auto& v = product.first;
	try {
		auto& cass = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_CLASS]);
		auto& name = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]);
		auto& puid = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]);
		//check the privilage
		bool c = (cass == "CONTROLLED");
		if (c) {
			wxBusyCursor cusor;
			if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST) 
				 && !wxGetApp().bAllowSellControlledMed){
				wxMessageBox("User account cannot sell controlled medications", "Sales", wxICON_INFORMATION | wxOK);
				return false;
			}
			if (!wxGetApp().mPoisonBookManager.IsBookCreated(puid)) {
				wxMessageBox(fmt::format("No controlled drug register created for {}", name), "Sales", wxICON_WARNING | wxOK);
				return false;
			}
			if (wxGetApp().bAlwaysCreateEntryIntoRegister || wxMessageBox(fmt::format("{} is a controlled drug, do you want to create an entry into the poison book register?", name), "Sales", wxICON_INFORMATION | wxYES_NO) == wxYES) {
				//create a dialog if we have not before on this sale
				if (!std::get<2>(mPosionBookDetails)) {
				
					wxDialog dialog(this, wxID_ANY, "Add Control drug entry", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

					//dialog.SetSizeHints(wxDefaultSize, wxDefaultSize);
					dialog.SetBackgroundColour(*wxWHITE);
					wxDialog* d = std::addressof(dialog);

					wxBoxSizer* bSizer1;
					bSizer1 = new wxBoxSizer(wxVERTICAL);

					wxPanel* m_panel1 = new wxPanel(d, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
					m_panel1->SetBackgroundColour(wxColour(255, 255, 255));

					wxBoxSizer* bSizer2;
					bSizer2 = new wxBoxSizer(wxVERTICAL);

					auto TitleText = new wxStaticText(m_panel1, wxID_ANY, wxT("Please enter poision book details"), wxDefaultPosition, wxDefaultSize, 0);
					TitleText->Wrap(-1);
					TitleText->SetFont(wxFontInfo().Bold().AntiAliased());
					bSizer2->Add(TitleText, 0, wxALL, 5);

					auto m_panel4 = new wxPanel(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

					wxStaticBoxSizer* sbSizer7 = new wxStaticBoxSizer(new wxStaticBox(m_panel4, wxID_ANY, wxT("Patient Details")), wxVERTICAL);


					auto m_scrolledWindow2 = new wxPanel(sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
					wxFlexGridSizer* fgSizer2;
					fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
					fgSizer2->AddGrowableCol(1);
					fgSizer2->SetFlexibleDirection(wxBOTH);
					fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

					auto patientName = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Patient Name"), wxDefaultPosition, wxDefaultSize, 0);
					patientName->Wrap(-1);
					fgSizer2->Add(patientName, 0, wxALL, 5);

					auto patientNameValue = new wxTextCtrl(m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
					patientNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
					patientNameValue->SetMaxLength(250);
					fgSizer2->Add(patientNameValue, 1, wxALL | wxEXPAND, 5);


					auto patientAddress = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Patient Address"), wxDefaultPosition, wxDefaultSize, 0);
					patientAddress->Wrap(-1);
					fgSizer2->Add(patientAddress, 0, wxALL, 5);

					auto patientAddressValue = new wxTextCtrl(m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
					patientAddressValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
					patientAddressValue->SetMaxLength(250);
					fgSizer2->Add(patientAddressValue, 1, wxALL | wxEXPAND, 5);

					m_scrolledWindow2->SetSizer(fgSizer2);
					m_scrolledWindow2->Layout();
					fgSizer2->Fit(m_scrolledWindow2);
					sbSizer7->Add(m_scrolledWindow2, 0, wxEXPAND | wxALL, 5);


					m_panel4->SetSizer(sbSizer7);
					m_panel4->Layout();
					sbSizer7->Fit(m_panel4);
					
					bSizer2->Add(m_panel4, 1, wxEXPAND | wxALL, 5);

					m_panel1->SetSizer(bSizer2);
					m_panel1->Layout();
					bSizer2->Fit(m_panel1);

					bSizer1->Add(m_panel1, 1, wxEXPAND | wxALL, 5);


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

					if(d->ShowModal() == wxID_CANCEL) return false;
					
					std::get<0>(mPosionBookDetails) = patientNameValue->GetValue().ToStdString();
					std::get<1>(mPosionBookDetails) = patientAddressValue->GetValue().ToStdString();
				}
				std::get<2>(mPosionBookDetails) = true;
				pof::base::data::row_t prec; 
				auto& v = prec.first;
				v.resize(pof::PoisonBookManager::MAX);

				v[pof::PoisonBookManager::PUID] = puid;
				v[pof::PoisonBookManager::PNAME] = std::get<0>(mPosionBookDetails);
				v[pof::PoisonBookManager::PADDY] = std::get<1>(mPosionBookDetails);
				v[pof::PoisonBookManager::PHARMNAME] = ""s;
				v[pof::PoisonBookManager::ISVERIFED] = static_cast<std::uint64_t>(0);
				v[pof::PoisonBookManager::QUAN] = static_cast<std::uint64_t>(0);
				v[pof::PoisonBookManager::STARTSTOCK] = static_cast<std::uint64_t>(0);
				v[pof::PoisonBookManager::RUNBALANCE] = static_cast<std::uint64_t>(0);
				v[pof::PoisonBookManager::DATE] = pof::base::data::clock_t::now();
				
				mPosionBookEntries.insert(std::make_pair(puid, std::move(prec)));
				mInfoBar->ShowMessage(fmt::format("Posion book entry for {} successful", name));
				return true;
			}
		}
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return false;
	}


	return false;
}

std::optional<pof::base::data::iterator> pof::SaleView::CheckAlreadyAdded(const pof::base::data::text_t& productName)
{
	auto& datastore = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();
	auto iter = std::ranges::find_if(datastore, [&](const pof::base::data::row_t& row) -> bool {
		return productName == (boost::variant2::get<pof::base::data::text_t>(row.first[pof::SaleManager::PRODUCT_NAME]));
	});
	if (iter == datastore.end()) return std::nullopt;

	//update the quantity;
	auto& quan = boost::variant2::get<std::uint64_t>(iter->first[pof::SaleManager::PRODUCT_QUANTITY]);
	quan++;
	return iter;
}

void pof::SaleView::ProductNameKeyEvent()
{
	mProductNameValue->Bind(wxEVT_CHAR, [&](wxKeyEvent& evt) {
		spdlog::info("{:d} Key code", evt.GetKeyCode());
		if (evt.GetKeyCode() == WXK_DOWN) {
			spdlog::info("Setting focus");
			mSearchPopup->CaptureFocus();
		}
		else {
			evt.Skip();
		}
	});
}

void pof::SaleView::LoadProductDetails(const pof::base::data::row_t& product)
{
	try {
		auto& v = product.first;
		auto& pName = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]);
		productName->SetValue(pName);
		
		auto& dirForUse = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_USAGE_INFO]);
		wxArrayString arrString = SplitIntoArrayString(dirForUse);
		wxPGChoices choices;
		choices.Set(arrString);
		dirArray->SetChoices(choices);

		auto& genericName = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_GENERIC_NAME]);
		genArray->SetValue(wxVariant(genericName));

		auto& stren = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_STRENGTH]);
		strength->SetValue(stren);

		auto& stren_type = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_STRENGTH_TYPE]);
		strength_type->SetValue(stren_type);

		std::uint64_t pk = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_PACKAGE_SIZE]);
		packageSize->SetValue(static_cast<int>(pk));

		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));

	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
	}
}

void pof::SaleView::OnProductUpdate(pof::base::data::const_iterator prop)
{
	auto& sett = prop->second.second;
	auto& saleData = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();
	auto iter = std::ranges::find_if(saleData, [&](const pof::base::data::row_t& row) -> bool {
		return (boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::SaleManager::PRODUCT_UUID]) == 
		boost::variant2::get<pof::base::data::duuid_t>(prop->first[pof::ProductManager::PRODUCT_UUID]));
	});
	if (iter == saleData.end()) return;
	auto& s = iter->first;
	auto& p = prop->first;

	m_dataViewCtrl1->Freeze();
	if (sett.test(pof::ProductManager::PRODUCT_NAME)){
		s[pof::SaleManager::PRODUCT_NAME] = p[pof::ProductManager::PRODUCT_NAME];
	}
	else if (sett.test(pof::ProductManager::PRODUCT_UNIT_PRICE)) {
		s[pof::SaleManager::PRODUCT_PRICE] = p[pof::ProductManager::PRODUCT_UNIT_PRICE];
		s[pof::SaleManager::PRODUCT_EXT_PRICE] = (boost::variant2::get<pof::base::currency>(p[pof::ProductManager::PRODUCT_UNIT_PRICE]) *
		static_cast<double>(boost::variant2::get<std::uint64_t>(s[pof::SaleManager::PRODUCT_QUANTITY])));
	}

	LoadProductDetails(*prop);
	m_dataViewCtrl1->Thaw();
	m_dataViewCtrl1->Refresh();
	UpdateSaleDisplay();
}

//the refreshing might be an overkill
void pof::SaleView::SetActiveSaleIdText(const boost::uuids::uuid& saleId)
{
	//TODO: size issues.
	mBottomTools->Freeze();
	mActiveSaleId->Freeze();
	if (saleId == boost::uuids::nil_uuid()){
		mActiveSaleId->SetLabelText("No Active Sale");
	}
	else {
		std::ostringstream os;
		os << saleId;
		mActiveSaleId->SetLabelText(fmt::format("Active Sale - {}", os.str()));
	}
	mActiveSaleId->Update();
	mActiveSaleTextItem->SetMinSize(mActiveSaleId->GetMinSize());
	mActiveSaleId->Thaw();
	mActiveSaleId->Refresh();
	
	mBottomTools->Thaw();
	mBottomTools->Realize();
	mBottomTools->Refresh();
	this->Layout();
}

void pof::SaleView::OnRemoveDiscount(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;

	auto& s = wxGetApp().mSaleManager.GetSaleData()->GetDatastore()[pof::DataModel::GetIdxFromItem(item)];
	auto& pid = boost::variant2::get<boost::uuids::uuid>(s.first[pof::SaleManager::PRODUCT_UUID]);
	auto& extprice = boost::variant2::get<pof::base::currency>(s.first[pof::SaleManager::PRODUCT_EXT_PRICE]);

	auto i = mDiscounts.find(pid);
	if (i == mDiscounts.end()) return;
	
	extprice += i->second;
	mTotalDiscount -= i->second;

	mDiscounts.erase(pid);
	wxGetApp().mSaleManager.GetSaleData()->ItemChanged(item);
	UpdateSaleDisplay(); 
}

void pof::SaleView::ReloadLabelInfo(const pof::base::data::duuid_t& suid)
{
	mProductLabels.clear();
	auto labels = wxGetApp().mSaleManager.LoadLabels(suid);
	if (!labels.has_value()) {
		wxMessageBox("Cannot load sale labels", "Sale", wxICON_ERROR | wxOK);
		return;
	}
	nl::json json = nl::json::parse(labels.value());
	try {
		if (!json.is_array()) {
			wxMessageBox("Invalid label format in sale database", "Sale", wxICON_ERROR | wxOK);
			return;
		}
		for (auto iter = json.begin(); iter != json.end(); iter++) {
			auto& obj = *iter;
			LabelInfo info;
			info.mProductName = static_cast<std::string>(obj["productName"]);
			info.mDirForUse = static_cast<std::string>(obj["dirForUse"]);
			info.mWarning = static_cast<std::string>(obj["warning"]);
			info.mQuantity = static_cast<std::uint64_t>(obj["quantity"]);
			info.mStrength = static_cast<std::string>(obj["strength"]);
			info.mStrengthType = static_cast<std::string>(obj["strengthType"]);
			info.mFormulation = static_cast<std::string>(obj["formulation"]);

			mProductLabels.emplace_back(std::move(info));
		}
	}
	catch (nl::json::exception& exp) {
		spdlog::error(exp.what());
		return;
	}


}

void pof::SaleView::StorePoisonBookEnteries()
{
	if (mPosionBookEntries.empty()) return;
	auto& saledata = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();

	for (auto& s : saledata) {
		auto& p = boost::variant2::get<pof::base::data::duuid_t>(s.first[pof::SaleManager::PRODUCT_UUID]);
		auto iter = mPosionBookEntries.find(p);
		if (iter == mPosionBookEntries.end()) continue;
		auto& v = iter->second.first;

		v[pof::PoisonBookManager::QUAN] = boost::variant2::get<std::uint64_t>(s.first[pof::SaleManager::PRODUCT_QUANTITY]);

		wxGetApp().mPoisonBookManager.GetBook()->StoreData(std::move(iter->second));
	}
}

wxHtmlPrintout* pof::SaleView::CreateHtmlReciept()
{
	auto htmlprintout = new wxHtmlPrintout("Receipt");
	std::stringstream out; 

	html::Document document("Reciept");
	html::Div main("container");


	document.head() << HTML::Meta("utf-8")
		<< HTML::Meta("viewport", "width=device-width, initial-scale=1, shrink-to-fit=no");
	auto ap = wxGetApp().GetAssertsPath();
	ap /= "bootstrap.css";

	document.head() << HTML::Style(receiptcss.str().c_str());
	document.head() << HTML::Style(".navbar{margin-bottom:20px;}");

	// Body
	document.body().cls("bg-light");


	main << HTML::Header1("Welcome to HTML").id("anchor_link_1");
	main << "Text directly in the body.";
	main << HTML::Text("Text directly in the body. ") << HTML::Text("Text directly in the body.") << HTML::Break()
		<< HTML::Text("Text directly in the body.");
	main << HTML::Paragraph("This is the way to go for a big text in a multi-line paragraph.");
	main << HTML::Link("Google", "http://google.com").cls("my_style");
	main << (HTML::Paragraph("A paragraph. ").style("font-family:arial")
		<< HTML::Text("Text child.") << HTML::Break() << HTML::Text("And more text."));

	main << (HTML::List()
		<< (HTML::ListItem("Text item"))
		<< (HTML::ListItem() << HTML::Link("Github Link", "http://srombauts.github.io").title("SRombaut's Github home page"))
		<< (HTML::ListItem() << (HTML::List()
			<< HTML::ListItem("val1")
			<< HTML::ListItem("val2"))));

	main << (HTML::Table().cls("table table-hover table-sm")
		<< HTML::Caption("Table caption")
		<< (HTML::Row() << HTML::ColHeader("A") << HTML::ColHeader("B"))
		<< (HTML::Row() << HTML::Col("Cell_11") << HTML::Col("Cell_12"))
		<< (HTML::Row() << HTML::Col("Cell_21") << (HTML::Col() << HTML::Link("Wikipedia", "https://www.wikipedia.org/")))
		<< (HTML::Row() << HTML::Col("") << HTML::Col("Cell_32")));

	main << HTML::Small("Copyright Sebastien Rombauts @ 2017-2021");

	main << HTML::Link().id("anchor_link_2");

	// TODO HTML::Form(), InputCheckbox() etc

	document << std::move(main);

	out << document;
	htmlprintout->SetHtmlText(out.str());
	return htmlprintout;
}

void pof::SaleView::SaveDiscounts(const boost::uuids::uuid& saleID)
{
	auto info = wxGetApp().mSaleManager.GetInfo(saleID);
	if (!info.has_value()) {
		mInfoBar->ShowMessage("Failed to save discounts");
		return;
	}
	try {
		if (info->empty()) {
			//no info for this saleID, the saleID is not in the table
			if (mDiscounts.empty()) return; //no discount to add to sale

			nl::json object = nl::json::object();
			nl::json disarray = nl::json::array();
			for (auto& dis : mDiscounts)
			{
				nl::json discount = nl::json::object();
				discount["pid"] = boost::lexical_cast<std::string>(dis.first);
				discount["amount"] = boost::lexical_cast<float>(dis.second);

				disarray.push_back(discount);
			}
			object["discounts"] = disarray;
			wxGetApp().mSaleManager.SaveInfo(saleID, object.dump());
		}
		else {
			nl::json object = nl::json::parse(*info);
			auto i = object.find("discounts");
			if (i != object.end()) {
				nl::json disarray = nl::json::array();
				for (auto& dis : mDiscounts)
				{
					nl::json discount = nl::json::object();
					discount["pid"] = boost::lexical_cast<std::string>(dis.first);
					discount["amount"] = boost::lexical_cast<float>(dis.second);

					disarray.push_back(discount);
				}
				object["discounts"] = disarray;
			}
			else {
				//clear the discounts and load the updated one for this sale
				nl::json& disarray = *i;
				disarray.clear();

				for (auto& dis : mDiscounts)
				{
					nl::json discount = nl::json::object();
					discount["pid"] = boost::lexical_cast<std::string>(dis.first);
					discount["amount"] = boost::lexical_cast<float>(dis.second);

					disarray.push_back(discount);
				}
				object["discounts"] = disarray;

			}
			wxGetApp().mSaleManager.SaveInfo(saleID, object.dump());
		}
	}
	catch (nl::json::exception& exp) {
		spdlog::error(exp.what());
		wxMessageBox("Critial error in saving discount", "Sale", wxICON_ERROR | wxOK);
	}
}

void pof::SaleView::RestoreDiscounts(const boost::uuids::uuid& saleID)
{
	auto info = wxGetApp().mSaleManager.GetInfo(saleID);
	if (!info.has_value()) {
		mInfoBar->ShowMessage("Failed to retore discounts");
		return;
	}
	try {
		if (info->empty()) return;//nothing to retore
		nl::json object = nl::json::parse(*info);
		auto i = object.find("discounts");
		if (i == object.end()) return; //no dicounts

		auto& disarray = *i;
		mDiscounts.clear();

		for (const auto& discount : disarray) {
			auto pid = boost::lexical_cast<boost::uuids::uuid>(static_cast<std::string>(discount["pid"]));
			auto amount = pof::base::currency(static_cast<float>(discount["amount"]));

			mDiscounts.insert({ pid, amount });
		}
	}
	catch (std::exception& exp){
		spdlog::error(exp.what());
		wxMessageBox("Critical error in restoring discounts", "Sales", wxICON_ERROR | wxOK);
		return;
	}
}

void pof::SaleView::LoadLabelDetails(LabelInfo& info, const pof::base::data::row_t& product)
{

	auto& v = product.first;
	info.mProductName = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME]);
		
	auto& dirForUse = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_USAGE_INFO]);
	wxArrayString arrString = SplitIntoArrayString(dirForUse);
	if (!arrString.empty()) info.mDirForUse = arrString[0].ToStdString();

	info.mStrength = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_STRENGTH]);
	info.mStrengthType = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_STRENGTH_TYPE]);

	auto warns = wxGetApp().mProductManager.GetWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
	if (warns && !warns.value().empty()) {

		info.mWarning = (*warns)[0].second;
	}

	info.mQuantity = boost::variant2::get<std::uint64_t>(v[pof::ProductManager::PRODUCT_PACKAGE_SIZE]);
	info.mFormulation = boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_FORMULATION]);
}

void pof::SaleView::SaveLabelInfo(const pof::base::data::duuid_t& suid)
{
	if (mProductLabels.empty()) return;
	nl::json arr = nl::json::array();
	for (const auto& d : mProductLabels) {
		auto obj = nl::json::object();
		obj["productName"] = d.mProductName;
		obj["dirForUse"] = d.mDirForUse;
		obj["warning"] = d.mWarning;
		obj["quantity"] = d.mQuantity;
		obj["strength"] = d.mStrength;
		obj["strengthType"] = d.mStrengthType;
		obj["formulation"] = d.mFormulation;
		arr.push_back(std::move(obj));
	}
	if (!wxGetApp().mSaleManager.SaveLabels(suid, arr.dump())) {
		spdlog::error("Failed to save label information");
	}
	spdlog::info(arr.dump());
}



