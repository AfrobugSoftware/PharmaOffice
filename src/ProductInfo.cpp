
#include "ProductInfo.h"
//#include "Application.h"
#include "PofPch.h"

BEGIN_EVENT_TABLE(pof::ProductInfo, wxPanel)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_GO_BACK, pof::ProductInfo::OnGoBack)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_ADD_INVENTORY, pof::ProductInfo::OnAddInventory)
	EVT_TOOL(pof::ProductInfo::ID_SHOW_PRODUCT_SALE_HISTORY, pof::ProductInfo::OnShowProducSaleHistory)
	
	EVT_PG_CHANGED(pof::ProductInfo::ID_PROPERTY_GRID, pof::ProductInfo::OnPropertyChanged)
	EVT_PG_CHANGING(pof::ProductInfo::ID_PROPERTY_GRID, pof::ProductInfo::OnPropertyChanging)
	
	EVT_SPLITTER_UNSPLIT(pof::ProductInfo::ID_SPLIT_WINDOW, pof::ProductInfo::OnUnspilt)
	EVT_SPLITTER_DCLICK(pof::ProductInfo::ID_SPLIT_WINDOW, pof::ProductInfo::OnSashDoubleClick)
	
	EVT_TOOL(pof::ProductInfo::ID_TOOL_SHOW_PRODUCT_INFO, pof::ProductInfo::OnShowProductInfo)
	EVT_TOOL(pof::ProductInfo::ID_WARNINGS, pof::ProductInfo::OnWarnings)
	EVT_TOOL(pof::ProductInfo::ID_RESET, pof::ProductInfo::OnReset)
	EVT_TOOL(pof::ProductInfo::ID_ADD_BARCODE, pof::ProductInfo::OnAddBarcode)


	EVT_DATE_CHANGED(pof::ProductInfo::ID_DATE, pof::ProductInfo::OnDateChange)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(pof::ProductInfo::ID_DATA_VIEW, pof::ProductInfo::OnInvenContextMenu)
	
	EVT_MENU(pof::ProductInfo::ID_INVEN_MENU_REMOVE, pof::ProductInfo::OnRemoveInventory)
	EVT_MENU(pof::ProductInfo::ID_INVEN_MENU_CREATE_INVOICE, pof::ProductInfo::OnCreateInvoice)
	EVT_BUTTON(pof::ProductInfo::ID_TOOL_ADD_INVENTORY, pof::ProductInfo::OnAddInventory)

END_EVENT_TABLE()

