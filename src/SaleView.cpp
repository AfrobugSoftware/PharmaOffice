#include "SaleView.h"
#include "Application.h"
BEGIN_EVENT_TABLE(pof::SaleView, wxPanel)
	EVT_BUTTON(pof::SaleView::ID_CHECKOUT, pof::SaleView::OnCheckout)
	EVT_BUTTON(pof::SaleView::ID_CLEAR, pof::SaleView::OnClear)
	EVT_BUTTON(pof::SaleView::ID_SAVE, pof::SaleView::OnSave)
	EVT_DATAVIEW_ITEM_BEGIN_DRAG(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnBeginDrag)
	EVT_DATAVIEW_ITEM_DROP_POSSIBLE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDropPossible)
	EVT_DATAVIEW_ITEM_DROP(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnDrop)
	EVT_DATAVIEW_ITEM_EDITING_STARTED(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnEditingStarted)
	EVT_DATAVIEW_ITEM_EDITING_DONE(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnEditingDone)
	EVT_DATAVIEW_ITEM_START_EDITING(pof::SaleView::ID_SALE_DATA_VIEW, pof::SaleView::OnValueChanged)
	EVT_TEXT(pof::SaleView::ID_PRODUCT_SEARCH_NAME, pof::SaleView::OnProductNameSearch)
	EVT_TEXT(pof::SaleView::ID_PRODUCT_SCAN, pof::SaleView::OnScanBarCode)
END_EVENT_TABLE()


