#include "PharmacySetupWizard.h"
BEGIN_EVENT_TABLE(pof::PharmacySetupWizard, wxWizard)
    EVT_WIZARD_FINISHED(wxID_ANY, pof::PharmacySetupWizard::OnFinished)
    EVT_BUTTON(pof::PharmacySetupWizard::ID_ADD_ACCOUNT, pof::PharmacySetupWizard::OnAddAccount)
END_EVENT_TABLE()

pof::PharmacySetupWizard::PharmacySetupWizard(wxFrame* frame)
{
   // SetExtraStyle(wxWIZARD_EX_HELPBUTTON);
    auto path = wxGetApp().GetAssertsPath() / "icons" / "wiztest.svg";

    Create(frame, wxID_ANY, "PharmaOffice - Setup pharmacy",
        wxBitmapBundle::FromSVGFile(path.string(), wxSize(116, 260)),
        wxDefaultPosition,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    pageSize = wxSize(600, 400);
    SetBitmapPlacement(wxWIZARD_TILE);
    SetBackgroundColour(*wxWHITE);
    SetSize(wxSize(700, 400));
   
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
    SetIcon(appIcon);

    SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);
    SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

   
    CreateFirstPage();
    CreateContactPage();
    CreateAddressPage();
    CreateAddAccountPage();
    CreateSummaryPage();
}

bool pof::PharmacySetupWizard::ValidateEmail(const std::string email)
{
    try {
        const std::regex rex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
        if (!std::regex_match(email, rex)) {
            wxMessageBox("Invalid Email Address", "Registration", wxICON_WARNING | wxOK);
            return false;
        }
    }
    catch (const std::exception& exp) {
        auto what = exp.what();
        spdlog::error(what);
    }
    return true;
}

bool pof::PharmacySetupWizard::TransferDataFromWindow()
{
    if (!mp) {
        wxMessageBox("Critical Error, failed to create pharmacy, please call admin", "ERROR", wxICON_ERROR | wxOK);
        return false; //critial error;
    }
    int sel = mPharamcyTypeValue->GetSelection();
    if (sel != wxNOT_FOUND) {
        mp->pharmacyType.set(sel);
    }

    mp->name = mPharmacyNameValue->GetValue();
    mp->addy.city = mCityValue->GetValue();
    mp->addy.country = mCountryValue->GetValue();
    mp->addy.lga = mLgaValue->GetValue();
    mp->addy.number = mNoValue->GetValue();
    mp->addy.state = mStateValue->GetValue();
    mp->addy.street = mStreetValue->GetValue();
    mp->contact.phoneNumber = mPhoneNoValue->GetValue().ToStdString();
    mp->contact.email = mEmailValue->GetValue();
    mp->contact.website = mWebsiteValue->GetValue();

	return true;
}

bool pof::PharmacySetupWizard::InsertUserDataIntoDatabase(const pof::Account& acc)
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

