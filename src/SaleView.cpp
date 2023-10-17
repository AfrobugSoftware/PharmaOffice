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
	EVT_DATAVIEW_ITEM_BEGIN_DRAG(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnBeginDrag)
	EVT_DATAVIEW_ITEM_DROP_POSSIBLE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDropPossible)
	EVT_DATAVIEW_ITEM_DROP(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDrop)
	EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnEditingStarted)
	EVT_DATAVIEW_ITEM_EDITING_DONE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnEditingDone)
	EVT_DATAVIEW_ITEM_START_EDITING(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnValueChanged)
	EVT_DATAVIEW_SELECTION_CHANGED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnSelected)
	EVT_DATAVIEW_ITEM_ACTIVATED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnSelected)
	EVT_TEXT(pof::SaleView::ID_PRODUCT_SEARCH_NAME, pof::SaleView::OnProductNameSearch)
	EVT_SEARCH(pof::SaleView::ID_PRODUCT_SCAN, pof::SaleView::OnScanBarCode)
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
	mCurSaleuuid = boost::uuids::nil_uuid();
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	mTopTools = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_HORZ_TEXT | wxAUI_TB_OVERFLOW | wxNO_BORDER);
	mTopTools->SetMinSize(wxSize(-1, 40));

	mProductNameText = new wxStaticText(mTopTools, wxID_ANY, wxT("Product Name: "), wxDefaultPosition, wxDefaultSize, 0);
	mProductNameText->SetBackgroundColour(*wxWHITE);
	mProductNameText->Wrap(-1);
	mTopTools->AddControl(mProductNameText);

	mTopTools->AddSpacer(10);

	mProductNameValue = new wxSearchCtrl(mTopTools, ID_PRODUCT_SEARCH_NAME, wxEmptyString, wxDefaultPosition, wxSize(300, -1));
	//mProductNameValue->SetValidator(wxTextValidator{wxFILTER_SPACE});
	mProductNameValue->SetHint("Search product by name");
	mTopTools->AddControl(mProductNameValue);

	mTopTools->AddSpacer(10);

	mScanProduct = new wxStaticText(mTopTools, wxID_ANY, wxT("Scan Product: "), wxDefaultPosition, wxDefaultSize, 0);
	mScanProduct->SetBackgroundColour(*wxWHITE);
	mScanProduct->Wrap(-1);
	mTopTools->AddControl(mScanProduct);

	mTopTools->AddSpacer(10);

	mScanProductValue = new wxSearchCtrl(mTopTools, ID_PRODUCT_SCAN, wxEmptyString, wxDefaultPosition, wxSize(300, -1), 0);
	mScanProductValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	mScanProductValue->SetHint("Scan products");
	mTopTools->AddControl(mScanProductValue);

	mTopTools->AddStretchSpacer();
	
	mTopTools->AddTool(ID_PACKS, wxT("Rx Packs"), wxArtProvider::GetBitmap(wxART_FOLDER));
	mTopTools->AddTool(ID_PRINT_LABELS, wxT("Print As Labels"), wxArtProvider::GetBitmap("download"));
	mTopTools->AddTool(ID_REMOVE_PRODUCT, wxT("Remove Product"), wxArtProvider::GetBitmap("action_remove"));
	mTopTools->AddTool(ID_HIDE_PRODUCT_VIEW_PROPERTY, wxT("Hide product view"), wxArtProvider::GetBitmap("pen"));
	mTopTools->AddTool(ID_FORM_M, wxT("Generate FORM M"), wxArtProvider::GetBitmap("application"));
	
	mTopTools->Realize();

	
	mDataPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);


	wxPanel* panel = new wxPanel(mDataPane, wxID_ANY);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	mInfoBar = new wxInfoBar(panel, wxID_ANY);
	
	m_dataViewCtrl1 = new wxDataViewCtrl(panel, ID_SALE_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	m_dataViewCtrl1->AssociateModel(wxGetApp().mSaleManager.GetSaleData().get());


	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("PRODUCT NAME"), pof::SaleManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 600, wxALIGN_CENTER);
	mQuantityColumn = m_dataViewCtrl1->AppendTextColumn(wxT("QUANTITY"), pof::SaleManager::PRODUCT_QUANTITY, wxDATAVIEW_CELL_EDITABLE, 100, wxALIGN_CENTER);
	mPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("PRICE"), pof::SaleManager::PRODUCT_PRICE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mExtPriceColumn = m_dataViewCtrl1->AppendTextColumn(wxT("EXT PRICE"), pof::SaleManager::PRODUCT_EXT_PRICE);
	
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
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	mSaleDisplayPane = new wxPanel(mSaleOutputPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);


	bSizer3->Add(0, 0, 1, wxEXPAND, 5);

	mTextOutPut = new wxPanel(mSaleDisplayPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER | wxTAB_TRAVERSAL);
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer(4, 2, 5, 5);

	const int fontSize = 12;
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
	bSizer3->Add(mTextOutPut, 1, wxEXPAND | wxALL, 5);


	mSaleDisplayPane->SetSizer(bSizer3);
	mSaleDisplayPane->Layout();
	bSizer3->Fit(mSaleDisplayPane);
	bSizer2->Add(mSaleDisplayPane, 1, wxEXPAND | wxALL, 5);

	mSalePaymentButtonsPane = new wxPanel(mSaleOutputPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER | wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);


	mClear = new wxButton(mSalePaymentButtonsPane, ID_CLEAR, wxT("CLEAR"), wxDefaultPosition, wxSize(-1, 80), wxSIMPLE_BORDER);
	mClear->SetBackgroundColour(*wxWHITE);
	bSizer4->Add(mClear, 0, wxALL, 5);

	mSave = new wxButton(mSalePaymentButtonsPane, ID_SAVE, wxT("SAVE"), wxDefaultPosition, wxSize(-1, 80), wxSIMPLE_BORDER);
	mSave->SetBackgroundColour(*wxWHITE);
	bSizer4->Add(mSave, 0, wxALL, 5);

	mCheckout = new wxButton(mSalePaymentButtonsPane, ID_CHECKOUT, wxT("CHECK OUT"), wxDefaultPosition, wxSize(-1, 80), wxSUNKEN_BORDER);
	mCheckout->SetBackgroundColour(*wxWHITE);
	bSizer4->Add(mCheckout, 0, wxALL, 5);

	bSizer4->Add(0, 0, 1, wxEXPAND, 5);

	mTotalAmountLabel = new wxStaticText(mSalePaymentButtonsPane, wxID_ANY, wxT("TOTAL AMOUNT: "), wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmountLabel->SetFont(wxFont(wxFontInfo(24).Bold().AntiAliased()));
	bSizer4->Add(mTotalAmountLabel, 0, wxALIGN_CENTER | wxALL, 5);

	mTotalAmount = new wxStaticText(mSalePaymentButtonsPane, wxID_ANY, fmt::format("{:cu}", pof::base::currency{}), wxDefaultPosition, wxDefaultSize, 0);
	mTotalAmount->SetFont(wxFont(wxFontInfo(24).Bold().AntiAliased()));
	bSizer4->Add(mTotalAmount, 0, wxALIGN_CENTER | wxALL, 5);
	
	mSalePaymentButtonsPane->SetSizer( bSizer4 );
	mSalePaymentButtonsPane->Layout();
	bSizer4->Fit( mSalePaymentButtonsPane );
	bSizer2->Add( mSalePaymentButtonsPane, 0, wxEXPAND | wxALL, 5 );
	
	
	mSaleOutputPane->SetSizer( bSizer2 );
	mSaleOutputPane->Layout();
	bSizer2->Fit( mSaleOutputPane );

	bSizer1->Add(mTopTools, 0, wxALL, 0);
	bSizer1->Add(mDataPane, 1, wxEXPAND | wxALL, 0);
	bSizer1->Add( mSaleOutputPane, 0, wxEXPAND | wxALL, 0 );
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit(this);

	wxGetApp().mProductManager.GetProductData()->ConnectSlot(std::bind_front(&pof::SaleView::OnProductUpdate, this), pof::DataModel::Signals::UPDATE);

	SetupDropTarget();
	CreateSpecialColumnHandlers();
	CreateSearchPopup();
	ProductNameKeyEvent(); //experiment

	wxGetApp().mPrintManager->printSig.connect(std::bind_front(&pof::SaleView::OnSaleComplete, this));
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

	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	pof::base::data& dataStore = wxGetApp().mSaleManager.GetSaleData()->GetDatastore();

	extPriceCol.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		auto& price = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_PRICE]);
		auto& quantiy = boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]);
		pof::base::currency extPrice = price * static_cast<double>(quantiy);
		return wxVariant(fmt::format("{:cu}", extPrice));
	};

	quantityCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		try {
			auto quan = static_cast<std::uint64_t>(atoi(value.GetString().ToStdString().c_str()));

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

			v[pof::SaleManager::PRODUCT_QUANTITY] = quan;
			v[pof::SaleManager::PRODUCT_EXT_PRICE] = extPrice;
			UpdateSaleDisplay();
			return true;
		}
		catch (const std::exception& exp) {
			spdlog::error(exp.what());
			return false;
		}
	};

	model->SetSpecialColumnHandler(pof::SaleManager::PRODUCT_QUANTITY, std::move(quantityCol));
	model->SetSpecialColumnHandler(pof::SaleManager::PRODUCT_EXT_PRICE, std::move(extPriceCol));
}