pof::ProductInfo::ProductInfo( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, ID_SPLIT_WINDOW, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE|wxSP_NOBORDER | wxNO_BORDER );
	m_splitter1->SetExtraStyle(wxWS_EX_PROCESS_IDLE);
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( ProductInfo::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_auiToolBar1 = new wxAuiToolBar( this , wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT|wxAUI_TB_HORZ_TEXT|wxAUI_TB_OVERFLOW | wxNO_BORDER); 
	m_auiToolBar1->SetMinSize( wxSize( -1,30 ) );
	
	m_auiToolBar1->AddTool(ID_TOOL_GO_BACK, wxT("Back"), wxArtProvider::GetBitmap("arrow_back"), "Back to product list", wxITEM_NORMAL);
	m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddSpacer(2);
	mProductNameText = m_auiToolBar1->AddTool( wxID_ANY, wxEmptyString, wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddTool(ID_TOOL_ADD_INVENTORY, wxT("Add Stock"), wxArtProvider::GetBitmap("action_add"), "Add Inventory", wxITEM_NORMAL);
	m_auiToolBar1->AddSpacer(2);
	mShowAddInfo = m_auiToolBar1->AddTool(ID_TOOL_SHOW_PRODUCT_INFO, wxT("Product Information"), wxArtProvider::GetBitmap("application"), "Show the products information", wxITEM_CHECK);
	std::bitset<32> bitset(mShowAddInfo->GetState());
	bitset.set(5);
	mShowAddInfo->SetState(bitset.to_ulong());

	m_auiToolBar1->AddSpacer(2);
	mProductHist = m_auiToolBar1->AddTool(ID_SHOW_PRODUCT_SALE_HISTORY, wxT("Sale History"), wxArtProvider::GetBitmap("sci"), "Show product history", wxITEM_CHECK);
	m_auiToolBar1->AddTool(ID_WARNINGS, wxT("Warnings"), wxArtProvider::GetBitmap(wxART_WARNING, wxART_TOOLBAR, wxSize(16,16)), "Warnings associated with this product", wxITEM_NORMAL);
	m_auiToolBar1->AddStretchSpacer();
	mInventoryDate = new wxDatePickerCtrl(m_auiToolBar1, ID_DATE, wxDateTime::Now(), wxDefaultPosition, wxSize(200, -1), wxDP_DROPDOWN);
	
	m_auiToolBar1->AddControl(mInventoryDate);
	m_auiToolBar1->AddSpacer(5);
	m_auiToolBar1->AddTool(ID_RESET, wxT("Reset"), wxArtProvider::GetBitmap(wxART_REFRESH, wxART_TOOLBAR, wxSize(16,16)),"Reset date filter");
	m_auiToolBar1->Realize(); 
	
	mBook = new wxSimplebook(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL );
	InventoryView = new wxDataViewCtrl( mBook, ID_DATA_VIEW, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_SINGLE | wxNO_BORDER | wxDV_ROW_LINES);
	mHistView = new wxDataViewCtrl( mBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_SINGLE | wxNO_BORDER | wxDV_ROW_LINES );
	mBook->AddPage(InventoryView, wxEmptyString, false);
	mBook->AddPage(mHistView, wxEmptyString, false);
	CreateInventoryView();
	CreateHistoryView();
	CreateEmptyPanel();
	mBook->AddPage(mEmpty, wxEmptyString, false);

	bSizer2->Add( mBook , wxSizerFlags().Expand().Proportion(1));
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_propertyGridManager1 = new wxPropertyGridManager(m_panel2, ID_PROPERTY_GRID, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE|wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPG_SPLITTER_AUTO_CENTER| wxPG_TOOLBAR | wxNO_BORDER);
	m_propertyGridManager1->SetExtraStyle( wxPG_EX_MODE_BUTTONS | wxPG_EX_NATIVE_DOUBLE_BUFFERING); 
	
	auto tool = m_propertyGridManager1->GetToolBar();
	if (tool){
		tool->SetBackgroundColour(*wxWHITE);
		tool->SetWindowStyleFlag(wxTB_HORZ_TEXT);
		tool->AddStretchableSpace();
		tool->AddSeparator();
		tool->AddTool(ID_ADD_BARCODE, "Barcode", wxArtProvider::GetBitmap("action_add"), "Add barcode to the product");

		tool->Realize();
		m_propertyGridManager1->Update();
	}


	m_propertyGridPage1 = m_propertyGridManager1->AddPage( wxT("Info"), wxNullBitmap );
	m_propertyGridItem1 = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Product details"), wxT("Product Details") ) ); 
	mNameItem = m_propertyGridPage1->Append( new wxStringProperty( wxT("NAME"), wxPG_LABEL) );
	m_propertyGridPage1->SetPropertyHelpString( mNameItem, wxT("The product brand name") );
	mGenericNameItem = m_propertyGridPage1->Append( new wxArrayStringProperty( wxT("GENERIC NAME"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mGenericNameItem, wxT("The generic name for the product") );
	mPackageSizeItem = m_propertyGridPage1->Append( new wxIntProperty( wxT("PACKAGE SIZE"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mPackageSizeItem, wxT("Size of a single unit of product for sale. For example, for anti-malaria with 6 tablets would have package size set to 6 and formulation set to tablets") );
	mBarcode = m_propertyGridPage1->Append(new wxStringProperty(wxT("BARCODE"), wxPG_LABEL));
	m_propertyGridPage1->SetPropertyHelpString(mBarcode, wxT("Product barcode"));
	mBarcode->Enable(false);

	ProductClassChoices.Add("POM");
	ProductClassChoices.Add("OTC");
	ProductClassChoices.Add("CONTROLLED");
	mProductClass = m_propertyGridPage1->Append(new wxEnumProperty(wxT("CLASS"), wxPG_LABEL, ProductClassChoices));
	m_propertyGridPage1->SetPropertyHelpString( mProductClass, wxT("Products can be POM for Prescription only medication, these medicines can only be sold with a valid prescription. Either online or offline. OTC for Over the counter medicines. There are medications that can be sold on pharmacy. Without a prescription. An alert is sent when an attempt is made to sell a POM without a prescription. Controlled medications are medications that are to be sold only be a pharmacist.") );
	
	mFormulationItem = m_propertyGridPage1->Append( new wxEnumProperty( wxT("FORMULATION"), wxPG_LABEL, wxGetApp().FormulationChoices));
	m_propertyGridPage1->SetPropertyHelpString( mFormulationItem, wxT("Product formulation is the form in which this product is in, for example, tablet, capsules, injectables, or solutions.") );
	


	mMoreProductInfo = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("More product information"), wxT("More Product Information") ) ); 
	mDirForUse = m_propertyGridPage1->Append( new wxArrayStringProperty( wxT("DIRECTION FOR USE"), wxPG_LABEL ));
	m_propertyGridPage1->SetPropertyHelpString( mDirForUse, wxT("Information of usage. This would be printed on the label when sold over the counter") );
	mHealthCond = m_propertyGridPage1->Append( new wxArrayStringProperty( wxT("HEALTH CONDITIONS"), wxPG_LABEL));
	m_propertyGridPage1->SetPropertyHelpString( mHealthCond, wxT("A list of possible health ") );
	mProductDescription = m_propertyGridPage1->Append( new wxLongStringProperty( wxT("DESCRIPTION"), wxPG_LABEL) );
	m_propertyGridPage1->SetPropertyHelpString( mProductDescription, wxT("Describes the product in a way that it can be added to a formulary") );
	mSideEffects = m_propertyGridPage1->Append(new wxArrayStringProperty(wxT("SIDE EFFECTS"), wxPG_LABEL));
	m_propertyGridPage1->SetPropertyHelpString(mSideEffects, wxT("Possible Side effects associated this this product"));

	mStrengthValueItem = m_propertyGridPage1->Append(new wxStringProperty(wxT("STRENGTH/CONC."), wxPG_LABEL));
	m_propertyGridPage1->SetPropertyHelpString(mStrengthValueItem, wxT("The pharmalogical strength of the medication"));


	mStrengthTypeItem = m_propertyGridPage1->Append(new wxEnumProperty(wxT("STRENGTH TYPE"), wxPG_LABEL, wxGetApp().StrengthChoices));
	mSettings = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Settings"), wxT("Settings"))); 
	mMinStockCount = m_propertyGridPage1->Append( new wxIntProperty( wxT("MINIMUM STOCK COUNT"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mMinStockCount, wxT("The amount of stock that should indicate stock level is low. ") );
	mExpDateCount = m_propertyGridPage1->Append( new wxIntProperty( wxT("EXPIRE PERIOD COUNT"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mExpDateCount, wxT("Number of (Days, Weeks, Months) before expiry date that an alert should be sent") );
	
	ExpChoices.Add("DAY");
	ExpChoices.Add("WEEK");
	ExpChoices.Add("MONTH");

	mExpDatePeriod = m_propertyGridPage1->Append( new wxEnumProperty( wxT("EXPIRE PERIOD"), wxPG_LABEL, ExpChoices) );
	m_propertyGridPage1->SetPropertyHelpString( mExpDatePeriod, wxT("Select the period in which the expire alert defines") );
	
	mSaleSettings = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Sale"), wxT("Sale") )); 

	mUnitPrice = m_propertyGridPage1->Append( new wxFloatProperty( wxT("UNIT PRICE"), wxPG_LABEL));
	m_propertyGridPage1->SetPropertyHelpString( mUnitPrice, wxT("Sale price per package size of the product\n") );
	mCostPrice = m_propertyGridPage1->Append(new wxFloatProperty(wxT("COST PRICE"), wxPG_LABEL));
	m_propertyGridPage1->SetPropertyHelpString(mUnitPrice, wxT("Cost price per package size of the product\n"));
	wxFloatingPointValidator<double> val(2, &mStubPrice, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 999999999999);
	mUnitPrice->SetValidator(val);
	mCostPrice->SetValidator(val);

	
	bSizer3->Add( m_propertyGridManager1, 1, wxALL|wxEXPAND, 0 );
	m_panel2->SetSize(wxSize(400, -1));
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 0); //splitter posistion
	m_splitter1->SetSashGravity(1.0);
	
	bSizer1->Add( m_auiToolBar1, 0, wxALL|wxEXPAND, 0 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	CreateNameToProductElemTable();
	StyleProductPropertyManager();
	StyleSheet();

	//set update signal
	wxGetApp().mUpdateChoices.connect(std::bind_front(&pof::ProductInfo::UpdateDropDowns, this));
}

pof::ProductInfo::~ProductInfo()
{
}

void pof::ProductInfo::StyleProductPropertyManager()
{
	m_propertyGridManager1->SetBackgroundColour(*wxWHITE); //FROM THEME
}

void pof::ProductInfo::Load(const pof::base::data::row_t& row)
{
	try {
		auto& Name = boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]);
		if (Name.empty()) return; //nothing to load

		mProductNameText->SetLabel(fmt::format("{} - Inventory history", Name));

		//should get data from the server on the selected product 
		
		auto& Model = wxGetApp().mProductManager.GetInventory();
		//unpack data from where 
		//Model->Unpack(...);
		m_auiToolBar1->Realize();
		m_auiToolBar1->Refresh();
		m_auiToolBar1->Update();
		mProductData = row;
		LoadProductProperty(row);

		auto firstMonth = wxGetApp().mProductManager.GetLastInventoryDate(boost::variant2::get<pof::base::data::duuid_t>(row.first[pof::ProductManager::PRODUCT_UUID]));
		if (!firstMonth.has_value()) {
			firstMonth.emplace();
		}
		mInventoryDate->SetRange(wxDateTime(std::chrono::system_clock::to_time_t(firstMonth.value())), wxDateTime::Now());
		mInventoryDate->SetValue(wxDateTime::Now());

		if (Model->GetDatastore().empty())
			mBook->SetSelection(PAGE_EMPTY);
		else
			mBook->SetSelection(PAGE_INVENTORY);

 	}
	catch (const std::exception& exp) {
		spdlog::critical(exp.what());
	}

}

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


void pof::ProductInfo::LoadProductProperty(const pof::base::data::row_t& row)
{
	pof::ProductManager::relation_t::tuple_t tup;
	bool status = pof::base::build(tup, row);
	if (!status) {
		spdlog::error("Cannot build tuple from row");
		return;
	}
	mNameItem->SetValue(std::get<pof::ProductManager::PRODUCT_NAME>(tup));
	mGenericNameItem->SetValue(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_GENERIC_NAME>(tup)));
	mProductClass->SetValue(std::get<pof::ProductManager::PRODUCT_CLASS>(tup));
	mPackageSizeItem->SetValue(static_cast<int>(std::get<pof::ProductManager::PRODUCT_PACKAGE_SIZE>(tup)));
	mUnitPrice->SetValue(static_cast<double>(std::get<pof::ProductManager::PRODUCT_UNIT_PRICE>(tup)));
	mCostPrice->SetValue(static_cast<double>(std::get<pof::ProductManager::PRODUCT_COST_PRICE>(tup)));
	mMinStockCount->SetValue(static_cast<int>(std::get<pof::ProductManager::PRODUCT_MIN_STOCK_COUNT>(tup)));
	mDirForUse->SetValue(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_USAGE_INFO>(tup)));
	mHealthCond->SetValue(wxVariant(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_HEALTH_CONDITIONS>(tup))));
	mProductDescription->SetValue(std::get<pof::ProductManager::PRODUCT_DESCRIP>(tup));
	mSideEffects->SetValue(wxVariant(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_SIDEEFFECTS>(tup))));
	mBarcode->SetValue(std::get<pof::ProductManager::PRODUCT_BARCODE>(tup));
	auto FormIter = std::find(wxGetApp().FormulationChoices.begin(), wxGetApp().FormulationChoices.end(), std::get<pof::ProductManager::PRODUCT_FORMULATION>(tup));
	if ((FormIter != wxGetApp().FormulationChoices.end())) {
		auto idx = std::distance(wxGetApp().FormulationChoices.begin(), FormIter);
		mFormulationItem->SetValue(wxVariant(static_cast<int>(idx)));
	}
	else {
		mFormulationItem->SetValue(static_cast<int>(0));
	}

	mStrengthValueItem->SetValue(wxVariant(std::get<pof::ProductManager::PRODUCT_STRENGTH>(tup)));

	auto Iter = std::find(wxGetApp().StrengthChoices.begin(), wxGetApp().StrengthChoices.end(), std::get<pof::ProductManager::PRODUCT_STRENGTH_TYPE>(tup));
	if ((Iter != wxGetApp().StrengthChoices.end())) {
		auto idx = std::distance(wxGetApp().StrengthChoices.begin(), Iter);
		mStrengthTypeItem->SetValue(wxVariant(static_cast<int>(idx)));
	}
	else {
		mStrengthTypeItem->SetValue(static_cast<int>(0));
	}
	//load the inventory data
	wxGetApp().mProductManager.GetInventory()->Clear();
	wxGetApp().mProductManager.LoadInventoryData(std::get<pof::ProductManager::PRODUCT_UUID>(tup));


	//expire peroid 
	//expire date
	SplitPeriodString(tup);
}

boost::signals2::connection pof::ProductInfo::AttachBackSlot(back_signal_t::slot_type&& slot)
{
	return mBackSignal.connect(std::forward<pof::ProductInfo::back_signal_t::slot_type>(slot));
}

boost::signals2::connection pof::ProductInfo::AttachPropertyUpdateSlot(update_signal_t::slot_type&& slot)
{
	return mUpdatePropertySignal.connect(std::forward<pof::ProductInfo::update_signal_t::slot_type>(slot));
}

void pof::ProductInfo::SignalUpdate(const PropertyUpdate& update)
{
	mUpdatePropertySignal(update);
}

void pof::ProductInfo::CreateNameToProductElemTable()
{
	mNameToProductElem.insert({ "CLASS", pof::ProductManager::PRODUCT_CLASS });
	mNameToProductElem.insert({ "NAME", pof::ProductManager::PRODUCT_NAME });
	mNameToProductElem.insert({ "GENERIC NAME", pof::ProductManager::PRODUCT_GENERIC_NAME });
	mNameToProductElem.insert({ "PACKAGE SIZE", pof::ProductManager::PRODUCT_PACKAGE_SIZE });
	mNameToProductElem.insert({ "UNIT PRICE", pof::ProductManager::PRODUCT_UNIT_PRICE });
	mNameToProductElem.insert({ "COST PRICE", pof::ProductManager::PRODUCT_COST_PRICE });
	mNameToProductElem.insert({ "MINIMUM STOCK COUNT", pof::ProductManager::PRODUCT_MIN_STOCK_COUNT });
	mNameToProductElem.insert({ "DIRECTION FOR USE", pof::ProductManager::PRODUCT_USAGE_INFO });
	mNameToProductElem.insert({ "HEALTH CONDITIONS", pof::ProductManager::PRODUCT_HEALTH_CONDITIONS });
	mNameToProductElem.insert({ "DESCRIPTION", pof::ProductManager::PRODUCT_DESCRIP });
	mNameToProductElem.insert({ "STRENGTH/CONC.", pof::ProductManager::PRODUCT_STRENGTH });
	mNameToProductElem.insert({ "STRENGTH TYPE", pof::ProductManager::PRODUCT_STRENGTH_TYPE });
	mNameToProductElem.insert({ "SIDE EFFECTS", pof::ProductManager::PRODUCT_SIDEEFFECTS});
	mNameToProductElem.insert({ "EXPIRE PERIOD", pof::ProductManager::PRODUCT_EXPIRE_PERIOD });
	mNameToProductElem.insert({ "EXPIRE PERIOD COUNT", pof::ProductManager::PRODUCT_EXPIRE_PERIOD });
	mNameToProductElem.insert({ "FORMULATION", pof::ProductManager::PRODUCT_FORMULATION });
	mNameToProductElem.insert({ "BARCODE", pof::ProductManager::PRODUCT_BARCODE });
}

void pof::ProductInfo::UpdateDropDowns()
{
	auto ff = dynamic_cast<wxEnumProperty*>(mFormulationItem);
	if (!ff) return;
	wxPGChoices choice, choice2;
	for (auto& i : wxGetApp().FormulationChoices)
	{
		choice.Add(i);
	}
	ff->SetChoices(choice);
	auto qq = dynamic_cast<wxEnumProperty*>(mStrengthTypeItem);
	if (!qq) return;
	for (auto& i : wxGetApp().StrengthChoices) {
		choice2.Add(i);
	}
	qq->SetChoices(choice2);
}

void pof::ProductInfo::m_splitter1OnIdle(wxIdleEvent&)
{
	m_splitter1->SetSashPosition(550);
	m_splitter1->Disconnect(wxEVT_IDLE, wxIdleEventHandler(ProductInfo::m_splitter1OnIdle), NULL, this);
}

void pof::ProductInfo::CreateInventoryView()
{
	InventoryView->AssociateModel(wxGetApp().mProductManager.GetInventory().get());
	

	mInputDate = InventoryView->AppendTextColumn(wxT("Input Date"), pof::ProductManager::INVENTORY_INPUT_DATE, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mBactchNo = InventoryView->AppendTextColumn(wxT("Batch No"), pof::ProductManager::INVENTORY_LOT_NUMBER, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mExpiryDate = InventoryView->AppendTextColumn(wxT("Expiry Date"), pof::ProductManager::INVENTORY_EXPIRE_DATE, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockCount = InventoryView->AppendTextColumn(wxT("Entry Stock Amount"), pof::ProductManager::INVENTORY_STOCK_COUNT, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	InventoryView->AppendTextColumn(wxT("Entry Stock Cost"), pof::ProductManager::INVENTORY_COST, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mManuFactureName = InventoryView->AppendTextColumn(wxT("Supplier's Name"), pof::ProductManager::INVENTORY_MANUFACTURER_NAME, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);

	pof::DataModel::SpeicalColHandler_t costHandler;
	costHandler.first = [&](size_t row, size_t col) -> wxVariant {
		auto& datastore = wxGetApp().mProductManager.GetInventory()->GetDatastore();
		const auto& unitCost = datastore[row].first[col];
		const auto& invenCount = datastore[row].first[pof::ProductManager::INVENTORY_STOCK_COUNT];

		auto cur = boost::variant2::get<pof::base::currency>(unitCost) *
			static_cast<double>(boost::variant2::get<std::uint64_t>(invenCount));
		return fmt::format("{:cu}", cur);
	};
	wxGetApp().mProductManager.GetInventory()->SetSpecialColumnHandler(pof::ProductManager::INVENTORY_COST, std::move(costHandler));

}

void pof::ProductInfo::CreateHistoryView()
{
	mHistView->AssociateModel(wxGetApp().mSaleManager.GetProductHistory().get());
	
	mHistView->AppendTextColumn(wxT("Date"), pof::SaleManager::HIST_DATE, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mHistView->AppendTextColumn(wxT("Quantity Sold"), pof::SaleManager::HIST_QUANTITY, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mHistView->AppendTextColumn(wxT("Price"), pof::SaleManager::HIST_EXTPRICE, wxDATAVIEW_CELL_INERT, 120, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
}

void pof::ProductInfo::CreateEmptyPanel()
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

	wxStaticBitmap* b1 = new wxStaticBitmap(m7, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(b1, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxStaticText* t1 = new wxStaticText(m7, wxID_ANY, wxT("No transaction avaliable for product"), wxDefaultPosition, wxDefaultSize, 0);
	t1->Wrap(-1);
	bSizer9->Add(t1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxButton* btn = new wxButton(m7, ID_TOOL_ADD_INVENTORY);
	btn->SetBitmap(wxArtProvider::GetBitmap("action_add"));
	btn->SetLabel("Add stock");
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

}

void pof::ProductInfo::RemoveCheckedState(wxAuiToolBarItem* item)
{
	if (!item) return;
	m_auiToolBar1->Freeze();
	std::bitset<32> bitset(item->GetState());
	if (bitset.test(5)) bitset.flip(5);
	item->SetState(bitset.to_ulong());
	m_auiToolBar1->Thaw();
	m_auiToolBar1->Refresh();
}

void pof::ProductInfo::OnGoBack(wxCommandEvent& evt)
{
	mBackSignal();
	if (mPropertyUpdate.has_value()) {
		wxIcon cop;
		cop.CopyFromBitmap(wxArtProvider::GetBitmap("supplement-bottle"));
		wxBusyInfo info
		(
			wxBusyInfoFlags()
			.Parent(this)
			.Icon(cop)
			.Title("Saving product")
			.Text("Please wait...")
			.Foreground(*wxBLACK)
			.Background(*wxWHITE)
			.Transparency(4 * wxALPHA_OPAQUE / 5)
		);

		mUpdatePropertySignal(mPropertyUpdate.value());
		mPropertyUpdate = {};
		RemovePropertyModification();
	}

	if (mBook->GetSelection() == PAGE_SALE_HIST)
	{
		wxGetApp().mSaleManager.GetProductHistory()->Clear();

		std::bitset<32> bitset(mProductHist->GetState());
		if (bitset.test(5)) {
			bitset.flip(5);
			mProductHist->SetState(bitset.to_ulong());
		}
		mBook->SetSelection(PAGE_INVENTORY);
	}
	wxGetApp().mProductManager.GetInventory()->Clear();
}

void pof::ProductInfo::OnAddInventory(wxCommandEvent& evt)
{
	//check if product has expired inventory
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST) && !wxGetApp().bAllowOtherUsersInventoryPermission){
		wxMessageBox("User accoount cannot add inventory to stock", "Add Inventory", wxICON_INFORMATION | wxOK);
		return;
	}
	//check if product has stock
	if (boost::variant2::get<std::uint64_t>(mProductData.first[pof::ProductManager::PRODUCT_STOCK_COUNT])
		!= static_cast<std::uint64_t>(0) &&
		wxMessageBox("Product has stock avaliable, do you wish to add more stock?", "Add stock", wxICON_INFORMATION | wxYES_NO) == wxNO) {
		return; //has stock and does not what to add more
	}

	auto items = wxGetApp().mProductManager.DoExpiredProducts();
	if (!items.has_value()) return;
	if (std::ranges::any_of(items.value(), [&](const wxDataViewItem& i) -> bool {
		auto& row = wxGetApp().mProductManager.GetProductData()->GetDatastore()[pof::DataModel::GetIdxFromItem(i)];	
		return boost::variant2::get<boost::uuids::uuid>(row.first[pof::ProductManager::PRODUCT_UUID])
				== boost::variant2::get<boost::uuids::uuid>(mProductData.first[pof::ProductManager::PRODUCT_UUID]);
	})) {
		wxMessageBox("Product has expired inventory, clear inventory before adding new", "Add Inventory", wxICON_WARNING | wxOK);
		return;
	}

	iscontrolled = false;
	if (mPropertyUpdate.has_value()){
		if (boost::variant2::holds_alternative<pof::base::data::text_t>(mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_CLASS])) {
			iscontrolled = boost::variant2::get<pof::base::data::text_t>(mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_CLASS])
				== "CONTROLLED";
		}
		
	}
	if (!iscontrolled){
		std::string cls = boost::variant2::get<pof::base::data::text_t>(mProductData.first[pof::ProductManager::PRODUCT_CLASS]);
		iscontrolled = cls == "CONTROLLED";
	}

	pof::InventoryDialog dialog(nullptr, boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]));
	if (dialog.ShowModal() == wxID_OK) {
		auto& Inven = dialog.GetData();
		Inven.first[pof::ProductManager::INVENTORY_PRODUCT_UUID] = mProductData.first[pof::ProductManager::PRODUCT_UUID];

		if (!mPropertyUpdate.has_value()) {
			mPropertyUpdate.emplace();
			mPropertyUpdate->mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
			mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] =
				mProductData.first[pof::ProductManager::PRODUCT_UUID];
		}

		//stock
		mPropertyUpdate->mUpdatedElememts.set(pof::ProductManager::PRODUCT_STOCK_COUNT);
		if (boost::variant2::holds_alternative<std::uint64_t>(mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_STOCK_COUNT]))
		{
			boost::variant2::get<std::uint64_t>(mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_STOCK_COUNT]) +=
				boost::variant2::get<std::uint64_t>(Inven.first[pof::ProductManager::INVENTORY_STOCK_COUNT]);
		}
		else {
			mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_STOCK_COUNT] = 
				Inven.first[pof::ProductManager::INVENTORY_STOCK_COUNT];
		}


		//update a controlled medicaiton
		if (iscontrolled) {
			pof::base::data::row_t row;
			//create the first entry into the book table	
			auto& v = row.first;
			auto& p = mProductData.first;

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


		//cost
		mPropertyUpdate->mUpdatedElememts.set(pof::ProductManager::PRODUCT_COST_PRICE);
		mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_COST_PRICE] =
			Inven.first[pof::ProductManager::INVENTORY_COST];
		mCostPrice->SetValue(wxVariant(static_cast<float>(boost::variant2::get<pof::base::currency>(Inven.first[pof::ProductManager::INVENTORY_COST]))));


		wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Added inventory with batch {} to {}",
			boost::variant2::get<pof::base::data::text_t>(Inven.first[pof::ProductManager::INVENTORY_LOT_NUMBER]),
			boost::variant2::get<pof::base::data::text_t>(mProductData.first[pof::ProductManager::PRODUCT_NAME])));
		wxGetApp().mProductManager.GetInventory()->StoreData(std::move(Inven));

		//refresh inventory view
		
		//force and update to the system when we add an inventory
		mUpdatePropertySignal(mPropertyUpdate.value());
		mPropertyUpdate = {};
		RemovePropertyModification();
		
		int sel = mBook->GetSelection();
		if (sel == PAGE_EMPTY) {
			mBook->SetSelection(PAGE_INVENTORY);
		}
	}
	else {
		//rejected
	}

}