void pof::PharmacySetupWizard::CreateFirstPage()
{
    // a wizard page may be either an object of predefined class
    mFirstPage = new wxWizardPageSimple(this);
    mFirstPage->SetSize(pageSize);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    m_panel1 = new wxPanel(mFirstPage, wxID_ANY, wxDefaultPosition, pageSize, wxTAB_TRAVERSAL);
    m_panel1->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxVERTICAL);
    
   // bSizer2->AddSpacer(10);

    mTitle = new wxStaticText(m_panel1, wxID_ANY, wxT("Welcome to PharmaOffice\n"), wxDefaultPosition, wxDefaultSize, 0);
    mTitle->Wrap(-1);
    mTitle->SetFont(wxFont(wxFontInfo(12).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

    bSizer2->Add(mTitle, 0, wxTOP | wxLEFT, 5);

    //bSizer2->Add(20, 0, 0, wxEXPAND, 5);

    mDescription = new wxStaticText(m_panel1, wxID_ANY, wxT(R"(PharmaOffice is an on-premise pharmacy management application used for the day to day management of the pharmacy. To begin please register a pharmacy
Please note that a registered pharamcy cannot be removed until uninstall)"), wxDefaultPosition, wxDefaultSize, 0);
    mDescription->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
    mDescription->Wrap(-1);

    bSizer2->Add(mDescription, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

    mPharmacyType = new wxStaticText(m_panel1, wxID_ANY, wxT("Pharmacy type"), wxDefaultPosition, wxDefaultSize, 0);
    mPharmacyType->Wrap(-1);
    bSizer2->Add(mPharmacyType, 0, wxEXPAND | wxALL, 5);

    wxString mPharamcyTypeValueChoices[] = { wxT("COMMUNITY"), wxT("HOSPITAL"), wxT("INDUSTRY"), wxT("DRF"), wxT("EDUCATIONAL") };
    int mPharamcyTypeValueNChoices = sizeof(mPharamcyTypeValueChoices) / sizeof(wxString);
    mPharamcyTypeValue = new wxChoice(m_panel1, wxID_ANY, wxDefaultPosition, wxSize(450, -1), mPharamcyTypeValueNChoices, mPharamcyTypeValueChoices, 0);
    mPharamcyTypeValue->SetSelection(0);
    mPharamcyTypeValue->Bind(wxEVT_CHOICE, [&](wxCommandEvent& evt) {
        int sel = evt.GetSelection();
    if (sel == wxNOT_FOUND) return;

    });

    bSizer2->Add(mPharamcyTypeValue, 0, wxALL, 5);

    mPharamcyName = new wxStaticText(m_panel1, wxID_ANY, wxT("Pharmacy name"), wxDefaultPosition, wxDefaultSize, 0);
    mPharamcyName->Wrap(-1);
    bSizer2->Add(mPharamcyName, 0, wxEXPAND | wxALL, 5);

    mPharmacyNameValue = new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mPharmacyNameValue->SetMaxLength(250);
    mPharmacyNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
    bSizer2->Add(mPharmacyNameValue, 0, wxALL, 5);

    m_panel1->SetSizer(bSizer2);
    m_panel1->Layout();
 //   bSizer2->Fit(m_panel1);
    bSizer1->Add(m_panel1, 0, wxEXPAND | wxALL, 5);

    mFirstPage->SetSizer(bSizer1);
    mFirstPage->Layout();
    GetPageAreaSizer()->Add(mFirstPage);

}

void pof::PharmacySetupWizard::CreateContactPage()
{
    mContactPage = new wxWizardPageSimple(this);
   
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    auto contTitle = new wxStaticText(mContactPage, wxID_ANY, wxT("Please enter the pharmacy contact details.\n"), wxDefaultPosition, wxDefaultSize, 0);
    contTitle->Wrap(-1);
    contTitle->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

    bSizer1->Add(contTitle, 0, wxTOP | wxLEFT, 5);

    auto conDescription = new wxStaticText(mContactPage, wxID_ANY, wxT(R"(PharmaOffice uses the contact information for receipts and reports, please fill in the information)"), wxDefaultPosition, wxDefaultSize, 0);
    conDescription->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
    conDescription->Wrap(-1);

    bSizer1->Add(conDescription, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

    mPhoneNo = new wxStaticText(mContactPage, wxID_ANY, wxT("Phone No"), wxDefaultPosition, wxDefaultSize, 0);
    mPhoneNo->Wrap(-1);
    bSizer1->Add(mPhoneNo, 0, wxEXPAND | wxALL, 5);

    mPhoneNoValue = new wxTextCtrl(mContactPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mPhoneNoValue->SetMaxLength(250);
    mPhoneNoValue->SetValidator(pof::RegexValidator(std::regex("(0|91)?[6-9][0-9]{9}"), "Invalid phone number"));
    bSizer1->Add(mPhoneNoValue, 0, wxALL , 5);

    mEmail = new wxStaticText(mContactPage, wxID_ANY, wxT("Email"), wxDefaultPosition, wxSize(450, -1), 0);
    mEmail->Wrap(-1);
    bSizer1->Add(mEmail, 0, wxEXPAND | wxALL, 5);

    mEmailValue = new wxTextCtrl(mContactPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mEmailValue->SetMaxLength(250);
    mEmailValue->SetValidator(pof::RegexValidator(std::regex(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)"), "Invalid email address"));
    bSizer1->Add(mEmailValue, 0, wxALL , 5);

    mWebsiteText = new wxStaticText(mContactPage, wxID_ANY, wxT("Website"), wxDefaultPosition, wxSize(450, -1), 0);
    mWebsiteText->Wrap(-1);
    bSizer1->Add(mWebsiteText, 0, wxALL, 5);

    mWebsiteValue = new wxTextCtrl(mContactPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mWebsiteValue->SetMaxLength(250);
    bSizer1->Add(mWebsiteValue, 0, wxALL , 5);

    mContactPage->SetSizer(bSizer1);
    mContactPage->Layout();
    
    mFirstPage->Chain(mContactPage);
}

void pof::PharmacySetupWizard::CreateAddressPage()
{
    mAddressPage = new wxWizardPageSimple(this);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    auto contTitle = new wxStaticText(mAddressPage, wxID_ANY, wxT("Please enter the pharmacy address details.\n"), wxDefaultPosition, wxDefaultSize, 0);
    contTitle->Wrap(-1);
    contTitle->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

    bSizer1->Add(contTitle, 0, wxTOP | wxLEFT, 5);

    auto conDescription = new wxStaticText(mAddressPage, wxID_ANY, wxT(R"(PharmaOffice uses the address information for receipts and reports, please fill in the information)"), wxDefaultPosition, wxDefaultSize, 0);
    conDescription->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
    conDescription->Wrap(-1);

    bSizer1->Add(conDescription, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

    mCountyText = new wxStaticText(mAddressPage, wxID_ANY, wxT("Country"), wxDefaultPosition, wxDefaultSize, 0);
    mCountyText->Wrap(-1);
    bSizer1->Add(mCountyText, 0, wxALL, 5);

    mCountryValue = new wxTextCtrl(mAddressPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mCountryValue->SetMaxLength(250);
    bSizer1->Add(mCountryValue, 0, wxALL, 5);

    mLgaText = new wxStaticText(mAddressPage, wxID_ANY, wxT("L.G.A"), wxDefaultPosition, wxDefaultSize, 0);
    mLgaText->Wrap(-1);
    bSizer1->Add(mLgaText, 0, wxALL, 5);

    mLgaValue = new wxTextCtrl(mAddressPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mLgaValue->SetMaxLength(250);
    bSizer1->Add(mLgaValue, 0, wxALL , 5);

    mNoText = new wxStaticText(mAddressPage, wxID_ANY, wxT("No."), wxDefaultPosition, wxDefaultSize, 0);
    mNoText->Wrap(-1);
    bSizer1->Add(mNoText, 0, wxALL, 5);

    mNoValue = new wxTextCtrl(mAddressPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mNoValue->SetMaxLength(250);
    mNoValue->SetValidator(wxTextValidator{ wxFILTER_NUMERIC });
    bSizer1->Add(mNoValue, 0, wxALL, 5);

    mStreetText = new wxStaticText(mAddressPage, wxID_ANY, wxT("Street"), wxDefaultPosition, wxDefaultSize, 0);
    mStreetText->Wrap(-1);
    bSizer1->Add(mStreetText, 0, wxALL, 5);

    mStreetValue = new wxTextCtrl(mAddressPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mStreetValue->SetMaxLength(250);
    bSizer1->Add(mStreetValue, 0, wxALL , 5);

    mCityText = new wxStaticText(mAddressPage, wxID_ANY, wxT("City"), wxDefaultPosition, wxDefaultSize, 0);
    mCityText->Wrap(-1);
    bSizer1->Add(mCityText, 0, wxALL, 5);

    mCityValue = new wxTextCtrl(mAddressPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mCityValue->SetMaxLength(250);
    bSizer1->Add(mCityValue, 0, wxALL, 5);

    mStateText = new wxStaticText(mAddressPage, wxID_ANY, wxT("State"), wxDefaultPosition, wxSize(450, -1), 0);
    mStateText->Wrap(-1);
    bSizer1->Add(mStateText, 0, wxALL, 5);

    mStateValue = new wxTextCtrl(mAddressPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mStateValue->SetMaxLength(250);
    bSizer1->Add(mStateValue, 0, wxALL, 5);



    mAddressPage->SetSizer(bSizer1);
    mAddressPage->Layout();

    mContactPage->Chain(mAddressPage);
}

void pof::PharmacySetupWizard::CreateAddAccountPage()
{
    mAddAccountPage = new wxWizardPageSimple(this);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    auto contTitle = new wxStaticText(mAddAccountPage, wxID_ANY, wxT("Please add an account for the pharmacy.\n"), wxDefaultPosition, wxDefaultSize, 0);
    contTitle->Wrap(-1);
    contTitle->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

    bSizer1->Add(contTitle, 0, wxTOP | wxLEFT, 5);

    auto conDescription = new wxStaticText(mAddAccountPage, wxID_ANY, wxT(R"(Please an a user account to the pharmacy.)"), wxDefaultPosition, wxDefaultSize, 0);
    conDescription->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
    conDescription->Wrap(-1);

    bSizer1->Add(conDescription, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

    btn = new wxButton(mAddAccountPage, ID_ADD_ACCOUNT);
    btn->SetBitmap(wxArtProvider::GetBitmap("action_add"));
    btn->SetLabel("Add user account");
    btn->SetBackgroundColour(*wxWHITE);
    bSizer1->Add(btn, 0,  wxALL, 5);


    mAddAccountPage->SetSizer(bSizer1);
    mAddAccountPage->Layout();

    mAddressPage->Chain(mAddAccountPage);
}

void pof::PharmacySetupWizard::CreateSummaryPage()
{
    mSummaryPage = new wxWizardPageSimple(this);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    auto contTitle = new wxStaticText(mSummaryPage, wxID_ANY, wxT("You have completed the pharmacy registration.\n"), wxDefaultPosition, wxDefaultSize, 0);
    contTitle->Wrap(-1);
    contTitle->SetFont(wxFont(wxFontInfo(10).Bold().AntiAliased().Family(wxFONTFAMILY_SWISS)));

    bSizer1->Add(contTitle, 0, wxTOP | wxLEFT, 5);

    auto conDescription = new wxStaticText(mSummaryPage, wxID_ANY, wxT(R"(Click finish to start using PharmaOffice.)"), wxDefaultPosition, wxDefaultSize, 0);
    conDescription->SetFont(wxFont(wxFontInfo().AntiAliased().Family(wxFONTFAMILY_SWISS)));
    conDescription->Wrap(-1);

    bSizer1->Add(conDescription, 0, wxTOP | wxBOTTOM | wxLEFT, 5);


    mSummaryPage->SetSizer(bSizer1);
    mSummaryPage->Layout();

    mAddAccountPage->Chain(mSummaryPage);
}

void pof::PharmacySetupWizard::OnFinished(wxWizardEvent& evt)
{
   state = TransferDataFromWindow();
}

void pof::PharmacySetupWizard::OnAddAccount(wxCommandEvent& evt)
{
    pof::RegistrationDialog regDialog(this);
    regDialog.mAccount.mLocalDatabase = wxGetApp().mLocalDatabase;
    int ret = regDialog.ShowModal();
    bool status = false;
    if (ret == wxID_OK) {
        wxBusyInfo info("Registring account and Signing in...");
        if (!InsertUserDataIntoDatabase(regDialog.GetAccount())) {
            wxMessageBox(wxGetApp().mLocalDatabase->err_msg().data(), "REGISTRATION", wxICON_ERROR | wxOK);
            return;
        }
        btn->Disable();
    }
}
