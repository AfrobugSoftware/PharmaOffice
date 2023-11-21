#include "AddProductDialog.h"
#include "PofPch.h"
//#include "Application.h"

BEGIN_EVENT_TABLE(pof::AddProdutDialog, wxDialog)
	EVT_BUTTON(pof::AddProdutDialog::ID_SCAN_PRODUCT, pof::AddProdutDialog::OnScanProduct)
	EVT_CHECKBOX(pof::AddProdutDialog::ID_INVENTORY_ADD, pof::AddProdutDialog::OnInventoryCheck)
END_EVENT_TABLE()


pof::AddProdutDialog::AddProdutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) :
	wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel1->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	TitleText = new wxStaticText( m_panel1, wxID_ANY, wxT("Add Product"), wxDefaultPosition, wxDefaultSize, 0 );
	TitleText->Wrap( -1 );
	TitleText->SetFont( wxFont(wxFontInfo(10).AntiAliased().Bold()));
	
	bSizer2->Add( TitleText, 1, wxALL, 5 );

	wxStaticText* ext = new wxStaticText(m_panel1, wxID_ANY, wxT("Creates a product for the pharmacy"), wxDefaultPosition, wxDefaultSize, 0);
	ext->Wrap(-1);
	ext->SetFont(wxFont(wxFontInfo().AntiAliased()));

	bSizer2->Add(ext, 0, wxALL, 2);
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 0, wxEXPAND | wxALL, 5 );
	
	m_panel2 = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	m_panel2->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, wxT("Product Details") ), wxVERTICAL );
	
	mProductDetailsPanel = new wxPanel( sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mProductName = new wxStaticText( mProductDetailsPanel, wxID_ANY, wxT("Product Brand Name"), wxDefaultPosition, wxDefaultSize, 0 );
	mProductName->Wrap( -1 );
	fgSizer2->Add( mProductName, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mProductNameValue = new wxTextCtrl( mProductDetailsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mProductNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	fgSizer2->Add( mProductNameValue, 1, wxALL|wxEXPAND, 5 );
	
	mProductGenericName = new wxStaticText( mProductDetailsPanel, wxID_ANY, wxT("Generic Name"), wxDefaultPosition, wxDefaultSize, 0 );
	mProductGenericName->Wrap( -1 );
	fgSizer2->Add( mProductGenericName, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mGenericNameValue = new wxTextCtrl( mProductDetailsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mGenericNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	fgSizer2->Add( mGenericNameValue, 1, wxALL|wxEXPAND, 5 );
	
	mFormulation = new wxStaticText( mProductDetailsPanel, wxID_ANY, wxT("Formulation"), wxDefaultPosition, wxDefaultSize, 0 );
	mFormulation->Wrap( -1 );
	fgSizer2->Add( mFormulation, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	

	FormulationChoices.Add("TABLET");
	FormulationChoices.Add("CAPSULE");
	FormulationChoices.Add("SOLUTION");
	FormulationChoices.Add("SUSPENSION");
	FormulationChoices.Add("IV");
	FormulationChoices.Add("IM");
	FormulationChoices.Add("EMULSION");
	FormulationChoices.Add("CREAM");
	FormulationChoices.Add("COMSUMABLE"); //needles, cannula and the rest
	mFormulationValue = new wxChoice( mProductDetailsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, FormulationChoices, 0 );
	mFormulationValue->SetSelection( 0 );
	mFormulationValue->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	fgSizer2->Add( mFormulationValue, 0, wxALL|wxEXPAND, 5 );
	
	mStrength = new wxStaticText( mProductDetailsPanel, wxID_ANY, wxT("Strength"), wxDefaultPosition, wxDefaultSize, 0 );
	mStrength->Wrap( -1 );
	fgSizer2->Add( mStrength, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	wxFloatingPointValidator<double> val(2, &mFloatValidator, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 999999999999);

	mStrengthValue = new wxTextCtrl( mProductDetailsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mStrengthValue, 1, wxALL|wxEXPAND, 5 );
	
	mStrengthType = new wxStaticText( mProductDetailsPanel, wxID_ANY, wxT("Strength Type"), wxDefaultPosition, wxDefaultSize, 0 );
	mStrengthType->Wrap( -1 );
	fgSizer2->Add( mStrengthType, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
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
	mStrengthTypeValue = new wxChoice( mProductDetailsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, StrengthChoices, 0 );
	mStrengthTypeValue->SetSelection( 0 );
	fgSizer2->Add( mStrengthTypeValue, 1, wxALL|wxEXPAND, 5 );
	
	mClassLabel= new wxStaticText(mProductDetailsPanel, wxID_ANY, wxT("Product Class"), wxDefaultPosition, wxDefaultSize, 0);
	mClassLabel->Wrap(-1);
	fgSizer2->Add(mClassLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	ProductClassChoices.Add("POM");
	ProductClassChoices.Add("OTC");
	ProductClassChoices.Add("CONTROLLED");
	mClassValue = new wxChoice(mProductDetailsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, ProductClassChoices, 0);
	mClassValue->SetSelection(0);
	fgSizer2->Add(mClassValue, 1, wxALL | wxEXPAND, 5);


	mProductDetailsPanel->SetSizer( fgSizer2 );
	mProductDetailsPanel->Layout();
	fgSizer2->Fit( mProductDetailsPanel );
	sbSizer7->Add( mProductDetailsPanel, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel4->SetSizer( sbSizer7 );
	m_panel4->Layout();
	sbSizer7->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 0, wxEXPAND | wxALL, 5 );
	
	m_panel5 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxT("Product Additional Details") ), wxVERTICAL );
	
	mProductAddDetails = new wxPanel( sbSizer8->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_panel71 = new wxPanel( mProductAddDetails, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	mCostPriceLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("Cost Price"), wxDefaultPosition, wxDefaultSize, 0 );
	mCostPriceLabel->Wrap( -1 );
	bSizer7->Add( mCostPriceLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mCostPriceValue = new wxTextCtrl( m_panel71, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mCostPriceValue->SetValidator(val);
	bSizer7->Add( mCostPriceValue, 0, wxALL, 5 );
	
	mSalePriceLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("Sale Price"), wxDefaultPosition, wxDefaultSize, 0 );
	mSalePriceLabel->Wrap( -1 );
	bSizer7->Add( mSalePriceLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mSalePriceValue = new wxTextCtrl( m_panel71, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mSalePriceValue->SetValidator(val);
	bSizer7->Add( mSalePriceValue, 0, wxALL, 5 );
	
	mDoMarkup = new wxCheckBox( m_panel71, wxID_ANY, wxT("Mark up cost price"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( mDoMarkup, 0, wxBOTTOM|wxLEFT|wxTOP, 10 );
	
	mCategoryLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("Category"), wxDefaultPosition, wxDefaultSize, 0 );
	mCategoryLabel->Wrap( -1 );
	bSizer7->Add( mCategoryLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxArrayString mCategoryValueChoices;
	auto& cat = wxGetApp().mProductManager.GetCategories();
	for (auto& p : cat){
		mCategoryValueChoices.push_back(boost::variant2::get<std::string>(p.first[pof::ProductManager::CATEGORY_NAME]));
	}
	mCategoryValueChoices.push_back("No Category");
	mCategoryValue = new wxChoice( m_panel71, wxID_ANY, wxDefaultPosition, wxDefaultSize, mCategoryValueChoices, 0 );
	mCategoryValue->SetSelection( 0 );
	bSizer7->Add( mCategoryValue, 1, wxALL, 5 );
	
	mPackageSizeLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("Package Size"), wxDefaultPosition, wxDefaultSize, 0 );
	mPackageSizeLabel->Wrap( -1 );
	bSizer7->Add( mPackageSizeLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	mPackageSizeValue = new wxTextCtrl( m_panel71, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPackageSizeValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	bSizer7->Add( mPackageSizeValue, 0, wxALL, 5 );
	
	
	m_panel71->SetSizer( bSizer7 );
	m_panel71->Layout();
	bSizer7->Fit( m_panel71 );
	bSizer5->Add( m_panel71, 0, wxEXPAND | wxALL, 2 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mProductDescription = new wxStaticText( mProductAddDetails, wxID_ANY, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	mProductDescription->Wrap( -1 );
	fgSizer3->Add( mProductDescription, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mProductDescriptionValue = new wxTextCtrl( mProductAddDetails, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mProductDescriptionValue, 1, wxALL|wxEXPAND, 5 );
		
	mDirectionForUse = new wxStaticText( mProductAddDetails, wxID_ANY, wxT("Direction For Use"), wxDefaultPosition, wxDefaultSize, 0 );
	mDirectionForUse->Wrap( -1 );
	fgSizer3->Add( mDirectionForUse, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mDirForUseValue = new wxTextCtrl( mProductAddDetails, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mDirForUseValue, 0, wxALL|wxEXPAND, 5 );
	
	mHealthConditions = new wxStaticText( mProductAddDetails, wxID_ANY, wxT("Health Conditions"), wxDefaultPosition, wxDefaultSize, 0 );
	mHealthConditions->Wrap( -1 );
	fgSizer3->Add( mHealthConditions, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mHealthConditionsValue = new wxTextCtrl( mProductAddDetails, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mHealthConditionsValue, 0, wxALL|wxEXPAND, 5 );
	

	mSideEffectsLabel = new wxStaticText( mProductAddDetails, wxID_ANY, wxT("Adverse Reactions"), wxDefaultPosition, wxDefaultSize, 0 );
	mSideEffectsLabel->Wrap( -1 );
	fgSizer3->Add( mSideEffectsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mSideEffectsValue = new wxTextCtrl( mProductAddDetails, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mSideEffectsValue, 1, wxALL|wxEXPAND, 5 );
		
	bSizer5->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	m_panel8 = new wxPanel( mProductAddDetails, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	mScanProduct = new wxButton( m_panel8, ID_SCAN_PRODUCT, wxT("Scan Product"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( mScanProduct, 0, wxALIGN_CENTER|wxALL, 0 );
	
	
	m_panel8->SetSizer( bSizer8 );
	m_panel8->Layout();
	bSizer8->Fit( m_panel8 );
	bSizer5->Add( m_panel8, 0, wxEXPAND | wxALL, 5 );
	
	
	mProductAddDetails->SetSizer( bSizer5 );
	mProductAddDetails->Layout();
	bSizer5->Fit( mProductAddDetails );
	sbSizer8->Add( mProductAddDetails, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel5->SetSizer( sbSizer8 );
	m_panel5->Layout();
	sbSizer8->Fit( m_panel5 );
	bSizer3->Add( m_panel5, 0, wxALL|wxEXPAND|wxFIXED_MINSIZE, 5 );
	
	m_panel6 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Product Inventory") ), wxVERTICAL );
	
	mProductInvenPanel = new wxPanel( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer21->AddGrowableCol( 1 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mAddInventory = new wxCheckBox(mProductInvenPanel, ID_INVENTORY_ADD,wxT("Add Inventory"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer21->Add(mAddInventory, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	fgSizer21->AddStretchSpacer();

	mBacthNumber = new wxStaticText( mProductInvenPanel, wxID_ANY, wxT("Batch Number"), wxDefaultPosition, wxDefaultSize, 0 );
	mBacthNumber->Wrap( -1 );
	fgSizer21->Add( mBacthNumber, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mBatchNumbeValue = new wxTextCtrl( mProductInvenPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mBatchNumbeValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	fgSizer21->Add( mBatchNumbeValue, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText8 = new wxStaticText( mProductInvenPanel, wxID_ANY, wxT("Product Expiry Date"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer21->Add( m_staticText8, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	m_datePicker1 = new wxDatePickerCtrl( mProductInvenPanel, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_DROPDOWN );
	m_datePicker1->SetRange(wxDateTime::Now(), wxDateTime{});
	fgSizer21->Add( m_datePicker1, 1, wxALL|wxEXPAND, 5 );
	
	mQuntity = new wxStaticText( mProductInvenPanel, wxID_ANY, wxT("Quantity"), wxDefaultPosition, wxDefaultSize, 0 );
	mQuntity->Wrap( -1 );
	fgSizer21->Add( mQuntity, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	
	mQunatityValue = new wxTextCtrl( mProductInvenPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mQunatityValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	fgSizer21->Add( mQunatityValue, 0, wxALL|wxEXPAND, 5 );
	
	mSupplierName = new wxStaticText(mProductInvenPanel, wxID_ANY, wxT("Supplier Name"), wxDefaultPosition, wxDefaultSize, 0);
	mSupplierName->Wrap(-1);
	fgSizer21->Add(mSupplierName, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mSuplierNameValue = new wxTextCtrl(mProductInvenPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer21->Add(mSuplierNameValue, 0, wxALL | wxEXPAND, 5);

	mCostPerUnitName = new wxStaticText(mProductInvenPanel, wxID_ANY, wxT("Cost Price Per Unit(N)"), wxDefaultPosition, wxDefaultSize, 0);
	mSupplierName->Wrap(-1);
	fgSizer21->Add(mCostPerUnitName, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	mCostPerUnitValue = new wxTextCtrl(mProductInvenPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mCostPerUnitValue->SetValidator(val);
	fgSizer21->Add(mCostPerUnitValue, 0, wxALL | wxEXPAND, 5);


	mProductInvenPanel->SetSizer( fgSizer21 );
	mProductInvenPanel->Layout();
	fgSizer21->Fit( mProductInvenPanel );
	sbSizer3->Add( mProductInvenPanel, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel6->SetSizer( sbSizer3 );
	m_panel6->Layout();
	sbSizer3->Fit( m_panel6 );
	bSizer3->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer1->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	/*m_panel7 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	*/
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	
	
	bSizer1->Add( m_sdbSizer2, 0, wxEXPAND | wxALL, 5 );
	
	//m_panel7->SetSizer( bSizer4 );
	//m_panel7->Layout();
	//bSizer4->Fit( m_panel7 );
	//bSizer1->Add( m_panel7, 0, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	//bSizer1->Fit(this);
	
	this->Centre( wxBOTH );

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::AddProdutDialog::~AddProdutDialog()
{
}

bool pof::AddProdutDialog::TransferDataFromWindow()
{
	datum.emplace();
	datum->second.first.set(static_cast<std::underlying_type_t<pof::base::data::state>>(pof::base::data::state::CREATED));
	auto& v = datum->first;
	v.resize(pof::ProductManager::PRODUCT_MAX);
	auto& productDatastore = wxGetApp().mProductManager.GetProductData()->GetDatastore();
	if (std::ranges::any_of(productDatastore, [&](const auto& row) -> bool {
		return boost::variant2::get<std::string>(row.first[pof::ProductManager::PRODUCT_NAME]) ==
			mProductNameValue->GetValue().ToStdString();
	})){
		wxMessageBox("Product with that name exists in pharmacy", "Add Product", wxICON_WARNING | wxOK);
		return false;
	}

	v[pof::ProductManager::PRODUCT_UUID] = boost::uuids::random_generator_mt19937{}();
	v[pof::ProductManager::PRODUCT_SERIAL_NUM] =  pof::GenRandomId(); //change this 
	v[pof::ProductManager::PRODUCT_NAME] = std::move(mProductNameValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_GENERIC_NAME] = std::move(mGenericNameValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_FORMULATION] = std::move(FormulationChoices[mFormulationValue->GetSelection()].ToStdString());
	v[pof::ProductManager::PRODUCT_CLASS] = std::move(ProductClassChoices[mClassValue->GetSelection()].ToStdString());
	v[pof::ProductManager::PRODUCT_MIN_STOCK_COUNT] = static_cast<std::uint64_t>(0);
	v[pof::ProductManager::PRODUCT_BARCODE] = std::move(mScanProductString);
	v[pof::ProductManager::PRODUCT_COST_PRICE] = pof::base::data::currency_t(mCostPriceValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_UNIT_PRICE] = pof::base::data::currency_t(mSalePriceValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_USAGE_INFO] = std::move(mDirForUseValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_HEALTH_CONDITIONS] = std::move(mHealthConditionsValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_DESCRIP] = std::move(mProductDescriptionValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_STRENGTH] = std::move(mStrengthValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_STRENGTH_TYPE] = std::move(mStrengthTypeValue->GetStringSelection().ToStdString());
	v[pof::ProductManager::PRODUCT_PACKAGE_SIZE] = static_cast<std::uint64_t>(atoi(mPackageSizeValue->GetValue().ToStdString().c_str()));
	v[pof::ProductManager::PRODUCT_SIDEEFFECTS] = std::move(mSideEffectsValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_STOCK_COUNT] = static_cast<std::uint64_t>(atoi(mQunatityValue->GetValue().ToStdString().c_str()));
	
	//
	v[pof::ProductManager::PRODUCT_EXPIRE_PERIOD] = static_cast<std::uint64_t>(0);
	v[pof::ProductManager::PRODUCT_TO_EXPIRE_DATE] = ""s;

	auto& cat = wxGetApp().mProductManager.GetCategories();
	if (cat.empty()) v[pof::ProductManager::PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);
	else {
		int idx = mCategoryValue->GetSelection();
		if(idx == wxNOT_FOUND) v[pof::ProductManager::PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);
		else {
			if(idx >= cat.size()) v[pof::ProductManager::PRODUCT_CATEGORY] = static_cast<std::uint64_t>(0);
			auto& r = cat[idx];
			v[pof::ProductManager::PRODUCT_CATEGORY] = r.first[pof::ProductManager::CATEGORY_ID];
		}
	}

	//only emplace when the fields are 
	if (mAddInventory->GetValue()) {
		datumInven.emplace();
		datumInven->second.first.set(static_cast<std::underlying_type_t<pof::base::data::state>>(pof::base::data::state::CREATED));
		auto& i = datumInven->first;
		i.resize(pof::ProductManager::INVENTORY_MAX);
		i[pof::ProductManager::INVENTORY_PRODUCT_UUID] = v[pof::ProductManager::PRODUCT_UUID];
		i[pof::ProductManager::INVENTORY_ID] = pof::GenRandomId();//static_cast<std::uint64_t>(0); //first ever inventory
		i[pof::ProductManager::INVENTORY_LOT_NUMBER] = std::move(mBatchNumbeValue->GetValue().ToStdString());
		i[pof::ProductManager::INVENTORY_STOCK_COUNT] = static_cast<std::uint64_t>(atoi(mQunatityValue->GetValue().ToStdString().c_str()));
		i[pof::ProductManager::INVENTORY_INPUT_DATE] = pof::base::data::clock_t::now();
		auto expDate = std::chrono::system_clock::from_time_t(m_datePicker1->GetValue().GetTicks());
		if (date::floor<date::days>(expDate) == date::floor<date::days>(pof::base::data::clock_t::now())) {
			wxMessageBox("Expiry date cannot be today's date, check and try again", "Add Stock", wxICON_INFORMATION | wxOK);
			return false;
		}
		i[pof::ProductManager::INVENTORY_EXPIRE_DATE] = expDate;
		i[pof::ProductManager::INVENTORY_COST] = pof::base::currency{ mCostPerUnitValue->GetValue().ToStdString() };
		i[pof::ProductManager::INVENTORY_MANUFACTURER_NAME] = std::move(mSuplierNameValue->GetValue().ToStdString()); //test
		i[pof::ProductManager::INVENTORY_MANUFACTURER_ADDRESS_ID] = static_cast<std::uint64_t>(9999);
	}
	return true;
}

void pof::AddProdutDialog::OnScanProduct(wxCommandEvent& evt)
{
	wxTextEntryDialog dialog(this, "Please scan a product with a barcode scanner","SCAN PRODUCT");
	dialog.SetTextValidator(wxTextValidator{ wxFILTER_DIGITS });
	if (dialog.ShowModal() == wxID_OK) {
		mScanProductString = std::move(dialog.GetValue().ToStdString());
	}
}

void pof::AddProdutDialog::OnMoreDescription(wxCommandEvent& evt)
{
}

void pof::AddProdutDialog::OnMoreDirForUse(wxCommandEvent& evt)
{
}

void pof::AddProdutDialog::OnMoreSideffects(wxCommandEvent& evt)
{
}

void pof::AddProdutDialog::OnMoreHealthConditions(wxCommandEvent& evt)
{
}

void pof::AddProdutDialog::OnInventoryCheck(wxCommandEvent& evt)
{
	if (evt.IsChecked())
	{
		mBatchNumbeValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	}
	else {
		mBatchNumbeValue->SetValidator(wxTextValidator{ });
	}
}
