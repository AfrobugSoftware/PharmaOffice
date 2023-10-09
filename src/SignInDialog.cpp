#include "SignInDIalog.h"
//#include "Application.h"
#include "PofPch.h"
BEGIN_EVENT_TABLE(pof::SignInDialog, wxDialog)
	EVT_BUTTON(pof::SignInDialog::ID_LOGON, pof::SignInDialog::onLogon)
	EVT_BUTTON(pof::SignInDialog::SignInDialog::ID_SIGNUP, pof::SignInDialog::onSignup)
END_EVENT_TABLE()



pof::SignInDialog::SignInDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	RootPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	RootPane->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel2 = new wxPanel( RootPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer2->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel3 = new wxPanel( RootPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_bitmap1 = new wxStaticBitmap( m_panel3, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_bitmap1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mWelcomText = new wxStaticText( m_panel3, wxID_ANY, wxT("WELCOME TO PHARMAOFFICE"), wxDefaultPosition, wxDefaultSize, 0 );
	mWelcomText->Wrap( -1 );
	mWelcomText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer5->Add( mWelcomText, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	bSizer5->Add( 0, 30, 0, wxEXPAND, 5 );
	
	mUserName = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mUserName->SetMinSize( wxSize( 300,-1 ) );
	mUserName->SetHint("Username");
	mUserName->SetValidator(wxTextValidator{ wxFILTER_EMPTY });

	bSizer5->Add( mUserName, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mPassword = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	mPassword->SetMinSize( wxSize( 300,-1 ) );
	mPassword->SetHint("Password");
	mPassword->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	
	bSizer5->Add( mPassword, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mKeepMeSigned = new wxCheckBox(m_panel3, wxID_ANY, wxT("Keep me signed in"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(mKeepMeSigned, 0, wxALL, 5);
	
	bSizer5->Add( 0, 20, 0, wxEXPAND, 5 );
	
	m_panel5 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	mLogOn = new wxButton( m_panel5, ID_LOGON, wxT("Log on"), wxDefaultPosition, wxDefaultSize, 0 );
	mLogOn->SetDefault(); 
	bSizer6->Add( mLogOn, 0, wxALL, 5 );
	
	mSignup = new wxButton( m_panel5, ID_SIGNUP, wxT("Sign Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( mSignup, 0, wxALL, 5 );
	
	
	m_panel5->SetSizer( bSizer6 );
	m_panel5->Layout();
	bSizer6->Fit( m_panel5 );
	bSizer5->Add( m_panel5, 0, wxEXPAND | wxALL, 0 );
	
	
	bSizer5->Add( 0, 20, 0, wxEXPAND, 5 );
	
	mPharmacySignupPanel = new wxPanel(m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxHORIZONTAL);


	bSizer7->Add(0,0, 1, wxEXPAND, 5);

	mLoginPharmacyLink = new wxHyperlinkCtrl(mPharmacySignupPanel, wxID_ANY, wxT("Log In Pharmacy"), wxT(""), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizer7->Add(mLoginPharmacyLink, 0, wxALL, 5);

	mHelpLink = new wxHyperlinkCtrl(mPharmacySignupPanel, wxID_ANY, wxT("Help"), wxT(""), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizer7->Add(mHelpLink, 0, wxALL, 5);


	mPharmacySignupPanel->SetSizer(bSizer7);
	mPharmacySignupPanel->Layout();
	bSizer7->Fit(mPharmacySignupPanel);
	bSizer5->Add(mPharmacySignupPanel, 1, wxEXPAND | wxALL, 5);


	
	m_panel3->SetSizer( bSizer5 );
	m_panel3->Layout();
	bSizer5->Fit( m_panel3 );
	bSizer2->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );
	
	m_panel4 = new wxPanel( RootPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	bSizer2->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );
	
	
	RootPane->SetSizer( bSizer2 );
	RootPane->Layout();
	bSizer2->Fit( RootPane );
	bSizer1->Add( RootPane, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("dglopaico"));
	SetIcon(appIcon);
}

pof::SignInDialog::~SignInDialog()
{
}

void pof::SignInDialog::onLogon(wxCommandEvent& evt)
{
	if (!Validate()) return;
	bool ret = false;
	if (wxGetApp().bUsingLocalDatabase) {
		ret = ValidateLocal();
	}
	else {
		ret = ValidateGlobal();
	}
	if (!ret) return;

	EndModal(wxID_OK);
}

void pof::SignInDialog::onSignup(wxCommandEvent& evt)
{
	pof::RegistrationDialog regDialog(nullptr);
	//Hide();
	int ret = regDialog.ShowModal();
	bool status = false;
	if (ret == wxID_OK) {
		wxBusyInfo info("Registring account and Signing in...");
		if (!InsertUserDataIntoDatabase(regDialog.GetAccount())){
			wxMessageBox(wxGetApp().mLocalDatabase->err_msg().data(), "REGISTRATION", wxICON_ERROR | wxOK);
			return;
		}
	}
	//Show();
}

bool pof::SignInDialog::ValidateLocal()
{
	auto& dbPtr = wxGetApp().mLocalDatabase;
	if(!dbPtr) return false;
	std::string Username = mUserName->GetValue().ToStdString();
	std::string UserPassword = mPassword->GetValue().ToStdString();
	wxGetApp().bKeepMeSignedIn = mKeepMeSigned->IsChecked();
	auto& account = wxGetApp().MainAccount;
	const std::string sql = fmt::format("SELECT * FROM USERS WHERE username = \'{}\';", Username);

	auto stmt = dbPtr->prepare(sql);
	if (!stmt.has_value())
	{
		wxMessageBox(dbPtr->err_msg().data(), "SIGN IN EXCEPTION", wxICON_ERROR | wxOK);
		return false;
	}
	auto rel = dbPtr->retrive<
						std::uint64_t, //ID
						std::uint32_t, //PRV
						pof::base::data::text_t, //NAME
						pof::base::data::text_t, //LAST NAME
						pof::base::data::text_t, //EMAIL
						pof::base::data::text_t, // PHONEUMBER
						pof::base::data::text_t, // REGNUMBER
						pof::base::data::text_t, //USERNAME
						pof::base::data::text_t>(*stmt); //PASSWORD IN BCRYPT
	if (!rel.has_value()) {
		dbPtr->finalise(*stmt);
		return false;
	}
	if (rel->empty()) {
		wxMessageBox(fmt::format("USERNAME \"{}\" DOES NOT EXIST", Username), "SIGN IN", wxICON_WARNING | wxOK);
		dbPtr->finalise(*stmt);
		return false;
	}
	auto& v = rel->front();

	if (!bcrypt::validatePassword(UserPassword, std::get<8>(v))) {
		wxMessageBox("INVALID PASSWORD", "SIGN IN", wxICON_WARNING | wxOK);
		dbPtr->finalise(*stmt);
		return false;
	}

	account->signintime = pof::Account::clock_t::now();
	account->accountID = std::get<0>(v);
	account->priv = pof::Account::privilage_set_t(std::get<1>(v));
	account->name = fmt::format("{} {}", std::get<2>(v), std::get<3>(v));
	account->email = std::get<4>(v);
	account->phonenumber = std::get<5>(v);
	account->regnumber = std::get<6>(v);
	account->passhash = std::get<7>(v);
	account->SetSignInTime();
	dbPtr->finalise(*stmt);
	return true;
}

bool pof::SignInDialog::ValidateGlobal()
{
	std::string Username = mUserName->GetValue().ToStdString();
	std::string UserPassword = mPassword->GetValue().ToStdString();
	bool check = mKeepMeSigned->IsChecked();

	try {
		//do verification how ??
			//send to chws?
		wxProgressDialog dlg("SIGING IN", "CONNECTING TO FILODOXIA...", 100, this, wxPD_CAN_ABORT | wxPD_SMOOTH | wxPD_APP_MODAL);

		js::json payload;
		payload["Username"] = Username;
		payload["Password"] = UserPassword;
#if 0
		//not implemented yet, still an idea
		std::string NetAddress = wxGetApp()["network.address"s].get_value<std::string>();
		std::string NetPort = wxGetApp()["network.port"s].get_value<std::string>();
#endif
		auto sess = std::make_shared<pof::base::ssl::session<http::string_body, http::string_body>>(wxGetApp().mNetManager.io(), wxGetApp().mNetManager.ssl());
		auto fut = sess->req<http::verb::post>("localhost", "/accounts/signin", "80", payload.dump());

		dlg.Update(10, "Sending requests...");

		//cache the sign in if the keep signed in was checked.

		std::future_status s = fut.wait_for(3ms);
		constexpr std::array<std::string_view, 3> wait_text{ ".", "..", "..." };
		size_t i = 0, count = 10;
		while (s != std::future_status::ready) {
			//display visual feedback
			auto end = dlg.Update(count, fmt::format("Waiting{}", wait_text[i]));
			if (!end) {
				if (wxMessageBox("Do you really want to cancel sign in", "SIGN IN", wxICON_WARNING | wxYES_NO) == wxYES) {
					sess->cancel();
					break;
				}
			}
			i = ++i % 3;
			count = ++count % 80;
			s = fut.wait_for(3us);
		}

		auto data = fut.get();
		mUserData = data.body();
#if 0
		wxGetApp()["session"].put("sesion.keep_alive", check);
#endif
		dlg.Update(100);
	}
	catch (const std::exception& exp) {
		wxMessageBox(exp.what(), "SIGN IN");
		return false;
	}
	return true;
}

bool pof::SignInDialog::InsertUserDataIntoDatabase(const pof::Account& acc)
{
	constexpr const std::string_view sql = "INSERT INTO USERS (priv, name, last_name, email, phonenumber, regnumber, username, password) VALUES (?,?,?,?,?,?,?,?);";
	auto& dbPtr = wxGetApp().mLocalDatabase;
	if (!dbPtr) return false;

	auto stmt = dbPtr->prepare(sql);
	assert(stmt.has_value());
	dbPtr->bind(*stmt, std::make_tuple(acc.priv.to_ulong(), acc.name, acc.lastname,
		acc.email, acc.phonenumber, acc.regnumber, acc.username, acc.passhash));
	bool ret = dbPtr->execute(*stmt);
	dbPtr->finalise(*stmt);
	return ret;
}