pof::SaleView::SaleView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	mMainPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	mTopTools = new wxPanel(mMainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxHORIZONTAL);

	mProductNameText = new wxStaticText(mTopTools, wxID_ANY, wxT("Product Name: "), wxDefaultPosition, wxDefaultSize, 0);
	mProductNameText->Wrap(-1);
	bSizer7->Add(mProductNameText, 0, wxALIGN_CENTER | wxALL, 5);

	mProductNameValue = new wxSearchCtrl(mTopTools, ID_PRODUCT_SEARCH_NAME, wxEmptyString, wxDefaultPosition, wxSize(300, -1));
	mProductNameValue->SetValidator(wxTextValidator{wxFILTER_ALPHANUMERIC});
	mProductNameValue->SetHint("Search product by name");
	bSizer7->Add(mProductNameValue, 0, wxALL, 5);

	mScanProduct = new wxStaticText(mTopTools, wxID_ANY, wxT("Scan Product: "), wxDefaultPosition, wxDefaultSize, 0);
	mScanProduct->Wrap(-1);
	bSizer7->Add(mScanProduct, 0, wxALIGN_CENTER | wxALL, 5);

	mScanProductValue = new wxSearchCtrl(mTopTools, ID_PRODUCT_SCAN, wxEmptyString, wxDefaultPosition, wxSize(300, -1), 0);
	mScanProductValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	mScanProductValue->SetHint("Scan products");
	bSizer7->Add(mScanProductValue, 0, wxALL, 5);


	mTopTools->SetSizer(bSizer7);
	mTopTools->Layout();
	bSizer7->Fit(mTopTools);
	bSizer5->Add(mTopTools, 0, wxEXPAND | wxALL, 0);

	mDataPane = new wxPanel(mMainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_dataViewCtrl1 = new wxDataViewCtrl(mDataPane, ID_SALE_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES);
	m_dataViewCtrl1->AssociateModel(wxGetApp().mSaleManager.GetSaleData().get());


	mSerialNumber = m_dataViewCtrl1->AppendTextColumn(wxT("SERIAL NUMBER"), pof::SaleManager::PRODUCT_SERIAL_NUM, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("PRODUCT NAME"), pof::SaleManager::PRODUCT_NAME, wxDATAVIEW_CELL_INERT, 600, wxALIGN_CENTER);
	mProductNameCol = m_dataViewCtrl1->AppendTextColumn(wxT("PRODUCT CATEGORY"), pof::SaleManager::PRODUCT_CATEGORY, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mQuantityColumn = m_dataViewCtrl1->AppendTextColumn(wxT("QUANTITY"), pof::SaleManager::PRODUCT_QUANTITY, wxDATAVIEW_CELL_EDITABLE, 100, wxALIGN_CENTER);
	mPriceCol = m_dataViewCtrl1->AppendTextColumn(wxT("PRICE"), pof::SaleManager::PRODUCT_PRICE, wxDATAVIEW_CELL_INERT, 100, wxALIGN_CENTER);
	mExtPriceColumn = m_dataViewCtrl1->AppendTextColumn(wxT("EXT PRICE"), pof::SaleManager::PRODUCT_EXT_PRICE);
	auto choiceRender = new wxDataViewChoiceRenderer(wxArrayString(), wxDATAVIEW_CELL_INERT);
	m_dataViewCtrl1->AppendColumn(new wxDataViewColumn(wxT("DIRECTION FOR USE"), choiceRender, pof::SaleManager::MAX + 1, 200));
	bSizer6->Add(m_dataViewCtrl1, 1, wxALL | wxEXPAND, 0);


	mDataPane->SetSizer(bSizer6);
	mDataPane->Layout();
	bSizer6->Fit(mDataPane);
	bSizer5->Add(mDataPane, 1, wxEXPAND | wxALL, 0);


	mMainPane->SetSizer(bSizer5);
	mMainPane->Layout();
	bSizer5->Fit(mMainPane);
	bSizer1->Add(mMainPane, 1, wxEXPAND | wxALL, 0);

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


	mClear = new wxButton(mSalePaymentButtonsPane, ID_CLEAR, wxT("CLEAR"), wxDefaultPosition, wxSize(-1, 20), 0);
	bSizer4->Add(mClear, 0, wxALL, 5);

	mSave = new wxButton(mSalePaymentButtonsPane, ID_SAVE, wxT("SAVE"), wxDefaultPosition, wxSize(-1, 20), 0);
	bSizer4->Add(mSave, 0, wxALL, 5);

	mCheckout = new wxButton(mSalePaymentButtonsPane, ID_CHECKOUT, wxT("CHECK OUT"), wxDefaultPosition, wxSize(-1, 20), 0);
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
	bSizer1->Add( mSaleOutputPane, 0, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();

	SetupDropTarget();
	CreateSpecialColumnHandlers();
	CreateSearchPopup();
	ProductNameKeyEvent(); //experiment
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
	pof::DataModel::SpeicalColHandler_t dirForUseCol;
	pof::DataModel::SpeicalColHandler_t quantityCol;

	pof::DataModel* model = dynamic_cast<pof::DataModel*>(m_dataViewCtrl1->GetModel());
	pof::base::data& dataStore = model->GetDatastore();
	extPriceCol.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		auto& price = boost::variant2::get<pof::base::data::currency_t>(v[pof::SaleManager::PRODUCT_PRICE]);
		auto& quantiy = boost::variant2::get<std::uint64_t>(v[pof::SaleManager::PRODUCT_QUANTITY]);
		pof::base::currency extPrice = price * static_cast<double>(quantiy);
		return wxVariant(fmt::format("{:cu}", extPrice));
	};

	dirForUseCol.first = [&](size_t row, size_t col) -> wxVariant {
		wxArrayString choices;
		choices.push_back("Zino");
		choices.push_back("is");
		choices.push_back("the");
		choices.push_back("c++");
		choices.push_back("goat");
		return wxVariant(std::move(choices));
	};

	quantityCol.second = [&](size_t row, size_t col, const wxVariant& value) -> bool {
		auto& datum = dataStore[row];
		auto& v = datum.first;
		try {
			auto quan = static_cast<std::uint64_t>(atoi(value.GetString().ToStdString().c_str()));

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
	model->SetSpecialColumnHandler(pof::SaleManager::MAX + 1, std::move(dirForUseCol));
}

void pof::SaleView::CreateSearchPopup()
{
	auto sharedData = wxGetApp().mProductManager.GetProductData()->ShareDatastore();
	mSearchPopup = new pof::SearchPopup(mProductNameValue, sharedData, { {"NAME", pof::ProductManager::PRODUCT_NAME}, {"COST", pof::ProductManager::PRODUCT_UNIT_PRICE}});
	mSearchPopup->sSelectedSignal.connect(std::bind_front(&pof::SaleView::OnSearchPopup, this));
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

void pof::SaleView::Checkout()
{
}

void pof::SaleView::OnClear(wxCommandEvent& evt)
{
	wxMessageBox("CLEAR", "CLEARING");
}

void pof::SaleView::OnCheckout(wxCommandEvent& evt)
{
	wxMessageBox("CHECKOUT", "CHECKOUT");
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
	mSearchPopup->Popup();
	mSearchPopup->SearchString(pof::ProductManager::PRODUCT_NAME, searchString);

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
	if (row.has_value()) {
		auto& val = row.value();
		bool status = CheckInStock(val);
		if (!status) {
			wxMessageBox(fmt::format("{} is out of stock",
				boost::variant2::get<pof::base::data::text_t>(val.first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
		CheckExpired(val);
		status = CheckProductClass(val);
		if (status) {
			wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale",
				boost::variant2::get<pof::base::data::text_t>(val.first[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}

		auto row = pof::base::data::row_t();
		row.first.resize(pof::SaleManager::MAX);
		row.second.first.set(static_cast<std::underlying_type_t<pof::base::data::state>>(pof::base::data::state::CREATED));
		auto& v = row.first;
		v[pof::SaleManager::PRODUCT_SERIAL_NUM] = val.first[pof::ProductManager::PRODUCT_SERIAL_NUM];
		v[pof::SaleManager::PRODUCT_NAME] = val.first[pof::ProductManager::PRODUCT_NAME];
		v[pof::SaleManager::PRODUCT_CATEGORY] = "MEDICINE"s;
		v[pof::SaleManager::PRODUCT_PRICE] = val.first[pof::ProductManager::PRODUCT_UNIT_PRICE];
		v[pof::SaleManager::PRODUCT_QUANTITY] = static_cast<std::uint64_t>(1);
		v[pof::SaleManager::PRODUCT_EXT_PRICE] = val.first[pof::ProductManager::PRODUCT_UNIT_PRICE];

		wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(row));
		UpdateSaleDisplay();
	}
	else {
		spdlog::error("Drop data invalid or does not exist");
	}
}

void pof::SaleView::OnSearchPopup(const pof::base::data::row_t& row)
{
	try {
		auto& v = row.first;
		pof::base::data::row_t rowSale;
		bool status = CheckInStock(row);
		if (!status) {
			wxMessageBox(fmt::format("{} is out of stock",
				boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}
		CheckExpired(row);
		status = CheckProductClass(row);
		if (status) {
			wxMessageBox(fmt::format("{} is a prescription only medication, Requires a prescription for sale",
				boost::variant2::get<pof::base::data::text_t>(v[pof::ProductManager::PRODUCT_NAME])), "SALE PRODUCT", wxICON_WARNING | wxOK);
			return;
		}

		auto& vS = rowSale.first;
		vS.resize(pof::SaleManager::MAX);

		vS[pof::SaleManager::PRODUCT_SERIAL_NUM] = v[pof::ProductManager::PRODUCT_SERIAL_NUM];
		vS[pof::SaleManager::PRODUCT_NAME] = v[pof::ProductManager::PRODUCT_NAME];
		vS[pof::SaleManager::PRODUCT_CATEGORY] = "MEDICINE"s;
		vS[pof::SaleManager::PRODUCT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];
		vS[pof::SaleManager::PRODUCT_QUANTITY] = static_cast<std::uint64_t>(1);
		vS[pof::SaleManager::PRODUCT_EXT_PRICE] = v[pof::ProductManager::PRODUCT_UNIT_PRICE];


		wxGetApp().mSaleManager.GetSaleData()->EmplaceData(std::move(rowSale));
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
		auto& uu = boost::variant2::get<pof::base::data::duuid_t>(v[pof::ProductManager::PRODUCT_UUID]);
		auto tt = pof::base::data::clock_t::now().time_since_epoch().count();
		if (wxGetApp().mLocalDatabase) {
			if (!mExpiredStatement) {
				constexpr const std::string_view sql = "SELECT lot_number FROM inventory WHERE uuid = ? AND expire_date < ?;";
				auto stmt = wxGetApp().mLocalDatabase->prepare(sql);
				if (!stmt.has_value()) {
					spdlog::error(wxGetApp().mLocalDatabase->err_msg());
					return false;
				}
				mExpiredStatement = *stmt;
			}
			bool status = wxGetApp().mLocalDatabase->bind(mExpiredStatement, std::make_tuple(uu, tt));
			if (!status) {
				spdlog::error(wxGetApp().mLocalDatabase->err_msg());
				return false;
			}

			auto ret = wxGetApp().mLocalDatabase->retrive<pof::base::data::text_t>(mExpiredStatement);
			if (!ret.has_value()) {
				spdlog::error(wxGetApp().mLocalDatabase->err_msg());
				return false;
			}
			if (ret.value().empty()) {
				return false;
			}
			else {
				std::vector<std::string> batches;
				batches.reserve(ret->size());
				for (auto& t : *ret) {
					batches.emplace_back(std::move(std::get<0>(t)));
				}
				wxMessageBox(fmt::format("Product May be expired if it belongs to the following batches \n {} \n Please check batch No before continuing"), "SALE PRODUCT", wxICON_WARNING | wxOK);
				return true;
			}

		}
	}
	catch (const std::exception& exp) {
		spdlog::error(exp.what());
		return false;
	}
	return true;
}

void pof::SaleView::ProductNameKeyEvent()
{
	mProductNameValue->Bind(wxEVT_CHAR, [&](wxKeyEvent& evt) {
		spdlog::info("{:d} Key code", evt.GetKeyCode());
		if (evt.GetKeyCode() == WXK_DOWN) {
			mSearchPopup->SetFocus();
		}
		else {
			evt.Skip();
		}
	});
}