static std::string JoinArrayList(const wxVariant& Value) {
	auto values = std::move(Value.GetArrayString());
	std::vector<std::string> arrayString;
	arrayString.reserve(values.size());
	for (auto& str : values) {
		arrayString.emplace_back(std::move(str.ToStdString()));
	}
	return fmt::format("{}", fmt::join(arrayString, ","));
}

std::uint64_t pof::ProductInfo::PeriodTime(int periodCount) const
{
	int idx = mExpDatePeriod->GetChoiceSelection();
	if (idx == wxNOT_FOUND) {
		idx = 0; //if no selection use days
	}
	pof::base::data::datetime_t::duration expireDuration = {};
	switch (idx) {
	case 0: //day
	{
		date::days point(periodCount);
		expireDuration = pof::base::data::datetime_t(point).time_since_epoch();
	}
		break;
	case 1: //week
	{
		date::weeks point(periodCount * 7);
		expireDuration = pof::base::data::datetime_t(point).time_since_epoch();
	}
		break;
	case 2: //month
	{
		date::months point(periodCount);
		expireDuration = pof::base::data::datetime_t(point).time_since_epoch();
	}
		break;
	}
	return expireDuration.count();
}

pof::base::data::text_t pof::ProductInfo::CreatePeriodString()
{
	//do also for count 
	if (mExpDateCount->GetValue().IsNull()) {
		wxMessageBox("Expired Period count not specified", "PRODUCT INFO", wxICON_WARNING | wxOK);
		return {};
	}
	std::string val;
	if (!mExpDatePeriod->GetValue().IsNull())
		val = mExpDatePeriod->GetValueAsString().ToStdString();
	else val = "DAY";
	int count = mExpDateCount->GetValue().GetInteger();
	return fmt::format("{:d},{}", count, val);
}

