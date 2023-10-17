#include "AccountReg.h"
BEGIN_EVENT_TABLE(pof::RegistrationDialog, wxDialog)
	EVT_CHECKBOX(pof::RegistrationDialog::ID_SHOW_PASSWORD, pof::RegistrationDialog::OnShowPassword)
	EVT_CHOICE(pof::RegistrationDialog::ID_ACCOUNT_TYPE, pof::RegistrationDialog::OnAccountTypeSelect)
END_EVENT_TABLE()



pof::RegistrationDialog::RegistrationDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxDIALOG_EX_CONTEXTHELP );
	SetBackgroundColour(*wxWHITE); //THEME
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	MainPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	CreateAccount = new wxPanel( MainPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_scrolledWindow1 = new wxScrolledWindow( CreateAccount, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_panel5 = new wxPanel( m_scrolledWindow1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText12 = new wxStaticText( m_panel5, wxID_ANY, wxT("Creates an account with a pharmacy"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer5->Add( m_staticText12, 1, wxALL|wxEXPAND, 10 );
	
	
	m_panel5->SetSizer( bSizer5 );
	m_panel5->Layout();
	bSizer5->Fit( m_panel5 );
	bSizer4->Add( m_panel5, 0, wxALL|wxEXPAND, 2 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mLastNameLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Last Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	mLastNameLabel->Wrap( -1 );
	fgSizer1->Add( mLastNameLabel, 0, wxALL, 5 );
	
	mLastNameValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mLastNameValue->SetMaxLength( 30 ); 
	mLastNameValue->SetMinSize( wxSize( 100,-1 ) );
	mLastNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY | wxFILTER_ALPHA});
	
	fgSizer1->Add( mLastNameValue, 0, wxALL|wxEXPAND, 5 );
	
	mFirstNameLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("First Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	mFirstNameLabel->Wrap( -1 );
	fgSizer1->Add( mFirstNameLabel, 0, wxALL, 5 );
	
	mFirstNameValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mFirstNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY | wxFILTER_ALPHA });
	fgSizer1->Add( mFirstNameValue, 0, wxALL|wxEXPAND, 5 );

	mUserNameLabel = new wxStaticText(m_scrolledWindow1, wxID_ANY, wxT("Username:"), wxDefaultPosition, wxDefaultSize, 0);
	mUserNameLabel->Wrap(-1);
	fgSizer1->Add(mUserNameLabel, 0, wxALL, 5);

	mUserNameValue = new wxTextCtrl(m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	mUserNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY | wxFILTER_ALPHA });
	fgSizer1->Add(mUserNameValue, 0, wxALL | wxEXPAND, 5);

	mAccountTypeLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Account Type"), wxDefaultPosition, wxDefaultSize, 0 );
	mAccountTypeLabel->Wrap( -1 );
	fgSizer1->Add( mAccountTypeLabel, 0, wxALL, 5 );
	
	//arranged according to the account priv bits
	wxString mAccountTypeChoices[] = { wxT("SUPERINTENDENT PHARMACIST"), 
			wxT("PHARMACY TECH"),
			wxT("DISPENSER"),
			wxT("SALES ASSISTANT"), 
			wxT("INTERN PHARMACIST"), 
			wxT("STUDENT PHARMACIST"),
			wxT("MANAGER"),
		};
	int mAccountTypeNChoices = sizeof( mAccountTypeChoices ) / sizeof( wxString );
	mAccountType = new wxChoice( m_scrolledWindow1, ID_ACCOUNT_TYPE, wxDefaultPosition, wxDefaultSize, mAccountTypeNChoices, mAccountTypeChoices, 0 );
	fgSizer1->Add( mAccountType, 0, wxALL|wxEXPAND, 5 );
	
	mEmailLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Email"), wxDefaultPosition, wxDefaultSize, 0 );
	mEmailLabel->Wrap( -1 );
	fgSizer1->Add( mEmailLabel, 0, wxALL, 5 );
	
	mEmailValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mEmailValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY }); 
	fgSizer1->Add( mEmailValue, 0, wxALL|wxEXPAND, 5 );
	
	mPhoneNo = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Phone No"), wxDefaultPosition, wxDefaultSize, 0 );
	mPhoneNo->Wrap( -1 );
	fgSizer1->Add( mPhoneNo, 0, wxALL, 5 );
	
	mPhoneNoValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPhoneNoValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY | wxFILTER_NUMERIC });
	mPhoneNoValue->SetMaxLength(11);
	fgSizer1->Add( mPhoneNoValue, 0, wxALL|wxEXPAND, 5 );
	
	mPasswordLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
	mPasswordLabel->Wrap( -1 );
	fgSizer1->Add( mPasswordLabel, 0, wxALL, 5 );
	
	mPasswordValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	mPasswordValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY});
	fgSizer1->Add( mPasswordValue, 0, wxALL|wxEXPAND, 5 );
	
	mConfirmPasswordLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Confirm Password"), wxDefaultPosition, wxDefaultSize, 0 );
	mConfirmPasswordLabel->Wrap( -1 );
	fgSizer1->Add( mConfirmPasswordLabel, 0, wxALL, 5 );
	
	mConfirmPasswordValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	mConfirmPasswordValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	fgSizer1->Add( mConfirmPasswordValue, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBox1 = new wxCheckBox( m_scrolledWindow1, ID_SHOW_PASSWORD, wxT("Show password"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	fgSizer1->Add( m_checkBox1, 0, wxALL, 5 );
	
	mRegNumberLabel = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("Reg-No"), wxDefaultPosition, wxDefaultSize, 0 );
	mRegNumberLabel->Wrap( -1 );
	fgSizer1->Add( mRegNumberLabel, 0, wxALL, 5 );
	
	mRegNumValue = new wxTextCtrl( m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mRegNumValue, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxString m_radioBox2Choices[] = { wxT("Princepal Pharmacist"), wxT("Loccum Pharmacist") };
	int m_radioBox2NChoices = sizeof( m_radioBox2Choices ) / sizeof( wxString );
	m_radioBox2 = new wxRadioBox( m_scrolledWindow1, wxID_ANY, wxT("Phamacist role"), wxDefaultPosition, wxDefaultSize, m_radioBox2NChoices, m_radioBox2Choices, 1, wxRA_SPECIFY_COLS );
	fgSizer1->Add( m_radioBox2, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer4->Add( fgSizer1, 1, wxALL|wxEXPAND, 10 );
	
	
	m_scrolledWindow1->SetSizer( bSizer4 );
	m_scrolledWindow1->Layout();
	bSizer4->Fit( m_scrolledWindow1 );
	bSizer8->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 5 );
	
	
	CreateAccount->SetSizer( bSizer8 );
	CreateAccount->Layout();
	//bSizer8->Fit( CreateAccount );
	bSizer3->Add( CreateAccount, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer3 = new wxStdDialogButtonSizer();
	m_sdbSizer3Save = new wxButton( MainPane, wxID_OK );
	m_sdbSizer3->AddButton( m_sdbSizer3Save );
	m_sdbSizer3Cancel = new wxButton( MainPane, wxID_CANCEL );
	m_sdbSizer3->AddButton( m_sdbSizer3Cancel );
	m_sdbSizer3->Realize();
	
	bSizer3->Add( m_sdbSizer3, 0, wxALL|wxEXPAND, 10 );
	
	
	MainPane->SetSizer( bSizer3 );
	MainPane->Layout();
	bSizer3->Fit( MainPane );
	bSizer2->Add( MainPane, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer2 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

pof::RegistrationDialog::~RegistrationDialog()
{

}

bool pof::RegistrationDialog::TransferDataFromWindow()
{
	if (mPasswordValue->GetValue() != mConfirmPasswordValue->GetValue()) {
		wxMessageBox("Password and Confirm password mismatch", "REGISTRATION", wxICON_WARNING | wxOK);
		return false;
	}
	const auto pass = mPasswordValue->GetValue().ToStdString();
	const auto hash = bcrypt::generateHash(pass);
	const auto email = mEmailValue->GetValue().ToStdString();
	if (!ValidateEmail(email)) {
		return false;
	}
	int sel = mAccountType->GetSelection();
	if (sel == wxNOT_FOUND) {
		wxMessageBox("Please select an account type for your account", "REGISTRATION", wxICON_WARNING | wxOK);
		return false;
	}
	mAccount.priv.set(sel);
	const auto phone = mPhoneNoValue->GetValue().ToStdString();
	if (phone.size() != 11) {
		//cannot properly validate number just the count
		wxMessageBox("Phone number is not complete, please enter valid phone number", "REGISTRATION", wxICON_WARNING | wxOK);
		return false;
	}

	mAccount.accountID = mAccount.GetLastId() + 1;
	mAccount.name = mFirstNameValue->GetValue().ToStdString();
	mAccount.lastname = mLastNameValue->GetValue().ToStdString();
	mAccount.passhash = hash;
	mAccount.username = mUserNameValue->GetValue().ToStdString();
	mAccount.email = email;
	mAccount.phonenumber = phone;
	mAccount.regnumber = mRegNumValue->GetValue().ToStdString(); 
	
	return true;
}

bool pof::RegistrationDialog::ValidateEmail(const std::string email)
{
	try {
		const std::regex rex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
		if (!std::regex_match(email, rex)) {
			wxMessageBox("Invalid Email Address", "REGISTRATION", wxICON_WARNING | wxOK);
			return false;
		}
	}
	catch (const std::exception& exp) {
		auto what = exp.what();
		spdlog::error(what);
	}
	return true;
}

void pof::RegistrationDialog::OnShowPassword(wxCommandEvent& evt)
{
	auto pflags = mPasswordValue->GetWindowStyle();
	auto cflags = mConfirmPasswordValue->GetWindowStyle();

	if (evt.IsChecked()) {
		pflags = (pflags & (~wxTE_PASSWORD));
		cflags = (cflags & (~wxTE_PASSWORD));

	}
	else {
		pflags = pflags | wxTE_PASSWORD;
		cflags = cflags | wxTE_PASSWORD;
	}
	Freeze();
	mPasswordValue->SetWindowStyle(pflags);
	mConfirmPasswordValue->SetWindowStyle(cflags);
	mPasswordValue->Refresh();
	mPasswordValue->Update();
	mConfirmPasswordValue->Refresh();
	mConfirmPasswordValue->Update();
	Thaw();
	Update();
}

void pof::RegistrationDialog::OnAccountTypeSelect(wxCommandEvent& evt)
{
	int sel = evt.GetSelection();
	if (sel == wxNOT_FOUND) return;
	if (sel == PHARAMCIST || sel == INTERN_PHARAMCIST) {
		mRegNumValue->SetValidator(wxTextValidator{wxFILTER_EMPTY});
	}
	else {
		mRegNumValue->SetValidator(wxTextValidator{});
	}
}
