
#include "AddProductDialog.h"
BEGIN_EVENT_TABLE(pof::AddProdutDialog, wxDialog)
	EVT_BUTTON(pof::AddProdutDialog::ID_SCAN_PRODUCT, pof::AddProdutDialog::OnScanProduct)
END_EVENT_TABLE()


pof::AddProdutDialog::AddProdutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel1->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	TitleText = new wxStaticText( m_panel1, wxID_ANY, wxT("ADD PRODUCT"), wxDefaultPosition, wxDefaultSize, 0 );
	TitleText->Wrap( -1 );
	TitleText->SetFont( wxFont( 15, 70, 90, 92, false, wxEmptyString ) );
	
	bSizer2->Add( TitleText, 1, wxALL, 15 );
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 0, wxEXPAND | wxALL, 5 );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, wxT("Product Details") ), wxVERTICAL );
	
	m_scrolledWindow2 = new wxScrolledWindow( sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mProductName = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Product Brand Name"), wxDefaultPosition, wxDefaultSize, 0 );
	mProductName->Wrap( -1 );
	fgSizer2->Add( mProductName, 0, wxALL, 5 );
	
	mProductNameValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mProductNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	fgSizer2->Add( mProductNameValue, 1, wxALL|wxEXPAND, 5 );
	
	mProductGenericName = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Generic Name"), wxDefaultPosition, wxDefaultSize, 0 );
	mProductGenericName->Wrap( -1 );
	fgSizer2->Add( mProductGenericName, 0, wxALL, 5 );
	
	mGenericNameValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mGenericNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	fgSizer2->Add( mGenericNameValue, 1, wxALL|wxEXPAND, 5 );
	
	mFormulation = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Formulation"), wxDefaultPosition, wxDefaultSize, 0 );
	mFormulation->Wrap( -1 );
	fgSizer2->Add( mFormulation, 0, wxALL, 5 );
	

	FormulationChoices.Add("TABLET");
	FormulationChoices.Add("CAPSULE");
	FormulationChoices.Add("SOLUTION");
	FormulationChoices.Add("SUSPENSION");
	FormulationChoices.Add("IV");
	FormulationChoices.Add("IM");
	FormulationChoices.Add("EMULSION");
	FormulationChoices.Add("COMSUMABLE"); //needles, cannula and the rest
	mFormulationValue = new wxChoice( m_scrolledWindow2, wxID_ANY, wxDefaultPosition, wxDefaultSize, FormulationChoices, 0 );
	mFormulationValue->SetSelection( 0 );
	mFormulationValue->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	fgSizer2->Add( mFormulationValue, 0, wxALL|wxEXPAND, 5 );
	
	mStrength = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Strength"), wxDefaultPosition, wxDefaultSize, 0 );
	mStrength->Wrap( -1 );
	fgSizer2->Add( mStrength, 0, wxALL, 5 );
	
	wxFloatingPointValidator<double> val(2, &mFloatValidator, wxNUM_VAL_ZERO_AS_BLANK);
	val.SetRange(0, 999999999999);

	mStrengthValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mStrengthValue, 1, wxALL|wxEXPAND, 5 );
	
	mStrengthType = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Strength Type"), wxDefaultPosition, wxDefaultSize, 0 );
	mStrengthType->Wrap( -1 );
	fgSizer2->Add( mStrengthType, 0, wxALL, 5 );
	
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
	mStrengthTypeValue = new wxChoice( m_scrolledWindow2, wxID_ANY, wxDefaultPosition, wxDefaultSize, StrengthChoices, 0 );
	mStrengthTypeValue->SetSelection( 0 );
	fgSizer2->Add( mStrengthTypeValue, 1, wxALL|wxEXPAND, 5 );
	
	mClassLabel= new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Product Class"), wxDefaultPosition, wxDefaultSize, 0);
	mClassLabel->Wrap(-1);
	fgSizer2->Add(mClassLabel, 0, wxALL, 5);

	ProductClassChoices.Add("POM");
	ProductClassChoices.Add("OTC");
	mClassValue = new wxChoice(m_scrolledWindow2, wxID_ANY, wxDefaultPosition, wxDefaultSize, ProductClassChoices, 0);
	mClassValue->SetSelection(0);
	fgSizer2->Add(mClassValue, 1, wxALL | wxEXPAND, 5);


	m_scrolledWindow2->SetSizer( fgSizer2 );
	m_scrolledWindow2->Layout();
	fgSizer2->Fit( m_scrolledWindow2 );
	sbSizer7->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel4->SetSizer( sbSizer7 );
	m_panel4->Layout();
	sbSizer7->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );
	
	m_panel5 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxT("Product Additional Details") ), wxVERTICAL );
	
	m_scrolledWindow1 = new wxScrolledWindow( sbSizer8->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_panel71 = new wxPanel( m_scrolledWindow1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
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
	fgSizer3 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mProductDescription = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	mProductDescription->Wrap( -1 );
	fgSizer3->Add( mProductDescription, 0, wxALL, 5 );
	
	mProductDescriptionValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mProductDescriptionValue, 1, wxALL|wxEXPAND, 5 );
	
	mMoreDescription = new wxBitmapButton( m_scrolledWindow1, wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	fgSizer3->Add( mMoreDescription, 0, wxALL, 5 );
	
	mDirectionForUse = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Direction For Use"), wxDefaultPosition, wxDefaultSize, 0 );
	mDirectionForUse->Wrap( -1 );
	fgSizer3->Add( mDirectionForUse, 0, wxALL, 5 );
	
	mDirForUseValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mDirForUseValue, 0, wxALL|wxEXPAND, 5 );
	
	mMoreDirForUse = new wxBitmapButton( m_scrolledWindow1, wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	fgSizer3->Add( mMoreDirForUse, 0, wxALL, 5 );
	
	mHealthConditions = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Health Conditions"), wxDefaultPosition, wxDefaultSize, 0 );
	mHealthConditions->Wrap( -1 );
	fgSizer3->Add( mHealthConditions, 0, wxALL, 5 );
	
	mHealthConditionsValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mHealthConditionsValue, 0, wxALL|wxEXPAND, 5 );
	
	mMoreHealthConditons = new wxBitmapButton( m_scrolledWindow1, wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	fgSizer3->Add( mMoreHealthConditons, 0, wxALL, 5 );
	
	mSideEffectsLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Adverse Reactions"), wxDefaultPosition, wxDefaultSize, 0 );
	mSideEffectsLabel->Wrap( -1 );
	fgSizer3->Add( mSideEffectsLabel, 0, wxALL, 5 );
	
	mSideEffectsValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( mSideEffectsValue, 1, wxALL|wxEXPAND, 5 );
	
	mMoreSideffects = new wxBitmapButton( m_scrolledWindow1, wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	fgSizer3->Add( mMoreSideffects, 0, wxALL, 5 );
	
	
	bSizer5->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	m_panel8 = new wxPanel( m_scrolledWindow1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	mScanProduct = new wxButton( m_panel8, wxID_ANY, wxT("Scan Product"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( mScanProduct, 0, wxALIGN_CENTER|wxALL, 0 );
	
	
	m_panel8->SetSizer( bSizer8 );
	m_panel8->Layout();
	bSizer8->Fit( m_panel8 );
	bSizer5->Add( m_panel8, 0, wxEXPAND | wxALL, 5 );
	
	
	m_scrolledWindow1->SetSizer( bSizer5 );
	m_scrolledWindow1->Layout();
	bSizer5->Fit( m_scrolledWindow1 );
	sbSizer8->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel5->SetSizer( sbSizer8 );
	m_panel5->Layout();
	sbSizer8->Fit( m_panel5 );
	bSizer3->Add( m_panel5, 1, wxALL|wxEXPAND|wxFIXED_MINSIZE, 5 );
	
	m_panel6 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Product Inventory") ), wxVERTICAL );
	
	m_scrolledWindow3 = new wxScrolledWindow( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow3->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer21->AddGrowableCol( 1 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mBacthNumber = new wxStaticText( m_scrolledWindow3, wxID_ANY, wxT("Batch Number"), wxDefaultPosition, wxDefaultSize, 0 );
	mBacthNumber->Wrap( -1 );
	fgSizer21->Add( mBacthNumber, 0, wxALL, 5 );
	
	mBatchNumbeValue = new wxTextCtrl( m_scrolledWindow3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( mBatchNumbeValue, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText8 = new wxStaticText( m_scrolledWindow3, wxID_ANY, wxT("Product Expiry Date"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer21->Add( m_staticText8, 0, wxALL, 5 );
	
	m_datePicker1 = new wxDatePickerCtrl( m_scrolledWindow3, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT|wxDP_DROPDOWN );
	fgSizer21->Add( m_datePicker1, 1, wxALL|wxEXPAND, 5 );
	
	mQuntity = new wxStaticText( m_scrolledWindow3, wxID_ANY, wxT("Quantity"), wxDefaultPosition, wxDefaultSize, 0 );
	mQuntity->Wrap( -1 );
	fgSizer21->Add( mQuntity, 0, wxALL, 5 );
	
	mQunatityValue = new wxTextCtrl( m_scrolledWindow3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mQunatityValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	fgSizer21->Add( mQunatityValue, 0, wxALL|wxEXPAND, 5 );
	
	
	m_scrolledWindow3->SetSizer( fgSizer21 );
	m_scrolledWindow3->Layout();
	fgSizer21->Fit( m_scrolledWindow3 );
	sbSizer3->Add( m_scrolledWindow3, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel6->SetSizer( sbSizer3 );
	m_panel6->Layout();
	sbSizer3->Fit( m_panel6 );
	bSizer3->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer1->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel7 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( m_panel7, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( m_panel7, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	
	bSizer4->Add( m_sdbSizer2, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel7->SetSizer( bSizer4 );
	m_panel7->Layout();
	bSizer4->Fit( m_panel7 );
	bSizer1->Add( m_panel7, 0, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

pof::AddProdutDialog::~AddProdutDialog()
{
}

bool pof::AddProdutDialog::TransferDataFromWindow()
{
	datum.second.set(static_cast<std::underlying_type_t<pof::base::data::state>>(pof::base::data::state::CREATED));
	auto& v = datum.first;
	v.resize(pof::ProductManager::PRODUCT_MAX);

	v[pof::ProductManager::PRODUCT_SERIAL_NUM] = static_cast<std::uint64_t>(9099090);
	v[pof::ProductManager::PRODUCT_NAME] = std::move(mProductNameValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_GENERIC_NAME] = std::move(mGenericNameValue->GetValue().ToStdString());
	v[pof::ProductManager::PRODUCT_FORMULATION] = std::move(FormulationChoices[mFormulationValue->GetSelection()].ToStdString());
	v[pof::ProductManager::PRODUCT_CLASS] = std::move(ProductClassChoices[mClassValue->GetSelection()].ToStdString());
	v[pof::ProductManager::PRODUCT_MIN_STOCK_COUNT] = 0;
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

	datumInven.second.set(static_cast<std::underlying_type_t<pof::base::data::state>>(pof::base::data::state::CREATED));
	auto& i = datumInven.first;
	i.resize(pof::ProductManager::INVENTORY_MAX);
	i[pof::ProductManager::INVENTORY_ID] = static_cast<std::uint64_t>(9999999); //
	i[pof::ProductManager::INVENTORY_LOT_NUMBER] = std::move(mBatchNumbeValue->GetValue().ToStdString());
	i[pof::ProductManager::INVENTORY_STOCK_COUNT] = static_cast<std::uint64_t>(atoi(mQunatityValue->GetValue().ToStdString().c_str()));
	i[pof::ProductManager::INVENTORY_INPUT_DATE] = pof::base::data::clock_t::now();
	



	return true;
}

void pof::AddProdutDialog::OnScanProduct(wxCommandEvent& evt)
{
	wxTextEntryDialog dialog(this, "Please scan a product with a barcode scanner");
	dialog.SetValidator(wxTextValidator{ wxFILTER_DIGITS });
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
