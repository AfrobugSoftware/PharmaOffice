
#include "ProductInfo.h"
#include "Application.h"

BEGIN_EVENT_TABLE(pof::ProductInfo, wxPanel)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_GO_BACK, pof::ProductInfo::OnGoBack)
	EVT_TOOL(pof::ProductInfo::ID_TOOL_ADD_INVENTORY, pof::ProductInfo::OnAddInventory)
END_EVENT_TABLE()


pof::ProductInfo::ProductInfo( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE|wxSP_NOBORDER | wxNO_BORDER );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( ProductInfo::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_auiToolBar1 = new wxAuiToolBar( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT|wxAUI_TB_HORZ_TEXT|wxAUI_TB_OVERFLOW | wxNO_BORDER); 
	m_auiToolBar1->SetMinSize( wxSize( -1,30 ) );
	
	m_auiToolBar1->AddTool(ID_TOOL_GO_BACK, wxEmptyString, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_TOOLBAR), "Back", wxITEM_NORMAL);
	mProductNameText = m_auiToolBar1->AddTool( wxID_ANY, wxEmptyString, wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_auiToolBar1->AddSeparator();
	m_auiToolBar1->AddTool(ID_TOOL_ADD_INVENTORY, wxT("Add Inventory"), wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR), "Add Inventory", wxITEM_NORMAL);
	m_auiToolBar1->AddTool(ID_TOOL_REMV_EXPIRE_BATCH, wxT("Remove Expired Inventory Batches"), wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR), "Remove all expired batches", wxITEM_NORMAL);

	m_auiToolBar1->Realize(); 
	
	bSizer2->Add( m_auiToolBar1, 0, wxALL|wxEXPAND, 0 );
	
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
	
	m_propertyGridManager1 = new wxPropertyGridManager(m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE|wxPG_BOLD_MODIFIED|wxPG_DESCRIPTION|wxPG_SPLITTER_AUTO_CENTER|wxPG_TOOLBAR|wxPG_TOOLTIPS|wxTAB_TRAVERSAL | wxNO_BORDER);
	m_propertyGridManager1->SetExtraStyle( wxPG_EX_MODE_BUTTONS ); 
	
	m_propertyGridPage1 = m_propertyGridManager1->AddPage( wxT("Product Information"), wxNullBitmap );
	m_propertyGridItem1 = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Product Details"), wxT("Product Details") ) ); 
	mNameItem = m_propertyGridPage1->Append( new wxStringProperty( wxT("Name"), wxT("Name") ) );
	m_propertyGridPage1->SetPropertyHelpString( mNameItem, wxT("The product brand name") );
	mGenericNameItem = m_propertyGridPage1->Append( new wxStringProperty( wxT("Generic Name"), wxT("Generic Name") ) );
	m_propertyGridPage1->SetPropertyHelpString( mGenericNameItem, wxT("The generic name for the product") );
	mPackageSizeItem = m_propertyGridPage1->Append( new wxIntProperty( wxT("Package Size"), wxT("Package Size") ) );
	m_propertyGridPage1->SetPropertyHelpString( mPackageSizeItem, wxT("Size of a single unit of product for sale. For example, for anti-malaria with 6 tablets would have package size set to 6 and formulation set to tablets") );

	ProductClassChoices.Add("POM");
	ProductClassChoices.Add("OTC");
	mProductClass = m_propertyGridPage1->Append(new wxEnumProperty(wxT("Class"), wxT("Class"), ProductClassChoices));
	m_propertyGridPage1->SetPropertyHelpString( mProductClass, wxT("Products can be POM for Prescription only medication, these medicines can only be sold with a valid prescription. Either online or offline. OTC for Over the counter medicines. There are medications that can be sold on pharmacy. Without a prescription. An alert is sent when an attempt is made to sell a POM without a prescription.") );
	
	FormulationChoices.Add("TABLET");
	FormulationChoices.Add("CAPSULE");
	FormulationChoices.Add("SOLUTION");
	FormulationChoices.Add("SUSPENSION");
	FormulationChoices.Add("IV");
	FormulationChoices.Add("IM");
	FormulationChoices.Add("EMULSION");
	FormulationChoices.Add("COMSUMABLE"); //needles, cannula and the rest
	mFormulationItem = m_propertyGridPage1->Append( new wxEnumProperty( wxT("Formulation"), wxT("Formulation"), FormulationChoices));
	m_propertyGridPage1->SetPropertyHelpString( mFormulationItem, wxT("Product formulation is the form in which this product is in, for example, tablet, capsules, injectables, or solutions.") );
	
	mMoreProductInfo = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("More Product Information"), wxT("More Product Information") ) ); 
	mDirForUse = m_propertyGridPage1->Append( new wxArrayStringProperty( wxT("Direction For Use"), wxT("Direction For Use") ) );
	m_propertyGridPage1->SetPropertyHelpString( mDirForUse, wxT("Information of usage. This would be printed on the label when sold over the counter") );
	mHealthCond = m_propertyGridPage1->Append( new wxArrayStringProperty( wxT("Health Conditions"), wxT("Health Conditions") ) );
	m_propertyGridPage1->SetPropertyHelpString( mHealthCond, wxT("A list of possible health ") );
	mProductDescription = m_propertyGridPage1->Append( new wxLongStringProperty( wxT("Description"), wxT("Description") ) );
	m_propertyGridPage1->SetPropertyHelpString( mProductDescription, wxT("Describes the product in a way that it can be added to a formulary") );
	mSettings = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Settings"), wxT("Settings") ) ); 
	mMinStockCount = m_propertyGridPage1->Append( new wxIntProperty( wxT("Minimum Stock Count"), wxT("Minimum Stock Count") ) );
	m_propertyGridPage1->SetPropertyHelpString( mMinStockCount, wxT("The amount of stock that should indicate stock level is low. ") );
	mExpDateCount = m_propertyGridPage1->Append( new wxIntProperty( wxT("Expire Alert"), wxT("Expire Alert") ) );
	m_propertyGridPage1->SetPropertyHelpString( mExpDateCount, wxT("Number of (Days, Weeks, Months) before expiry date that an alert should be sent") );
	
	ExpChoices.Add("DAY");
	ExpChoices.Add("WEEK");
	ExpChoices.Add("MONTH");

	mExpDatePeriod = m_propertyGridPage1->Append( new wxEnumProperty( wxT("Expire Alert Period"), wxT("Expire Alert Period"), ExpChoices) );
	m_propertyGridPage1->SetPropertyHelpString( mExpDatePeriod, wxT("Select the period in which the expire alert defines") );
	
	mSaleSettings = m_propertyGridPage1->Append( new wxPropertyCategory( wxT("Sale"), wxT("Sale") ) ); 
	mUnitPrice = m_propertyGridPage1->Append( new wxFloatProperty( wxT("Unit Price"), wxT("Unit Price")) );
	m_propertyGridPage1->SetPropertyHelpString( mUnitPrice, wxT("Price per Package size of the Product\n") );

	bSizer3->Add( m_propertyGridManager1, 1, wxALL|wxEXPAND, 0 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 400); //splitter posistion
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
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