void pof::SaleView::CreateSearchPopup()
{
	auto sharedData = wxGetApp().mProductManager.GetProductData()->ShareDatastore();
	mSearchPopup = new pof::SearchPopup(mProductNameValue, sharedData, { {"NAME", pof::ProductManager::PRODUCT_NAME}, {"COST", pof::ProductManager::PRODUCT_UNIT_PRICE} }, {275, 100});
	mSearchPopup->sSelectedSignal.connect(std::bind_front(&pof::SaleView::OnSearchPopup, this));
}

void pof::SaleView::CreateProductDetails()
{
	//create the property grid
	productName = new wxStringProperty("PRODUCT NAME");
	strength = new wxStringProperty("UNIT STRENGTH");
	strength_type = new wxStringProperty("STRENGTH TYPE");
	genArray = new wxStringProperty("PRODUCT GENERIC NAME");
	dirArray = new wxEditEnumProperty("DIRECTION FOR USE");
	stock = new wxIntProperty("CURRENT STOCK");
	warning = new wxArrayStringProperty("WARNING");

	stock->Enable(false);
	//warning->Enable(false);
		
	mPropertyManager->Append(productName);
	mPropertyManager->Append(strength);
	mPropertyManager->Append(strength_type);
	mPropertyManager->Append(genArray);
	mPropertyManager->Append(dirArray);
	mPropertyManager->Append(stock);
	mPropertyManager->Append(warning);

	mProperties.insert({ genArray, [&](const wxVariant& value) {
				
	} });

	mProperties.insert({ dirArray, [&](const wxVariant& value) {
			
	}});
	
	//auto grid = mPropertyManager->GetGrid();
	mPropertyManager->SetBackgroundColour(*wxWHITE);
	mPropertyManager->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	mPropertyManager->SetCaptionTextColour(*wxBLACK);
}

