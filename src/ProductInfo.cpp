
#include "ProductInfo.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::ProductInfo, wxPanel)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_GO_BACK, pof::ProductInfo::OnGoBack)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_ADD_INVENTORY, pof::ProductInfo::OnAddInventory)
	EVT_PG_CHANGED(pof::ProductInfo::ID_PROPERTY_GRID, pof::ProductInfo::OnPropertyChanged)
	EVT_SPLITTER_UNSPLIT(pof::ProductInfo::ID_SPLIT_WINDOW, pof::ProductInfo::OnUnspilt)
	EVT_SPLITTER_DCLICK(pof::ProductInfo::ID_SPLIT_WINDOW, pof::ProductInfo::OnSashDoubleClick)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_SHOW_PRODUCT_INFO, pof::ProductInfo::OnShowProductInfo)
END_EVENT_TABLE()


pof::ProductInfo::ProductInfo( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, ID_SPLIT_WINDOW, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE|wxSP_NOBORDER | wxNO_BORDER );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( ProductInfo::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_auiToolBar1 = new wxAuiToolBar( this , wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT|wxAUI_TB_HORZ_TEXT|wxAUI_TB_OVERFLOW | wxNO_BORDER); 
	m_auiToolBar1->SetMinSize( wxSize( -1,30 ) );
	
	m_auiToolBar1->AddTool(ID_TOOL_GO_BACK, wxEmptyString, wxArtProvider::GetBitmap("arrow_back"), "Back", wxITEM_NORMAL);
	mProductNameText = m_auiToolBar1->AddTool( wxID_ANY, wxEmptyString, wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddTool(ID_TOOL_ADD_INVENTORY, wxT("Add Inventory"), wxArtProvider::GetBitmap("action_add"), "Add Inventory", wxITEM_NORMAL);
	m_auiToolBar1->AddTool(ID_TOOL_REMV_EXPIRE_BATCH, wxT("Remove Expired Inventory Batches"), wxArtProvider::GetBitmap("action_remove"), "Remove all expired batches", wxITEM_NORMAL);
	mShowAddInfo = m_auiToolBar1->AddTool(ID_TOOL_SHOW_PRODUCT_INFO, wxT("Product Information"), wxArtProvider::GetBitmap("action_check"), "Show the products information grid", wxITEM_CHECK);
	m_auiToolBar1->Realize(); 
	
	
	InventoryView = new wxDataViewCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_SINGLE | wxNO_BORDER );
	InventoryView->AssociateModel(wxGetApp().mProductManager.GetInventory().get());

	
	mInputDate = InventoryView->AppendTextColumn( wxT("Input Date"), pof::ProductManager::INVENTORY_INPUT_DATE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mBactchNo = InventoryView->AppendTextColumn( wxT("Batch No"), pof::ProductManager::INVENTORY_LOT_NUMBER, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mExpiryDate = InventoryView->AppendTextColumn( wxT("Expiry Date"), pof::ProductManager::INVENTORY_EXPIRE_DATE, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mStockCount = InventoryView->AppendTextColumn( wxT("Entry Stock Amount"), pof::ProductManager::INVENTORY_STOCK_COUNT, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	mManuFactureName = InventoryView->AppendTextColumn( wxT("Manufactural Name"), pof::ProductManager::INVENTORY_MANUFACTURER_NAME, wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE);
	bSizer2->Add( InventoryView, 1, wxALL|wxEXPAND, 0 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_propertyGridManager1 = new wxPropertyGridManager(m_panel2, ID_PROPERTY_GRID, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE|wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPG_SPLITTER_AUTO_CENTER|wxPG_TOOLBAR|wxPG_TOOLTIPS|wxTAB_TRAVERSAL | wxNO_BORDER);
	m_propertyGridManager1->SetExtraStyle( wxPG_EX_MODE_BUTTONS ); 
	
	m_propertyGridPage1 = m_propertyGridManager1->AddPage( wxT("Product Information"), wxNullBitmap );
	m_propertyGridItem1 = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Product Details"), wxT("Product Details") ) ); 
	mNameItem = m_propertyGridPage1->Append( new wxStringProperty( wxT("NAME"), wxPG_LABEL) );
	m_propertyGridPage1->SetPropertyHelpString( mNameItem, wxT("The product brand name") );
	mGenericNameItem = m_propertyGridPage1->Append( new wxArrayStringProperty( wxT("GENERIC NAME"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mGenericNameItem, wxT("The generic name for the product") );
	mPackageSizeItem = m_propertyGridPage1->Append( new wxIntProperty( wxT("PACKAGE SIZE"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mPackageSizeItem, wxT("Size of a single unit of product for sale. For example, for anti-malaria with 6 tablets would have package size set to 6 and formulation set to tablets") );

	ProductClassChoices.Add("POM");
	ProductClassChoices.Add("OTC");
	mProductClass = m_propertyGridPage1->Append(new wxEnumProperty(wxT("CLASS"), wxPG_LABEL, ProductClassChoices));
	m_propertyGridPage1->SetPropertyHelpString( mProductClass, wxT("Products can be POM for Prescription only medication, these medicines can only be sold with a valid prescription. Either online or offline. OTC for Over the counter medicines. There are medications that can be sold on pharmacy. Without a prescription. An alert is sent when an attempt is made to sell a POM without a prescription.") );
	
	FormulationChoices.Add("TABLET");
	FormulationChoices.Add("CAPSULE");
	FormulationChoices.Add("SOLUTION");
	FormulationChoices.Add("SUSPENSION");
	FormulationChoices.Add("IV");
	FormulationChoices.Add("IM");
	FormulationChoices.Add("EMULSION");
	FormulationChoices.Add("COMSUMABLE"); //needles, cannula and the rest
	FormulationChoices.Add("NOT SPECIFIED"); //NOT SPECIFIED
	mFormulationItem = m_propertyGridPage1->Append( new wxEnumProperty( wxT("FORMULATION"), wxPG_LABEL, FormulationChoices));
	m_propertyGridPage1->SetPropertyHelpString( mFormulationItem, wxT("Product formulation is the form in which this product is in, for example, tablet, capsules, injectables, or solutions.") );
	


	mMoreProductInfo = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("MORE PRODUCT INFORMATION"), wxT("More Product Information") ) ); 
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

	StrengthChoices.Add("g");
	StrengthChoices.Add("mg");
	StrengthChoices.Add("mcg");

	StrengthChoices.Add("L");
	StrengthChoices.Add("ml");
	StrengthChoices.Add("ml");

	StrengthChoices.Add("%v/v");
	StrengthChoices.Add("%w/v");

	StrengthChoices.Add("mol");
	StrengthChoices.Add("mmol");
	StrengthChoices.Add("NOT SPECIFIED");

	mStrengthTypeItem = m_propertyGridPage1->Append(new wxEnumProperty(wxT("STRENGTH TYPE"), wxPG_LABEL, StrengthChoices));
	mSettings = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Settings"), wxT("Settings"))); 
	mMinStockCount = m_propertyGridPage1->Append( new wxIntProperty( wxT("MINIMUM STOCK COUNT"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mMinStockCount, wxT("The amount of stock that should indicate stock level is low. ") );
	mExpDateCount = m_propertyGridPage1->Append( new wxIntProperty( wxT("EXPIRE ALERT"), wxPG_LABEL ) );
	m_propertyGridPage1->SetPropertyHelpString( mExpDateCount, wxT("Number of (Days, Weeks, Months) before expiry date that an alert should be sent") );
	
	ExpChoices.Add("DAY");
	ExpChoices.Add("WEEK");
	ExpChoices.Add("MONTH");

	mExpDatePeriod = m_propertyGridPage1->Append( new wxEnumProperty( wxT("EXPIRE ALERT PERIOD"), wxPG_LABEL, ExpChoices) );
	m_propertyGridPage1->SetPropertyHelpString( mExpDatePeriod, wxT("Select the period in which the expire alert defines") );
	
	mSaleSettings = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("SALE"), wxT("Sale") )); 

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

		mProductNameText->SetLabel(fmt::format("{} - INVENTORY HISTORY", Name));

		//should get data from the server on the selected product 
		
		auto& Model = wxGetApp().mProductManager.GetInventory();
		//unpack data from where 
		//Model->Unpack(...);
		m_auiToolBar1->Realize();
		m_auiToolBar1->Refresh();
		m_auiToolBar1->Update();
		mProductData = row;
		LoadProductProperty(row);
 	}
	catch (const std::exception& exp) {
		spdlog::critical(exp.what());
	}

}

wxArrayString SplitIntoArrayString(const std::string& string)
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
	mGenericNameItem->SetValue(std::get<pof::ProductManager::PRODUCT_GENERIC_NAME>(tup));
	mProductClass->SetValue(std::get<pof::ProductManager::PRODUCT_CLASS>(tup));
	mPackageSizeItem->SetValue(static_cast<int>(std::get<pof::ProductManager::PRODUCT_PACKAGE_SIZE>(tup)));
	mUnitPrice->SetValue(static_cast<double>(std::get<pof::ProductManager::PRODUCT_UNIT_PRICE>(tup)));
	mCostPrice->SetValue(static_cast<double>(std::get<pof::ProductManager::PRODUCT_COST_PRICE>(tup)));
	mMinStockCount->SetValue(static_cast<int>(std::get<pof::ProductManager::PRODUCT_MIN_STOCK_COUNT>(tup)));
	mDirForUse->SetValue(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_USAGE_INFO>(tup)));
	mHealthCond->SetValue(wxVariant(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_HEALTH_CONDITIONS>(tup))));
	mProductDescription->SetValue(std::get<pof::ProductManager::PRODUCT_DESCRIP>(tup));
	mSideEffects->SetValue(wxVariant(SplitIntoArrayString(std::get<pof::ProductManager::PRODUCT_SIDEEFFECTS>(tup))));

	wxArrayString Ar = std::move(FormulationChoices.GetLabels());
	auto FormIter = std::find(Ar.begin(), Ar.end(), std::get<pof::ProductManager::PRODUCT_FORMULATION>(tup));
	if ((FormIter != Ar.end())) {
		auto idx = std::distance(Ar.begin(), FormIter);
		mFormulationItem->SetValue(wxVariant(static_cast<int>(idx)));
	}
	else {
		mFormulationItem->SetValue(static_cast<int>(Ar.size() - 1));
	}

	mStrengthValueItem->SetValue(wxVariant(std::get<pof::ProductManager::PRODUCT_STRENGTH>(tup)));

	Ar = std::move(StrengthChoices.GetLabels());
	auto Iter = std::find(Ar.begin(), Ar.end(), std::get<pof::ProductManager::PRODUCT_STRENGTH_TYPE>(tup));
	if ((Iter != Ar.end())) {
		auto idx = std::distance(Ar.begin(), Iter);
		mStrengthTypeItem->SetValue(wxVariant(static_cast<int>(idx)));
	}
	else {
		mStrengthTypeItem->SetValue(static_cast<int>(Ar.size() - 1));
	}
	//load the inventory data
	wxGetApp().mProductManager.GetInventory()->Clear();
	wxGetApp().mProductManager.LoadInventoryData(std::get<pof::ProductManager::PRODUCT_UUID>(tup));


	//expire peroid 
	//expire date
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
	mNameToProductElem.insert({ "STRENGTH/CONC. ", pof::ProductManager::PRODUCT_STRENGTH });
	mNameToProductElem.insert({ "STRENGTH TYPE", pof::ProductManager::PRODUCT_STRENGTH_TYPE });
	mNameToProductElem.insert({ "SIDE EFFECTS", pof::ProductManager::PRODUCT_SIDEEFFECTS});
	mNameToProductElem.insert({ "EXPIRE ALERT PERIOD", pof::ProductManager::PRODUCT_EXPIRE_PERIOD });
	mNameToProductElem.insert({ "EXPIRE ALERT", pof::ProductManager::PRODUCT_TO_EXPIRE_DATE });
	mNameToProductElem.insert({ "FORMULATION", pof::ProductManager::PRODUCT_FORMULATION });
}

void pof::ProductInfo::OnGoBack(wxCommandEvent& evt)
{
	mBackSignal();
	if (mPropertyUpdate.has_value()) {
		wxBusyInfo info("Saving...");
		mUpdatePropertySignal(mPropertyUpdate.value());
		mPropertyUpdate = {};
		RemovePropertyModification();
	}
}

void pof::ProductInfo::OnAddInventory(wxCommandEvent& evt)
{
	pof::InventoryDialog dialog(nullptr);
	if (dialog.ShowModal() == wxID_OK) {
		auto& Inven = dialog.GetData();
		Inven.first[pof::ProductManager::INVENTORY_MANUFACTURER_NAME] = "D-GLOPA PHARMACEUTICALS";
		Inven.first[pof::ProductManager::INVENTORY_PRODUCT_UUID] = mProductData.first[pof::ProductManager::PRODUCT_UUID];

		if (!mPropertyUpdate.has_value()) {
			mPropertyUpdate.emplace();
			mPropertyUpdate->mUpdatedElementsValues.first.resize(pof::ProductManager::PRODUCT_MAX);
			mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_UUID] =
				mProductData.first[pof::ProductManager::PRODUCT_UUID];
		}

		mPropertyUpdate->mUpdatedElememts.set(pof::ProductManager::PRODUCT_STOCK_COUNT);
		mPropertyUpdate->mUpdatedElementsValues.first[pof::ProductManager::PRODUCT_STOCK_COUNT] =
			boost::variant2::get<std::uint64_t>(Inven.first[pof::ProductManager::INVENTORY_STOCK_COUNT]) 
				+ boost::variant2::get<std::uint64_t>(mProductData.first[pof::ProductManager::PRODUCT_STOCK_COUNT]);

		wxGetApp().mProductManager.GetInventory()->StoreData(std::move(Inven));
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

void pof::ProductInfo::OnPropertyChanged(wxPropertyGridEvent& evt)
{
	wxPGProperty* props = evt.GetProperty();
	if(props->IsCategory()) return;
	spdlog::info("{}", evt.GetPropertyName().ToStdString());
	auto PropIter = mNameToProductElem.find(evt.GetPropertyName().ToStdString());
	if (PropIter == mNameToProductElem.end()) return;
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
	case pof::ProductManager::PRODUCT_TO_EXPIRE_DATE:
		//v[pof::ProductManager::PRODUCT_TO_EXPIRE_DATE] = std::move("");
		break;
	case pof::ProductManager::PRODUCT_SIDEEFFECTS:
		v[pof::ProductManager::PRODUCT_SIDEEFFECTS] = std::move(JoinArrayList(PropertyValue));
		break;
	case pof::ProductManager::PRODUCT_FORMULATION:
		v[pof::ProductManager::PRODUCT_FORMULATION] = 
				std::move(FormulationChoices.GetLabel(PropertyValue.GetInteger()).ToStdString());
		break;
	case pof::ProductManager::PRODUCT_STRENGTH:
		v[pof::ProductManager::PRODUCT_STRENGTH] = PropertyValue.GetString().ToStdString();
		break;
	case pof::ProductManager::PRODUCT_STRENGTH_TYPE:
		v[pof::ProductManager::PRODUCT_STRENGTH_TYPE] = std::move(StrengthChoices.GetLabel(PropertyValue.GetInteger()).ToStdString());
		break;
	default:
		return;
	}
	
	spdlog::info("Property {} Changed", evt.GetPropertyName().ToStdString());
	evt.Veto(); //might not be necessary
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
		m_panel2->SetSize(wxSize(400, -1));
		m_panel2->Layout();
		m_splitter1->SplitVertically(m_panel1, m_panel2, 0);
		m_splitter1->SetSashGravity(1.0);
		m_splitter1->Update();
		//m_splitter1->Connect(wxEVT_IDLE, wxIdleEventHandler(ProductInfo::m_splitter1OnIdle), NULL, this);
	}
	else {
		m_splitter1->Unsplit();
	}
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
