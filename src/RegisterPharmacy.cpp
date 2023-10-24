#include "RegisterPharmacy.h"

pof::PharmacyRegistration::PharmacyRegistration( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) :
	wxDialog( parent, id, "Register pharmacy", pos, size, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
{
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	bSizer2->AddSpacer(10);

	mTitle = new wxStaticText( m_panel1, wxID_ANY, wxT("Welcome to PharmaOffice, please register pharmacy"), wxDefaultPosition, wxDefaultSize, 0 );
	mTitle->Wrap( -1 );
	mTitle->SetFont(wxFont(wxFontInfo(12).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));
	
	bSizer2->Add( mTitle, 0, wxALIGN_CENTRE_HORIZONTAL | wxTOP | wxLEFT, 5 );
	
	
	bSizer2->Add( 20, 0, 0, wxEXPAND, 5 );
	
	mDescription = new wxStaticText( m_panel1, wxID_ANY, wxT("Please note that a registered pharamcy cannot be removed until uninstall"), wxDefaultPosition, wxDefaultSize, 0 );
	mDescription->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
	mDescription->Wrap( -1 );
	mDescription->SetFont( wxFont( 7, 70, 90, 90, false, wxEmptyString ) );
	
	bSizer2->Add( mDescription, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM | wxLEFT, 5);
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 0, wxEXPAND | wxALL, 5 );
	
	m_scrolledWindow1 = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow1, wxID_ANY, wxT("Pharamcy Details") ), wxVERTICAL );
	
	m_panel3 = new wxPanel( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 100 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mPharmacyType = new wxStaticText( m_panel3, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	mPharmacyType->Wrap( -1 );
	fgSizer1->Add( mPharmacyType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString mPharamcyTypeValueChoices[] = { wxT("COMMUNITY"), wxT("HOSPITAL"), wxT("INDUSTRY"), wxT("DRF"), wxT("EDUCATIONAL") };
	int mPharamcyTypeValueNChoices = sizeof( mPharamcyTypeValueChoices ) / sizeof( wxString );
	mPharamcyTypeValue = new wxChoice( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, mPharamcyTypeValueNChoices, mPharamcyTypeValueChoices, 0 );
	mPharamcyTypeValue->SetSelection( 0 );
	mPharamcyTypeValue->Bind(wxEVT_CHOICE, [&](wxCommandEvent& evt) {
		int sel = evt.GetSelection();
		if (sel == wxNOT_FOUND) return;

	});
	fgSizer1->Add( mPharamcyTypeValue, 1, wxALL|wxEXPAND, 5 );
	
	mPharamcyName = new wxStaticText( m_panel3, wxID_ANY, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	mPharamcyName->Wrap( -1 );
	fgSizer1->Add( mPharamcyName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mPharmacyNameValue = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPharmacyNameValue->SetMaxLength( 250 ); 
	mPharmacyNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	fgSizer1->Add( mPharmacyNameValue, 0, wxALL|wxEXPAND, 5 );
	
	mPhoneNo = new wxStaticText( m_panel3, wxID_ANY, wxT("Phone No"), wxDefaultPosition, wxDefaultSize, 0 );
	mPhoneNo->Wrap( -1 );
	fgSizer1->Add( mPhoneNo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mPhoneNoValue = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPhoneNoValue->SetMaxLength( 250 ); 
	mPhoneNoValue->SetValidator(wxTextValidator{ wxFILTER_NUMERIC });
	fgSizer1->Add( mPhoneNoValue, 1, wxALL|wxEXPAND, 5 );
	
	mEmail = new wxStaticText( m_panel3, wxID_ANY, wxT("Email"), wxDefaultPosition, wxDefaultSize, 0 );
	mEmail->Wrap( -1 );
	fgSizer1->Add( mEmail, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mEmailValue = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mEmailValue->SetMaxLength( 250 ); 
	fgSizer1->Add( mEmailValue, 0, wxALL|wxEXPAND, 5 );
	
	mWebsiteText = new wxStaticText( m_panel3, wxID_ANY, wxT("Website"), wxDefaultPosition, wxDefaultSize, 0 );
	mWebsiteText->Wrap( -1 );
	fgSizer1->Add( mWebsiteText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mWebsiteValue = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mWebsiteValue->SetMaxLength( 250 ); 
	fgSizer1->Add( mWebsiteValue, 0, wxALL|wxEXPAND, 5 );
	
	
	m_panel3->SetSizer( fgSizer1 );
	m_panel3->Layout();
	fgSizer1->Fit( m_panel3 );
	sbSizer3->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );
	
	bSizer3->Add( sbSizer3, 0, wxEXPAND | wxALL, 10 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow1, wxID_ANY, wxT("Address") ), wxVERTICAL );
	
	m_panel4 = new wxPanel( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 100 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mCountyText = new wxStaticText( m_panel4, wxID_ANY, wxT("Country"), wxDefaultPosition, wxDefaultSize, 0 );
	mCountyText->Wrap( -1 );
	fgSizer2->Add( mCountyText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mCountryValue = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mCountryValue->SetMaxLength( 250 ); 
	fgSizer2->Add( mCountryValue, 0, wxALL|wxEXPAND, 5 );
	
	mLgaText = new wxStaticText( m_panel4, wxID_ANY, wxT("L.G.A"), wxDefaultPosition, wxDefaultSize, 0 );
	mLgaText->Wrap( -1 );
	fgSizer2->Add( mLgaText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mLgaValue = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mLgaValue->SetMaxLength( 250 ); 
	fgSizer2->Add( mLgaValue, 0, wxALL|wxEXPAND, 5 );
	
	mNoText = new wxStaticText( m_panel4, wxID_ANY, wxT("No."), wxDefaultPosition, wxDefaultSize, 0 );
	mNoText->Wrap( -1 );
	fgSizer2->Add( mNoText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mNoValue = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mNoValue->SetMaxLength( 250 ); 
	mNoValue->SetValidator(wxTextValidator{ wxFILTER_NUMERIC });
	fgSizer2->Add( mNoValue, 0, wxALL|wxEXPAND, 5 );
	
	mStreetText = new wxStaticText( m_panel4, wxID_ANY, wxT("Street"), wxDefaultPosition, wxDefaultSize, 0 );
	mStreetText->Wrap( -1 );
	fgSizer2->Add( mStreetText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mStreetValue = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mStreetValue->SetMaxLength( 250 ); 
	fgSizer2->Add( mStreetValue, 0, wxALL|wxEXPAND, 5 );
	
	mCityText = new wxStaticText( m_panel4, wxID_ANY, wxT("City"), wxDefaultPosition, wxDefaultSize, 0 );
	mCityText->Wrap( -1 );
	fgSizer2->Add( mCityText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mCityValue = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mCityValue->SetMaxLength( 250 ); 
	fgSizer2->Add( mCityValue, 0, wxALL|wxEXPAND, 5 );
	
	mStateText = new wxStaticText( m_panel4, wxID_ANY, wxT("State"), wxDefaultPosition, wxDefaultSize, 0 );
	mStateText->Wrap( -1 );
	fgSizer2->Add( mStateText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mStateValue = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mStateValue->SetMaxLength( 250 ); 
	fgSizer2->Add( mStateValue, 0, wxALL|wxEXPAND, 5 );
	
	
	m_panel4->SetSizer( fgSizer2 );
	m_panel4->Layout();
	fgSizer2->Fit( m_panel4 );
	sbSizer4->Add( m_panel4, 1, wxEXPAND | wxALL, 10 );
	
	
	bSizer3->Add( sbSizer4, 0, wxEXPAND, 5 );
	
	
	m_scrolledWindow1->SetSizer( bSizer3 );
	m_scrolledWindow1->Layout();
	bSizer3->Fit( m_scrolledWindow1 );
	bSizer1->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 10 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND | wxALL, 10 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	this->Centre(wxBOTH);
}

pof::PharmacyRegistration::~PharmacyRegistration()
{
}

bool pof::PharmacyRegistration::TransferDataFromWindow()
{
	if (!mp){
		wxMessageBox("Critical Error, failed to create pharmacy, please call admin", "ERROR", wxICON_ERROR | wxOK);
		return false; //critial error;
	}
	mp->name = mPharmacyNameValue->GetValue();
	
	mp->addy.city = mCityValue->GetValue();
	mp->addy.country = mCountryValue->GetValue();
	mp->addy.lga = mLgaValue->GetValue();
	mp->addy.number = mNoValue->GetValue();
	mp->addy.state = mStateValue->GetValue();
	mp->addy.street = mStreetValue->GetValue();
	
	mp->contact.phoneNumber = mPhoneNoValue->GetValue();
	mp->contact.email = mEmailValue->GetValue();
	mp->contact.website = mWebsiteValue->GetValue();

	return true;
}


