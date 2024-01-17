#include "PharmacySetupWizard.h"

pof::PharmacySetupWizard::PharmacySetupWizard(wxFrame* frame)
{
   // SetExtraStyle(wxWIZARD_EX_HELPBUTTON);

    Create(frame, wxID_ANY, "PharmaOffice - Setup pharmacy",
        wxNullBitmap,
        wxDefaultPosition,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    pageSize = wxSize(900, 400);

    SetBackgroundColour(*wxWHITE);
    SetSize(wxSize(700, 400));
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
    SetIcon(appIcon);

    SetBitmapPlacement(wxWIZARD_VALIGN_CENTRE);
    SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

    CreateFirstPage();
    CreateContactPage();

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

    const auto phone = mPhoneNoValue->GetValue().ToStdString();
    if (phone.size() != 11) {
        //cannot properly validate number just the count
        wxMessageBox("Phone number is not complete, please enter valid phone number", "Registration", wxICON_WARNING | wxOK);
        return false;
    }
    mp->contact.phoneNumber = mPhoneNoValue->GetValue();
    const auto email = mEmailValue->GetValue().ToStdString();
    if (!ValidateEmail(email)) {
        return false;
    }
    mp->contact.email = mEmailValue->GetValue();
    mp->contact.website = mWebsiteValue->GetValue();

	return true;
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
    mPhoneNoValue->SetValidator(wxTextValidator{ wxFILTER_NUMERIC });
    bSizer1->Add(mPhoneNoValue, 0, wxALL , 5);

    mEmail = new wxStaticText(mContactPage, wxID_ANY, wxT("Email"), wxDefaultPosition, wxSize(450, -1), 0);
    mEmail->Wrap(-1);
    bSizer1->Add(mEmail, 0, wxEXPAND | wxALL, 5);

    mEmailValue = new wxTextCtrl(mContactPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(450, -1), 0);
    mEmailValue->SetMaxLength(250);
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