void pof::ProductInfo::SplitPeriodString(const pof::ProductManager::relation_t::tuple_t& tup)
{
	mExpDateCount->SetValue(wxVariant());
	mExpDatePeriod->SetValue(wxVariant());

	auto string = std::get<pof::ProductManager::PRODUCT_EXPIRE_PERIOD>(tup);
	if (string.empty()) return;

	size_t n = string.find_first_of(",");
	if (n == std::string::npos) return;

	mExpDateCount->SetValue(wxVariant(atoi(string.substr(0, n).c_str())));
	auto stringArray = ExpChoices.GetLabels();
	auto iter = std::ranges::find(stringArray, string.substr(n + 1, string.size()));
	if (iter == stringArray.end()) return;

	mExpDatePeriod->SetValue(wxVariant((int)std::distance(stringArray.begin(), iter)));
}

void pof::ProductInfo::LoadInventoryByDate(const pof::base::data::datetime_t& dt)
{
	bool status = wxGetApp().mProductManager.LoadInventoryByDate(boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]),dt);
	if (!status) {
		wxMessageBox("Cannot load inventory by date", "Inventory", wxICON_INFORMATION | wxOK);
	}
}

void pof::ProductInfo::LoadHistoryByDate(const pof::base::data::datetime_t& dt)
{
	bool status = wxGetApp().mSaleManager.LoadHistoryByDate(boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]),dt);
	if (!status) {
		wxMessageBox("Cannot load history by date", "History", wxICON_INFORMATION | wxOK);

	}
}