void pof::SaleView::UpdateSaleDisplay()
{
	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	const pof::base::data& dataStore = model->GetDatastore();
	assert(model != nullptr && "failed model dynamic cast, pof::DataModel not a subclass of model");
	size_t quantity = dataStore.size(), exactQuantity = 0;

	pof::base::currency totalAmount;
	pof::base::currency discountAmount;
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
	mDiscountValue->SetLabel(fmt::format("{:cu}", discountAmount));
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
	if (datastore.empty()) return;
	if (wxMessageBox("Are you sure you want to clear current sale?", "SALE", wxICON_WARNING | wxYES_NO) == wxNO) return;

	wxGetApp().mSaleManager.GetSaleData()->Clear();
	if (mPropertyManager->IsShown()) {
		mPropertyManager->Hide();
		mDataPane->Layout();
		mDataPane->Refresh();
	}
	ResetSaleDisplay();
	mCurSaleuuid = boost::uuids::nil_uuid();

}

void pof::SaleView::OnCheckout(wxCommandEvent& evt)
{
	//sale dialog
	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	pof::base::data& dataStore = model->GetDatastore();
	assert(model != nullptr && "failed model dynamic cast, pof::DataModel not a subclass of model");
	if (dataStore.empty()) return;
	pof::base::currency totalAmount;
	try {
		totalAmount = std::accumulate(dataStore.begin(),
			dataStore.end(), totalAmount, [&](pof::base::currency v, const pof::base::data::row_t& i) {
				return v + boost::variant2::get<pof::base::currency>(i.first[pof::SaleManager::PRODUCT_EXT_PRICE]);

		});
	}
	catch (const std::exception& exp) {
		wxMessageBox(exp.what(), "FATAL ERROR", wxICON_ERROR | wxOK);
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
	wxGetApp().mPrintManager->gPrintState = pof::PrintManager::RECEIPT;
	wxGetApp().mPrintManager->PrintSaleReceipt(this);
}

void pof::SaleView::OnSaleComplete(bool status, size_t printState)
{
	if (!status) {
		wxMessageBox("Sale could not be completed, please contact admin", "Sale", wxICON_ERROR | wxOK);
		return;
	}
	//to put back when testing is over
	switch (printState)
	{
	case pof::PrintManager::RECEIPT:
		wxGetApp().mSaleManager.GetSaleData()->Clear();
		if (mPropertyManager->IsShown()) {
			mPropertyManager->Hide();
			mDataPane->Layout();
			mDataPane->Refresh();
		}
		ResetSaleDisplay();
		mInfoBar->ShowMessage("Sale complete");
		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::SALE, "Sale completed");
		break;
	case pof::PrintManager::REPRINT_RECEIPT:
		break;
	default:
		break;
	}
}