void pof::ProductInfo::LoadProductProperty(const pof::base::data::row_t& row)
{
	auto& d = row.first;
	mNameItem->SetValue(wxVariant(boost::variant2::get<std::string>(d[pof::ProductManager::PRODUCT_NAME])));
	mPackageSizeItem->SetValue(wxVariant(boost::variant2::get<std::int32_t>(d[pof::ProductManager::PRODUCT_PACKAGE_SIZE])));

	//
}

boost::signals2::connection pof::ProductInfo::AttachBackSlot(back_signal_t::slot_type&& slot)
{
	return mBackSignal.connect(std::forward<pof::ProductInfo::back_signal_t::slot_type>(slot));
}

boost::signals2::connection pof::ProductInfo::AttachPropertyUpdateSlot(update_signal_t::slot_type&& slot)
{
	return mUpdatePropertySignal.connect(std::forward<pof::ProductInfo::update_signal_t::slot_type>(slot));
}

void pof::ProductInfo::OnGoBack(wxCommandEvent& evt)
{
	mBackSignal();
}

void pof::ProductInfo::OnAddInventory(wxCommandEvent& evt)
{
	pof::InventoryDialog dialog(nullptr);
	if (dialog.ShowModal() == wxID_OK) {
		auto& Inven = dialog.GetData();
		Inven.first[pof::ProductManager::INVENTORY_MANUFACTURER_NAME] = "D-GLOPA PHARMACEUTICALS";

		wxGetApp().mProductManager.GetInventory()->EmplaceData(std::move(Inven));
	}
	else {
		//rejected
	}
}