void pof::ProductInfo::OnPropertyChanged(wxPropertyGridEvent& evt)
{
	wxBusyCursor cursor;
	wxPGProperty* props = evt.GetProperty();
	if(props->IsCategory()) return;
	spdlog::info("Editing prop: {}", evt.GetPropertyName().ToStdString());
	auto PropIter = mNameToProductElem.find(evt.GetPropertyName().ToStdString());
	if (PropIter == mNameToProductElem.end()) {
		evt.Skip();
		return;
	}
	if (!mPropertyUpdate.has_value()) {
		mPropertyUpdate.emplace();
		mPropertyUpdate->mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
		mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] =
			mProductData.first[pof::ProductManager::PRODUCT_UUID];
	}
	int ProductElemIdx = PropIter->second;
	mPropertyUpdate->mUpdatedElememts.set(ProductElemIdx);
	auto PropertyValue = evt.GetPropertyValue();
	auto& v = mPropertyUpdate->mUpdatedElementsValues.first;
	switch (ProductElemIdx)
	{
	case pof::ProductManager::PRODUCT_NAME:
		v[pof::ProductManager::PRODUCT_NAME] = std::move(PropertyValue.GetString().ToStdString());
		break;
	case pof::ProductManager::PRODUCT_GENERIC_NAME:
		v[pof::ProductManager::PRODUCT_GENERIC_NAME] = std::move(JoinArrayList(PropertyValue));
		break;
	case pof::ProductManager::PRODUCT_CLASS:
		v[pof::ProductManager::PRODUCT_CLASS] = std::move(ProductClassChoices.GetLabel(PropertyValue.GetInteger()).ToStdString());
		break;
	case pof::ProductManager::PRODUCT_PACKAGE_SIZE:
		v[pof::ProductManager::PRODUCT_PACKAGE_SIZE] = static_cast<std::uint64_t>(PropertyValue.GetInteger());
		break;
	case pof::ProductManager::PRODUCT_UNIT_PRICE:
		v[pof::ProductManager::PRODUCT_UNIT_PRICE] = pof::base::data::currency_t(PropertyValue.GetDouble());
		break;
	case pof::ProductManager::PRODUCT_COST_PRICE:
		v[pof::ProductManager::PRODUCT_COST_PRICE] = pof::base::data::currency_t(PropertyValue.GetDouble());
		break;
	case pof::ProductManager::PRODUCT_MIN_STOCK_COUNT:
		v[pof::ProductManager::PRODUCT_MIN_STOCK_COUNT] = static_cast<std::uint32_t>(PropertyValue.GetInteger());
		break;
	case pof::ProductManager::PRODUCT_USAGE_INFO:
		v[pof::ProductManager::PRODUCT_USAGE_INFO] = std::move(JoinArrayList(PropertyValue));
		break;
	case pof::ProductManager::PRODUCT_HEALTH_CONDITIONS:
		v[pof::ProductManager::PRODUCT_HEALTH_CONDITIONS] = std::move(JoinArrayList(PropertyValue));
		break;
	
	case pof::ProductManager::PRODUCT_DESCRIP:
		v[pof::ProductManager::PRODUCT_DESCRIP] = std::move(PropertyValue.GetString().ToStdString());
		break;
	case pof::ProductManager::PRODUCT_EXPIRE_PERIOD:
	{
		size_t count = 0;
		if(!mExpDateCount->GetValue().IsNull()) count = mExpDateCount->GetValue().GetInteger();
		auto periodString = CreatePeriodString();
		if (periodString.empty()) {
			mPropertyUpdate->mUpdatedElememts.set(ProductElemIdx, false);
			break;
		}
		
		v[pof::ProductManager::PRODUCT_EXPIRE_PERIOD] = periodString;
		v[pof::ProductManager::PRODUCT_TO_EXPIRE_DATE] = PeriodTime(count);
		mPropertyUpdate->mUpdatedElememts.set(pof::ProductManager::PRODUCT_TO_EXPIRE_DATE, true);
	}
		break;
	case pof::ProductManager::PRODUCT_SIDEEFFECTS:
		v[pof::ProductManager::PRODUCT_SIDEEFFECTS] = std::move(JoinArrayList(PropertyValue));
		break;
	case pof::ProductManager::PRODUCT_FORMULATION:
		v[pof::ProductManager::PRODUCT_FORMULATION] = wxGetApp().FormulationChoices[PropertyValue.GetInteger()].ToStdString();
		break;
	case pof::ProductManager::PRODUCT_STRENGTH:
		v[pof::ProductManager::PRODUCT_STRENGTH] = PropertyValue.GetString().ToStdString();
		break;
	case pof::ProductManager::PRODUCT_STRENGTH_TYPE:
		v[pof::ProductManager::PRODUCT_STRENGTH_TYPE] = wxGetApp().StrengthChoices[PropertyValue.GetInteger()].ToStdString();
		break;
	default:
		return;
	}
	
	spdlog::info("Property {} Changed", evt.GetPropertyName().ToStdString());
	evt.Veto(); //might not be necessary
}

