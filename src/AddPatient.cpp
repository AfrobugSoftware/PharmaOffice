#include "AddPatient.h"

using namespace pof;

pof::AddPatient::AddPatient( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel1->SetBackgroundColour( wxColour( 255, 255, 255 ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	TitleText = new wxStaticText( m_panel1, wxID_ANY, wxT("Add Patient"), wxDefaultPosition, wxDefaultSize, 0 );
	TitleText->Wrap( -1 );
	TitleText->SetFont( wxFontInfo().Bold().AntiAliased());
	
	bSizer2->Add( TitleText, 1, wxALL, 5 );
	
	m_staticText18 = new wxStaticText( m_panel1, wxID_ANY, wxT("Create a patient for the pharmacy"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer2->Add( m_staticText18, 0, wxALL, 5 );
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 0, wxEXPAND | wxALL, 5 );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, wxT("Patient Details") ), wxVERTICAL );
	
	m_scrolledWindow2 = new wxPanel( sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	//m_scrolledWindow2->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mPatientName = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	mPatientName->Wrap( -1 );
	fgSizer2->Add( mPatientName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mPatientNameValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPatientNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	mPatientNameValue->SetMaxLength(250);
	fgSizer2->Add( mPatientNameValue, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	mPatientLastName = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Last name"), wxDefaultPosition, wxDefaultSize, 0 );
	mPatientLastName->Wrap( -1 );
	fgSizer2->Add( mPatientLastName, 0, wxALL, 5 );
	
	mPatientLastNameValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPatientLastNameValue->SetValidator(wxTextValidator{ wxFILTER_EMPTY });
	mPatientLastNameValue->SetMaxLength(250);
	fgSizer2->Add( mPatientLastNameValue, 1, wxALL|wxEXPAND, 5 );
	
	mDateOfBirth = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Date of birth"), wxDefaultPosition, wxDefaultSize, 0 );
	mDateOfBirth->Wrap( -1 );
	fgSizer2->Add( mDateOfBirth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	//change to date picker
	mDOBValue = new wxDatePickerCtrl( m_scrolledWindow2, wxID_ANY, wxDateTime::Now(), wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
	fgSizer2->Add( mDOBValue, 0, wxALL|wxEXPAND, 5 );
	
	mGenderLabel = new wxStaticText(m_scrolledWindow2, wxID_ANY, wxT("Gender"), wxDefaultPosition, wxDefaultSize, 0);
	mGenderLabel->Wrap(-1);
	fgSizer2->Add(mGenderLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	genChoies.Add("Female");
	genChoies.Add("Male");
	genChoies.Add("Not specified");

	mGenderValue = new wxChoice(m_scrolledWindow2, wxID_ANY, wxDefaultPosition, wxDefaultSize, genChoies);
	fgSizer2->Add( mGenderValue, 0, wxALL|wxEXPAND, 5 );


	mPhonenumber = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Phone number"), wxDefaultPosition, wxDefaultSize, 0 );
	mPhonenumber->Wrap( -1 );
	fgSizer2->Add( mPhonenumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mPhonenumberValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mPhonenumberValue->SetValidator(wxTextValidator{ wxFILTER_DIGITS });
	fgSizer2->Add( mPhonenumberValue, 0, wxALL|wxEXPAND, 5 );
	
	mAddress = new wxStaticText( m_scrolledWindow2, wxID_ANY, wxT("Address"), wxDefaultPosition, wxDefaultSize, 0 );
	mAddress->Wrap( -1 );
	fgSizer2->Add( mAddress, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mAddressValue = new wxTextCtrl( m_scrolledWindow2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mAddressValue, 1, wxALL|wxEXPAND, 5 );
	
	
	m_scrolledWindow2->SetSizer( fgSizer2 );
	m_scrolledWindow2->Layout();
	fgSizer2->Fit( m_scrolledWindow2 );
	sbSizer7->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 5 );
	
	
	m_panel4->SetSizer( sbSizer7 );
	m_panel4->Layout();
	sbSizer7->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 0, wxEXPAND | wxALL, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer1->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel7 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( m_panel7, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( m_panel7, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	
	bSizer4->Add( m_sdbSizer2, 0, wxEXPAND, 15 );
	
	
	m_panel7->SetSizer( bSizer4 );
	m_panel7->Layout();
	bSizer4->Fit( m_panel7 );
	bSizer1->Add( m_panel7, 0, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );

	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxArtProvider::GetBitmap("pharmaofficeico"));
	SetIcon(appIcon);
}

pof::AddPatient::~AddPatient()
{
	//
}

bool pof::AddPatient::TransferDataFromWindow()
{
	//validate phone number
	const std::regex pattern("(0|91)?[6-9][0-9]{9}");
	auto ph = mPhonenumberValue->GetValue().ToStdString();
	if (!std::regex_match(ph, pattern)) {
		wxMessageBox("Invalid phone number", "Add Patient", wxICON_INFORMATION | wxOK);
		return false;
	}
	auto& v = mPatientEntry.first;
	v.resize(pof::PatientManager::PATIENT_MAX);
	v[pof::PatientManager::PATIENT_UUID] = boost::uuids::random_generator_mt19937{}();
	v[pof::PatientManager::PATIENT_NAME] = mPatientNameValue->GetValue().ToStdString();
	v[pof::PatientManager::PATIENT_LAST_NAME] = mPatientLastNameValue->GetValue().ToStdString();
	auto date = pof::base::data::clock_t::from_time_t(mDOBValue->GetValue().GetTicks());
	v[pof::PatientManager::PATIENT_AGE] = date;
	int sel = mGenderValue->GetSelection();
	if (sel == wxNOT_FOUND) sel = 2;
	v[pof::PatientManager::PATIENT_GENDER] = genChoies[sel].ToStdString();
	v[pof::PatientManager::PATIENT_PHONE_NUMBER] = ph;
	v[pof::PatientManager::PATIENT_ADDRESS] = mAddressValue->GetValue().ToStdString();

	v[pof::PatientManager::PATIENT_BMI] = static_cast<std::uint64_t>(0);
	v[pof::PatientManager::PATIENT_WEIGHT] = static_cast<std::uint64_t>(0);
	v[pof::PatientManager::PATIENT_HR] = static_cast<std::uint64_t>(0);
	v[pof::PatientManager::PATIENT_BP_SYS] = static_cast<std::uint64_t>(0);
	v[pof::PatientManager::PATIENT_BP_DYS] = static_cast<std::uint64_t>(0);
	v[pof::PatientManager::PATIENT_RR] = static_cast<std::uint64_t>(0);
	v[pof::PatientManager::PATIENT_TEMPT] = static_cast<std::uint64_t>(0);

	auto today = pof::base::data::clock_t::now();
	v[pof::PatientManager::PATIENT_ENTERED_DATE] = today;
	v[pof::PatientManager::PATIENT_MODIFIED_DATE] = today;
	v[pof::PatientManager::PATIENT_CLINICAL_INDICATION] = ""s;

	return true;
}