void pof::SaleView::OnSave(wxCommandEvent& evt)
{
	wxMessageBox("SAVE", "SAVE");
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
	mSearchPopup->SetSize(wxSize(sz.x + 100, 600));

	//mSearchPopup->Show();
	mSearchPopup->CaptureFocus();
	mSearchPopup->Popup();
	mSearchPopup->SearchString(pof::ProductManager::PRODUCT_NAME, searchString);
	mSearchPopup->SetFocus();
}

void pof::SaleView::OnRemoveProduct(wxCommandEvent& evt)
{
	auto item = m_dataViewCtrl1->GetSelection();
	if (!item.IsOk()) return;
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

	wxGetApp().mSaleManager.GetSaleData()->RemoveData(item);
	UpdateSaleDisplay();
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

}

void pof::SaleView::OnShowPacks(wxCommandEvent& evt)
{
	pof::PackView dialog(this, true);
	if (dialog.ShowModal() == wxID_OK) {
		wxBusyCursor cursor;
		auto prods = dialog.GetPackProducts();
		if (prods.empty()) return;

		if (mCurSaleuuid == boost::uuids::nil_uuid()) {
			mCurSaleuuid = uuidGen();
		}

		for (auto& prod : prods) {

			pof::base::data::row_t rowSale;
			auto& vS = rowSale.first;
			vS.resize(pof::SaleManager::MAX);
			vS[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			vS[pof::SaleManager::PRODUCT_UUID] = std::get<1>(prod);

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

			
			vS[pof::SaleManager::PRODUCT_NAME] = std::move(std::get<2>(prod));
			vS[pof::SaleManager::PRODUCT_QUANTITY] = std::get<3>(prod);
			vS[pof::SaleManager::PRODUCT_PRICE] = std::get<5>(prod);
			vS[pof::SaleManager::PRODUCT_EXT_PRICE] = std::get<6>(prod);

			CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(iter->first[pof::ProductManager::PRODUCT_UUID]));
			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));
		}
		UpdateSaleDisplay();
	}
}