void pof::ProductInfo::OnPropertyChanging(wxPropertyGridEvent& evt)
{
	if (!wxGetApp().HasPrivilage(pof::Account::Privilage::PHARMACIST)) {
		wxMessageBox("User account cannot perform this function", "Product Info", wxICON_INFORMATION | wxOK);
		evt.Veto();
		return;
	}
}

void pof::ProductInfo::StyleSheet()
{
	auto grid = m_propertyGridPage1->GetGrid();
	grid->SetBackgroundColour(*wxWHITE);
	grid->SetCaptionBackgroundColour(wxTheColourDatabase->Find("Aqua"));
	grid->SetCaptionTextColour(*wxBLACK);
	grid->SetMarginColour(wxTheColourDatabase->Find("Aqua"));
	auto tool = m_propertyGridManager1->GetToolBar();
	if (tool){
		tool->SetBackgroundColour(*wxWHITE);
	}
}

void pof::ProductInfo::OnSashDoubleClick(wxSplitterEvent& evt)
{
	std::bitset<32> set(mShowAddInfo->GetState());
	set.set(5, false);
	mShowAddInfo->SetState(set.to_ulong());
	evt.Skip();
}

void pof::ProductInfo::OnUnspilt(wxSplitterEvent& evt)
{
	m_panel1->Layout();
}

