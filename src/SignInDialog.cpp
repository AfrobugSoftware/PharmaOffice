#include "SignInDIalog.h"
//#include "Application.h"
#include "PofPch.h"
BEGIN_EVENT_TABLE(pof::SignInDialog, wxDialog)
	EVT_BUTTON(pof::SignInDialog::ID_LOGON, pof::SignInDialog::onLogon)
	EVT_BUTTON(pof::SignInDialog::ID_SIGNUP, pof::SignInDialog::onSignup)
	EVT_HYPERLINK(pof::SignInDialog::ID_FORGOT_PASS, pof::SignInDialog::OnForgotPassword)
	EVT_HYPERLINK(pof::SignInDialog::ID_HELP, pof::SignInDialog::OnHelp)
END_EVENT_TABLE()



pof::SignInDialog::SignInDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, wxT("PharmaOffice - Personal"), pos, size, style)
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

	mForgotPasswordLink = new wxHyperlinkCtrl(mPharmacySignupPanel, ID_FORGOT_PASS, wxT("Forgot password"), wxT(""), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizer7->Add(mForgotPasswordLink, 0, wxALL, 5);

	mHelpLink = new wxHyperlinkCtrl(mPharmacySignupPanel, ID_HELP, wxT("Help"), wxT(""), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
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
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
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
	regDialog.mAccount.mLocalDatabase = wxGetApp().mLocalDatabase;
	//Hide();
	int ret = regDialog.ShowModal();
	bool status = false;
	if (ret == wxID_OK) {
		wxBusyInfo info("Registring account and Signing in...");
		if (!InsertUserDataIntoDatabase(regDialog.GetAccount())){
			wxMessageBox(wxGetApp().mLocalDatabase->err_msg().data(), "REGISTRATION", wxICON_ERROR | wxOK);
			return;
		}
		regDialog.mAccount.CreateAccountInfo();
	}
	//Show();
}

void pof::SignInDialog::OnForgotPassword(wxHyperlinkEvent& evt)
{
	auto acc = wxGetApp().MainAccount;
	assert(acc);

	auto username = wxGetTextFromUser("Please enter your username", "Forgot password").ToStdString();
	if (username.empty()) return;

	auto secquest = acc->GetSecurityQuestion(username);
	if (!secquest.has_value()) {
		wxMessageBox("No security question set, call admin to reset password", "Forgot password", wxICON_ERROR | wxOK);
		return;
	}

	while (1) {
		auto secanswer = wxGetTextFromUser(fmt::format("Please answer the following security question;\n\n{}", secquest.value()), "Forgot password").ToStdString();
		if (secanswer.empty()) return;

		auto verified = acc->ValidateSecurityQuestion(username, secanswer);
		if (!verified.has_value()) {
			wxMessageBox("Fatal error in password recovery, please call admin", "Forgot password", wxICON_ERROR | wxOK);
			return;
		}
		if (!*verified) {
			if(wxMessageBox("Invalid security answer, please try again?", "Forgot password", wxICON_INFORMATION | wxYES_NO) == wxNO){
				return;
			}
			continue;
		}
		break;
	}

	//reset password
	wxDialog dialog(this, wxID_ANY, "Reset passord", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	//dialog.SetSizeHints(wxDefaultSize, wxDefaultSize);
	dialog.SetBackgroundColour(*wxWHITE);
	wxDialog* d = std::addressof(dialog);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	auto contTitle = new wxStaticText(d, wxID_ANY, wxT("Please enter new password.\n"), wxDefaultPosition, wxDefaultSize, 0);
	contTitle->Wrap(-1);
	contTitle->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

	bSizer1->Add(contTitle, 0, wxTOP | wxLEFT, 5);

	auto q = new wxStaticText(d, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0);
	q->Wrap(-1);
	bSizer1->Add(q, 0, wxALL, 5);

	auto qv = new wxTextCtrl(d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	qv->SetMaxLength(250);
	bSizer1->Add(qv, 0, wxALL, 5);

	auto q2 = new wxStaticText(d, wxID_ANY, wxT("Confirm Password"), wxDefaultPosition, wxDefaultSize, 0);
	q2->Wrap(-1);
	bSizer1->Add(q2, 0, wxALL, 5);

	auto qv2 = new wxTextCtrl(d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	qv2->SetMaxLength(250);
	bSizer1->Add(qv2, 0, wxALL, 5);

	auto m_panel7 = new wxPanel(d, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxVERTICAL);

	auto m_sdbSizer2 = new wxStdDialogButtonSizer();
	auto m_sdbSizer2OK = new wxButton(m_panel7, wxID_OK);
	m_sdbSizer2->AddButton(m_sdbSizer2OK);
	auto m_sdbSizer2Cancel = new wxButton(m_panel7, wxID_CANCEL);
	m_sdbSizer2->AddButton(m_sdbSizer2Cancel);
	m_sdbSizer2->Realize();
	bSizer4->Add(m_sdbSizer2, 0, wxEXPAND, 5);



	m_panel7->SetSizer(bSizer4);
	m_panel7->Layout();
	bSizer4->Fit(m_panel7);

	bSizer1->Add(m_panel7, 0, wxEXPAND | wxALL, 5);


	d->SetSizer(bSizer1);
	d->Layout();
	d->Center(wxBOTH);
	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	d->SetIcon(appIcon);

	while (1) {
		if (d->ShowModal() == wxID_CANCEL) return;

		auto pass = qv->GetValue().ToStdString();
		auto cpass = qv2->GetValue().ToStdString();

		if (pass != cpass) {
			wxMessageBox("Password mismatch", "Forgot password", wxICON_WARNING | wxOK);
			continue;
		}
		break;
	}
	
}

void pof::SignInDialog::OnHelp(wxHyperlinkEvent& evt)
{
	wxMessageBox("Help not yet avaliable");
}

bool pof::SignInDialog::ValidateLocal()
{
	wxBusyCursor cursor;
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
		wxMessageBox(fmt::format("Username \"{}\" does not exists", Username), "Sign in", wxICON_WARNING | wxOK);
		dbPtr->finalise(*stmt);
		return false;
	}
	auto& v = rel->front();

	if (!bcrypt::validatePassword(UserPassword, std::get<8>(v))) {
		wxMessageBox("Password is incorrect", "Sign in", wxICON_WARNING | wxOK);
		dbPtr->finalise(*stmt);
		return false;
	}

	account->signintime = pof::Account::clock_t::now();
	account->accountID = std::get<0>(v);
	account->priv = pof::Account::privilage_set_t(std::get<1>(v));
	account->name = std::get<2>(v);
	account->lastname = std::get<3>(v);
	account->email = std::get<4>(v);
	account->phonenumber = std::get<5>(v);
	account->regnumber = std::get<6>(v);
	account->username = std::get<7>(v);
	account->passhash = std::get<8>(v);
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
		auto q = std::make_shared<pof::base::datastmtquery>(wxGetApp().mMysqlDatabase, "SELECT * FROM users WHERE username = ?;"s);
		auto fut = q->get_future();
		pof::base::datastmtquery::row_t row;
		row.emplace_back(boost::mysql::field(Username));
		q->m_arguments.push_back(std::move(row));

		wxGetApp().mMysqlDatabase->push(q);

		if (wxGetApp().BusyWait(fut, "Signing in")){
			try {
				auto data = fut.get();
				if (data) {
					if (data->empty()) {
						wxMessageBox(fmt::format("Username \"{}\" does not exists", Username), "Sign in", wxICON_WARNING | wxOK);
						return false;
					}
					auto& r = *data->begin();
					if (!bcrypt::validatePassword(UserPassword, boost::variant2::get<std::string>(r.first[8]))) {
						wxMessageBox("Password is incorrect", "Sign in", wxICON_WARNING | wxOK);
						return false;
					}
					auto& account = wxGetApp().MainAccount;
					account->signintime = pof::Account::clock_t::now();
					account->accountID = boost::variant2::get<std::int64_t>(r.first[0]); //cannot be int32
					account->priv = pof::Account::privilage_set_t(boost::variant2::get<std::int64_t>(r.first[1]));
					account->name = boost::variant2::get<std::string>(r.first[2]);
					account->lastname = boost::variant2::get<std::string>(r.first[3]);
					account->email = boost::variant2::get<std::string>(r.first[4]);
					account->phonenumber = boost::variant2::get<std::string>(r.first[5]);
					account->regnumber = boost::variant2::get<std::string>(r.first[6]);
					account->username = boost::variant2::get<std::string>(r.first[7]);
					account->passhash = boost::variant2::get<std::string>(r.first[8]);
					account->SetSignInTime();
					return true;
				}
			}
			catch (boost::mysql::error_with_diagnostics& err) {
				wxGetApp().DatabaseError(err.what());
				return false;
			}
		}
		else {
			wxMessageBox("Cannot read database", "Sign in", wxICON_ERROR | wxOK);
		}
	}
	catch (std::exception& exp){
		wxMessageBox(exp.what(), "Sign-in", wxICON_ERROR | wxOK);
	}
	return false;
}

bool pof::SignInDialog::InsertUserDataIntoDatabase(const pof::Account& acc)
{
	if (wxGetApp().mLocalDatabase) {
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
	else {
		 auto q = std::make_shared<pof::base::querystmt<pof::base::databasemysql>>(wxGetApp().mMysqlDatabase,
			"INSERT INTO USERS (priv, name, last_name, email, phonenumber, regnumber, username, password) VALUES (?,?,?,?,?,?,?,?);"s);
		 pof::base::datastmtquery::row_t row;
		 row.reserve(8);
		 row.emplace_back(boost::mysql::field(acc.priv.to_ullong()));
		 row.emplace_back(boost::mysql::field(acc.name));
		 row.emplace_back(boost::mysql::field(acc.lastname));
		 row.emplace_back(boost::mysql::field(acc.email));
		 row.emplace_back(boost::mysql::field(acc.phonenumber));
		 row.emplace_back(boost::mysql::field(acc.regnumber));
		 row.emplace_back(boost::mysql::field(acc.username));
		 row.emplace_back(boost::mysql::field(acc.passhash));

		 q->m_arguments.push_back(std::move(row));
		auto fut = q->get_future();
		wxGetApp().mMysqlDatabase->push(q);

		//busy wait for the completion
		if (wxGetApp().BusyWait(fut)){
			try {
				auto gt = fut.get();
				return static_cast<bool>(gt);
			}
			catch (boost::mysql::error_with_diagnostics& err){
				wxGetApp().DatabaseError(err.what());
				return false;
			}
		}
	}
	return false;
}