void pof::SaleView::OnFormM(wxCommandEvent& evt)
{
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

void pof::SaleView::DropData(const pof::DataObject& dat)
{
	auto& meta = dat.GetMeta();
	auto& row = dat.GetSetData();

	if (mCurSaleuuid == boost::uuids::nil_uuid()) {
		mCurSaleuuid = uuidGen();
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
			wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale",
				boost::variant2::get<pof::base::data::text_t>(val.first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
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

			auto row = pof::base::data::row_t();
			row.first.resize(pof::SaleManager::MAX);
			auto& v = row.first;

			v[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			v[pof::SaleManager::PRODUCT_UUID] = val.first[pof::ProductManager::PRODUCT_UUID];
			v[pof::SaleManager::PRODUCT_NAME] = val.first[pof::ProductManager::PRODUCT_NAME];
			v[pof::SaleManager::PRODUCT_PRICE] = val.first[pof::ProductManager::PRODUCT_UNIT_PRICE];
			v[pof::SaleManager::PRODUCT_QUANTITY] = static_cast<std::uint64_t>(1);
			v[pof::SaleManager::PRODUCT_EXT_PRICE] = val.first[pof::ProductManager::PRODUCT_UNIT_PRICE];

			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(row));
		}
		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
		UpdateSaleDisplay();
	}
	else {
		spdlog::error("Drop data invalid or does not exist");
	}
}

void pof::SaleView::OnSearchPopup(const pof::base::data::row_t& row)
{
	try {
		if (mCurSaleuuid == boost::uuids::nil_uuid()){
			mCurSaleuuid = uuidGen();
		}

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

			pof::base::data::row_t rowSale;
			auto& vS = rowSale.first;
			vS.resize(pof::SaleManager::MAX);

			vS[pof::SaleManager::SALE_UUID] = mCurSaleuuid;
			vS[pof::SaleManager::PRODUCT_UUID] = v[pof::ProductManager::PRODUCT_UUID];
			vS[pof::SaleManager::PRODUCT_NAME] = v[pof::ProductManager::PRODUCT_NAME];
			vS[pof::SaleManager::PRODUCT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];
			vS[pof::SaleManager::PRODUCT_QUANTITY] = static_cast<std::uint64_t>(1);
			vS[pof::SaleManager::PRODUCT_EXT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];
			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));
		}
		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
		UpdateSaleDisplay();

		mProductNameValue->Clear(); 
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}
}

void pof::SaleView::OnScanBarCode(wxCommandEvent& evt)
{
	auto value = evt.GetString().ToStdString();
	if (value.empty()) return;
	constexpr auto inrange = [](int c) ->bool {return (c >= -1 && c <= 255); };
	bool isNum = std::ranges::all_of(value, [&](char c) -> bool { return (inrange(c) && std::isdigit(c)); });
	if (!isNum) {
		wxMessageBox("Unrecongised character in barcode", "SALE", wxICON_WARNING | wxOK);
		return;
	}
	if (mCurSaleuuid == boost::uuids::nil_uuid()) {
		mCurSaleuuid = uuidGen();
	}

	auto& datastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	try {
		auto iter = std::ranges::find_if(datastore, [&](const pof::base::data::row_t& row) -> bool {
			auto& barcode = boost::variant2::get<pof::base::data::text_t>(row.first[pof::ProductManager::PRODUCT_BARCODE]);
			return (value == barcode);
		});
		if (iter == datastore.end()) {

			wxMessageBox("NO SUCH PRODUCT IN STORE", "SALE PRODUCT", wxICON_WARNING | wxOK);
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
			wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale",
				boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
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

			wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));
		}
		CheckProductWarning(boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]));
		UpdateSaleDisplay();

		mScanProductValue->Clear();
	}
	catch (std::exception& exp) {
		spdlog::error(exp.what());
		return;
	}


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
	if (!values.empty()) warning->SetValue(wxVariant(std::move(values)));
	else warning->SetValue(wxVariant{});
	if (wxGetApp().bAlertCriticalWarnings && !mCritical.empty()) {
		auto colorbg = mInfoBar->GetBackgroundColour();
		mInfoBar->ShowMessage(fmt::format("{}", fmt::join(mCritical, ",")), wxICON_WARNING);
	}
	else {
		mInfoBar->Dismiss();
	}

	return true;
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