void pof::ProductInfo::OnShowProductInfo(wxCommandEvent& evt)
{
	if(evt.IsChecked() && !m_splitter1->IsSplit()){
		//m_panel2->SetSize(wxSize(400, -1));
		//m_splitter1->Connect(wxEVT_IDLE, wxIdleEventHandler(ProductInfo::m_splitter1OnIdle), NULL, this);
		//m_splitter1->SetSashPosition(50);
		m_splitter1->SetSashGravity(1.0);
		m_splitter1->SplitVertically(m_panel1, m_panel2, -450);
		m_splitter1->Update();
		m_panel2->Layout();
	}
	else {
		m_splitter1->Unsplit();
	}
}

void pof::ProductInfo::OnShowProducSaleHistory(wxCommandEvent& evt)
{
	if (evt.IsChecked()) {
		pof::base::data::duuid_t& puid = boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]);
		wxGetApp().mSaleManager.LoadProductSaleHistory(puid);
		if (wxGetApp().mSaleManager.GetProductHistory()->GetDatastore().empty()) {
			//uncheck
			std::bitset<32> bitset(mProductHist->GetState());
			bitset.set(5, false);
			mProductHist->SetState(bitset.to_ulong());
			wxMessageBox("No sale history for the product", "Product", wxICON_INFORMATION | wxOK);
		}
		else {
			wxGetApp().mSaleManager.GetProductHistory()->Reload();
			mBook->SetSelection(PAGE_SALE_HIST);
		}
	}
	else {
		wxGetApp().mSaleManager.GetProductHistory()->Clear();

		std::bitset<32> bitset(mProductHist->GetState());
		if (bitset.test(5)) {
			bitset.flip(5);
			mProductHist->SetState(bitset.to_ulong());
		}

		InventoryView->Freeze();
		wxGetApp().mProductManager.GetInventory()->Reload();
		InventoryView->Thaw();

		if (wxGetApp().mProductManager.GetInventory()->GetDatastore().empty())
			mBook->SetSelection(PAGE_EMPTY);
		else
			mBook->SetSelection(PAGE_INVENTORY);
	}
}

void pof::ProductInfo::OnDateChange(wxDateEvent& evt)
{
	auto& dt = evt.GetDate();
	pof::base::data::datetime_t dtt = pof::base::data::clock_t::from_time_t(dt.GetTicks());
	const int sel = mBook->GetSelection();
	switch (sel)
	{
	case 0:
		LoadInventoryByDate(dtt);
		break;
	case 1:
		LoadHistoryByDate(dtt);
		break;
	default:
		break;
	}
}

void pof::ProductInfo::OnRemoveInventory(wxCommandEvent& evt)
{
	//only pharmacist or manager can alter inventroy entries
	constexpr const int p = static_cast<int>(pof::Account::Privilage::PHARMACIST);
	constexpr const int m = static_cast<int>(pof::Account::Privilage::MANAGER);
	auto accountPriv = wxGetApp().MainAccount->priv;
	auto& productManager = wxGetApp().mProductManager;
	if (!accountPriv.test(p) && !accountPriv.test(m)) {
		wxMessageBox("This account do not have the privilage to remove an inventory entry", "Inventory", wxICON_WARNING | wxOK);
		return;
	}
	auto item = InventoryView->GetSelection();
	if (!item.IsOk()) return;
	auto& datastore = productManager.GetInventory()->GetDatastore();
	auto iter = std::next(datastore.begin(), pof::DataModel::GetIdxFromItem(item));
	pof::base::data::duuid_t& uid = boost::variant2::get<pof::base::data::duuid_t>(iter->first[pof::ProductManager::INVENTORY_PRODUCT_UUID]);
	std::uint64_t& id = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::INVENTORY_ID]);
	
	if (id != productManager.GetLastInventoryId(uid)){
		wxMessageBox("Can only remove the most recently entered inventory", "Inventory", wxICON_WARNING | wxOK);
		return;
	}

	//check if we have sold from this inventory
	std::uint64_t invenstock = boost::variant2::get<std::uint64_t>(iter->first[pof::ProductManager::INVENTORY_STOCK_COUNT]);
	std::uint64_t presentStock = boost::variant2::get<std::uint64_t>(mProductData.first[pof::ProductManager::PRODUCT_STOCK_COUNT]);
	float fpt = (static_cast<float>(presentStock) - static_cast<float>(invenstock));
	if (std::signbit(fpt)){
		wxMessageBox("Removing inventory would result in an invalid stock count as products have been sold from inventory", "Inventory", wxICON_INFORMATION | wxOK);
		return;
	}

	wxBusyCursor curor;
	mStockRemvSig(uid, static_cast<std::uint64_t>(presentStock - invenstock));

	wxGetApp().mAuditManager.WriteAudit(pof::AuditManager::auditType::PRODUCT, fmt::format("Removed inventory with batch {} from {}", 
			boost::variant2::get<pof::base::data::text_t>(iter->first[pof::ProductManager::INVENTORY_LOT_NUMBER]),
			boost::variant2::get<pof::base::data::text_t>(mProductData.first[pof::ProductManager::PRODUCT_NAME])));
	productManager.RemoveInventoryData(iter);
	productManager.GetInventory()->RemoveData(item);

	if (productManager.GetInventory()->GetDatastore().empty()){
		mBook->SetSelection(PAGE_EMPTY);
	}
}

void pof::ProductInfo::OnInvenContextMenu(wxDataViewEvent& evt)
{
	if (!InventoryView->GetSelection().IsOk()) return;
	wxMenu* menu = new wxMenu;
	auto s = menu->Append(ID_INVEN_MENU_CREATE_INVOICE, "Create invoice for inventory", nullptr);
	menu->AppendSeparator();
	auto rv = menu->Append(ID_INVEN_MENU_REMOVE, "Remove Inventory", nullptr);

	PopupMenu(menu);
}

void pof::ProductInfo::OnWarnings(wxCommandEvent& evt)
{
	pof::Warning warn(this);
	warn.LoadWarnings(boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]));
	warn.ShowModal();
}

void pof::ProductInfo::OnReset(wxCommandEvent& evt)
{
	int sel = mBook->GetSelection();
	pof::base::data::duuid_t& puid = boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]);
	switch (sel)
	{
	case PAGE_EMPTY:
		break;
	case PAGE_SALE_HIST:
		mHistView->Freeze();
		wxGetApp().mSaleManager.GetProductHistory()->Clear();
		wxGetApp().mSaleManager.LoadProductSaleHistory(puid);
		mHistView->Thaw();
		mInventoryDate->SetValue(wxDateTime::Now());
		//wxGetApp().mSaleManager.GetProductHistory()->Reload();
		break;
	case PAGE_INVENTORY:
		InventoryView->Freeze();
		wxGetApp().mProductManager.GetInventory()->Clear();
		wxGetApp().mProductManager.LoadInventoryData(puid);
		InventoryView->Thaw();
		mInventoryDate->SetValue(wxDateTime::Now());
		mInventoryDate->Update();
		break;
	default:
		break;
	}
}

void pof::ProductInfo::OnAddBarcode(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	auto str = wxGetTextFromUser("Please scan a product barcode or enter barcode manually", "Barcode").ToStdString();
	if (str.empty()) return;

	//check if is not numeric
	if (std::ranges::any_of(str, [&](char c) -> bool { return !std::isdigit(c); })) {
		wxMessageBox("Barcode cannot contain non-digits", "Product info", wxICON_WARNING | wxOK);
		return;
	}

	auto& pid = boost::variant2::get<boost::uuids::uuid>(mProductData.first[pof::ProductManager::PRODUCT_UUID]);

	//update the current product data
	auto iter = std::ranges::find_if(wxGetApp().mProductManager.GetProductData()->GetDatastore(),
		[&](const pof::base::data::row_t& row) -> bool {
			return boost::variant2::get<boost::uuids::uuid>(row.first[pof::ProductManager::PRODUCT_UUID])
			== pid;
		});
	if (iter == wxGetApp().mProductManager.GetProductData()->GetDatastore().end()) return;
	iter->first[pof::ProductManager::PRODUCT_BARCODE] = str;
	mBarcode->SetValue(str);
	wxGetApp().mProductManager.UpdatePD(std::make_tuple(pid, str), { "uuid", "barcode" });
	wxMessageBox("Product barcode updated", "Product info", wxICON_INFORMATION | wxOK);
}

void pof::ProductInfo::OnCreateInvoice(wxCommandEvent& evt)
{
	wxBusyCursor cursor;
	auto item = InventoryView->GetSelection();
	if (!item.IsOk()) return;

	size_t idx = pof::DataModel::GetIdxFromItem(item);
	auto& irow = wxGetApp().mProductManager.GetInventory()->GetDatastore()[idx];

	std::string str = wxGetTextFromUser("Please enter invoice number for inventory entry", "Add invoice").ToStdString();
	if (str.empty()) return;

	auto& suppname = boost::variant2::get<std::string>(irow.first[pof::ProductManager::INVENTORY_MANUFACTURER_NAME]);
	auto& suppliers = wxGetApp().mProductManager.GetSupplier()->GetDatastore();
	auto it = std::ranges::find_if(suppliers, [&](const pof::base::data::row_t& row) -> bool {
		return boost::variant2::get<std::string>(row.first[pof::ProductManager::SUPPLIER_NAME]) == suppname;
	});

	if (it == suppliers.end()) {
		pof::base::data::row_t row;
		auto& vq = row.first;
		vq.resize(pof::ProductManager::SUPPLIER_MAX);
		auto dt = pof::base::data::clock_t::now();

		vq[pof::ProductManager::SUPPLIER_ID] = pof::GenRandomId();
		vq[pof::ProductManager::SUPPLIER_NAME] = suppname;
		vq[pof::ProductManager::SUPPLIER_DATE_CREATED] = dt;
		vq[pof::ProductManager::SUPPLIER_DATE_MODIFIED] = dt;
		vq[pof::ProductManager::SUPPLIER_INFO] = ""s;


		//Add invoice entry for this stock
		pof::base::data::row_t inv;
		auto& iq = inv.first;
		iq.resize(pof::ProductManager::INVOICE_MAX);

		iq[pof::ProductManager::INVOICE_SUPP_ID] = boost::variant2::get<std::uint64_t>(vq[pof::ProductManager::SUPPLIER_ID]);
		iq[pof::ProductManager::INVOICE_ID] = str;
		iq[pof::ProductManager::INVOICE_PROD_UUID] = mProductData.first[pof::ProductManager::PRODUCT_UUID];
		iq[pof::ProductManager::INVOICE_INVENTORY_ID] = irow.first[pof::ProductManager::INVENTORY_ID];


		wxGetApp().mProductManager.GetInvoices()->StoreData(std::move(inv));
		wxGetApp().mProductManager.GetSupplier()->StoreData(std::move(row));
	}
	else {
		auto suppid = boost::variant2::get<std::uint64_t>(it->first[pof::ProductManager::SUPPLIER_ID]);
		auto vv = wxGetApp().mProductManager.CheckIfProductInInvoice(
			suppid, str,
			boost::variant2::get<pof::base::data::duuid_t>(mProductData.first[pof::ProductManager::PRODUCT_UUID]));
		if (!vv.has_value()) {
			wxMessageBox("Error in checking product status in invoice, call D-Glopa Admin", "Add invoice", wxICON_ERROR | wxOK);
			return;
		}
		
		if (vv.value()){
			wxMessageBox(fmt::format("Product inventory already exists in {} for supplier {}", str, suppname), "Add invoice", wxICON_WARNING | wxOK);
			return;
		}

		pof::base::data::row_t inv;
		auto& iq = inv.first;
		iq.resize(pof::ProductManager::INVOICE_MAX);

		iq[pof::ProductManager::INVOICE_SUPP_ID] = boost::variant2::get<std::uint64_t>(it->first[pof::ProductManager::SUPPLIER_ID]);
		iq[pof::ProductManager::INVOICE_ID] = str;
		iq[pof::ProductManager::INVOICE_PROD_UUID] = mProductData.first[pof::ProductManager::PRODUCT_UUID];
		iq[pof::ProductManager::INVOICE_INVENTORY_ID] = irow.first[pof::ProductManager::INVENTORY_ID];

		wxGetApp().mProductManager.GetInvoices()->StoreData(std::move(inv));	
	}
	wxMessageBox(fmt::format("Created invoice {} for {} successfully!", str, suppname), "Add stock", wxICON_INFORMATION | wxOK);
}

void pof::ProductInfo::RemovePropertyModification()
{
	auto grid = m_propertyGridPage1->GetGrid();
	if (!grid->IsAnyModified()) return;

	auto gridIter = grid->GetIterator();
	while (!gridIter.AtEnd()) {
		if (gridIter.GetProperty()->HasFlag(wxPG_PROP_MODIFIED)) {
			gridIter.GetProperty()->SetFlagRecursively(wxPG_PROP_MODIFIED, false);
		}
		gridIter++;
	}
}
